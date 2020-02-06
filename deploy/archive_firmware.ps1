function PackFirmware ($modelListFile, $model, $fwVersion) {
    $initErrorCount = $error.count
    mkdir $model | out-null

    $entries = Get-Content $modelListFile
    $archive = @()

    for ($i=0;$i -lt $entries.Count;$i++)
    {
        $entry = $entries[$i].Split(";")
        $sourceFile = $ExecutionContext.InvokeCommand.ExpandString($entry[0])
        
        if ($entry[1] -ne "")
        {
            $destFile = "$model\$($entry[1])"
            Copy-Item -Path $sourceFile -Destination $destFile -PassThru  | out-null
            $archive += $destFile
        }
        else {
            $archive += $sourceFile
        }
    }

    $currentTime = Get-Date -Format "dd-mm-yyyy_HHmm"
    $archiveName = ".\Output\" + $model + "_" + $fwVersion + "_" + $currentTime + ".fw2"

    Compress-Archive -Path $archive -DestinationPath $archiveName
    Remove-Item -LiteralPath $model -Force -Recurse

    $result = "" | Select-Object -Property Color,Message
    if ($error.count -gt $initErrorCount)
    {
        $result.Color = "red"
        $result.Message = "$model packing failed"
    }
    else {
        $result.Color = "green"
        $result.Message = "$model archive packed successfully"
    }

    return $result
}


Push-Location $PSScriptRoot

$ver = Select-String -Pattern '#define SHORT_BUILD_VERSION "(.+)"' -Path '..\Configuration_Version.h' | %{$_.Matches[0].Groups[1].Value}

$modelFiles = dir "*.txt"
$packResults = @()
foreach ($modelFile in $modelFiles)
{
    $packResults += PackFirmware $modelFile $modelFile.BaseName $ver
}

foreach ($result in $packResults)
{
    Write-Host $result.Message -ForegroundColor $result.Color
}

Pop-Location

Read-Host -Prompt "Press Enter to exit"
