/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * printer.cpp
 *
 * Copyright (C) 2017 Alberto Cotronei @MagoKimbra
 */

#include "../../../MK4duo.h"

const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'
#if DRIVER_EXTRUDERS > 1
  , 'U'
#endif
#if DRIVER_EXTRUDERS > 2
  , 'V'
#endif
#if DRIVER_EXTRUDERS > 3
  , 'W'
#endif
#if DRIVER_EXTRUDERS > 4
  , 'K'
#endif
#if DRIVER_EXTRUDERS > 5
  , 'L'
#endif
};

Printer printer;

bool Printer::axis_relative_modes[] = AXIS_RELATIVE_MODES;

// Print status related
long    Printer::currentLayer  = 0,
        Printer::maxLayer      = -1;   // -1 = unknown

char    Printer::printName[64] = "";   // max. 63 chars + 0

uint8_t Printer::progress = 0;

// Inactivity shutdown
long_timer_t  Printer::max_inactivity_timer;
uint16_t      Printer::max_inactive_time  = 0;

#if ENABLED(HOST_KEEPALIVE_FEATURE)
  watch_t Printer::host_keepalive_watch(DEFAULT_KEEPALIVE_INTERVAL * 1000UL);
#endif

// Interrupt Event
MK4duoInterruptEvent Printer::interruptEvent = INTERRUPT_EVENT_NONE;

// Printer mode
PrinterMode Printer::mode =
  #if ENABLED(PLOTTER)
    PRINTER_MODE_PLOTTER
  #elif ENABLED(SOLDER)
    PRINTER_MODE_SOLDER;
  #elif ENABLED(PICK_AND_PLACE)
    PRINTER_MODE_PICKER;
  #elif ENABLED(CNCROUTER)
    PRINTER_MODE_CNC;
  #elif ENABLED(LASER)
    PRINTER_MODE_LASER;
  #else
    PRINTER_MODE_FFF;
  #endif

#if ENABLED(RFID_MODULE)
  uint32_t  Printer::Spool_ID[EXTRUDERS] = ARRAY_BY_EXTRUDERS(0);
  bool      Printer::RFID_ON = false,
            Printer::Spool_must_read[EXTRUDERS]  = ARRAY_BY_EXTRUDERS(false),
            Printer::Spool_must_write[EXTRUDERS] = ARRAY_BY_EXTRUDERS(false);
#endif

#if ENABLED(BARICUDA)
  int Printer::baricuda_valve_pressure  = 0,
      Printer::baricuda_e_to_p_pressure = 0;
#endif

#if ENABLED(IDLE_OOZING_PREVENT)
  millis_l  Printer::axis_last_activity   = 0;
  bool      Printer::IDLE_OOZING_enabled  = true,
            Printer::IDLE_OOZING_retracted[EXTRUDERS] = ARRAY_BY_EXTRUDERS(false);
#endif

#if HAS_CHDK
  watch_t Printer::chdk_watch(CHDK_DELAY);
  bool    Printer::chdkActive = false;
#endif

// Private

uint8_t   Printer::mk_debug_flag  = 0, // For debug
          Printer::mk_1_flag      = 0; // For Homed

uint16_t  Printer::mk_2_flag      = 0; // For various

int8_t   Printer::json_autoreport_mode = -1;
uint8_t   Printer::json_autoreport_interval = 2;

PrinterStatus Printer::status = Busy;

/**
 * Public Function
 */

/**
 * MK4duo entry-point: Set up before the program loop
 *  - Set up Hardware Board
 *  - Set up the kill pin, filament runout, power hold
 *  - Start the serial port
 *  - Print startup messages and diagnostics
 *  - Get EEPROM or default settings
 *  - Initialize managers for:
 *    • temperature
 *    • CNCROUTER
 *    • planner
 *    • watchdog
 *    • stepper
 *    • photo pin
 *    • laserbeam, laser and laser_raster
 *    • servos
 *    • LCD controller
 *    • Digipot I2C
 *    • Z probe sled
 *    • status LEDs
 */
void Printer::setup() {

  HAL::hwSetup();

  setup_pinout();

  #if HAS_POWER_SWITCH
    #if PS_DEFAULT_OFF
      powerManager.power_off();
    #else
      powerManager.power_on();
    #endif
  #endif

  #if HAS_STEPPER_RESET
    stepper.disableStepperDrivers();
  #endif

  // Init Serial for HOST
  Com::setBaudrate();

  SERIAL_L(START);

  // Init TMC stepper drivers CS or Serial
  #if ENABLED(HAVE_TMC2130)
    tmc_init_cs_pins();
  #endif
  #if ENABLED(HAVE_TMC2208)
    tmc2208_serial_begin();
  #endif

  #if MECH(MUVE3D) && ENABLED(PROJECTOR_PORT) && ENABLED(PROJECTOR_BAUDRATE)
    DLPSerial.begin(PROJECTOR_BAUDRATE);
  #endif

  // Check startup
  SERIAL_STR(INFO);
  HAL::showStartReason();

  // Init Watchdog
  watchdog.init();

  eeprom.Load_Const();
  SERIAL_LM(ECHO, CUSTOM_MACHINE_NAME);
  SERIAL_LMV(ECHO, "VER:", eeprom.printerVersion);
  SERIAL_LMV(ECHO, "SN:", eeprom.printerSN);
  SERIAL_LM(ECHO, BUILD_VERSION);
  #if ENABLED(STRING_DISTRIBUTION_DATE) && ENABLED(STRING_CONFIG_H_AUTHOR)
    SERIAL_LM(ECHO, MSG_CONFIGURATION_VER STRING_DISTRIBUTION_DATE MSG_AUTHOR STRING_CONFIG_H_AUTHOR);
    SERIAL_LM(ECHO, MSG_COMPILED __DATE__);
  #endif // STRING_DISTRIBUTION_DATE

  SERIAL_SMV(ECHO, MSG_FREE_MEMORY, HAL::getFreeRam());
  SERIAL_EMV(MSG_PLANNER_BUFFER_BYTES, (int)sizeof(block_t)*BLOCK_BUFFER_SIZE);



  //configure hardware SPI
  HAL::spiBegin();

  print_job_counter.init();

  mechanics.init();

  // Init endstops and pullups
  endstops.init();

  #if ENABLED(NEXTION_HMI)
	NextionHMI::Init();
  #endif

  // Load data from EEPROM if available (or use defaults)
  // This also updates variables in the planner, elsewhere

  const bool sys_eeprom_loaded = eeprom.Load_Settings();

  #if HAS_SD_SUPPORT
    sdStorage.init();
	sdStorage.mountAll();
  #endif

  #if ENABLED(WORKSPACE_OFFSETS)
    // Initialize current position based on home_offset
    COPY_ARRAY(mechanics.current_position, mechanics.home_offset);
  #else
    ZERO(mechanics.current_position);
  #endif

  // Vital to init stepper/planner equivalent for current_position
  mechanics.sync_plan_position_mech_specific();

  thermalManager.init();  // Initialize temperature loop

  stepper.init(); // Initialize stepper, this enables interrupts!

  #if MB(ALLIGATOR) || MB(ALLIGATOR_V3)
    externaldac.begin();
    externaldac.set_driver_current();
  #endif

  #if ENABLED(CNCROUTER)
    cnc.init();
  #endif

  #if HAS_SERVOS
    servo_init(); // Initialize all Servo
  #endif

  #if HAS_CASE_LIGHT
    caselight.update();
  #endif

  #if HAS_SOFTWARE_ENDSTOPS
    endstops.setSoftEndstop(true);
  #endif

  #if HAS_STEPPER_RESET
    stepper.enableStepperDrivers();
  #endif

  #if ENABLED(DIGIPOT_I2C)
    digipot_i2c_init();
  #endif

  #if HAS_COLOR_LEDS
    leds.setup();
  #endif

  #if ENABLED(LASER)
    laser.init();
  #endif

  #if ENABLED(FLOWMETER_SENSOR)
    #if ENABLED(MINFLOW_PROTECTION)
      flowmeter.flow_firstread = false;
    #endif
    flowmeter.flow_init();
  #endif

  #if ENABLED(RFID_MODULE)
    RFID_ON = rfid522.init();
    if (RFID_ON)
      SERIAL_EM("RFID CONNECT");
  #endif

  #if ENABLED(NEXTION_HMI)
    NextionHMI::SetBrightness(NextionHMI::lcdBrightness);
  #else
	lcd_init();
	LCD_MESSAGEPGM(WELCOME_MSG);
  #endif


  #if ENABLED(SHOW_BOOTSCREEN)
    #if ENABLED(DOGLCD) || ENABLED(ULTRA_LCD)
      lcd_bootscreen(); // Show MK4duo boot screen
    #endif
  #endif

  #if ENABLED(COLOR_MIXING_EXTRUDER) && MIXING_VIRTUAL_TOOLS > 1
    mixing_tools_init();
  #endif

  #if ENABLED(BLTOUCH)
    probe.bltouch_init();
  #endif

  // All Initialized set Running to true.
  setRunning(true);

  #if ENABLED(DELTA_HOME_ON_POWER) || ENABLED(HOME_ON_POWER)
  if (sys_eeprom_loaded)
  {
    mechanics.home();
	#if ENABLED(EG6_EXTRUDER)
    	tools.change(0, 0, false, true);
	#endif
  }
  #endif

  #if ENABLED(NEXTION_HMI)
    if (NextionHMI::GetActiveState()!=PAGE_MESSAGE)
	{
		StateStatus::Activate();
	}
  #endif
  setStatus(Idle);

  #if FAN_COUNT > 0
    LOOP_FAN() fans[f].setSpeed(0);
  #endif

  if (!sys_eeprom_loaded) lcd_eeprom_allert();

  #if HAS_SD_RESTART
    restart.do_print_job();
  #endif

}

/**
 * The main MK4duo program loop
 *
 *  - Save or log commands to SD
 *  - Process available commands (if not saving)
 *  - Call heater manager
 *  - Call Fans manager
 *  - Call inactivity manager
 *  - Call endstop manager
 *  - Call LCD update
 */
void Printer::loop() {

  printer.keepalive(NotBusy);

  #if HAS_SD_SUPPORT

    if (printer.isCancelPrint()) {
    	printer.setCancelPrint(false);
      cancel_print();
    }

  #endif // HAS_SD_SUPPORT

  commands.get_available();
  commands.advance_queue();
  endstops.report_state();
  idle();
}

void Printer::cancel_print()
{
	#if ENABLED(NEXTION_HMI)
	  printer.setStatus(Busy); //to prevent pausing
	  NextionHMI::RaiseEvent(PRINT_CANCELLING);
	#endif

	#if HAS_SD_RESTART
	// Save Job for restart
	if (card.cardOK && IS_SD_PRINTING) restart.save_data(true);
	#endif

	if (sdStorage.isPrinting() || sdStorage.isPaused())
	{
		// Stop SD printing
		SERIAL_LMT(JOB, MSG_JOB_CANCEL, sdStorage.getActivePrintSDCard()->fileName);
		sdStorage.endFilePrint();
	}
	else //Host
	{
		SERIAL_LMT(JOB, MSG_JOB_CANCEL, printName);
	}

	// Clear all command in quee
	commands.clear_queue();

	// Stop all stepper
	stepper.quickstop_stepper();

	// Fiber cutting is needed?
	bool needCut = !tools.fiber_is_cut;
	if(needCut) tools.cut_fiber();

	//Reset parameters that were tuned during the print
	clean_after_print();

	// Auto home
	#if Z_HOME_DIR > 0
	mechanics.home();
	#else
	mechanics.home(true, true, false);
	#endif

	// Disabled Heaters and Fan
	thermalManager.disable_all_heaters();
	#if FAN_COUNT > 0
	LOOP_FAN() fans[f].setSpeed(0);
	#endif

	// Stop printer job timer
	print_job_counter.stop();

	#if ENABLED(NEXTION_HMI)
		NextionHMI::RaiseEvent(PRINT_CANCELLED);
		printer.setStatus(Idle);
		PrintPause::SdPrintingPaused = false;
		NextionHMI::RaiseEvent(NONE);
	#endif

}

void Printer::check_periodical_actions() {

  static uint8_t  cycle_1000ms = 10,  // Event 1.0 Second
                  cycle_2500ms = 25,  // Event 2.5 Second
  	  	  	  	  cycle_json_report = 20;  // Event

  // Control interrupt events
  handle_interrupt_events();

  // Tick timer job counter
  print_job_counter.tick();

  // Event 100 Ms - 10Hz
  if (HAL::execute_100ms) {
    HAL::execute_100ms = false;
    planner.check_axes_activity();
    thermalManager.spin();

    /*Serial.print("Commands:");
    Serial.print(commands.buffer_lenght);
    Serial.print(" - Moves:");
    Serial.print(planner.movesplanned());
    Serial.print(" - Pause:");
    Serial.println(PrintPause::Status);*/

    // Event 1.0 Second
    if (--cycle_1000ms == 0) {
      cycle_1000ms = 10;
      if (!isSuspendAutoreport() && isAutoreportTemp())
      {
         thermalManager.report_temperatures();
         SERIAL_EOL();
      }

      #if HAS_SD_SUPPORT
        sdStorage.processAutoreport();
      #endif
      #if ENABLED(NEXTION)
        nextion_draw_update();
      #endif
	  #if ENABLED(NEXTION_HMI)
		NextionHMI::DrawUpdate();
	  #endif
    }

    if (!isSuspendAutoreport() && isJsonAutoreportEnabled())
    {
        if (--cycle_json_report == 0) {
        	cycle_json_report = json_autoreport_interval*10;
          	reportStatusJson(getJsonAutoreportMode());
        }
    }

    // Event 2.5 Second
    if (--cycle_2500ms == 0) {
      cycle_2500ms = 25;
      #if FAN_COUNT > 0
        LOOP_FAN() fans[f].spin();
      #endif
      #if HAS_POWER_SWITCH
        powerManager.spin();
      #endif
    }
  }

}

void Printer::safe_delay(millis_l ms) {
  while (ms > 50) {
    ms -= 50;
    HAL::delayMilliseconds(50);
    check_periodical_actions();
  }
  HAL::delayMilliseconds(ms);
  check_periodical_actions();
}

void Printer::safe_idle(millis_l ms) {
	ms += millis();
	while (PENDING(millis(), ms)) {
	  printer.keepalive(InProcess);
	  printer.idle();
	}
}

/**
 * Prepare to do endstop or probe moves
 * with custom feedrates.
 *
 *  - Save current feedrates
 *  - Reset the rate multiplier
 *  - Reset the command timeout
 *  - Enable the endstops (for endstop moves)
 */
void Printer::bracket_probe_move(const bool before) {
  static float saved_feedrate_mm_s;
  static int16_t saved_feedrate_percentage;
  #if ENABLED(DEBUG_LEVELING_FEATURE)
    if (printer.debugLeveling()) DEBUG_POS("bracket_probe_move", mechanics.current_position);
  #endif
  if (before) {
    saved_feedrate_mm_s = mechanics.feedrate_mm_s;
    saved_feedrate_percentage = mechanics.feedrate_percentage;
    mechanics.feedrate_percentage = 100;
  }
  else {
    mechanics.feedrate_mm_s = saved_feedrate_mm_s;
    mechanics.feedrate_percentage = saved_feedrate_percentage;
  }
}

void Printer::setup_for_endstop_or_probe_move()       { bracket_probe_move(true); }
void Printer::clean_up_after_endstop_or_probe_move()  { bracket_probe_move(false); }

/**
 * Kill all activity and lock the machine.
 * After this the machine will need to be reset.
 */
void Printer::kill(const char* lcd_msg) {
  SERIAL_LM(ER, MSG_ERR_KILLED);

  printer.setStatus(Halted);
  thermalManager.disable_all_heaters();
  stepper.disable_all_steppers();
  printer.clean_after_print();

  #if ENABLED(KILL_METHOD) && (KILL_METHOD == 1)
    HAL::resetHardware();
  #endif
  #if ENABLED(FLOWMETER_SENSOR) && ENABLED(MINFLOW_PROTECTION)
    flowmeter.flow_firstread = false;
  #endif

  #if ENABLED(ULTRA_LCD)
    kill_screen(lcd_msg);
  #else
	#if ENABLED(NEXTION_HMI)
		NextionHMI::RaiseEvent(HMIevent::PRINTER_KILLED, 0, lcd_msg);
	#else
		UNUSED(lcd_msg);
	#endif
  #endif

  printer.safe_delay(600);  // Wait a short time (allows messages to get out before shutting down.
  #if DISABLED(CPU_32_BIT)
    cli(); // Stop interrupts
  #endif

  printer.safe_delay(250);  // Wait to ensure all interrupts routines stopped
  thermalManager.disable_all_heaters(); // Turn off heaters again

  #if ENABLED(LASER)
    laser.init();
    #if ENABLED(LASER_PERIPHERALS)
      laser.peripherals_off();
    #endif
  #endif

  #if ENABLED(CNCROUTER)
    cnc.disable_router();
  #endif

  #if HAS_POWER_SWITCH
    powerManager.power_off();
  #endif

  #if HAS_SUICIDE
    suicide();
  #endif

  while(1) { watchdog.reset(); } // Wait for reset

}

/**
 * Turn off heaters and stop the print in progress
 * After a stop the machine may be resumed with M999
 */
void Printer::Stop() {
  #if ENABLED(FLOWMETER_SENSOR) && ENABLED(MINFLOW_PROTECTION)
    flowmeter.flow_firstread = false;
  #endif

  thermalManager.disable_all_heaters();
  printer.clean_after_print();

  #if ENABLED(PROBING_FANS_OFF)
    LOOP_FAN() {
      if (fans[f].isIdle()) fans[f].setIdle(false); // put things back the way they were
    }
  #endif

  #if ENABLED(LASER)
    if (laser.diagnostics) SERIAL_EM("Laser set to off, Stop() called");
    laser.extinguish();
    #if ENABLED(LASER_PERIPHERALS)
      laser.peripherals_off();
    #endif
  #endif

  #if ENABLED(CNCROUTER)
     cnc.disable_router();
  #endif

  if (isRunning()) {
    setRunning(false);
    SERIAL_LM(ER, MSG_ERR_STOPPED);
    printer.setStatus(Halted);


    SERIAL_STR(PAUSE);
    SERIAL_EOL();
    LCD_MESSAGEPGM(MSG_STOPPED);
	#if ENABLED(NEXTION_HMI)
		NextionHMI::RaiseEvent(HMIevent::ERROR, 0, MSG_ERR_STOPPED);
	#endif
  }
}

/**
 * Manage several activities:
 *  - Lcd update
 *  - Check periodical actions
 *  - Keep the command buffer full
 *  - Host Keepalive
 *  - Check Flow meter sensor
 *  - Cnc manage
 *  - Check for Filament Runout
 *  - Check for maximum inactive time between commands
 *  - Check for maximum inactive time between stepper commands
 *  - Check if pin CHDK needs to go LOW
 *  - Check for KILL button held down
 *  - Check for HOME button held down
 *  - Check if cooling fan needs to be switched on
 *  - Check if an idle but hot extruder needs filament extruded (EXTRUDER_RUNOUT_PREVENT)
 *  - Check oozing prevent
 *  - Read o Write Rfid
 */
void Printer::idle(const bool ignore_stepper_queue/*=false*/) {

  #if ENABLED(NEXTION)
    lcd_key_touch_update();
  #else
  #if ENABLED(NEXTION_HMI)
    NextionHMI::TouchUpdate();
  #else
    lcd_update();
  #endif
  #endif

  check_periodical_actions();

  commands.get_available();

  handle_safety_watch();

  if (max_inactivity_timer.expired(SECOND_TO_MILLIS(max_inactive_time))) {
    SERIAL_LMT(ER, MSG_KILL_INACTIVE_TIME, parser.command_ptr);
    kill(PSTR(MSG_KILL_INACTIVE_TIME));
  }

  #if ENABLED(DHT_SENSOR)
    dhtsensor.spin();
  #endif

  #if ENABLED(CNCROUTER)
    cnc.manage();
  #endif

  #if HAS_FIL_RUNOUT
    filamentrunout.spin();
  #endif

  #if ENABLED(FLOWMETER_SENSOR)

    flowmeter.flowrate_manage();

    #if ENABLED(MINFLOW_PROTECTION)
      if (flowmeter.flow_firstread && print_job_counter.isRunning() && (flowmeter.flowrate < (float)MINFLOW_PROTECTION)) {
        flowmeter.flow_firstread = false;
        kill(PSTR(MSG_KILLED));
      }
    #endif

  #endif // ENABLED(FLOWMETER_SENSOR)

	#if ENABLED(BABYSTEPPING)
	  babystep.spin();
	#endif

  // Prevent steppers timing-out in the middle of M6003
  #if ENABLED(ADVANCED_PAUSE_FEATURE) && ENABLED(PAUSE_PARK_NO_STEPPER_TIMEOUT)
    #define MOVE_AWAY_TEST !did_pause_print
  #else
	#if ENABLED(NEXTION_HMI)
      #define MOVE_AWAY_TEST (printer.getStatus()!=Paused)
	#else
      #define MOVE_AWAY_TEST true
	#endif
  #endif

  if (stepper.move_watch.stopwatch) {
    if (planner.has_blocks_queued())
      stepper.move_watch.start(); // reset stepper move watch to keep steppers powered
    else if (MOVE_AWAY_TEST && !ignore_stepper_queue && stepper.move_watch.elapsed()) {
      #if ENABLED(DISABLE_INACTIVE_X)
        disable_X();
      #endif
      #if ENABLED(DISABLE_INACTIVE_Y)
        disable_Y();
      #endif
      #if ENABLED(DISABLE_INACTIVE_Z)
        disable_Z();
      #endif
      #if ENABLED(DISABLE_INACTIVE_E)
        stepper.disable_e_steppers();
      #endif
      #if ENABLED(AUTO_BED_LEVELING_UBL) && ENABLED(ULTIPANEL)  // Only needed with an LCD
        if (ubl.lcd_map_control) ubl.lcd_map_control = defer_return_to_status = false;
      #endif
      #if ENABLED(LASER)
        if (laser.time / 60000 > 0) {
          laser.lifetime += laser.time / 60000; // convert to minutes
          laser.time = 0;
        }
        laser.extinguish();
        #if ENABLED(LASER_PERIPHERALS)
          laser.peripherals_off();
        #endif
      #endif
    }
  }

  #if HAS_CHDK // Check if pin should be set to LOW after M240 set it to HIGH
    if (chdkActive && chdk_watch.elapsed()) {
      chdkActive = false;
      WRITE(CHDK_PIN, LOW);
    }
  #endif

  #if HAS_KILL

    // Check if the kill button was pressed and wait just in case it was an accidental
    // key kill key press
    // -------------------------------------------------------------------------------
    static int killCount = 0;   // make the inactivity button a bit less responsive
    const int KILL_DELAY = 750;
    if (!READ(KILL_PIN))
       killCount++;
    else if (killCount > 0)
       killCount--;

    // Exceeded threshold and we can confirm that it was not accidental
    // KILL the machine
    // ----------------------------------------------------------------
    if (killCount >= KILL_DELAY) {
      SERIAL_LM(ER, MSG_KILL_BUTTON);
      kill(PSTR(MSG_KILLED));
    }
  #endif

  #if HAS_HOME
    // Check to see if we have to home, use poor man's debouncer
    // ---------------------------------------------------------
    static int homeDebounceCount = 0;   // poor man's debouncing count
    const int HOME_DEBOUNCE_DELAY = 750;
    if (!IS_SD_PRINTING && !READ(HOME_PIN)) {
      if (!homeDebounceCount) {
        commands.enqueue_and_echo_P(PSTR("G28"));
        LCD_MESSAGEPGM(MSG_AUTO_HOME);
      }
      if (homeDebounceCount < HOME_DEBOUNCE_DELAY)
        homeDebounceCount++;
      else
        homeDebounceCount = 0;
    }
  #endif

  #if ENABLED(EXTRUDER_RUNOUT_PREVENT)

    static watch_t extruder_runout_watch(EXTRUDER_RUNOUT_SECONDS * 1000UL);

    if (heaters[EXTRUDER_IDX].current_temperature > EXTRUDER_RUNOUT_MINTEMP
      && extruder_runout_watch.elapsed()
      && !planner.has_blocks_queued()
    ) {
      #if ENABLED(DONDOLO_SINGLE_MOTOR)
        const bool oldstatus = E0_ENABLE_READ;
        enable_E0();
      #else // !DONDOLO_SINGLE_MOTOR
        bool oldstatus;
        switch (tools.active_extruder) {
          case 0: oldstatus = E0_ENABLE_READ; enable_E0(); break;
          #if DRIVER_EXTRUDERS > 1
            case 1: oldstatus = E1_ENABLE_READ; enable_E1(); break;
            #if DRIVER_EXTRUDERS > 2
              case 2: oldstatus = E2_ENABLE_READ; enable_E2(); break;
              #if DRIVER_EXTRUDERS > 3
                case 3: oldstatus = E3_ENABLE_READ; enable_E3(); break;
                #if DRIVER_EXTRUDERS > 4
                  case 4: oldstatus = E4_ENABLE_READ; enable_E4(); break;
                  #if DRIVER_EXTRUDERS > 5
                    case 5: oldstatus = E5_ENABLE_READ; enable_E5(); break;
                  #endif // DRIVER_EXTRUDERS > 5
                #endif // DRIVER_EXTRUDERS > 4
              #endif // DRIVER_EXTRUDERS > 3
            #endif // DRIVER_EXTRUDERS > 2
          #endif // DRIVER_EXTRUDERS > 1
        }
      #endif // !DONDOLO_SINGLE_MOTOR

      const float olde = mechanics.current_position[E_AXIS];
      mechanics.current_position[E_AXIS] += EXTRUDER_RUNOUT_EXTRUDE;
      planner.buffer_line_kinematic(mechanics.current_position, MMM_TO_MMS(EXTRUDER_RUNOUT_SPEED), tools.active_extruder);
      mechanics.current_position[E_AXIS] = olde;
      planner.set_e_position_mm(olde);
      stepper.synchronize();
      #if ENABLED(DONDOLO_SINGLE_MOTOR)
        E0_ENABLE_WRITE(oldstatus);
      #else
        switch(tools.active_extruder) {
          case 0: E0_ENABLE_WRITE(oldstatus); break;
          #if DRIVER_EXTRUDERS > 1
            case 1: E1_ENABLE_WRITE(oldstatus); break;
            #if DRIVER_EXTRUDERS > 2
              case 2: E2_ENABLE_WRITE(oldstatus); break;
              #if DRIVER_EXTRUDERS > 3
                case 3: E3_ENABLE_WRITE(oldstatus); break;
                #if DRIVER_EXTRUDERS > 4
                  case 4: E4_ENABLE_WRITE(oldstatus); break;
                  #if DRIVER_EXTRUDERS > 5
                    case 5: E5_ENABLE_WRITE(oldstatus); break;
                  #endif // DRIVER_EXTRUDERS > 5
                #endif // DRIVER_EXTRUDERS > 4
              #endif // DRIVER_EXTRUDERS > 3
            #endif // DRIVER_EXTRUDERS > 2
          #endif // DRIVER_EXTRUDERS > 1
        }
      #endif // !DONDOLO_SINGLE_MOTOR

      extruder_runout_watch.start();
    }
  #endif // EXTRUDER_RUNOUT_PREVENT

  #if ENABLED(DUAL_X_CARRIAGE)
    // handle delayed move timeout
    if (mechanics.delayed_move_time && ELAPSED(millis(), mechanics.delayed_move_time + 1000UL) && isRunning()) {
      // travel moves have been received so enact them
      mechanics.delayed_move_time = 0xFFFFFFFFUL; // force moves to be done
      mechanics.set_destination_to_current();
      mechanics.prepare_move_to_destination();
    }
  #endif

  #if ENABLED(IDLE_OOZING_PREVENT)
    if (planner.has_blocks_queued()) axis_last_activity = millis();
    if (heaters[EXTRUDER_IDX].current_temperature > IDLE_OOZING_MINTEMP && !debugDryrun() && IDLE_OOZING_enabled) {
      #if ENABLED(FILAMENTCHANGEENABLE)
        if (!filament_changing)
      #endif
      {
        if (heaters[EXTRUDER_IDX].target_temperature < IDLE_OOZING_MINTEMP) {
          IDLE_OOZING_retract(false);
        }
        else if ((millis() - axis_last_activity) >  IDLE_OOZING_SECONDS * 1000UL) {
          IDLE_OOZING_retract(true);
        }
      }
    }
  #endif

  #if ENABLED(RFID_MODULE)
    for (int8_t e = 0; e < EXTRUDERS; e++) {
      if (Spool_must_read[e]) {
        if (rfid522.getID(e)) {
          Spool_ID[e] = rfid522.RfidDataID[e].Spool_ID;
          HAL::delayMilliseconds(200);
          if (rfid522.readBlock(e)) {
            Spool_must_read[e] = false;
            tools.density_percentage[e] = rfid522.RfidData[e].data.density;
            tools.filament_size[e] = rfid522.RfidData[e].data.size;
            #if ENABLED(VOLUMETRIC_EXTRUSION)
              tools.calculate_volumetric_multipliers();
            #endif
            tools.refresh_e_factor(e);
            rfid522.printInfo(e);
          }
        }
      }

      if (Spool_must_write[e]) {
        if (rfid522.getID(e)) {
          if (Spool_ID[e] == rfid522.RfidDataID[e].Spool_ID) {
            HAL::delayMilliseconds(200);
            if (rfid522.writeBlock(e)) {
              Spool_must_write[e] = false;
              SERIAL_SMV(INFO, "Spool on E", e);
              SERIAL_EM(" writed!");
              rfid522.printInfo(e);
            }
          }
        }
      }
    }
  #endif

  #if ENABLED(TEMP_STAT_LEDS)
    handle_status_leds();
  #endif

  #if ENABLED(MONITOR_DRIVER_STATUS)
    monitor_tmc_driver();
  #endif

  #if ENABLED(MOVE_DEBUG)
    char buf[100] = { 0 };
    sprintf(buf, "Interrupts scheduled %u, done %u, last %u, next %u sched at %u, now %u\n",
      numInterruptsScheduled, numInterruptsExecuted, lastInterruptTime, nextInterruptTime, nextInterruptScheduledAt, HAL_timer_get_count(STEPPER_TIMER));
    SERIAL_PS(buf);
    SERIAL_EOL();
  #endif

}

void Printer::setInterruptEvent(const MK4duoInterruptEvent event) {
  if (interruptEvent == INTERRUPT_EVENT_NONE)
    interruptEvent = event;
}

void Printer::handle_interrupt_events() {

  if (interruptEvent == INTERRUPT_EVENT_NONE) return; // Exit if none Event

  const MK4duoInterruptEvent event = interruptEvent;
  interruptEvent = INTERRUPT_EVENT_NONE;

  switch(event) {
    #if HAS_FIL_RUNOUT
      case INTERRUPT_EVENT_FIL_RUNOUT:
        if (!isFilamentOut() && (IS_SD_PRINTING || print_job_counter.isRunning())) {
          setFilamentOut(true);
          commands.enqueue_and_echo_P(PSTR(FILAMENT_RUNOUT_SCRIPT));
          stepper.synchronize();
        }
        break;
    #endif

    #if HAS_EXT_ENCODER
      case INTERRUPT_EVENT_ENC_DETECT:
        if (!isFilamentOut() && (IS_SD_PRINTING || print_job_counter.isRunning())) {
          setFilamentOut(true);
          stepper.synchronize();

          #if ENABLED(ADVANCED_PAUSE_FEATURE)
            commands.enqueue_and_echo_P(PSTR("M600"));
          #endif
        }
        break;
    #endif

    default:
      break;
  }
}

/**
 * Turn off heating after 30 minutes of inactivity
 */
void Printer::handle_safety_watch() {

  static watch_t safety_watch(30 * 60 * 1000UL);

  #if ENABLED(NEXTION_HMI)
	#define PRINT_IS_PAUSED (printer.getStatus()==Paused)
  #else
	#define PRINT_IS_PAUSED false
  #endif

  if (safety_watch.isRunning() && (IS_SD_PRINTING() || PRINT_IS_PAUSED || print_job_counter.isRunning() || !thermalManager.heaters_isON()))
    safety_watch.stop();
  else if (!safety_watch.isRunning() && thermalManager.heaters_isON())
    safety_watch.start();
  else if (safety_watch.isRunning() && safety_watch.elapsed())
    thermalManager.disable_all_heaters();

}

/**
 * Sensitive pin test for M42, M226
 */
bool Printer::pin_is_protected(const pin_t pin) {
  static const int8_t sensitive_pins[] PROGMEM = SENSITIVE_PINS;
  for (uint8_t i = 0; i < COUNT(sensitive_pins); i++)
    if (pin == pgm_read_byte(&sensitive_pins[i])) return true;
  return false;
}

void Printer::suicide() {
  #if HAS_SUICIDE
    OUT_WRITE(SUICIDE_PIN, LOW);
  #endif
}

/**
 * Private Function
 */

void Printer::setup_pinout() {

  #if MB(ALLIGATOR) || MB(ALLIGATOR_V3)

    // All SPI chip-select HIGH
    OUT_WRITE(DAC0_SYNC_PIN, HIGH);
    #if EXTRUDERS > 1
      OUT_WRITE(DAC1_SYNC_PIN, HIGH);
    #endif
    OUT_WRITE(SPI_EEPROM1_CS, HIGH);
    OUT_WRITE(SPI_EEPROM2_CS, HIGH);
    OUT_WRITE(SPI_FLASH_CS, HIGH);
    SET_INPUT(MOTOR_FAULT_PIN);
    #if MB(ALLIGATOR_V3)
      SET_INPUT(MOTOR_FAULT_PIGGY_PIN);
      SET_INPUT(FTDI_COM_RESET_PIN);
      SET_INPUT(ESP_WIFI_MODULE_RESET_PIN);
      OUT_WRITE(EXP1_OUT_ENABLE_PIN, HIGH);
    #elif MB(ALLIGATOR)
      // Init Expansion Port Voltage logic Selector
      OUT_WRITE(EXP_VOLTAGE_LEVEL_PIN, UI_VOLTAGE_LEVEL);
    #endif

    #if HAS_BUZZER
      BUZZ(10,10);
    #endif

  #elif MB(ULTRATRONICS)

    /* avoid floating pins */
    OUT_WRITE(ORIG_FAN0_PIN, LOW);
    OUT_WRITE(ORIG_FAN1_PIN, LOW);

    OUT_WRITE(ORIG_HEATER_0_PIN, LOW);
    OUT_WRITE(ORIG_HEATER_1_PIN, LOW);
    OUT_WRITE(ORIG_HEATER_2_PIN, LOW);
    OUT_WRITE(ORIG_HEATER_3_PIN, LOW);

    OUT_WRITE(ENC424_SS_PIN, HIGH);

  #endif

  #if HAS_MAX6675_SS
    OUT_WRITE(MAX6675_SS_PIN, HIGH);
  #endif

  #if HAS_MAX31855_SS0
    OUT_WRITE(MAX31855_SS0_PIN, HIGH);
  #endif
  #if HAS_MAX31855_SS1
    OUT_WRITE(MAX31855_SS1_PIN, HIGH);
  #endif
  #if HAS_MAX31855_SS2
    OUT_WRITE(MAX31855_SS2_PIN, HIGH);
  #endif
  #if HAS_MAX31855_SS3
    OUT_WRITE(MAX31855_SS3_PIN, HIGH);
  #endif

  #if HAS_SUICIDE
    OUT_WRITE(SUICIDE_PIN, HIGH);
  #endif

  #if HAS_KILL
    SET_INPUT_PULLUP(KILL_PIN);
  #endif

  #if HAS_PHOTOGRAPH
    OUT_WRITE(PHOTOGRAPH_PIN, LOW);
  #endif

  #if HAS_CASE_LIGHT && DISABLED(CASE_LIGHT_USE_NEOPIXEL)
    SET_OUTPUT(CASE_LIGHT_PIN);
  #endif

  #if HAS_Z_PROBE_SLED
    OUT_WRITE(SLED_PIN, LOW); // turn it off
  #endif

  #if HAS_HOME
    SET_INPUT_PULLUP(HOME_PIN);
  #endif

  #if PIN_EXISTS(STAT_LED_RED)
    OUT_WRITE(STAT_LED_RED_PIN, LOW); // turn it off
  #endif

  #if PIN_EXISTS(STAT_LED_BLUE)
    OUT_WRITE(STAT_LED_BLUE_PIN, LOW); // turn it off
  #endif

}

#if ENABLED(IDLE_OOZING_PREVENT)

  void Printer::IDLE_OOZING_retract(bool retracting) {

    if (retracting && !IDLE_OOZING_retracted[tools.active_extruder]) {

      float old_feedrate_mm_s = mechanics.feedrate_mm_s;

      mechanics.set_destination_to_current();
      mechanics.current_position[E_AXIS] += IDLE_OOZING_LENGTH
        #if ENABLED(VOLUMETRIC_EXTRUSION)
          / tools.volumetric_multiplier[tools.active_extruder]
        #endif
      ;
      mechanics.feedrate_mm_s = IDLE_OOZING_FEEDRATE;
      planner.set_e_position_mm(mechanics.current_position[E_AXIS]);
      mechanics.prepare_move_to_destination();
      mechanics.feedrate_mm_s = old_feedrate_mm_s;
      IDLE_OOZING_retracted[tools.active_extruder] = true;
      //SERIAL_EM("-");
    }
    else if (!retracting && IDLE_OOZING_retracted[tools.active_extruder]) {

      float old_feedrate_mm_s = mechanics.feedrate_mm_s;

      mechanics.set_destination_to_current();
      mechanics.current_position[E_AXIS] -= (IDLE_OOZING_LENGTH + IDLE_OOZING_RECOVER_LENGTH)
        #if ENABLED(VOLUMETRIC_EXTRUSION)
          / tools.volumetric_multiplier[tools.active_extruder]
        #endif
      ;

      mechanics.feedrate_mm_s = IDLE_OOZING_RECOVER_FEEDRATE;
      planner.set_e_position_mm(mechanics.current_position[E_AXIS]);
      mechanics.prepare_move_to_destination();
      mechanics.feedrate_mm_s = old_feedrate_mm_s;
      IDLE_OOZING_retracted[tools.active_extruder] = false;
      //SERIAL_EM("+");
    }
  }

#endif

/**
 * Flags Function
 */
void Printer::setDebugLevel(const uint8_t newLevel) {
  if (newLevel != mk_debug_flag) {
    mk_debug_flag = newLevel;
    if (debugDryrun() || debugSimulation()) {
      // Disable all heaters in case they were on
      thermalManager.disable_all_heaters();
    }
  }
  SERIAL_EMV("DebugLevel:", (int)mk_debug_flag);
}


/**
 * Clean parameters, that were tuned during print after print was finished/cancelled
 */
void Printer::clean_after_print() {

    mechanics.feedrate_percentage = 100;  // 100% mechanics.feedrate_mm_s
    tools.flow_percentage[E_AXIS-XYZ] = 100;
	#if (DRIVER_EXTRUDERS>2)
    	tools.flow_percentage[V_AXIS-XYZ] = 100;
	#endif

	#if HAS_HEATER_0
		heaters[HOT0_INDEX].temperature_correction = 0;
	#endif
	#if HAS_HEATER_1
    	heaters[HOT1_INDEX].temperature_correction = 0;
	#endif
	#if HAS_HEATER_BED
		heaters[BED_INDEX].temperature_correction = 0;
	#endif
	#if ENABLED(EG6_EXTRUDER)
	  tools.parked_near_wipe = false;
	#endif
	#if HAS_FAN0
		fans[FAN0_INDEX].speed_correction = 0;
	#endif
	#if HAS_FAN1
		fans[FAN1_INDEX].speed_correction = 0;
	#endif
	#if HAS_FAN2
		fans[FAN2_INDEX].speed_correction = 0;
	#endif

	tools.printing_with_fiber = false;
}

#if ENABLED(HOST_KEEPALIVE_FEATURE)

  /**
   * Output a "busy" message at regular intervals
   * while the machine is not accepting
   */
  void Printer::keepalive(const MK4duoBusyState state) {
    if (!isSuspendAutoreport() && host_keepalive_watch.stopwatch && host_keepalive_watch.elapsed()) {
      switch (state) {
        case InHandler:
        case InProcess:
          SERIAL_LM(BUSY, MSG_BUSY_PROCESSING);
          break;
        case WaitHeater:
          SERIAL_LM(BUSY, MSG_BUSY_WAIT_HEATER);
          break;
        case DoorOpen:
          SERIAL_LM(BUSY, MSG_BUSY_DOOR_OPEN);
          break;
        case PausedforUser:
          SERIAL_LM(BUSY, MSG_BUSY_PAUSED_FOR_USER);
          break;
        case PausedforInput:
          SERIAL_LM(BUSY, MSG_BUSY_PAUSED_FOR_INPUT);
          break;
        default:
          break;
      }
      host_keepalive_watch.start();
    }
  }

#endif // HOST_KEEPALIVE_FEATURE

#if ENABLED(TEMP_STAT_LEDS)

  void Printer::handle_status_leds() {

    static bool red_led = false;
    static millis_l next_status_led_update_ms = 0;

    if (ELAPSED(millis(), next_status_led_update_ms)) {
      next_status_led_update_ms += 500; // Update every 0.5s
      float max_temp = 0.0;
        #if HAS_TEMP_BED
          max_temp = MAX3(max_temp, heaters[BED_INDEX].target_temperature, heaters[BED_INDEX].current_temperature);
        #endif
      LOOP_HOTEND()
        max_temp = MAX3(max_temp, heaters[h].current_temperature, heaters[h].target_temperature);
      const bool new_led = (max_temp > 55.0) ? true : (max_temp < 54.0) ? false : red_led;
      if (new_led != red_led) {
        red_led = new_led;
        #if PIN_EXISTS(STAT_LED_RED)
          WRITE(STAT_LED_RED_PIN, new_led ? HIGH : LOW);
          #if PIN_EXISTS(STAT_LED_BLUE)
            WRITE(STAT_LED_BLUE_PIN, new_led ? LOW : HIGH);
          #endif
        #else
          WRITE(STAT_LED_BLUE_PIN, new_led ? HIGH : LOW);
        #endif

      }
    }
  }

#endif

#if ENABLED(JSON_OUTPUT)

  char Printer::get_status_character(){
	char status_char = ' ';
	switch (status) {
		case Busy:
			status_char = 'B';
			break;
		case Idle:
			status_char = 'I';
			break;
		case Printing:
			status_char = 'P';
			break;
		case WaitingToPause:
			status_char = 'P';
			break;
		case Pausing:
			status_char = 'D';
			break;
		case Paused:
			status_char = 'S';
			break;
		case Resuming:
			status_char = 'R';
			break;
		case Halted:
			status_char = 'H';
			break;
		case PrintFinished:
			status_char = 'W';
			break;
	}
	return status_char;
  }

  void Printer::reportStatusJson(uint8_t type) {

	  bool firstOccurrence;
	  char ch = get_status_character();
	      SERIAL_MSG("{\"status\":\"");
	      SERIAL_CHR(ch);

	      SERIAL_MSG("\",\"coords\": {\"axesHomed\":[");
	      SERIAL_MSG(isXHomed ? "1," : "0,");
	      SERIAL_MSG(isYHomed ? "1," : "0,");
	      SERIAL_MSG(isZHomed ? "1" : "0");

	      SERIAL_MV("],\"extr\":[", mechanics.current_position[E_AXIS]);
	  	#if DRIVER_EXTRUDERS > 1
	        SERIAL_MV(",", mechanics.current_position[U_AXIS]);
	  	#endif
	  	#if DRIVER_EXTRUDERS > 2
	  	  SERIAL_MV(",", mechanics.current_position[V_AXIS]);
	  	#endif
	  	#if DRIVER_EXTRUDERS > 3
	  	  SERIAL_MV(",", mechanics.current_position[W_AXIS]);
	  	#endif
	  	#if DRIVER_EXTRUDERS > 4
	  	  SERIAL_MV(",", mechanics.current_position[K_AXIS]);
	  	#endif
	  	#if DRIVER_EXTRUDERS > 5
	  	  SERIAL_MV(",", mechanics.current_position[L_AXIS]);
	  	#endif
	      SERIAL_MV("],\"xyz\":[", mechanics.current_position[X_AXIS]); // X AXIS
	      SERIAL_MV(",", mechanics.current_position[Y_AXIS]);           // Y AXIS
	      SERIAL_MV(",", mechanics.current_position[Z_AXIS]);           // Z AXIS

	      SERIAL_MV("]},\"currentTool\":", tools.active_extruder);

	      #if HAS_POWER_SWITCH
	        SERIAL_MSG(",\"params\": {\"atxPower\":");
	        SERIAL_CHR(powerManager.lastPowerOn ? '1' : '0');
	        SERIAL_CHR(',');
	      #else
	        SERIAL_MSG(",\"params\": {");
	      #endif

	      #if FAN_COUNT > 0
	        SERIAL_MSG("\"fanPercent\":[");
	        SERIAL_VAL(100 * fans[0].Speed / 255);
	  	  #if FAN_COUNT > 1
	        SERIAL_MSG(",");
	        SERIAL_VAL(100 * fans[1].Speed / 255);
	  	  #endif
	  	  #if FAN_COUNT > 2
	        SERIAL_MSG(",");
	  	  SERIAL_VAL(100 * fans[2].Speed / 255);
	  	  #endif
	  	  #if FAN_COUNT > 3
	  	  SERIAL_MSG(",");
	  	  SERIAL_VAL(100 * fans[3].Speed / 255);
	  	  #endif
	      #endif

	      SERIAL_MV("],\"speedFactor\":", mechanics.feedrate_percentage);

	      SERIAL_MSG(",\"extrFactors\":[");
	      firstOccurrence = true;
	      for (uint8_t e = 0; e < DRIVER_EXTRUDERS; e++) {
	        if (!firstOccurrence) SERIAL_CHR(',');
	        SERIAL_VAL(tools.flow_percentage[e]);
	        firstOccurrence = false;
	      }
	      SERIAL_MSG("]},");

	      SERIAL_MSG("\"temps\": {");
	      #if HAS_TEMP_BED
	        SERIAL_MV("\"bed\": {\"current\":", heaters[BED_INDEX].current_temperature, 1);
	        SERIAL_MV(",\"active\":", heaters[BED_INDEX].target_temperature);
	        SERIAL_MSG(",\"state\":");
	        SERIAL_CHR(heaters[BED_INDEX].target_temperature > 0 ? '2' : '1');
	        SERIAL_MSG("},");
	      #endif
	  	#if HAS_TEMP_CHAMBER
	  	  SERIAL_MV("\"chamber\": {\"current\":", heaters[CHAMBER_INDEX].current_temperature, 1);
	  	  SERIAL_MV(",\"active\":", heaters[CHAMBER_INDEX].target_temperature);
	  	  SERIAL_MSG(",\"state\":");
	  	  SERIAL_CHR(heaters[CHAMBER_INDEX].target_temperature > 0 ? '2' : '1');
	  	  SERIAL_MSG("},");
	  	#endif
	      SERIAL_MSG("\"heads\": {\"current\":[");
	      firstOccurrence = true;
	      for (int8_t h = 0; h < HOTENDS; h++) {
	        if (!firstOccurrence) SERIAL_CHR(',');
	        SERIAL_VAL(heaters[h].current_temperature, 1);
	        firstOccurrence = false;
	      }
	      SERIAL_MSG("],\"active\":[");
	      firstOccurrence = true;
	      for (int8_t h = 0; h < HOTENDS; h++) {
	        if (!firstOccurrence) SERIAL_CHR(',');
	        SERIAL_VAL(heaters[h].target_temperature);
	        firstOccurrence = false;
	      }
	      SERIAL_MSG("],\"state\":[");
	      firstOccurrence = true;
	      for (int8_t h = 0; h < HOTENDS; h++) {
	        if (!firstOccurrence) SERIAL_CHR(',');
	        SERIAL_CHR(heaters[h].target_temperature > HOTEND_AUTO_FAN_TEMPERATURE ? '2' : '1');
	        firstOccurrence = false;
	      }

	      SERIAL_MV("]}},\"time\":", HAL::timeInMilliseconds());

	      switch (type) {
	        case 0:
	        case 1:
	          break;
	        case 2:
	          SERIAL_MSG(",");
	          SERIAL_MSG("\"coldExtrudeTemp\":0,\"coldRetractTemp\":0.0,\"geometry\":\"");
	          #if MECH(CARTESIAN)
	            SERIAL_MSG("cartesian");
	          #elif MECH(COREXY)
	            SERIAL_MSG("corexy");
	          #elif MECH(COREYX)
	            SERIAL_MSG("coreyx");
	          #elif MECH(COREXZ)
	            SERIAL_MSG("corexz");
	          #elif MECH(COREZX)
	            SERIAL_MSG("corezx");
	          #elif MECH(DELTA)
	            SERIAL_MSG("delta");
	          #endif
	          SERIAL_MSG("\",\"name\":\"");
	          SERIAL_MSG(CUSTOM_MACHINE_NAME);
	          SERIAL_MSG("\",\"tools\":[");
	          firstOccurrence = true;
	          for (uint8_t i = 0; i < EXTRUDERS; i++) {
	            if (!firstOccurrence) SERIAL_CHR(',');
	            SERIAL_MV("{\"number\":", i + 1);
	            #if HOTENDS > 1
	              SERIAL_MV(",\"heaters\":[", i + 1);
	              SERIAL_MSG("],");
	            #else
	              SERIAL_MSG(",\"heaters\":[1],");
	            #endif
	            #if DRIVER_EXTRUDERS > 1
	              SERIAL_MV("\"drives\":[", i);
	              SERIAL_MSG("]");
	            #else
	              SERIAL_MSG("\"drives\":[0]");
	            #endif
	            SERIAL_MSG("}");
	            firstOccurrence = false;
	          }
	          break;
	        case 3:
	          SERIAL_MSG(",");
	          SERIAL_MSG("\"currentLayer\":");
	          #if HAS_SD_SUPPORT
	            if (sdStorage.isPrinting()) { // ONLY CAN TELL WHEN SD IS PRINTING
	              SERIAL_VAL((int)printer.currentLayer);
	            }
	            else SERIAL_VAL(0);
	          #else
	            SERIAL_VAL(-1);
	          #endif
	          #if HAS_SD_SUPPORT
	            if (sdStorage.isPrinting() | sdStorage.isPaused()) {
	              SERIAL_MSG(", \"fractionPrinted\":");
	              float fractionprinted = sdStorage.fractionDone();
	              SERIAL_VAL((float) floorf(fractionprinted * 1000) / 1000);
	            }
	          #endif
	  	      SERIAL_MSG(",\"used\":[");
	  	      firstOccurrence = true;
	  	      for (uint8_t e = 0; e < DRIVER_EXTRUDERS; e++) {
	  	        if (!firstOccurrence) SERIAL_CHR(',');
	  	        SERIAL_VAL(print_job_counter.currentFilamentUsed[e]);
	  	        firstOccurrence = false;
	  	      }
	  	      SERIAL_MSG("]");
	          break;
	        case 4:
	        case 5:
	          SERIAL_MSG(",");
	          SERIAL_MSG("\"axisMins\":[");
	          SERIAL_VAL((int) X_MIN_POS);
	          SERIAL_CHR(',');
	          SERIAL_VAL((int) Y_MIN_POS);
	          SERIAL_CHR(',');
	          SERIAL_VAL((int) Z_MIN_POS);
	          SERIAL_MSG("],\"axisMaxes\":[");
	          SERIAL_VAL((int) X_MAX_POS);
	          SERIAL_CHR(',');
	          SERIAL_VAL((int) Y_MAX_POS);
	          SERIAL_CHR(',');
	          SERIAL_VAL((int) Z_MAX_POS);
	          SERIAL_MSG("],\"planner.accelerations\":[");
	          SERIAL_VAL(mechanics.max_acceleration_mm_per_s2[X_AXIS]);
	          SERIAL_CHR(',');
	          SERIAL_VAL(mechanics.max_acceleration_mm_per_s2[Y_AXIS]);
	          SERIAL_CHR(',');
	          SERIAL_VAL(mechanics.max_acceleration_mm_per_s2[Z_AXIS]);
	          for (uint8_t i = 0; i < EXTRUDERS; i++) {
	            SERIAL_CHR(',');
	            SERIAL_VAL(mechanics.max_acceleration_mm_per_s2[E_AXIS + i]);
	          }
	          SERIAL_MSG("],");

	          #if MB(ALLIGATOR) || MB(ALLIGATOR_V3)
	            SERIAL_MSG("\"currents\":[");
	            SERIAL_VAL(externaldac.motor_current[X_AXIS]);
	            SERIAL_CHR(',');
	            SERIAL_VAL(externaldac.motor_current[Y_AXIS]);
	            SERIAL_CHR(',');
	            SERIAL_VAL(externaldac.motor_current[Z_AXIS]);
	            for (uint8_t i = 0; i < DRIVER_EXTRUDERS; i++) {
	              SERIAL_CHR(',');
	              SERIAL_VAL(externaldac.motor_current[E_AXIS + i]);
	            }
	            SERIAL_MSG("],");
	          #endif

	          SERIAL_MSG("\"firmwareElectronics\":\"");
	          #if MB(RAMPS_13_HFB) || MB(RAMPS_13_HHB) || MB(RAMPS_13_HFF) || MB(RAMPS_13_HHF) || MB(RAMPS_13_HHH)
	            SERIAL_MSG("RAMPS");
	          #elif MB(ALLIGATOR)
	            SERIAL_MSG("ALLIGATOR");
	          #elif MB(ALLIGATOR_V3)
	            SERIAL_MSG("ALLIGATOR_V3");
	          #elif MB(RADDS) || MB(RAMPS_FD_V1) || MB(RAMPS_FD_V2) || MB(SMART_RAMPS) || MB(RAMPS4DUE)
	            SERIAL_MSG("Arduino due");
	          #elif MB(ULTRATRONICS)
	            SERIAL_MSG("ULTRATRONICS");
	          #else
	            SERIAL_MSG("AVR");
	          #endif
	          SERIAL_MSG("\",\"firmwareName\":\"");
	          SERIAL_MSG(FIRMWARE_NAME);
	          SERIAL_MSG(",\"firmwareVersion\":\"");
	          SERIAL_MSG(SHORT_BUILD_VERSION);
	          SERIAL_MSG("\",\"firmwareDate\":\"");
	          SERIAL_MSG(STRING_DISTRIBUTION_DATE);

	          SERIAL_MSG("\",\"minFeedrates\":[0,0,0");
	          for (uint8_t i = 0; i < EXTRUDERS; i++) {
	            SERIAL_MSG(",0");
	          }
	          SERIAL_MSG("],\"maxFeedrates\":[");
	          SERIAL_VAL(mechanics.max_feedrate_mm_s[X_AXIS]);
	          SERIAL_CHR(',');
	          SERIAL_VAL(mechanics.max_feedrate_mm_s[Y_AXIS]);
	          SERIAL_CHR(',');
	          SERIAL_VAL(mechanics.max_feedrate_mm_s[Z_AXIS]);
	          for (uint8_t i = 0; i < EXTRUDERS; i++) {
	            SERIAL_CHR(',');
	            SERIAL_VAL(mechanics.max_feedrate_mm_s[E_AXIS + i]);
	          }
	          SERIAL_CHR(']');
	          break;
	      }
	      SERIAL_CHR('}');
	      SERIAL_EOL();
  }

#endif
