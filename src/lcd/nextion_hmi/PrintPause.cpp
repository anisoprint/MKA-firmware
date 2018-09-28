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
	static uint8_t resume_tool;
}

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
   * - Abort if TARGET temperature is too low
   * - Display "wait for start of filament change" (if a length was specified)
   * - Initial retract, if current temperature is hot enough
   * - Park the nozzle at the given position
   * - Call unload_filament (if a length was specified)
   *
   * Returns 'true' if pause was completed, 'false' for abort
   */
  uint8_t did_pause_print = 0;

  bool PrintPause::PausePrint(const float &retract, const point_t &park_point) {

    if (did_pause_print) return false; // already paused

    SERIAL_STR(PAUSE);
    SERIAL_EOL();

    NextionHMI::RaiseEvent(HMIevent::PRINT_PAUSED);

    // Indicate that the printer is paused
    ++did_pause_print;

    // Pause the print job and timer
    #if HAS_SDSUPPORT
      if (card.sdprinting) {
        card.pauseSDPrint();
        ++did_pause_print;
      }
    #endif
    print_job_counter.pause();

    // Wait for synchronize steppers
    stepper.synchronize();

    // Save current position
    COPY_ARRAY(resume_position, mechanics.current_position);
    resume_tool = tools.active_extruder;

    // Initial retract before move to park position
    if (retract && !thermalManager.tooColdToExtrude(tools.active_extruder))
    {
    	//get plastic driver of current extruder
    	int8_t drv = Tools::plastic_driver_of_extruder(tools.active_extruder);
    	if (drv>=0) PrintPause::DoPauseExtruderMove((AxisEnum)(E_AXIS+drv), retract, PAUSE_PARK_RETRACT_FEEDRATE);
    }

    // Park the nozzle by moving up by z_lift and then moving to (x_pos, y_pos)
    Nozzle::park(2, park_point);

    return true;
}


  /**
    * Resume or Start print procedure
    *
    * - Abort if not paused
    * - Reset heater idle timers
    * - Load filament if specified, but only if:
    *   - a nozzle timed out, or
    *   - the nozzle is already heated.
    * - Display "wait for print to resume"
    * - Re-prime the nozzle...
    *   -  FWRETRACT: Recover/prime from the prior G10.
    *   - !FWRETRACT: Retract by resume_position[E], if negative.
    *                 Not sure how this logic comes into use.
    * - Move the nozzle back to resume_position
    * - Sync the planner E to resume_position[E]
    * - Send host action for resume, if configured
    * - Resume the current SD print job, if any
    */

void PrintPause::ResumePrint(const float& load_length,
		const float& purge_length, const int8_t max_beep_count) {

   if (!did_pause_print) return;

   // Re-enable the heaters if they timed out
   bool  nozzle_timed_out  = false,
         bed_timed_out     = false;

   #if HAS_TEMP_BED && PAUSE_PARK_PRINTER_OFF > 0
     bed_timed_out |= heaters[BED_INDEX].isIdle();
     heaters[BED_INDEX].reset_idle_timer();
   #endif

   LOOP_HOTEND() {
     nozzle_timed_out |= heaters[h].isIdle();
     heaters[h].reset_idle_timer();
   }

   if (nozzle_timed_out || thermalManager.hotEnoughToExtrude(TARGET_EXTRUDER)) {
     // Load the new filament
     load_filament(load_length, purge_length, max_beep_count, true, nozzle_timed_out);
   }

   #if HAS_LCD
     // "Wait for print to resume"
     lcd_advanced_pause_show_message(ADVANCED_PAUSE_MESSAGE_RESUME);
   #endif

   // Intelligent resuming
   #if ENABLED(FWRETRACT)
     // If retracted before goto pause
     if (fwretract.retracted[tools.active_extruder])
       do_pause_e_move(-fwretract.retract_length, fwretract.retract_feedrate_mm_s);
   #endif
   // If resume_position is negative
   if (resume_position[E_AXIS] < 0) do_pause_e_move(resume_position[E_AXIS], PAUSE_PARK_RETRACT_FEEDRATE);

   // Move XY to starting position, then Z
   mechanics.do_blocking_move_to_xy(resume_position[X_AXIS], resume_position[Y_AXIS], NOZZLE_PARK_XY_FEEDRATE);

   // Set Z_AXIS to saved position
   mechanics.do_blocking_move_to_z(resume_position[Z_AXIS], NOZZLE_PARK_Z_FEEDRATE);

   // Now all extrusion positions are resumed and ready to be confirmed
   // Set extruder to saved position
   planner.set_e_position_mm(mechanics.destination[E_AXIS] = mechanics.current_position[E_AXIS] = resume_position[E_AXIS]);

   printer.setFilamentOut(false);

   #if HAS_LCD
     // Show status screen
     lcd_advanced_pause_show_message(ADVANCED_PAUSE_MESSAGE_STATUS);
   #endif

   #if ENABLED(NEXTION) && ENABLED(NEXTION_GFX)
     mechanics.Nextion_gfx_clear();
   #endif

   SERIAL_STR(RESUME);
   SERIAL_EOL();

   --did_pause_print;

   #if HAS_SDSUPPORT
     if (did_pause_print) {
       card.startFileprint();
       --did_pause_print;
     }
   #endif

 }

#endif
