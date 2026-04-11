
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
  bool detected_normal;
  bool detected_reverse;
  bool lockedNormal;
  bool lockedReverse;
} point;

// global points array

point points[] = {

  { 2322, 1, { { 0, false }, { 0, false } }, { { 4, false }, { 5, false } }, { 1, 2 }, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2323, 2, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, { 5, 0 }, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2324, 3, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, { 5, 0 }, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2325, 4, { { 0, false }, { 0, false } }, { { 1, false }, { 0, false } }, { 2, 0 }, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2326, 5, { { 0, false }, { 0, false } }, { { 1, false }, { 0, false } }, { 2, 0 }, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2327, 6, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, { 3, 0 }, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2328, 7, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, { 3, 4 }, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2329, 8, { { 0, false }, { 0, false } }, { { 7, false }, { 0, false } }, { 4, 0 }, 0, 0, 0, 0, 0, 0, 0, 0 },

};

// track circuit status array

typedef struct {
  uint16_t external_number;                 
  uint8_t internal_number; 
  bool occupied;
} trackCircuit;

trackCircuit trackCircuits[] = {

  { 809, 1, 1 },
  { 810, 2, 1 },
  { 811, 3, 1 },
  { 817, 4, 1 },
  { 820, 5, 1 }

};