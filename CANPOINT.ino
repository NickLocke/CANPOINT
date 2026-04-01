
/*
  Copyright (C) Duncan Greenwood M5767 (duncan_greenwood@hotmail.com), Philip Silver M4082, 2022, 2023, 2024, 2025

  This work is licensed under the:
      Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   To view a copy of this license, visit:
      http://creativecommons.org/licenses/by-nc-sa/4.0/
   or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

   License summary:
    You are free to:
      Share, copy and redistribute the material in any medium or format
      Adapt, remix, transform, and build upon the material

    The licensor cannot revoke these freedoms as long as you follow the license terms.

    Attribution : You must give appropriate credit, provide a link to the license,
                  and indicate if changes were made. You may do so in any reasonable manner,
                  but not in any way that suggests the licensor endorses you or your use.

    NonCommercial : You may not use the material for commercial purposes. **(see note below)

    ShareAlike : If you remix, transform, or build upon the material, you must distribute
                 your contributions under the same license as the original.

    No additional restrictions : You may not apply legal terms or technological measures that
                                 legally restrict others from doing anything the license permits.

   ** For commercial use, please contact the original copyright holder(s) to agree licensing terms

    This software is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE

*/

/*

  CANGATEXL

  CANGATEX was a reimplemenation and refactoring of Phil Silver's CANGATE design, allowing greater portability, extensibility and maintainability.
  CANGATEXL uses 16 bit gate input numbers to allow for >255 gate inputs, as well as AND and OR gates with up to 8 inputs

  -- gates are now defined in a data structure rather than in code, so any combination of gates is possible, and more powerful processors could support many more gates
  -- it also consumes its own produced events, so CANBUFFER is not required for cascading the output of one gate to the input of another
  -- an event can be the input to up to four gates, and gates can cascade as many times as you like
  -- up to 8 digital outputs (e.g. A0-A7 on the Nano) can be used to mimic the output state of up to 8 selected gates -- useful for visual debugging
  -- added NAND, NOR, XOR, XNOR, NOT, flip-flop, and buffer gates

  Code size:

  For Arduino Uno/Nano R3
  -- original CANGATE -- 21962/773 bytes -- 71%/37%
  -- this implementation -- 26146/1672 bytes -- 85%/81%
  -- 1.3 = 28176/1758 91%/85%
  -- 1.6 = 27204/1372 88%/66%

  For Arduino Uno/Nano R4
  -- 1.7 = 83616/8468 31%/25%

  Consumer events:

  -- accepts long or short opcodes
  -- consumes its own events so the output of one gate can be cascaded as the input to other gates, and so on (CANBUFFER is not required)
  -- an event can be the input to up to four gates, using ev1 (per the original code) plus optionally evs 6, 7 and 8
  -- ev3 of the input event inverts the gate's output state, for NAND/NOR/XNOR behaviour
  -- all input events for any one gate should have the same ev3 value
  -- can consume a SoD event which will produce events for all gates at startup, or self-SoD after a configurable delay

  Producer events:

  -- an event is produced whenever a gate's output state changes as a consequence of an input change
  -- can use taught producer events (evs 4 and 5 hold the output value)
  -- will construct events in code if no taught event is found, using the base value in NV22, 23 for the DN
  -- optionally, per NV1, it inserts new constructed events into the event table (which must be read back & configured by FCU)
  -- per NVs 2 and 3, can produce gate output state events at startup

  Accessory event requests:

  -- responds to AREQ/ASRQ requests for current gate output states
  -- the message's EN is the requested gate number, per Phil's numbering system

  NV map (25 of 30 configured);

  1         -- insert constructed (short) events into the event table, 0 = no, 1 = yes (default: 0)
  2         -- store gate input state changes in EEPROM, 0 = no, 1 = yes (default: 0)
  3         -- restore gate input states at startup, depends on NV2, 0 = no, 1 = yes (default: 0)
  4         -- send gate output state events at startup, 0 = no, 1 = yes (default: 0)
  5         -- SoD delay (in seconds)
  6 - 13    -- 8 digital outputs to drive LEDs (or other things) that mimic the state of gate outputs. The NV value is the gate number/10 to mimic
  14 - 21   -- the pin numbers of the 8 digital outputs
  22, 23    -- base value for constructed short event DNs - so we can use multiple modules with non-overlapping DN ranges
  24        -- send long events if taught producer event has an NN >0 = no, 1 = yes (default: 0)
  25        -- delay between successive sent messages, in milliseconds
  26-30     -- reserved for future developments

  EV map (12 EVs per event):

  1         -- gate input number 1 - required to identify the gate and input, per Phil's numbering system (zero rpresents a SoD event, 255 for taught producer event)
  2         -- gate number - not used in this implementation because we can infer this value, but harmless and ignored if set
  3         -- invert gate output polarity (for NAND/NOR/XNOR behaviour), 0 = no, non-zero = yes (unused, set to zero)
  4         -- taught producer event DN MSB -- this is the gate output value this event will be produced for
  5         -- taught producer event DN LSB
  6         -- further gate input number 2, zero if unused
  7         -- further gate input number 3, zero if unused
  8         -- further gate input number 4, zero if unused
  9         -- gate input number 1 MSB
  10        -- gate input number 2 MSB
  11        -- gate input number 3 MSB
  12        -- gate input number 4 MSB

  Max 128 events, 16 bytes per event = 2048 bytes
  Module config consumes 20 + 30 + 2048 = 2098 bytes
  Persistent gate input states stored from 2100 onwards

  --

  Changes from original CANGATE behaviour:

  -- uses the standard Arduino CBUS library for portability to other platforms
  -- ev2 is not used and ignored
  -- ev3 is a simple boolean value, 0 = don't invert, non-zero = invert gate output
  -- complementary gate types are provided

*/

/*

  Arduino libraries required (for Uno/Nano R4):

  Arduino CBUS libraries:
  -- CBUS
  -- CBUSUNOR4
  -- CBUSLED
  -- CBUSswitch
  -- CBUSconfig

  3rd party libraries:
  -- UNOR4CAN
  -- Streaming

*/

/*

  Version log (CANGATEX until 1.7):

  1.0   - first release
  1.1   - fixed minor bug
  1.2   -
  1.3   - switch to MCP_CAN library, fewer gates, reduce memory
  1.4   - add complementary gates
  1.5   - misc fixes
  1.6   - refactoring, fix taught producer event construction, improve debug, save gate inputs

  1.7   - first version of CANGATEXL with 16-bit gate input numbers & throttle message transmission (tracking CANGATEX versioning)

*/


// standard Arduino header files
#include <EEPROM.h>                     // functions for accessing the chip's EEPROM

// CBUS library header files
#include <CBUSACAN2040.h>               // CAN controller and CBUS class
#include <CBUSswitch.h>                 // CBUS pushbutton switch class
#include <CBUSLED.h>                    // CBUS LED class
#include <CBUSconfig.h>                 // module configuration
#include <CBUSParams.h>                 // CBUS parameter class
#include <cbusdefs.h>                   // CBUS constants

// other 3rd party library headers
#include <Streaming.h>                  // serial output formatting

// local includes
#include "defs.h"                       // misc definitions
#include "logic_gates.h"                // gate definitions

// CBUS objects
CBUSConfig module_config;               // module configuration object
CBUSACAN2040 CBUS(&module_config);      // main CBUS object
CBUSLED ledGrn, ledYlw;                 // two LED objects
CBUSSwitch pb_switch;                   // switch object
CBUScoe coe(NUM_COE_BUFS);              // consume own events buffer

// module name, must be 7 characters, space padded
unsigned char mname[7] = { 'G', 'A', 'T', 'E', 'X', 'L', ' ' };

// forward function declarations
void event_handler(byte, CANFrame *);
void frame_handler(CANFrame *);
void transmit_handler(CANFrame *);
void set_gate_output_state(const uint16_t gate_num);
void process_serial_input(void);
void print_config(void);
int16_t find_taught_producer_event_by_dn(const uint16_t dn);
void produce_single_gate_output_event(const uint16_t gate_num, const bool self_consume = false);
void produce_all_gate_output_events(void);
void send_cbus_message(CANFrame *msg);
void set_digital_output(const uint16_t gate_num);
char *format_CAN_message(const CANFrame *msg);
void save_gate_input_state(const uint16_t gatenum);
void restore_gate_input_state(const uint16_t gatenum);
void print_gate_states(void);

// global variables
bool do_self_sod;                                   // whether to send self-SoD events at startup
byte self_sod_delay;                                // self-SoD delay in seconds after startup
uint16_t base_dn_value;                             // base DN for constructed events

const char *gate_type_names[] = {                   // gate type name lookup
  "BUFFER",
  "NOT",
  "AND",
  "NAND",
  "OR",
  "NOR",
  "XOR",
  "XNOR",
  "FLIP-FLOP",
  "JK FLIP-FLOP"
};

//
/// setup CBUS - runs once at power-on from setup()
//

void setupCBUS(void) {

  // set config layout parameters
  module_config.EE_NVS_START = 20;
  module_config.EE_NUM_NVS = 30;
  module_config.EE_EVENTS_START = 50;
  module_config.EE_MAX_EVENTS = 255;
  module_config.EE_NUM_EVS = 12;
  module_config.EE_BYTES_PER_EVENT = (module_config.EE_NUM_EVS + 4);

  // 12 + 4 = 16, * 256 = 4080, + 20 + 30 = 4130 bytes used
  // saved gate input state begins at 4200

  // initialise and load module configuration
  module_config.setEEPROMtype(EEPROM_INTERNAL);
  module_config.begin();

  Serial << F("> mode = ") << ((module_config.FLiM) ? "FLiM" : "SLiM") << F(", CANID = ") << module_config.CANID;
  Serial << F(", NN = ") << module_config.nodeNum << endl;

  // show code version and copyright notice
  print_config();

  // set module parameters
  CBUSParams params(module_config);
  params.setVersion(VER_MAJ, VER_MIN, VER_BETA);
  params.setModuleId(MODULE_ID);
  params.setFlags(PF_FLiM | PF_COMBI | PF_COE);
  params.setManufacturerId(MANU_DEV);

  // assign to CBUS
  CBUS.setParams(params.getParams());
  CBUS.setName(mname);

  // set CBUS LED pins and assign to CBUS
  ledGrn.setPin(LED_GRN_PIN);
  ledYlw.setPin(LED_YLW_PIN);
  CBUS.setLEDs(ledGrn, ledYlw);

  // initialise CBUS switch and assign to CBUS
  pb_switch.setPin(CBUS_SWITCH_PIN, LOW);
  pb_switch.run();
  CBUS.setSwitch(pb_switch);

  // module reset - if switch is depressed at startup and module is in SLiM mode
  if (pb_switch.isPressed() && !module_config.FLiM) {
    Serial << F("> switch was pressed at startup in SLiM mode") << endl;
    module_config.resetModule(ledGrn, ledYlw, pb_switch);
    // module reboots
  }

  // opportunity to set one-time module configuration items after module reset, e.g. events, NVs
  // reset will have cleared the event table and set all NVs to zero
  if (module_config.isResetFlagSet()) {
    Serial << F("> setting default configuration after module reset") << endl;

    // reset gate output saved states
    for (uint16_t e = SAVE_OFFSET; e < EEPROM.length(); e++) {
      EEPROM[e] = 0;
    }

    module_config.writeNV(25, 5);           // default interval between produced events
    module_config.clearResetFlag();
  }

  // register our CAN frame handler, to receive every CAN frame
  CBUS.setFrameHandler(frame_handler);

  // register our CBUS event handler, to receive only previously taught events
  CBUS.setEventHandler(event_handler);

  // register handler to display transmitted messages
  CBUS.setTransmitHandler(transmit_handler);

  // consume own events
  CBUS.consumeOwnEvents(&coe);

  // set CBUS LEDs to indicate the current mode
  CBUS.indicateMode(module_config.FLiM);

  // configure and start CAN bus and CBUS message processing
  CBUS.setPins(CAN_TX_PIN, CAN_RX_PIN);                             // can2040 TX and RX pins
  CBUS.setNumBuffers(NUM_RX_BUFS, NUM_TX_BUFS);                     // CAN send and receive buffers -- more buffers = more memory used

  Serial << F("> initialising CBUS") << endl;

  if (!CBUS.begin()) {
    Serial << F("> error initialising CBUS") << endl;
  } else {
    Serial << F("> CBUS initialised ok") << endl;
  }

  return;
}

//
/// setup - runs once at power on
//

void setup(void) {

  uint32_t t1 = 0;

  Serial.begin(115200);
  while (!Serial && millis() - t1 < 5000);
  Serial << endl << endl << F("> ** CBUS Arduino CANGATEXL module ** ") << __FILE__ << endl;

  // configure the CBUS library and CAN bus interface
  setupCBUS();

  // initialise digital outputs
  for (byte i = 0; i < NUM_DIGITAL_OUTPUTS; i++) {
    byte pin_num = module_config.readNV(PIN_BASE + i);
    if (pin_num > 0) {
      pinMode(pin_num, OUTPUT);
      digitalWrite(pin_num, LOW);
    }
  }

  // if NVs 2 and 3 are set, load last gate input states from eeprom
  if (module_config.readNV(2) && module_config.readNV(3)) {
    Serial << F("> restoring gate input states from EEPROM") << endl;
    for (byte gate_num = 0; gate_num < (sizeof(gates) / sizeof(gates[0])); gate_num++) {
      restore_gate_input_state(gate_num);
    }
  }

  // set gate output states correctly, according to input states
  Serial << F("> setting gate output states according to current input states") << endl;

  for (uint16_t gate_num = 0; gate_num < (sizeof(gates) / sizeof(gates[0])); gate_num++) {
    set_gate_output_state(gate_num);
    set_digital_output(gate_num);
  }

  // calculate base DN for constructed produced events
  base_dn_value = (module_config.readNV(22) << 8) + module_config.readNV(23);
  Serial << F("> base DN for constructed events = ") << base_dn_value << endl;

  // load self-SoD configuration settings
  do_self_sod = module_config.readNV(4);
  self_sod_delay = module_config.readNV(5);

  Serial << F("> initial free SRAM = ") << module_config.freeSRAM() << F(" bytes") << endl;

  // end of setup
  Serial << F("> ready") << endl << endl;

  return;
}

//
/// loop - runs forever
//

void loop(void) {

  static bool self_sod_done = false;

  //
  /// do self-SoD once, if so configured, after a configurable delay
  //

  if (do_self_sod && !self_sod_done && (millis() >= (self_sod_delay * 1000))) {
    produce_all_gate_output_events();
    self_sod_done = true;
  }

  //
  /// do CBUS message, event, switch and LED processing
  //

  CBUS.process();

  //
  /// process console commands
  //

  process_serial_input();

  //
  /// bottom of loop()
  //

  return;
}

//
/// user-defined frame processing callback function
/// called from the CBUS library for every CAN frame received
/// it receives a pointer to the received CAN frame
/// don't handle taught events here, use the event_handler function instead
//

void frame_handler(CANFrame *msg) {

  size_t gate_num;
  CANFrame response_message;
  uint16_t requested_node_number = ((msg->data[1] << 8) + msg->data[2]);
  uint16_t requested_output_number = ((msg->data[3] << 8) + msg->data[4]);

  // print the formatted message
  Serial << F("> ->> ") << format_CAN_message(msg) << endl;

  // handle CBUS accessory event requests

  if ((msg->data[0] == OPC_AREQ && requested_node_number == module_config.nodeNum) || (msg->data[0] == OPC_ASRQ && requested_node_number == 0)) {

    bool match = false;

    Serial << F("> accessory request for state of gate number = ") << requested_output_number << endl;

    // search for matching gate
    for (gate_num = 0; gate_num < (sizeof(gates) / sizeof(gates[0])); gate_num++) {
      if (gates[gate_num].output_number == requested_output_number) {
        match = true;
        break;
      }
    }

    if (match) {
      Serial << F("> sending accessory response") << endl;
      response_message.len = 5;
      response_message.data[0] = msg->data[0] == OPC_AREQ ? (gates[gate_num].output_state ? OPC_ARON : OPC_AROF) : (gates[gate_num].output_state ? OPC_ARSON : OPC_ARSOF);
      memcpy(&response_message.data[1], &msg->data[1], 4);

      // send the response
      send_cbus_message(&response_message);

    } else {
      Serial << F("> *** gate not found ") << endl;
    }
  }

  return;
}

//
/// user-defined event processing callback function
/// called from the CBUS library whenever a previously taught event is received
/// it receives the event table index and a pointer to the incoming CAN frame
//

void event_handler(byte index, CANFrame *msg) {

  bool event_polarity;                                          // the polarity (on/off) of the consumed event
  bool match;                                                   // whether we found a matching gate and input
  bool prev_output_state;                                       // for comparison, to determine whether the gate's output has changed

  byte ev1;                                                     // the value of EV1 for this event
  uint16_t gate_num, input_num;                                 // array counters
  uint16_t gate_inputs[4];                                      // the (up to 4) gate inputs this event is plumbed into
  uint16_t nn, en;                                              // for debug

  ev1 = module_config.getEventEVval(index, 1);

  if (ev1 == 0) {                                               // this is a SoD event
    produce_all_gate_output_events();                           // produce an event for every gate
    return;
  }

  // per Phil's design, all input events for any one gate should have ev3 set identically
  // however, we can't enforce this
  // it is better to use the specific complmentary gates and always set EV3 to zero

  byte ev3 = module_config.getEventEVval(index, 3);             // whether to invert the gate output for e.g. NAND/NOR/XNOR behaviour, 0 = no, 1 = yes

  // the (up to) four gate inputs this event is 'wired' to

  gate_inputs[0] = ev1 + (module_config.getEventEVval(index, 9) << 8);
  gate_inputs[1] = module_config.getEventEVval(index, 6) + (module_config.getEventEVval(index, 10) << 8);
  gate_inputs[2] = module_config.getEventEVval(index, 7) + (module_config.getEventEVval(index, 11) << 8);
  gate_inputs[3] = module_config.getEventEVval(index, 8) + (module_config.getEventEVval(index, 12) << 8);

  event_polarity = !(msg->data[0] & 1);                         // CBUS accessory on opcodes are even numbered, off opcodes are odd numbered
  nn = (msg->data[1] << 8) + msg->data[2];                      // debug
  en = (msg->data[3] << 8) + msg->data[4];

  Serial << F("> incoming event: index = ") << index << F(", polarity = ") << event_polarity << F(", nn = ") << nn << F(", en = ") << en << F(", ev3 = ") << ev3 << endl;

  // loop through the four gate input evs
  for (byte j = 0; j < 4; j++) {

    if (gate_inputs[j] == 0) {                                  // ignore zero values - they won't match any gate inputs and we handled EV1 for SoD above
      continue;
    }

    Serial << F("> attempting to match on input = ") << j << F(", ev value = ") << gate_inputs[j] << endl;

    // find the gate and input that match this ev
    match = false;

    // loop through all configured gates and their inputs
    for (gate_num = 0; gate_num < (sizeof(gates) / sizeof(gates[0])); gate_num++) {
      for (input_num = 0; input_num < gates[gate_num].num_inputs; input_num++) {
        if (gates[gate_num].input_numbers[input_num] == gate_inputs[j]) {
          Serial << F("> got match for input = ") << input_num << F(", gate input num = ") << gate_inputs[j] << F(", at gate index = ") << gate_num << F(", gate output number = ") << gates[gate_num].output_number << endl;
          match = true;
          goto loop_exit;         // break out of nested loops
        }
      }
    }

loop_exit:

    // no match - try next EV
    if (!match) {
      Serial << F("> no gate matching this value") << endl;
      continue;
    }

    Serial << F("> processing change to gate input = ") << input_num << F(", type = ") << gates[gate_num].type << F("/") << gate_type_names[gates[gate_num].type] << F(", num inputs = ") << gates[gate_num].num_inputs << endl;

    // capture this gate's current output state for comparison later
    prev_output_state = gates[gate_num].output_state;

    // print the previous gate input states
    Serial << F("> gate input states were [ ");

    for (byte k = 0; k < gates[gate_num].num_inputs; k++) {
      Serial << gates[gate_num].input_states[k] << F(" ");
    }

    Serial << F("]") << endl;

    // set the state of the gate input depending on the event opcode polarity
    gates[gate_num].input_states[input_num] = event_polarity;

    // print the new gate input states
    Serial << F("> gate input states now  [ ");

    for (byte k = 0; k < gates[gate_num].num_inputs; k++) {
      Serial << gates[gate_num].input_states[k] << F(" ");
    }

    Serial << F("]") << endl;

    // per NV2, save the gate's input state to EEPROM
    if (module_config.readNV(2)) {
      save_gate_input_state(gate_num);
    }

    // this is the actual boolean logic processing
    // calculate the gate's output state, according to the new input state, gate type and number of inputs

    set_gate_output_state(gate_num);

    // flip the output state polarity if ev3 is set
    if (ev3) {
      Serial << F("> ev3 = ") << ev3 << F(", -- flipping output state") << endl;
      gates[gate_num].output_state = !gates[gate_num].output_state;
    }

    // if the gate's output state has changed as a consequence of this incoming event, send an output event
    if (gates[gate_num].output_state != prev_output_state) {
      Serial << F("> gate output state has changed = ") << gates[gate_num].output_state << F(" -> will send event") << endl;

      // produce the output event -- opcode polarity matches the gate's new output state
      produce_single_gate_output_event(gate_num, true);

      // set the digital output associated with this gate, if configured
      set_digital_output(gate_num);
    } else {
      Serial << F("> gate output state is unchanged = ") << gates[gate_num].output_state << endl;
    }

  } // for each of four possible input values

  Serial << endl;

  return;
}

//
/// transmit handler
//

void transmit_handler(CANFrame *msg) {

  // print the formatted message
  Serial << F("> <<- ") << format_CAN_message(msg) << endl;
  return;
}

//
/// set a gate's output state to match the current state of its inputs, according to boolean logic
//

void set_gate_output_state(uint16_t gate_num) {

  byte gate_sum = 0;

  for (byte k = 0; k < gates[gate_num].num_inputs; k++) {
    gate_sum += gates[gate_num].input_states[k];
  }

  switch (gates[gate_num].type) {

    // single input buffer gate - passes the input to the output, converting a consumed event to a produced one
    case BUFFER_GATE:
      gates[gate_num].output_state = gates[gate_num].input_states[0];
      break;

    // single input NOT gates - simple logic inverter
    case NOT_GATE:
      gates[gate_num].output_state = !(gates[gate_num].input_states[0]);
      break;

    // AND gates may have 2, 3 or more inputs
    case AND_GATE:
      gates[gate_num].output_state = (gate_sum == gates[gate_num].num_inputs);
      break;

    case NAND_GATE:
      gates[gate_num].output_state = !(gate_sum == gates[gate_num].num_inputs);
      break;

    case OR_GATE:
      gates[gate_num].output_state = (gate_sum > 0);
      break;

    case NOR_GATE:
      gates[gate_num].output_state = !(gate_sum > 0);
      break;

    // 2-input XOR gates - exclusive or, should only be 2-input
    case XOR_GATE:
      gates[gate_num].output_state = (gate_sum == 1);
      break;

    case XNOR_GATE:
      gates[gate_num].output_state = !(gate_sum == 1);
      break;

    // single input flip-flop or bistable - the output changes state each time any positive/on input event is received
    case FLIP_FLOP:
      gates[gate_num].output_state = !gates[gate_num].output_state;
      break;

    // 4-input JK flip-flop
    case JK_FLIP_FLOP:
      if (!gates[gate_num].input_states[0] && !gates[gate_num].input_states[1]) {
        // hold
        ;
      } else if (!gates[gate_num].input_states[0] && gates[gate_num].input_states[1]) {
        // reset
        gates[gate_num].output_state = 0;
      } else if (!gates[gate_num].input_states[0] && gates[gate_num].input_states[1]) {
        // set
        gates[gate_num].output_state = 1;
      } else if (gates[gate_num].input_states[0] && !gates[gate_num].input_states[1]) {
        // toggle
        gates[gate_num].output_state = !gates[gate_num].output_state;
      }
      break;

    default:
      Serial << F("> *** oops - unknown logic type for gate ") << gate_num << endl;
      break;

  }   // switch gate type
}

//
/// send an event with the opcode polarity representing the gate's output state and the DN as the gate's output value
/// the event may be read from the event table, or produced by the code
/// per NV1, the produced event may then be written to the event table, to be found in the future
/// events with an NN are sent as long if NV24 is set
//

void produce_single_gate_output_event(const uint16_t gate_num, const bool self_consume) {

  bool event_polarity;                                          // produced event opcode polarity -- on/off
  byte temp_event_data[4];                                      // taught event read from event table
  CANFrame output_event;                                        // the produced CBUS event

  Serial << F("> producing event for gate index = ") << gate_num << F(", output number = ") << gates[gate_num].output_number << endl;

  // determine the event polarity from the gate output state
  event_polarity = gates[gate_num].output_state;

  // lookup the producer event in the event table by matching the gate's output value with EVs 4 and 5
  int16_t idx = find_taught_producer_event_by_dn(gates[gate_num].output_number);

  if (idx >= 0) {                                               // a previously taught producer event was found in the event table, so we'll use it
    Serial << F("> using taught producer event from table index = ") << idx << endl;
    module_config.readEvent((uint8_t)idx, temp_event_data);

    Serial << F("> taught event = [ ");
    for (byte x = 0; x < 4; x++) {
      Serial << temp_event_data[x] << " ";
    }
    Serial << F("]") << endl;

    output_event.data[3] = temp_event_data[2];
    output_event.data[4] = temp_event_data[3];
  } else {                                                      // a previously taught event was not found, so we'll construct one using the gate's output number as the event's DN
    Serial << F("> constructing the producer event") << endl;

    // re-read the base DN value
    base_dn_value = (module_config.readNV(22) << 8) + module_config.readNV(23);

    // construct the event's DN
    uint16_t output_dn = base_dn_value + gates[gate_num].output_number;
    output_event.data[3] = (byte)(output_dn >> 8);
    output_event.data[4] = (byte)(output_dn & 0xff);
  }

  Serial << F("> DN = ") << ((output_event.data[3] << 8) + output_event.data[4]) << endl;

  // set the event length, long or short

  if (idx >= 0 && ((temp_event_data[0] << 8) + temp_event_data[1]) > 0 && module_config.readNV(24)) {       // send a long event if our NN is set in the taught event and NV24 is set
    Serial << F("> sending a long event") << endl;
    output_event.data[0] = (event_polarity ? OPC_ACON : OPC_ACOF);
    output_event.data[1] = temp_event_data[0];
    output_event.data[2] = temp_event_data[1];
  } else {
    Serial << F("> sending a short event") << endl;
    output_event.data[0] = (event_polarity ? OPC_ASON : OPC_ASOF);                              // otherwise send a short event
    output_event.data[1] = module_config.nodeNum >> 8;                                          // add our NN for traceability
    output_event.data[2] = module_config.nodeNum & 0xff;
  }

  // add the other parts of the event data
  output_event.len = 5;
  output_event.rtr = false;
  output_event.ext = false;

  // send the event
  send_cbus_message(&output_event);

  // place the event on the consume-own-events queue so we'll receive it as an incoming event
  if (self_consume) {
    coe.put(&output_event);
  }

  // optionally, per NV1, insert the newly constructed event into the module's event table
  // data will need to be sync'd and configured with the software tool (e.g. FCU)
  // by setting EVs 4 and 5, it will be found next time an event needs to be sent for this gate
  // setting the EVs in the software tool means this output event can then be wired to gate inputs

  if (module_config.readNV(1) && idx == -1) {

    byte nidx;
    uint16_t nn = 0;                                                                    // short events have no NN
    uint16_t en = (output_event.data[3] << 8) + output_event.data[4];

    output_event.data[1] = 0;                                                           // blank the NN
    output_event.data[2] = 0;

    Serial << F("> will insert new event into table") << endl;

    if (module_config.findExistingEvent(nn, en) == module_config.EE_MAX_EVENTS) {       // not already in the event table
      Serial << F("> event is not already in the table") << endl;

      if ((nidx = module_config.findEventSpace()) < module_config.EE_MAX_EVENTS) {      // we have a free slot in the table
        Serial << F("> inserting at index = ") << nidx << endl;
        module_config.writeEvent(nidx, &output_event.data[1]);                          // write the event NN and EN
        module_config.writeEventEV(nidx, 4, gates[gate_num].output_number >> 8);        // write the EV values
        module_config.writeEventEV(nidx, 5, gates[gate_num].output_number & 0xff);
        module_config.updateEvHashEntry(nidx);                                          // update the hash table
        Serial << F("> inserted event into the table") << endl;
      } else {
        Serial << F("> *** no free space in event table") << endl;
      }
    } else {
      Serial << F("> event is already in table") << endl;
    }
  } else {
    Serial << F("> not inserting this event") << endl;
  }

  return;
}

//
/// send an event for all gate output states, at SoD
//

void produce_all_gate_output_events(void) {

  Serial << F("> sending an event for all gate output states") << endl;

  for (byte i = 0; i < (sizeof(gates) / sizeof(gates[0])); i++) {
    produce_single_gate_output_event(i);
    delay(5);
  }

  return;
}

//
/// search the event table for an event matching the desired gate's output value
/// match the requested DN with a 16-bit value across EVs 4 and 5
/// returns the table index or -1 if not found
//

int16_t find_taught_producer_event_by_dn(const uint16_t dn) {

  int16_t ret = -1;

  for (int16_t idx = 0; idx < module_config.EE_MAX_EVENTS; idx++) {       // loop over the event table
    if (module_config.getEvTableEntry(idx)) {                             // this table slot is in use
      byte ev4 = module_config.getEventEVval(idx, 4);
      byte ev5 = module_config.getEventEVval(idx, 5);

      if ((uint16_t)((ev4 << 8) + ev5) == dn) {                           // compare with requested DN
        ret = idx;
        break;
      }
    }
  }

  return ret;                                                             // return the result
}

//
/// save a gate's input state to EEPROM
//

void save_gate_input_state(const uint16_t gatenum) {

  byte sum_of_inputs = 0;

  // calculate the EEPROM offset as the sum of input gates before this gate in the config list

  for (uint16_t i = 0; i < sizeof(gates) / sizeof(gates[0]); i++) {
    if (i == gatenum) {
      break;
    }

    sum_of_inputs += gates[i].num_inputs;
  }

  for (byte j = 0; j < gates[gatenum].num_inputs; j++) {
    EEPROM.update(SAVE_OFFSET + sum_of_inputs + j, gates[gatenum].input_states[j]);
  }

  return;
}

//
/// restore a gate's saved input state from EEPROM
//

void restore_gate_input_state(const uint16_t gatenum) {

  byte sum_of_inputs = 0;

  // calculate the EEPROM offset as the sum of input gates before this gate in the config list

  for (uint16_t i = 0; i < sizeof(gates) / sizeof(gates[0]); i++) {
    if (i == gatenum) {
      break;
    }

    sum_of_inputs += gates[i].num_inputs;
  }

  for (byte j = 0; j < gates[gatenum].num_inputs; j++) {
    gates[gatenum].input_states[j] = EEPROM[SAVE_OFFSET + sum_of_inputs + j];
  }

  return;
}

//
/// send a CBUS message - throttled
//

void send_cbus_message(CANFrame *msg) {

  static unsigned long last_message_sent_at = 0;

  msg->rtr = false;
  msg->ext = false;
  CBUS.makeHeader(msg);

  // apply inter-message delay, if required

  unsigned long last_message_sent_ago = millis() - last_message_sent_at;

  if (module_config.readNV(25) > last_message_sent_ago) {
    delay(module_config.readNV(25) - last_message_sent_ago);
  }

  // send the message

  if (CBUS.sendMessage(msg)) {
    Serial << F("> sent CBUS message ok") << endl;
    ledGrn.pulse();
  } else {
    Serial << F("> *** error sending CBUS message") << endl;
  }

  last_message_sent_at = millis();
  return;
}

//
/// set the state of a digital pin based on the output state of the given gate
/// based on gate number
//

void set_digital_output(const uint16_t gate_num) {

  // search NVs for a match with the requested gate number

  for (byte i = 0; i < NUM_DIGITAL_OUTPUTS; i++) {
    if (module_config.readNV(OUTPUT_BASE + i) == gates[gate_num].output_number / 10) {
      byte pin = module_config.readNV(PIN_BASE + i);
      Serial << F("> setting mimic pin = ") << pin << F(", for gate output number = ") << gates[gate_num].output_number << F(", to state = ") << gates[gate_num].output_state << endl;
      digitalWrite(pin, gates[gate_num].output_state);
      break;
    }
  }

  return;
}

//
/// return a CAN message as a formatted character string for display
//

char *format_CAN_message(const CANFrame *msg) {

  static char work_buffer[64];
  char tbuff[8];

  sprintf(work_buffer, "[%3lu] [%u] [ ", msg->id & 0x7f, msg->len);

  for (byte i = 0; i < msg->len && i < 8; i++) {
    sprintf(tbuff, "%02x ", msg->data[i]);
    strcat(work_buffer, tbuff);
  }

  strcat(work_buffer, "] ");
  strcat(work_buffer, msg->rtr ? "R" : "");
  strcat(work_buffer, msg->ext ? "X" : "");

  return work_buffer;
}

//
/// print current gate state info
//

void print_gate_states(void) {

  for (byte i = 0; i < (sizeof(gates) / sizeof(gates[0])); i++) {
    Serial << i << F(" ") << gate_type_names[gates[i].type] << F(" ") << gates[i].output_number << F(" [");
    for (byte j = 0; j < gates[i].num_inputs; j++) {
      Serial << gates[i].input_states[j];
    }
    Serial << F("] [") << gates[i].output_state << F("]") << endl;
  }

}

//
/// print code version config details and copyright notice
//

void print_config(void) {

  // code version
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;

  // copyright
  Serial << F("> © Duncan Greenwood (M5767), Philip Silver (M4082), 2022 - 2025") << endl;

  return;
}

//
/// command interpreter for serial console input
//

void process_serial_input(void) {

  static bool hex = true;
  byte uev = 0;
  char msgstr[32], dstr[32];

  if (Serial.available()) {

    char c = Serial.read();

    switch (c) {

      case 'n':

        // node config
        print_config();

        // node identity
        Serial << F("> CBUS node configuration") << endl;
        Serial << F("> mode = ") << (module_config.FLiM ? "FLiM" : "SLiM") << F(", CANID = ") << module_config.CANID << F(", node number = ") << module_config.nodeNum << endl;
        Serial << endl;
        break;

      case 'e':

        // EEPROM learned event data table
        Serial << F("> stored events ") << endl;
        Serial << F("  max events = ") << module_config.EE_MAX_EVENTS << F(", EVs per event = ") << module_config.EE_NUM_EVS << F(", bytes per event = ") << module_config.EE_BYTES_PER_EVENT << endl;

        for (byte j = 0; j < module_config.EE_MAX_EVENTS; j++) {
          if (module_config.getEvTableEntry(j) != 0) {
            ++uev;
          }
        }

        Serial << F("  stored events = ") << uev << F(", free = ") << (module_config.EE_MAX_EVENTS - uev) << endl;
        Serial << F("  using ") << (uev * module_config.EE_BYTES_PER_EVENT) << F(" of ") << (module_config.EE_MAX_EVENTS * module_config.EE_BYTES_PER_EVENT) << F(" bytes") << endl << endl;

        Serial << F("  Ev#  |  NNhi |  NNlo |  ENhi |  ENlo | ");

        for (byte j = 0; j < (module_config.EE_NUM_EVS); j++) {
          sprintf(dstr, "EV%03d | ", j + 1);
          Serial << dstr;
        }

        Serial << F("Hash |") << endl;

        Serial << F(" --------------------------------------------------------------") << endl;

        // for each event data line
        for (byte j = 0; j < module_config.EE_MAX_EVENTS; j++) {

          if (module_config.getEvTableEntry(j) != 0) {
            sprintf(dstr, "  %03d  | ", j);
            Serial << dstr;

            // for each data byte of this event
            for (byte e = 0; e < (module_config.EE_NUM_EVS + 4); e++) {
              if (hex) {
                sprintf(dstr, " 0x%02hx | ", module_config.readEEPROM(module_config.EE_EVENTS_START + (j * module_config.EE_BYTES_PER_EVENT) + e));
              } else {
                sprintf(dstr, " %4u | ", module_config.readEEPROM(module_config.EE_EVENTS_START + (j * module_config.EE_BYTES_PER_EVENT) + e));
              }

              Serial << dstr;
            }

            sprintf(dstr, "%4d |", module_config.getEvTableEntry(j));
            Serial << dstr << endl;
          }
        }

        Serial << endl;

        break;

      // NVs
      case 'v':

        // note NVs number from 1, not 0
        Serial << "> Node variables" << endl;
        Serial << F("   NV   Val") << endl;
        Serial << F("  --------------------") << endl;

        for (byte j = 1; j <= module_config.EE_NUM_NVS; j++) {
          byte v = module_config.readNV(j);
          sprintf(msgstr, " - %02d : %3hd | 0x%02hx", j, v, v);
          Serial << msgstr << endl;
        }

        Serial << endl << endl;

        break;

      case 'c':
        // CAN bus status
        CBUS.printStatus();
        break;

      case 'y':
        // reset CAN bus and CBUS message processing
        CBUS.reset();
        break;

      case '*':
        // reboot
        module_config.reboot();
        break;

      case 'm':
        // free memory
        Serial << F("> free SRAM = ") << module_config.freeSRAM() << F(" bytes") << endl;
        break;

      case 'g':
        // print gate info
        print_gate_states();
        break;

      case 'h':
        hex = true;
        break;

      case 'd':
        hex = false;
        break;

      case '\r':
      case '\n':
        Serial << endl;
        break;

      default:
        // Serial << F("> unknown command ") << c << endl;
        break;
    }
  }

  return;
}
