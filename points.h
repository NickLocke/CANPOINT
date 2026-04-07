
#pragma once

// point data structure

const int MAX_POINT_LOCKS = 2;

typedef struct {
  uint8_t internal_number;
  bool required;  // False = normal, True = reverse
} locking;

typedef struct {
  uint16_t external_number;                  // The number by which the point is known on the layout and the control panel
  uint8_t internal_number;                   // number by which the point is known internally to the CANPOINT (and used to create produced events)
  locking normal_locking[MAX_POINT_LOCKS];   // Point to point locking (normal)
  locking reverse_locking[MAX_POINT_LOCKS];  // Point to point locking (reverse)
  bool switch_normal;
  bool switch_reverse;
  bool normal_route_called;
  bool reverse_route_called;
  bool detected_normal;
  bool detected_reverse;
  bool track_occupied;
  bool lockedNormal;
  bool lockedReverse;
} point;

// global points array

point points[] = {

  { 2322, 1, { { 0, false }, { 0, false } }, { { 4, false }, { 5, false } }, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 2323, 2, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 2324, 3, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 2325, 4, { { 0, false }, { 0, false } }, { { 1, false }, { 0, false } }, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 2326, 5, { { 0, false }, { 0, false } }, { { 1, false }, { 0, false } }, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 2327, 6, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 2328, 7, { { 0, false }, { 0, false } }, { { 0, false }, { 0, false } }, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 2329, 8, { { 0, false }, { 0, false } }, { { 7, false }, { 0, false } }, 0, 0, 0, 0, 0, 0, 1, 0, 0 },

};
