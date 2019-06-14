/*
 * PrintPause.cpp
 *
 *  Created on: 28 ????. 2018 ?.
 *      Author: Azarov
 */
#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

  // private:
namespace {
	static float resume_position[XYZE];
	static int16_t resume_temperatures[HOTENDS];
	static uint8_t resume_tool;
}


float PrintPause::LoadDistance[DRIVER_EXTRUDERS] = { 0.0 };
float PrintPause::UnloadDistance[DRIVER_EXTRUDERS] = { 0.0 };;

float PrintPause::RetractDistance = 0,
	  PrintPause::RetractFeedrate = 0,
      PrintPause::LoadFeedrate = 0,
	  PrintPause::UnloadFeedrate = 0,
	  PrintPause::ExtrudeFeedrate = 0;

bool PrintPause::CanPauseNow = true;
bool PrintPause::SdPrintingPaused = false;
PrintPauseStatus PrintPause::Status = NotPaused;

  // public function

void PrintPause::DoPauseExtruderMove(AxisEnum axis, const float &length, const float fr) {
  mechanics.set_destination_to_current();
  mechanics.destination[axis] += length;
  planner.buffer_line_kinematic(mechanics.destination, fr, tools.active_extruder);
  stepper.synchronize();
  mechanics.set_current_to_destination();
}

  /**
   * Pause procedure
   *
   * - Abort if already paused
   * - Send host action for pause, if configured
   * - Initial retract, if current temperature is hot enough
   * - Park the nozzle at the given position
   *
   * Returns 'true' if pause was completed, 'false' for abort
   */

  bool PrintPause::PausePrint(const float &retract) {

    if (Status!=NotPaused && Status!=WaitingToPause) return false; // already paused

    //Printing with fiber, can't pause now
    if (CanPauseNow == false)
    {
    	Status = WaitingToPause;
    	NextionHMI::RaiseEvent(PRINT_PAUSE_SCHEDULED);
    	return false;
    }

    SERIAL_STR(PAUSE);
    SERIAL_EOL();

    // Pause the print job and timer
    #if HAS_SDSUPPORT
      if (IS_SD_PRINTING) {
        card.pauseSDPrint();
        SdPrintingPaused = true;
      }
    #endif
    Status = Pausing;
    NextionHMI::RaiseEvent(PRINT_PAUSING);

    // Wait for synchronize steppers
    while ((planner.has_blocks_queued() || stepper.cleaning_buffer_counter) && !printer.isAbortSDprinting()) {
      printer.idle();
      printer.keepalive(InProcess);
    }
    //memset(planner.block_buffer, 0, sizeof(block_t)*BLOCK_BUFFER_SIZE);

    // Handle cancel
    if (printer.isAbortSDprinting()) return false;

    // Save current position
    COPY_ARRAY(resume_position, mechanics.current_position);
    resume_tool = tools.active_extruder;

    //Save current temperatures
    LOOP_HOTEND()
	{
    	resume_temperatures[h] = heaters[h].target_temperature;
	}


    // Initial retract before move to park position
    if (retract && !thermalManager.tooColdToExtrude(tools.active_extruder))
    {
    	//get plastic driver of current extruder
    	int8_t drv = Tools::plastic_driver_of_extruder(tools.active_extruder);
    	if (drv>=0) PrintPause::DoPauseExtruderMove((AxisEnum)(E_AXIS+drv), -retract, PrintPause::UnloadFeedrate);
    }

    // Park the nozzle by moving up by z_lift and then moving to (x_pos, y_pos)
    const point_t park_point = NOZZLE_PARK_POINT;
    Nozzle::park(2, park_point);

    // Handle cancel
    if (printer.isAbortSDprinting()) return false;

    // Start the heater idle timers
    const millis_t nozzle_timeout = (millis_t)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
    const millis_t bed_timeout    = (millis_t)(PAUSE_PARK_PRINTER_OFF) * 60000UL;

    LOOP_HOTEND()
      heaters[h].start_idle_timer(nozzle_timeout);

    #if HAS_TEMP_BED && PAUSE_PARK_PRINTER_OFF > 0
      heaters[BED_INDEX].start_idle_timer(bed_timeout);
    #endif

    stepper.synchronize();
    // Indicate that the printer is paused
    Status = Paused;
    NextionHMI::RaiseEvent(PRINT_PAUSED);
    print_job_counter.pause();

    printer.keepalive(PausedforUser);
    printer.setWaitForUser(true);
    while (printer.isWaitForUser() && Status==Paused) {
    	printer.idle(true);
    }

    printer.keepalive(InHandler);

    return true;
}

  /**
    * Resume or Start print procedure
    */

void PrintPause::ResumePrint(const float& purge_length) {

   if (Status==WaitingToPause)
   {
	   Status = NotPaused;
	   NextionHMI::RaiseEvent(PRINT_PAUSE_UNSCHEDULED);
	   return;
   };
   if (Status!=Paused) return; // already not paused

   Status = Resuming;
   NextionHMI::RaiseEvent(PRINT_PAUSE_RESUMING);

   stepper.synchronize();

   //Switching to previously active extruder
	if (resume_tool!=tools.active_extruder) tools.change(resume_tool, 0, false, false, true);

   // Re-enable the heaters if they timed out
   bool  nozzle_timed_out  = false,
         bed_timed_out     = false;

   RestoreTemperatures();

   #if HAS_TEMP_BED && PAUSE_PARK_PRINTER_OFF > 0
     bed_timed_out |= heaters[BED_INDEX].isIdle();
     heaters[BED_INDEX].reset_idle_timer();
   #endif

   LOOP_HOTEND() {
     nozzle_timed_out |= heaters[h].isIdle();
     heaters[h].reset_idle_timer();
   }

   if (bed_timed_out && heaters[BED_INDEX].target_temperature>30)
   {
	  NextionHMI::RaiseEvent(HMIevent::HEATING_STARTED_BUILDPLATE, BED_INDEX);
	  Temperature::wait_heater(&heaters[BED_INDEX], false);
	  NextionHMI::RaiseEvent(HMIevent::HEATING_FINISHED);
   }

   if (nozzle_timed_out)
   {
	   LOOP_HOTEND() {
		   if (heaters[h].target_temperature>30)
		   {
			   NextionHMI::RaiseEvent(HMIevent::HEATING_STARTED_EXTRUDER, h);
			   Temperature::wait_heater(&heaters[h], false);
			   NextionHMI::RaiseEvent(HMIevent::HEATING_FINISHED);
		   }
	   }
   }

   printer.setWaitForHeatUp(false);

   //memset(planner.block_buffer, 0, sizeof(block_t)*BLOCK_BUFFER_SIZE);

   // Move XY to starting position, then Z
   mechanics.do_blocking_move_to_xy(resume_position[X_AXIS], resume_position[Y_AXIS], NOZZLE_PARK_XY_FEEDRATE);

   // Set Z_AXIS to saved position
   mechanics.do_blocking_move_to_z(resume_position[Z_AXIS], NOZZLE_PARK_Z_FEEDRATE);

   //memset(planner.block_buffer, 0, sizeof(block_t)*BLOCK_BUFFER_SIZE);

   // Purging plastic
   if (purge_length && !thermalManager.tooColdToExtrude(tools.active_extruder))
   {
   	//get plastic driver of current extruder
   	int8_t drv = Tools::plastic_driver_of_extruder(tools.active_extruder);
   	if (drv>=0) PrintPause::DoPauseExtruderMove((AxisEnum)(E_AXIS+drv), purge_length, PrintPause::LoadFeedrate);
   }

   //memset(planner.block_buffer, 0, sizeof(block_t)*BLOCK_BUFFER_SIZE);

   // Now all positions are resumed and ready to be confirmed
   // Set all to saved position
   COPY_ARRAY(mechanics.current_position, resume_position);
   COPY_ARRAY(mechanics.destination, resume_position);

   printer.setFilamentOut(false);

   SERIAL_STR(RESUME);
   SERIAL_EOL();

   #if HAS_SDSUPPORT
     if (SdPrintingPaused) {
       card.startFileprint();
       SdPrintingPaused=false;
     }
   #endif

   Status = NotPaused;
   printer.setWaitForUser(false);
   NextionHMI::RaiseEvent(PRINT_PAUSE_RESUMED);
   print_job_counter.start();

}

void PrintPause::RestoreTemperatures() {
	if (Status!=Paused && Status!=Resuming) return;

    //Restore current temperatures
    LOOP_HOTEND()
	{
/*		Serial.print("N ");
		Serial.print(h);
		Serial.print(" CUR:");
		Serial.println(heaters[h].target_temperature);
		Serial.print(" RESUME:");
		Serial.println(resume_temperatures[h]);*/

    	if (resume_temperatures[h] != heaters[h].target_temperature)
    		{
/*    			Serial.print("Restore ");
    			Serial.print(h);
    			Serial.print(" to ");
    			Serial.println(resume_temperatures[h]);*/
    			heaters[h].setTarget(resume_temperatures[h]);

    		}
	}
}

#endif
