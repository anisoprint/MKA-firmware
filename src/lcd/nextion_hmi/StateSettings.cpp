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

	//Settings Array
	StateSettings::SettingDefinition _settings[] = {
			{MSG_COMP_EXTRUDER, StateSettings::SettingType::HEADER, nullptr, 0, 0},
			{MSG_X_OFFSET, StateSettings::SettingType::FLOAT, &tools.hotend_offset[X_AXIS][1], -50, 50},
			{MSG_Y_OFFSET, StateSettings::SettingType::FLOAT, &tools.hotend_offset[Y_AXIS][1], -50, 50},
			{MSG_Z_OFFSET, StateSettings::SettingType::FLOAT, &tools.hotend_offset[Z_AXIS][1], -10, 10},
	};

	///////////// Nextion components //////////
	//Page
	NexObject _page  = NexObject(PAGE_SETTINGS,  0,  "settings");

	//Buttons
	NexObject _rRows[] = {
			NexObject(PAGE_SETTINGS,  18,  "r0"),
			NexObject(PAGE_SETTINGS,  19,  "r1"),
			NexObject(PAGE_SETTINGS,  20,  "r2"),
			NexObject(PAGE_SETTINGS,  21,  "r3"),
			NexObject(PAGE_SETTINGS,  22,  "r4"),
			NexObject(PAGE_SETTINGS,  23,  "r5"),
			NexObject(PAGE_SETTINGS,  24,  "r6")
	};
	NexObject _rValues[] = {
			NexObject(PAGE_SETTINGS,  27,  "v0"),
			NexObject(PAGE_SETTINGS,  28,  "v1"),
			NexObject(PAGE_SETTINGS,  29,  "v2"),
			NexObject(PAGE_SETTINGS,  30,  "v3"),
			NexObject(PAGE_SETTINGS,  31,  "v4"),
			NexObject(PAGE_SETTINGS,  32,  "v5"),
			NexObject(PAGE_SETTINGS,  33,  "v6")
	};

	NexObject _bFUp = NexObject(PAGE_SETTINGS,  25,  "up");
	NexObject _bFDown = NexObject(PAGE_SETTINGS,  26,  "dn");

	NexObject _bFUpIcon = NexObject(PAGE_SETTINGS,  14,  "pUP");
	NexObject _bFDownIcon = NexObject(PAGE_SETTINGS,  15,  "pDN");

	NexObject _bOK = NexObject(PAGE_SETTINGS, 9, "ok");

	NexObject *_listenList[] = { &_rRows[0],&_rRows[1],&_rRows[2],&_rRows[3],&_rRows[4],&_rRows[5],&_rRows[6],
								 &_rValues[0],&_rValues[1],&_rValues[2],&_rValues[3],&_rValues[4],&_rValues[5],&_rValues[6],
								 &_bFUp, &_bFDown, &_bOK, NULL };


	//Page_number
	NexObject _pageNumber  = NexObject(PAGE_C_NUMBER,  0,  "c_num");
	NexObject _pageNumberInput = NexObject(PAGE_C_NUMBER,  0,  "c_num.input");
	NexObject _pageNumberSetting = NexObject(PAGE_C_NUMBER,  0,  "c_num.setid");


	uint32_t _listPosition = 0;
	bool _insideDir = false;

	void PopulateRow(uint8_t row, StateSettings::SettingDefinition setting) {

		_rRows[row].setTextPGM(PSTR(setting.caption), 0);

		switch(setting.type)
		{
			case StateSettings::SettingType::HEADER:
				_rRows[row].Set_font_color_pco(1048);
				break;
			case StateSettings::SettingType::FLOAT:
				float* float_val = setting.setting;
				_rValues[row].setText(String(*float_val).c_str());
				_rRows[row].Set_font_color_pco(0);
				break;
			case StateSettings::SettingType::INT:
				int* int_val = setting.setting;
				_rValues[row].setText(String(*int_val).c_str());
				_rRows[row].Set_font_color_pco(0);
				break;

		}

	}

	void PopulateSettingsList(uint32_t number) {

	    uint16_t set_count = sizeof(_settings)/sizeof(_settings[0]);
	    uint32_t i = number;

	    uint8_t start_row = 0;

	    for (uint8_t row = start_row; row < 7; row++) {
	      if (i < set_count) {
	    	PopulateRow(row, _settings[i]);
	      }
	      else
	      {
	  		_rRows[row].setText("", 0);
	  		_rValues[row].setText("", 0);
	      }
	      i++;
	    }

	    if (number>0)
	    	_bFUpIcon.setPic(NEX_ICON_FILES_UP);
	    else
	    	_bFUpIcon.setPic(NEX_ICON_FILES_UP_GREY);

	    if ((number + 7) < set_count)
	    	_bFDownIcon.setPic(NEX_ICON_FILES_DOWN);
	    else
	    	_bFDownIcon.setPic(NEX_ICON_FILES_DOWN_GREY);
	}

}

void StateSettings::FUp_Push(void* ptr) {
	if (_listPosition>0)
	{
		_listPosition -= 7;
		if (_listPosition<0) _listPosition=0;
		PopulateSettingsList(_listPosition);
	}
}

void StateSettings::FDown_Push(void* ptr) {
    uint16_t fileCnt = card.getnrfilenames();
    if ((_listPosition + 7) <fileCnt)
    {
    	_listPosition += 7;
    	PopulateSettingsList(_listPosition);
    }
}

void StateSettings::FRow_Push(void* ptr) {

    ZERO(NextionHMI::buffer);
    int8_t row_index = -1;
    for (uint8_t row = 0; row<7; row++)
    {
        if (ptr == &_rRows[row] || ptr == &_rValues[row])
        {
        	row_index = row;
        	break;
        }
    }

    if (row_index>=0)
    {
    	uint8_t setting_index =_listPosition + row_index;
		switch(_settings[setting_index].type)
		{
			case StateSettings::SettingType::HEADER:
				break;
			case StateSettings::SettingType::FLOAT:
				float* float_val = _settings[setting_index].setting;
				_pageNumberInput.setText(String(*float_val).c_str());
				_pageNumberSetting.setValue(setting_index);
				break;
			case StateSettings::SettingType::INT:
				int* int_val = _settings[setting_index].setting;
				_pageNumberInput.setText(String(*int_val).c_str());
				_pageNumberSetting.setValue(setting_index);
				break;

		}

    }

    loadpageid

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
