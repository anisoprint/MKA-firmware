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
	static int16_t resume_temperatures[HEATER_COUNT];
	static uint8_t resume_tool;
	static uint8_t resume_fan_speed[FAN_COUNT];

}

float PrintPause::LoadDistance[DRIVER_EXTRUDERS] = { 0.0 };
float PrintPause::UnloadDistance[DRIVER_EXTRUDERS] = { 0.0 };;

float PrintPause::RetractDistance = 0,
	  PrintPause::RetractFeedrate = 0,
      PrintPause::LoadFeedrate = 0,
	  PrintPause::UnloadFeedrate = 0,
	  PrintPause::ExtrudeFeedrate = 0;

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
 * - Pause SD printing
 * - Queue nozzle parking
 *
 * Returns 'true' if pause was completed, 'false' for abort
 */

bool PrintPause::PausePrint() {

	if (Status!=NotPaused && Status!=WaitingToPause) return false; // already paused

    //Printing with fiber, can't pause now
    if (tools.printing_with_fiber)
    {
    	Status = WaitingToPause;
    	NextionHMI::RaiseEvent(PRINT_PAUSE_SCHEDULED);
    	return false;
    }
    else
    {
        SERIAL_STR(PAUSE);
        SERIAL_EOL();

        // Pause the print job
        #if HAS_SDSUPPORT
          if (IS_SD_PRINTING) {
            card.pauseSDPrint();
            SdPrintingPaused = true;
          }
        #endif

        print_job_counter.pause();

        commands.inject_rear_P(PSTR("M125")); // Must be enqueued with pauseSDPrint set to be last in the buffer

        Status = Pausing;
        NextionHMI::RaiseEvent(PRINT_PAUSING);


        return true;
    }
}


  /**
   * Park procedure
   *
   * - Abort if not pausing
   * - Initial retract, if current temperature is hot enough
   * - Park the nozzle at the given position
   *
   * Returns 'true' if pause was completed, 'false' for abort
   */


  bool PrintPause::ParkHead(const float &retract) {

    if (Status!=Pausing) return false; // incorrect state

    // Wait for synchronize steppers
    while ((planner.has_blocks_queued() || stepper.cleaning_buffer_counter) && !printer.isAbortSDprinting()) {
      printer.idle();
      printer.keepalive(InProcess);
    }

    // Handle cancel
    if (printer.isAbortSDprinting()) return false;

    // Save current position
    COPY_ARRAY(resume_position, mechanics.current_position);
    resume_tool = tools.active_extruder;

    //Save current temperatures
    LOOP_HEATER()
	{
    	resume_temperatures[h] = heaters[h].target_temp_nocorr;
	}

    //Save current fan speed
    LOOP_FAN()
    {
    	resume_fan_speed[f] = fans[f].speed_nocorr;
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
    const millis_l nozzle_timeout = (millis_l)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
    const millis_l bed_timeout    = (millis_l)(PAUSE_PARK_PRINTER_OFF) * 60000UL;

    LOOP_HOTEND()
      heaters[h].start_idle_timer(nozzle_timeout);

    #if HAS_TEMP_BED && PAUSE_PARK_PRINTER_OFF > 0
      heaters[BED_INDEX].start_idle_timer(bed_timeout);
    #endif

    stepper.synchronize();
    // Indicate that the printer is paused
    Status = Paused;
    NextionHMI::RaiseEvent(PRINT_PAUSED);

//    printer.keepalive(PausedforUser);
//    printer.setWaitForUser(true);
//    while (printer.isWaitForUser() && Status==Paused) {
//    	printer.idle(true);
//    }
//
//    printer.keepalive(InHandler);

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

   //Check if heaters are timed out or temperature restoration is needed
   #if HAS_TEMP_BED
     heaters[BED_INDEX].reset_idle_timer();
   #endif

   LOOP_HOTEND() {
     heaters[h].reset_idle_timer();
   }

   RestoreTemperatures();

   if (heaters[BED_INDEX].wait_for_heating() && heaters[BED_INDEX].target_temperature>30)
   {
	   NextionHMI::RaiseEvent(HMIevent::HEATING_STARTED_BUILDPLATE, BED_INDEX);
	   Temperature::wait_heater(&heaters[BED_INDEX], false);
	   NextionHMI::RaiseEvent(HMIevent::HEATING_FINISHED);
   }

   LOOP_HOTEND()
   {
	   if (heaters[h].wait_for_heating() && heaters[h].target_temperature>30)
	   {
		   NextionHMI::RaiseEvent(HMIevent::HEATING_STARTED_EXTRUDER, h);
		   Temperature::wait_heater(&heaters[h], false);
		   NextionHMI::RaiseEvent(HMIevent::HEATING_FINISHED);
		}
   }

   // Restore fan speed
   LOOP_FAN()
   {
   	 if (fans[f].autoMonitored==-1) fans[f].setSpeed(resume_fan_speed[f]);
   }

   printer.setWaitForHeatUp(false);

   // Move XY to starting position, then Z
   mechanics.do_blocking_move_to_xy(resume_position[X_AXIS], resume_position[Y_AXIS], NOZZLE_PARK_XY_FEEDRATE);

   // Set Z_AXIS to saved position
   mechanics.do_blocking_move_to_z(resume_position[Z_AXIS], NOZZLE_PARK_Z_FEEDRATE);

   // Purging plastic
   if (purge_length && !thermalManager.tooColdToExtrude(tools.active_extruder))
   {
   	//get plastic driver of current extruder
   	int8_t drv = Tools::plastic_driver_of_extruder(tools.active_extruder);
   	if (drv>=0) PrintPause::DoPauseExtruderMove((AxisEnum)(E_AXIS+drv), purge_length, PrintPause::LoadFeedrate);
   }

   // Now all positions are resumed and ready to be confirmed
   // Set all to saved position
   COPY_ARRAY(mechanics.current_position, resume_position);
   COPY_ARRAY(mechanics.destination, resume_position);

   // Now all extrusion positions are resumed and ready to be confirmed
   // Set extruder to saved position
   planner.set_only_e_position_mm(mechanics.current_position);

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
	LOOP_HEATER()
	{
    	heaters[h].setTarget(resume_temperatures[h]);
	}

}


#endif
