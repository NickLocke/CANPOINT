
#pragma once

// point data structure

const int MAX_POINT_LOCKS = 2;    // The maximum number of points which can lock another point
const int MAX_TRACK_CIRCUITS = 2; // The maximum number of track circuits which can lock a single point

typedef struct {
  uint8_t internal_number;
  bool required;  // False = normal, True = reverse
} locking;

typedef struct {
  uint16_t external_number;                  // The number by which the point is known on the layout and the control panel
  uint8_t internal_number;                   // number by which the point is known internally to the CANPOINT (and used to create produced events)
  locking normal_locking[MAX_POINT_LOCKS];   // Point to point locking (normal)
  locking reverse_locking[MAX_POINT_LOCKS];  // Point to point locking (reverse)
  uint8_t track_locking[MAX_TRACK_CIRCUITS]; // Track circuits which lock the points (internal number)
  bool switch_normal;
  bool switch_reverse;
  bool normal_route_called;
  bool reverse_route_called;
  bool detected_normal_A_end;
  bool detected_reverse_A_end;
  bool detected_normal_B_end;  // If there is no B end, this must be set to true to imply permanent detection
  bool detected_reverse_B_end; // If there is no B end, this must be set to true to imply permanent detection
} point;

// global points array

point points[] = {

//  Number       Normal 1      Normal 2          Reverse 1     Reverse 2      Tracks   SW N   SW R   Rte N  Rte R  Det NA Det RA Det NB Det RB 
  { 2322, 1, { { 0, false }, { 0, false } }, { { 4, false }, { 5, false } }, { 1, 2 }, false, false, false, false, false, false, false, false },
  { 2323, 2, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, { 5, 0 }, false, false, false, false, false, false, true,  true },
  { 2324, 3, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, { 5, 0 }, false, false, false, false, false, false, true,  true },
  { 2325, 4, { { 0, false }, { 0, false } }, { { 1, false }, { 0, false } }, { 2, 0 }, false, false, false, false, false, false, false, false },
  { 2326, 5, { { 0, false }, { 0, false } }, { { 1, false }, { 0, false } }, { 2, 0 }, false, false, false, false, false, false, true,  true },
  { 2327, 6, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, { 3, 0 }, false, false, false, false, false, false, true,  true },
  { 2328, 7, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, { 3, 4 }, false, false, false, false, false, false, false, false },
  { 2329, 8, { { 0, false }, { 0, false } }, { { 7, false }, { 0, false } }, { 4, 0 }, false, false, false, false, false, false, true,  true }

};

// track circuit status array

typedef struct {
  uint16_t external_number;                 
  uint8_t internal_number; 
  bool occupied;
} trackCircuit;

trackCircuit trackCircuits[] = {

  { 809, 1, false },
  { 810, 2, false },
  { 811, 3, false },
  { 817, 4, false },
  { 820, 5, false }

};