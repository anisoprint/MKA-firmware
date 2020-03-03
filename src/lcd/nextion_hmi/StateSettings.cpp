/*
 * StateFiles.cpp
 *
 *  Created on: 7 θών. 2018 γ.
 *      Author: Azarov
 */

#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateSettings.h"

//Settings Array
StateSettings::SettingDefinition StateSettings::SettingsList[] = {
			{MSG_COMP_EXTRUDER, StateSettings::SettingType::HEADER, nullptr, 0, 0},
			{" " MSG_X_OFFSET, StateSettings::SettingType::FLOAT, &tools.hotend_offset[X_AXIS][1], -50, 50},
			{" " MSG_Y_OFFSET, StateSettings::SettingType::FLOAT, &tools.hotend_offset[Y_AXIS][1], -50, 50},
			{" " MSG_Z_OFFSET, StateSettings::SettingType::FLOAT, &tools.hotend_offset[Z_AXIS][1], -10, 10},

			{MSG_FEEDERS_STEPS, StateSettings::SettingType::HEADER, nullptr, 0, 0},
			{" " MSG_PLASTIC, StateSettings::SettingType::FLOAT, &Mechanics::axis_steps_per_mm[E_AXIS], 1, 2000},
			{" " MSG_COMP_FIBER, StateSettings::SettingType::FLOAT, &Mechanics::axis_steps_per_mm[U_AXIS], 1, 2000},
			{" " MSG_COMP_PLASTIC, StateSettings::SettingType::FLOAT, &Mechanics::axis_steps_per_mm[V_AXIS], 1, 2000},

			{MSG_PID_COEFFICIENTS, StateSettings::SettingType::HEADER, nullptr, 0, 0},
			{" " MSG_PLASTIC_P, StateSettings::SettingType::FLOAT, &heaters[HOT0_INDEX].Kp, 0, 1000},
			{" " MSG_PLASTIC_I, StateSettings::SettingType::FLOAT, &heaters[HOT0_INDEX].Ki, 0, 1000},
			{" " MSG_PLASTIC_D, StateSettings::SettingType::FLOAT, &heaters[HOT0_INDEX].Kd, 0, 1000},
			{" " MSG_COMPOSITE_P, StateSettings::SettingType::FLOAT, &heaters[HOT1_INDEX].Kp, 0, 1000},
			{" " MSG_COMPOSITE_I, StateSettings::SettingType::FLOAT, &heaters[HOT1_INDEX].Ki, 0, 1000},
			{" " MSG_COMPOSITE_D, StateSettings::SettingType::FLOAT, &heaters[HOT1_INDEX].Kd, 0, 1000},
			{" " MSG_BUILDPLATE_P, StateSettings::SettingType::FLOAT, &heaters[BED_INDEX].Kp, 0, 1000},
			{" " MSG_BUILDPLATE_I, StateSettings::SettingType::FLOAT, &heaters[BED_INDEX].Ki, 0, 1000},
			{" " MSG_BUILDPLATE_D, StateSettings::SettingType::FLOAT, &heaters[BED_INDEX].Kd, 0, 1000},

	};


namespace {

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



	StateSettings::SettingDefinition* _settings;
	uint32_t _settingsLength = 0;

	uint32_t _listPosition = 0;
	uint32_t _editedSetting = 0;

	void PopulateRow(uint8_t row, StateSettings::SettingDefinition setting) {

		_rRows[row].setTextPGM(PSTR(setting.caption), 0);

		switch(setting.type)
		{
			case StateSettings::SettingType::HEADER:
				_rRows[row].Set_font_color_pco(40179);
				_rValues[row].setText("");
				break;
			case StateSettings::SettingType::FLOAT:
			{
				float* float_val = ((float*)setting.setting);
				_rValues[row].setText(String(*float_val).c_str());
				_rRows[row].Set_font_color_pco(0);
				break;
			}
			case StateSettings::SettingType::INT16:
			{
				int16_t* int16_val = (int16_t*)setting.setting;
				_rValues[row].setText(String(*int16_val).c_str());
				_rRows[row].Set_font_color_pco(0);
				break;
			}
			case StateSettings::SettingType::INT8:
			{
				int8_t* int8_val = (int8_t*)setting.setting;
				_rValues[row].setText(String(*int8_val).c_str());
				_rRows[row].Set_font_color_pco(0);
				break;
			}

		}

	}

	void PopulateSettingsList(uint32_t number) {

	    uint32_t i = number;

	    uint8_t start_row = 0;

	    for (uint8_t row = start_row; row < 7; row++) {
	      if (i < _settingsLength) {
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

	    if ((number + 7) < _settingsLength)
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

    if ((_listPosition + 7) <_settingsLength)
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
			{
				float* float_val = (float*)_settings[setting_index].setting;
				_editedSetting = setting_index;
				StateEditNumber::Activate(String(*float_val).c_str(), EditNumber_OK_Push, EditNumber_Cancel_Push);
				break;
			}
			case StateSettings::SettingType::INT16:
			{
				int16_t* int16_val = (int16_t*)_settings[setting_index].setting;
				_editedSetting = setting_index;
				StateEditNumber::Activate(String(*int16_val).c_str(), EditNumber_OK_Push, EditNumber_Cancel_Push);
				break;
			}
			case StateSettings::SettingType::INT8:
			{
				int8_t* int8_val = (int8_t*)_settings[setting_index].setting;
				_editedSetting = setting_index;
				StateEditNumber::Activate(String(*int8_val).c_str(), EditNumber_OK_Push, EditNumber_Cancel_Push);
				break;
			}
		}
    }
}

void StateSettings::Init() {
	_bFUp.attachPush(FUp_Push);
	_bFDown.attachPush(FDown_Push);

	for (int i=0; i<7; i++)
	{
        _rRows[i].attachPush(FRow_Push, &_rRows[i]);
        _rValues[i].attachPush(FRow_Push, &_rValues[i]);
	}
}


void StateSettings::Activate(StateSettings::SettingDefinition* settings, uint32_t settingsLength, NexTouchEventCb cbOK) {
	_settingsLength = settingsLength;
	_settings = settings;
    _listPosition = 0;
    NextionHMI::ActivateState(PAGE_SETTINGS);
    _page.show();
    PopulateSettingsList(_listPosition);
	_bOK.attachPush(cbOK);
}

void StateSettings::Activate(uint32_t setting_index) {
    _listPosition = (setting_index / 7) * 7;
    NextionHMI::ActivateState(PAGE_SETTINGS);
    _page.show();
    PopulateSettingsList(_listPosition);
}

void StateSettings::TouchUpdate() {
	nexLoop(_listenList);
}



void StateSettings::EditNumber_OK_Push(void* ptr) {

	StateEditNumber::GetInputToBuffer();
	switch(_settings[_editedSetting].type)
	{
		case StateSettings::SettingType::HEADER:
			break;
		case StateSettings::SettingType::FLOAT:
		{
			float* float_val = (float*)_settings[_editedSetting].setting;
			float val = atof(NextionHMI::buffer);
			if (val >= _settings[_editedSetting].minValue && val <= _settings[_editedSetting].maxValue)
			{
				*float_val = val;
				StateSettings::Activate(_editedSetting);
				eeprom.Store_Settings();
			}
			else
			{
				sprintf_P(NextionHMI::buffer, PSTR(MSG_INPUT_BOUNDS), _settings[_editedSetting].minValue, _settings[_editedSetting].maxValue);
				StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_WARNING, PSTR(MSG_INCORRECT_INPUT), NextionHMI::buffer, 1, MSG_OK, EditNumber_Cancel_Push, 0, 0, 0);
			}
			break;
		}
		case StateSettings::SettingType::INT16:
		{
			int16_t* int16_val = (int16_t*)_settings[_editedSetting].setting;
			int16_t val = atoi(NextionHMI::buffer);
			if (val >= _settings[_editedSetting].minValue && val <= _settings[_editedSetting].maxValue)
			{
				*int16_val = val;
				StateSettings::Activate(_editedSetting);
				eeprom.Store_Settings();
			}
			else
			{
				sprintf_P(NextionHMI::buffer, PSTR(MSG_INPUT_BOUNDS), _settings[_editedSetting].minValue, _settings[_editedSetting].maxValue);
				StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_WARNING, PSTR(MSG_INCORRECT_INPUT), NextionHMI::buffer, 1, MSG_OK, EditNumber_Cancel_Push, 0, 0, 0);
			}
			break;
		}
		case StateSettings::SettingType::INT8:
		{
			int8_t* int8_val = (int8_t*)_settings[_editedSetting].setting;
			int8_t val = atoi(NextionHMI::buffer);
			if (val >= _settings[_editedSetting].minValue && val <= _settings[_editedSetting].maxValue)
			{
				*int8_val = val;
				eeprom.Store_Settings();
				StateSettings::Activate(_editedSetting);
			}
			else
			{
				sprintf_P(NextionHMI::buffer, PSTR(MSG_INPUT_BOUNDS), _settings[_editedSetting].minValue, _settings[_editedSetting].maxValue);
				StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_WARNING, PSTR(MSG_INCORRECT_INPUT), NextionHMI::buffer, 1, MSG_OK, EditNumber_Cancel_Push, 0, 0, 0);
			}
			break;
		}
	}


}

void StateSettings::EditNumber_Cancel_Push(void* ptr) {
	StateSettings::Activate(_editedSetting);
}


#endif
