
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

#pragma once

// constants
const byte VER_MAJ = 1;                 // code major version
const char VER_MIN = 'g';               // code minor version = 6
const byte VER_BETA = 0;                // code beta sub-version
const byte MODULE_ID = 25;             // CBUS module type
const byte MAX_INPUTS = 16;             // max number of inputs for any gate = 8

const byte LED_GRN_PIN = 2;             // CBUS green SLiM LED pin
const byte LED_YLW_PIN = 3;             // CBUS yellow FLiM LED pin
const byte CBUS_SWITCH_PIN = 5;         // CBUS push button switch pin

const byte CAN_TX_PIN = 9;              // CAN TX pin
const byte CAN_RX_PIN = 1;              // CAN RX pin

const byte NUM_RX_BUFS = 16;            // number of CAN receive buffers
const byte NUM_TX_BUFS = 16;            // number of CAN transmit buffers (if supported by CAN driver)
const byte NUM_COE_BUFS = 16;           // size of consume-own-events buffer

const byte NUM_DIGITAL_OUTPUTS = 8;     // number of digital outputs
const byte OUTPUT_BASE = 6;             // the first mimic gate number NV
const byte PIN_BASE = 14;               // the first mimic IO pin number NV

const unsigned int SAVE_OFFSET = 4200;  // base eeprom offset of saved gate output states

enum {
  STATE_OFF = 0,
  STATE_ON = 1
};
