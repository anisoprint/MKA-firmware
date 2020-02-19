/*
 * StateFiles.cpp
 *
 *  Created on: 7 θών. 2018 γ.
 *      Author: Azarov
 */

#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateFiles.h"


namespace {

	///////////// Nextion components //////////
	//Page
	NexObject _page  = NexObject(PAGE_FILES,  0,  "files");

	//Buttons
	NexObject _tFName0 = NexObject(PAGE_FILES,  21,  "f0");
	NexObject _tFName1 = NexObject(PAGE_FILES,  22,  "f1");
	NexObject _tFName2 = NexObject(PAGE_FILES,  23,  "f2");
	NexObject _tFName3 = NexObject(PAGE_FILES,  24,  "f3");
	NexObject _tFName4 = NexObject(PAGE_FILES,  25,  "f4");
	NexObject _tFName5 = NexObject(PAGE_FILES,  26,  "f5");
	NexObject _tFName6 = NexObject(PAGE_FILES,  27,  "f6");

	NexObject _tFIcon0 = NexObject(PAGE_FILES,  10,  "i0");
	NexObject _tFIcon1 = NexObject(PAGE_FILES,  12,  "i1");
	NexObject _tFIcon2 = NexObject(PAGE_FILES,  14,  "i2");
	NexObject _tFIcon3 = NexObject(PAGE_FILES,  28,  "i3");
	NexObject _tFIcon4 = NexObject(PAGE_FILES,  29,  "i4");
	NexObject _tFIcon5 = NexObject(PAGE_FILES,  30,  "i5");
	NexObject _tFIcon6 = NexObject(PAGE_FILES,  31,  "i6");

	NexObject _bFUp = NexObject(PAGE_FILES,  32,  "up");
	NexObject _bFDown = NexObject(PAGE_FILES,  33,  "dn");

	NexObject _tLoading = NexObject(PAGE_FILES,  34,  "tL");

	NexObject _bFUpIcon = NexObject(PAGE_FILES,  17,  "pUP");
	NexObject _bFDownIcon = NexObject(PAGE_FILES,  18,  "pDN");

	NexObject _bFilesCancel = NexObject(PAGE_FILES,  9,  "cc");

	NexObject *_listenList[] = { &_tFName0, &_tFName1, &_tFName2, &_tFName3, &_tFName4, &_tFName5, &_tFName6,
			&_bFUp, &_bFDown, &_bFilesCancel, NULL };

	uint32_t _listPosition = 0;
	bool _insideDir = false;

	void Files_PopulateFileRow(uint32_t row, bool isDir, const char* filename) {
		NexObject* rowText;
		NexObject* rowIcon;

		switch(row) {
		    case 0 :
		    	 rowText = &_tFName0;
		    	 rowIcon = &_tFIcon0;
		         break;
		    case 1 :
		    	 rowText = &_tFName1;
		    	 rowIcon = &_tFIcon1;
		         break;
		    case 2 :
		    	 rowText = &_tFName2;
		    	 rowIcon = &_tFIcon2;
		         break;
		    case 3 :
		    	 rowText = &_tFName3;
		    	 rowIcon = &_tFIcon3;
		         break;
		    case 4 :
		    	 rowText = &_tFName4;
		    	 rowIcon = &_tFIcon4;
		         break;
		    case 5 :
		    	 rowText = &_tFName5;
		    	 rowIcon = &_tFIcon5;
		         break;
		    case 6 :
		    	 rowText = &_tFName6;
		    	 rowIcon = &_tFIcon6;
		         break;
		}

	    if (isDir) {
	    	rowIcon->setPic(NEX_ICON_FILE_FOLDER);
	    	rowText->attachPush(StateFiles::FFolder_Push, rowText);
	    }
	    else
	    {
	    	rowIcon->setPic(NEX_ICON_FILE_BLANK);
	    	if (filename == "") {
	        	rowText->detachPush();
	    	}
	    	else
	    	{
	        	rowText->attachPush(StateFiles::FFile_Push, rowText);
	    	}

	    }
	    rowText->setText(filename);
	}

	void Files_PopulateFileList(uint32_t number) {

	    uint16_t fileCnt = card.getnrfilenames();
	    uint32_t i = number;
	    card.getWorkDirName();
	    uint8_t start_row = 0;
		  //serial_print(card.fileName);
		  //serial_print("\n>>>>>>>>>>>>\n");
	    if (card.fileName[0] != '/') {
	    	_tFName0.setText("..");
	    	_tFIcon0.setPic(NEX_ICON_FILE_BACK);
	    	_tFName0.attachPush(StateFiles::FFolderUp_Push);
	    	start_row = 1;
	    	_insideDir = true;
	    }
	    else
	    {
	    	_insideDir = false;
	    }

	    for (uint8_t row = start_row; row < 7; row++) {
	      if (i < fileCnt) {
			#if ENABLED(SDCARD_SORT_ALPHA)
			  card.getfilename_sorted(i);
			#else
			  card.getfilename(i);
			#endif
	        Files_PopulateFileRow(row, card.filenameIsDir, card.fileName);

	      }
	      else
	      {
	    	  Files_PopulateFileRow(row, false, "");
	      }
	      i++;
	    }

	    if (number>0)
	    	_bFUpIcon.setPic(NEX_ICON_FILES_UP);
	    else
	    	_bFUpIcon.setPic(NEX_ICON_FILES_UP_GREY);

	    if ((number + (_insideDir ? 6 : 7)) <fileCnt)
	    	_bFDownIcon.setPic(NEX_ICON_FILES_DOWN);
	    else
	    	_bFDownIcon.setPic(NEX_ICON_FILES_DOWN_GREY);
	}

}

void StateFiles::FUp_Push(void* ptr) {
	if (_listPosition>0)
	{
		_listPosition -= _insideDir ? 6 : 7;
		if (_listPosition<0) _listPosition=0;
		Files_PopulateFileList(_listPosition);
	}
}

void StateFiles::FDown_Push(void* ptr) {
    uint16_t fileCnt = card.getnrfilenames();
    if ((_listPosition + (_insideDir ? 6 : 7)) <fileCnt)
    {
    	_listPosition += _insideDir ? 6 : 7;
    	Files_PopulateFileList(_listPosition);
    }
}

void StateFiles::FFile_Push(void* ptr) {
    ZERO(NextionHMI::buffer);
    if (ptr == &_tFName0)
    	_tFName0.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName1)
    	_tFName1.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName2)
    	_tFName2.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName3)
    	_tFName3.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName4)
    	_tFName4.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName5)
    	_tFName5.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName6)
    	_tFName6.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));

    if (strlen(NextionHMI::buffer)>50)
	{
    	StateMessage::ActivatePGM_M(MESSAGE_WARNING, NEX_ICON_WARNING, MSG_WARNING, MSG_LONG_FILENAME, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
    	return;
	}

    if (card.isFileOpen()) card.closeFile();
    card.selectFile(NextionHMI::buffer);
    StateFileinfo::Activate();



}

void StateFiles::FFolder_Push(void* ptr) {
    ZERO(NextionHMI::buffer);
//	  serial_print(ptr);
//	  serial_print("\n+++\n");
    if (ptr == &_tFName0)
    	_tFName0.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName1)
    	_tFName1.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName2)
    	_tFName2.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName3)
    	_tFName3.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName4)
    	_tFName4.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName5)
    	_tFName5.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    else if (ptr == &_tFName6)
    	_tFName6.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));

    if (strlen(NextionHMI::buffer)>50)
	{
    	StateMessage::ActivatePGM_M(MESSAGE_WARNING, NEX_ICON_WARNING, MSG_WARNING, MSG_LONG_FILENAME, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
    	return;
	}

	_tLoading.SetVisibility(true);
    card.chdir(NextionHMI::buffer);
    _listPosition = 0;
    Files_PopulateFileList(_listPosition);
	_tLoading.SetVisibility(false);
}

void StateFiles::FFolderUp_Push(void* ptr) {
	card.updir();
	_listPosition = 0;
	_tLoading.SetVisibility(true);
    Files_PopulateFileList(_listPosition);
	_tLoading.SetVisibility(false);
}

void StateFiles::FilesCancel_Push(void* ptr) {
	StateStatus::Activate();
}

void StateFiles::Init() {
	_bFilesCancel.attachPush(FilesCancel_Push);
	_bFUp.attachPush(FUp_Push);
	_bFDown.attachPush(FDown_Push);
}

void StateFiles::Activate() {
    _listPosition = 0;
    _insideDir = false;
    //StateMessage::ActivatePGM(0, NEX_ICON_FILES, PSTR(MSG_READING_SD), PSTR(MSG_READING_SD), 0, "", 0, "", 0, 0);
    NextionHMI::ActivateState(PAGE_FILES);
    _page.show();
    card.mount();
    if (card.cardOK)
    {
	  Files_PopulateFileList(_listPosition);
	  _tLoading.SetVisibility(false);
    }
    else
    {
        StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_WARNING, PSTR(MSG_ERROR), PSTR(MSG_NO_SD), 1, PSTR(MSG_BACK), FilesCancel_Push, "", 0, 0);

      //if (!card.cardOK)
    	  //StateStatus::Activate();
      //else
    	//  Files_PopulateFileList(_listPosition);
    }
	//NextionHMI::headerText.setTextPGM(PSTR(WELCOME_MSG));
}

void StateFiles::TouchUpdate() {
	nexLoop(_listenList);
}

#endif
