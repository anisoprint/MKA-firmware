/*
 * StateStatus.cpp
 *
 *  Created on: 31 мая 2018 г.
 *      Author: Azarov
 */

#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateStatusc.h"

NexObject StateStatus::_page = NexObject(PAGE_STATUS,  0,  "status");
//Build plate status
NexObject StateStatus::_tBuildPlateTemperature = NexObject(PAGE_STATUS,  14,  "tBPtemp");;
NexObject StateStatus::_tBuildPlateColorStatus = NexObject(PAGE_STATUS,  11,  "tBPcolor");
NexObject StateStatus::_bBuildPlateTemperature = NexObject(PAGE_STATUS,  31,  "bBPtemp");
//Print head status
NexObject StateStatus::_tPrintHeadTemperature = NexObject(PAGE_STATUS,  9,  "tPHtemp");
NexObject StateStatus::_tPrintHeadColorStatus = NexObject(PAGE_STATUS,  5,  "tPHcolor");
NexObject StateStatus::_bPrintHeadTemperature = NexObject(PAGE_STATUS,  30,  "bPHtemp");


NexObject StateStatus::_bPrint = NexObject(PAGE_STATUS,  24,  "bPrint");
NexObject StateStatus::_bMaintenance = NexObject(PAGE_STATUS,  25,  "bMaintenance");

NexObject* StateStatus::_listenList[5] = {&_bPrintHeadTemperature, &_bBuildPlateTemperature, &_bPrint, &_bMaintenance, NULL};

void StateStatus::PrintHeadTemperature_Push(void* ptr) {
}

void StateStatus::BuildPlateTemperature_Push(void* ptr) {
}

void StateStatus::Print_Push(void* ptr) {
}

void StateStatus::Maintenance_Push(void* ptr) {
}

StateStatus::StateStatus() {

}

void StateStatus::Init() {
	_bPrintHeadTemperature.attachPush(PrintHeadTemperature_Push);
	_bBuildPlateTemperature.attachPush(BuildPlateTemperature_Push);
	_bPrint.attachPush(Print_Push);
	_bMaintenance.attachPush(Maintenance_Push);
}

void StateStatus::Activate() {
	nextionHMI.NextionActivateState(PAGE_STATUS);
	_page.show();
	//SERIAL_MSG("SHOW \n");
	DrawUpdate();
}

void StateStatus::DrawUpdate() {
}

void StateStatus::TouchUpdate() {
	nexLoop(_listenList);
}

#endif

