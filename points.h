
#pragma once

// gate type enumeration

enum {
  UNKNOWN,
  NORMAL,
  REVERSE,
  LOCKED_NORMAL,
  LOCKED_REVERSE
};

// point data structure

typedef struct {
  uint16_t external_number;                 // The number by which the point is known on the layout and the control panel
  uint8_t internal_number;                  // number by which the point is known internally to the CANPOINT (and used to create produced events)
  uint8_t current_state;                    // the current state of the point (driven by the enum above)
} point;

// global lpoints array

point points[] = {

  { 2322, 1, UNKNOWN },   
  { 2323, 1, UNKNOWN },   
  { 2324, 1, UNKNOWN },   
  { 2325, 1, UNKNOWN },   
  { 2326, 1, UNKNOWN },   
  { 2327, 1, UNKNOWN },   
  { 2328, 1, UNKNOWN },   
  { 2329, 1, UNKNOWN },   

};
