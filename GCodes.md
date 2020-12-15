# Implemented Codes
Codes marked with * are specific for Composer MKA firmware (of their meaning differs from typical RepRap codes)

## G Codes

| Code | Descripion |
| ---: | :--- |
|   G0 | G1
|   G1 | Coordinated Movement X Y Z E U V F(feedrate)
|   G4 | Dwell S[seconds] or P[milliseconds], delay in Second or Millisecond
|  G27 | Nozzle Park
|  G28 | X Y Z Home all Axis
|  G60 | Save current position coordinates (all axes, for active extruder). S[SLOT] - specifies memory slot # (0-based) to save into (default 0)
|  G61 | Apply/restore saved coordinates to the active extruder. X Y Z E - Value to add at stored coordinates F[speed] - Set Feedrate S[SLOT] - specifies memory slot # (0-based) to restore from (default 0)
|  G90 | Use Absolute Coordinates
|  G91 | Use Relative Coordinates
|  G92 | Set current position to cordinates given

## M codes
| Code | Requires | Descripion |
| ---: | :---: | :--- |
|   M0 | LCD | Unconditional stop - Wait for user to press a button on the LCD
|   M1 | LCD | Same as M0
|  M17 | - | Enable stepper motors
|  M18 | - | Disable stepper motors; same as M84
|  M20 | SDCARD | List SD card
|  M21 | SDCARD | Init SD card
|  M22 | SDCARD | Release SD card
|  M23 | SDCARD | Select SD file (M23 filename.g)
|  M24 | SDCARD | Start/resume SD print
|  M25 | SDCARD | Pause SD print
|  M26 | SDCARD | Set SD position in bytes (M26 S12345)
|  M27 | SDCARD | Report SD print status
|  M28 | SDCARD | Start SD write (M28 filename.g)
|  M29 | SDCARD | Stop SD write
|  M30 | SDCARD | Delete file from SD (M30 filename.g)
|  M31 | SDCARD | Output time since last M109 or SD card start to serial
|  M32 | SDCARD | Make directory
|  M35 | NEXTION | Upload Firmware to Nextion from SD
|  M39* | SDCARD | Sets SD card SPI CS pin P[slot] U[pin]
|  M42 | - | Change pin status via gcode Use M42 Px Sy to set pin x to value y, when omitting Px the onboard led will be used.
|  M44 | - | Codes debug - report codes available (and how many of them there are) I - G-code list J - M-code list
|  M75 | - | Start the print job timer
|  M76 | - | Pause the print job timer
|  M77 | - | Stop the print job timer
|  M78 | - | Show statistical information about the print jobs
|  M81 | - | Turn off Power, if possible
|  M82 | - | Set E codes absolute (default)
|  M83 | - | Set E codes relative while in Absolute Coordinates (G90) mode
|  M84 | - | Disable steppers until next move, or use S[seconds] to specify an inactivity timeout, after which the steppers will be disabled. S0 to disable the timeout.
|  M85 | - | Set inactivity shutdown timer with parameter S[seconds]. To disable set zero (default)
|  M92 | - | Set axis steps per unit - same syntax as G92, H[microstep] L[Layer wanted]
| M104 | - | T[int] 0-5 For Select Hotends (default 0), S[C°] Set hotend target temperature, R[C°] Set hotend idle temperature
| M105 | - | Read current temp
| M106 | - | P[fan] S[speed] F[frequency] U[pin] L[min speed] X[max speed] I[inverted logic] H[int] Set Auto mode - H=7 for controller - H-1 for disabled T[int] Triggered temperature
| M107 | - | P[fan] Fan off
| M108 | EMERGENCY PARSER | Break out of heating loops (M109, M190, M303). With no controller, breaks out of M0/M1.
| M109 | - | S[xxx] - Wait for hotend current temp to reach target temp. Waits only when heating R[xxx] - Wait for hotend current temp to reach target temp. Waits when heating and cooling. IF AUTOTEMP is enabled, S[mintemp] B[maxtemp] F[factor]. Exit autotemp by any M109 without F
| M110 | - | Set the current line number
| M111 | - | Set debug flags with S[mask].
| M112 | - | Emergency stop
| M114 | - | Output current position to serial port
| M115 | EXTENDED CAPABILITIES REPORT* | Report capabilities. (* for extended capabilities)
| M117 | - | Display a message on the controller screen
| M118 | - | Display a message in the host console
| M119 | - | Output Endstop status to serial port
| M120 | - | Enable endstop detection
| M121 | - | Disable endstop detection
| M122 | MIN SOFTWARE ENDSTOPS or MAX SOFTWARE ENDSTOPS | S[bool] Enable or disable check software endstop
| M123 | ENDSTOP | Set Endstop Logic X[bool] Y[bool] Z[bool] I[X2 bool] J[Y2 bool] K[Z2 bool] P[Probe bool] D[Door bool]
| M124 | ENDSTOP | Set Endstop Pullup X[bool] Y[bool] Z[bool] I[X2 bool] J[Y2 bool] K[Z2 bool] P[Probe bool] D[Door bool]
| M125 | PARK HEAD ON PAUSE | Save current position and move to pause park position 
| M140 | - | T[int] 0-3 For Select Beds (default 0), S[C°] Set hot bed target temperature, R[C°] Set hot bed idle temperature
| M141 | - | T[int] 0-3 For Select Chambers (default 0), S[C°] Set hot chamber target temperature, R[C°] Set hot chamber idle temperature 
| M145 | - | Set the heatup state H[hotend] B[bed] C[chamber] F[fan speed] for S[material] (0=PLA, 1=ABS, 2=GUM)
| M155 | - | Auto report temperatures S[bool] Enable/disable
| M190 | - | Sxxx - Wait for bed current temp to reach target temp. Waits only when heating Rxxx - Wait for bed current temp to reach target temp. Waits when heating and cooling
| M191 | - | Sxxx - Wait for chamber current temp to reach target temp. Waits only when heating Rxxx Wait for chamber current temp to reach target temp. Waits when heating and cooling
| M201 | - | Set max acceleration in units/s^2 for print moves (M201 X1000 Y1000 Z1000 E0 S1000 E1 S1000 E2 S1000 E3 S1000) in mm/sec^2
| M203 | - | Set maximum feedrate that your machine can sustain (M203 X200 Y200 Z300 E0 S1000 E1 S1000 E2 S1000 E3 S1000) in mm/sec
| M204 | - | Set Accelerations in mm/sec^2: P for Printing moves, R for Retract moves and V for Travel (non printing) moves (ex. M204 P800 V3000 T0 R9000)
| M205 | - | Set Advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk, J=Junction deviation mm
| M206 | - | set additional homing offset
| M217* | - | Toolchange Parameters A[x_offset] B[y_offset] T[target_tool] S[step (0-11)] X[x_pos] Y[y_pos] V[speed mm/s] K[switch movement 0/1]
| M218 | - | set hotend offset (in mm): H[hotend_number] X[offset_on_X] Y[offset_on_Y] Z[offset_on_Z]
| M220 | - | S[factor in percent] set speed factor override percentage, B to backup, R to restore currently set override
| M221 | - | T[extruder] S[factor in percent] - set extrude factor override percentage
| M222 | - | T[extruder] S[factor in percent] - set density extrude factor percentage for purge
| M226 | - | Wait until the specified pin reaches the state required (M226 P[pin] S[state])
| M250* | - | Read and optionally set the LCD brightness M250 C[brightness]
| M280 | SERVO | Position an RC Servo P[index] S[angle/microseconds], ommit S to report back current angle
| M300 | - | Play beep sound S[frequency Hz] P[duration ms]
| M301 | - | Set PID parameters P I D and C. H[heaters] H = 0-3 Hotend, H = -1 BED, H = -2 CHAMBER, H = -3 COOLER, P[float] Kp term, I[float] Ki term, D[float] Kd term. 
| M302 | - | Allow cold extrudes, or set the minimum extrude S[temperature].
| M303 | - | PID relay autotune: H[heaters] H = 0-3 Hotend, H = -1 BED, H = -2 CHAMBER, H = -3 COOLER, S[temperature] sets the target temperature (default target temperature = 200C), C[cycles>, R[method>, U[Apply result>, R[Method] 0 = Classic Pid, 1 = Some overshoot, 2 = No Overshoot, 3 = Pessen Pid.
| M305 | - | Set thermistor and ADC parameters: H[heaters] H = 0-3 Hotend, H = -1 BED, H = -2 CHAMBER, H = -3 COOLER, A[float] Thermistor resistance at 25°C, B[float] BetaK, C[float] Steinhart-Hart C coefficien, R[float] Pullup resistor value, L[int] ADC low offset correction, O[int] ADC high offset correction, P[int] Sensor Pin. 
| M306 | - | Set Heaters parameters: H[heaters] H = 0-3 Hotend, H = -1 BED, H = -2 CHAMBER, H = -3 COOLER, A[int] Power Drive Min, B[int] Power Drive Max, C[int] Power Max, F[int] Frequency, L[int] Min temperature, O[int] Max temperature, U[bool] Use Pid/bang bang, I[bool] Hardware Inverted, T[bool] Thermal Protection, P[int] Pin, Q[bool] PWM Hardware
| M400 | - | Finish all moves
| M408* | - | Report status in JSON format. S[format 0-4] P[bool] - enable auto-report  (https://reprap.org/wiki/RepRap_Firmware_Status_responses) 
| M500* | - | Stores paramters in EEPROM. Use "M500 S" to store system settings, without "S" only user settings will be stored
| M501 | - | Reads parameters from EEPROM (if you need reset them after you changed them temporarily).
| M502 | - | Reverts to the hardcoded settings values.
| M503* | - | Print the current settings (from memory not from EEPROM) S - print system settings, U - print user settings, V - print only version, R - print only CRC, C - print clean g-code
| M524 | - | Cancel SD print
| M530 | - | Enables explicit printing mode (S1) or disables it (S0). L can set layer count
| M531 | - | Filename - Define filename being printed
| M532 | - | X[percent] L[curLayer] - update current print state progress (X=0..100) and layer L
| M569* | - | Stepper driver direction control M569. Use X, Y, Z, E, U, V letters for axes, e.g. “M569 X0 Y1” to turn off X-axis inversion and turn on Y-axis inversion)
| M704* | - | Load/unload filament config. E[extruder] - Extruder driver number. L[distance] - Load distance. U[distance] - Unload distance.
| M900 | LIN ADVANCE | K[factor] Set Linear Advance K-factor. LIN Advance is supported only for plastic extruder (E)
| M999 | - | Restart after being stopped by error
| M1001* | - | Start fiber reinforced polygon L[fiber polygon length]
| M1002* | - | End fiber reinforced polygon
| M1005* | - | Print SN to serial
| M1006* | - | Print hardware version to serial
| M1007* | - | Print firmware version to serial
| M1008* | - | Print machine name to serial
| M1009* | - | Display firmware update screen
| M1010* | - | Cut fiber (uses parameters set by M1011)
| M1011* | - | Fiber cut Parameters S[servo-id] A[cut-angle] B[neutral-angle]
| M1013* | - | Park/Unpark nozzle for heating near wipe station. R - Return from park
| M1014* | - | Set SBC status S1 - connected, S0 - temporarily disconnected
| M1015* | - | Set network status E[0/1] - ethernet, I[0/1] - wifi enabled, W[0/1] wifi connected, A[0/1] - Aura connect
| M1016* | - | Send command to network bridge
| M1017* | - | S[0/1] Set job awaiting flag  
| M1018* | - | Clear AC dir in internal SD storage 


