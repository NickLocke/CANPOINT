
#pragma once

// point data structure

typedef struct {
  uint16_t external_number;                 // The number by which the point is known on the layout and the control panel
  uint8_t internal_number;                  // number by which the point is known internally to the CANPOINT (and used to create produced events)
  bool switch_normal;                    
  bool switch_reverse;
  bool normal_route_called;
  bool reverse_route_called;
  bool detected_normal;
  bool detected_reverse;
  bool track_occupied;
} point;

// global points array

point points[] = {

  { 2322, 1 },   
  { 2323, 2 },   
  { 2324, 3 },   
  { 2325, 4 },   
  { 2326, 5 },   
  { 2327, 6 },   
  { 2328, 7 },   
  { 2329, 8 },   

};
