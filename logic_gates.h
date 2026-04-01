
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

// gate type enumeration

enum {
  BUFFER_GATE,
  NOT_GATE,
  AND_GATE,
  NAND_GATE,
  OR_GATE,
  NOR_GATE,
  XOR_GATE,
  XNOR_GATE,
  FLIP_FLOP,
  JK_FLIP_FLOP
};

// logic gate data structure

typedef struct {
  uint8_t type;                             // AND_GATE, OR_GATE, XOR_GATE, NOT_GATE, etc
  uint8_t num_inputs;                       // number of inputs to the gate -- 1-8
  uint16_t input_numbers[MAX_INPUTS];       // the ev value associated with each input, per Phil's numbering system
  bool input_states[MAX_INPUTS];            // the current logical state of each input
  uint16_t output_number;                   // the output number of the gate, per Phil's numbering system (10x the gate number)
  bool output_state;                        // the current logical output state, according to the inputs and the gate logic
} logic_gate;

// global logic gates array
// note: we don't store the gate number as this can be inferred from the output value

logic_gate gates[] = {

  // TRACK CIRCUIT 809
  // Route 12414 and track occupied --> 12310
  { AND_GATE, 2,  {1, 2}, {0, 0}, 110, 0 },   

  // Route 12415 or 12416
  { OR_GATE, 2, {3, 4}, {0, 0}, 120, 0 },   

  // Gate 120 and track occupied --> 12309  
  { AND_GATE, 2, {5, 6}, {0, 0}, 130, 0 },    

  // None of routes 12414 or gate 120
  { NOR_GATE, 2, {7, 8}, {0, 0}, 140, 1 },    

  // No route set and track occupied --> 12308
  { AND_GATE, 2, {9, 10}, {1, 0}, 150, 0 },   

  //-----------------------------------------------------------------------------------------------------------------------

  // TRACK CIRCUIT 810
  // Route 12410 or 12411 or 12422 or 12426
  { OR_GATE, 4, {21, 22, 23, 24}, {0, 0, 0, 0}, 210, 0}, 

  // Route 12409 or 12418  
  { OR_GATE, 2, {25, 26}, {0, 0}, 220, 0 },   

  // Route 12401 or 12402 or 12404 or 12405 or 12407 or 12408 or 12423 or 12424 or 12425 or 12427 or 12428 or 12429
  { OR_GATE, 12, {27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 230, 0},   
                                                                                                                    
  // Route 12400 or 12403 or 12406 or 12419 or 12420 or 12421
  { OR_GATE, 6, {39, 40, 41, 42, 43, 44}, {0, 0, 0, 0, 0, 0}, 240, 0},   

  // Gate 210 and track occupied --> 12312
  { AND_GATE, 2,  {45, 46}, {0, 0}, 250, 0 },   

  // Gate 220 and track occupied --> 12313
  { AND_GATE, 2,  {47, 48}, {0, 0}, 260, 0 },   

  // Gate 230 and track occupied --> 12314
  { AND_GATE, 2,  {49, 50}, {0, 0}, 270, 0 },   

  // Gate 240 and track occupied (via gate 320) --> 12315
  { AND_GATE, 2,  {51, 52}, {0, 0}, 280, 0 },  

  // Route 12414 and track occupied (via gate 320) --> 12316
  { AND_GATE, 2,  {53, 54}, {0, 0}, 290, 0 },  

  // None of gate 210, gate 220, gate230, gate 240 or route 12414
  { NOR_GATE, 5, {55, 56, 57, 58, 59}, {0, 0, 0, 0, 0}, 300, 1 },    

  // Gate 300 and track occupied (via gate 320) --> 12311
  { AND_GATE, 2, {60, 61}, {1, 0}, 310, 0 },  

  // Buffer for track occupied (493 / 6) to feed more than four gates
  { BUFFER_GATE, 1, {62}, {0}, 320, 0 },    

  //-----------------------------------------------------------------------------------------------------------------------

  // TRACK CIRCUIT 811
  // Route 12402 or 12405 or 12408 or 12411 or 12426 or 12427 or 12428 or 12429   
  { OR_GATE, 8, {71, 72, 73, 74, 75, 76, 77, 78}, {0, 0, 0, 0, 0, 0, 0, 0}, 410, 0}, 

  // Route 12401 or 12404 or 12407 or 12410 or 12422 or 12423 or 12424 or 12425
  { OR_GATE, 8, {79, 80, 81, 82, 83, 84, 85, 86}, {0, 0, 0, 0, 0, 0, 0, 0}, 420, 0}, 

  // Gate 410 and track occupied --> 12320
  { AND_GATE, 2,  {87, 88}, {0, 0}, 430, 0 },   

  // Gate 420 and track occupied --> 12321
  { AND_GATE, 2,  {89, 90}, {0, 0}, 440, 0 },    

  // Route 12416 and track occupied --> 12318
  { AND_GATE, 2,  {91, 92}, {0, 0}, 450, 0 },     

  // Route 12415 and track occupied (via gate 490) --> 12319
  { AND_GATE, 2,  {93, 94}, {0, 0}, 460, 0 },   

  // None of gate 410, gate 420, route 12415 or route 12416
  { NOR_GATE, 4, {95, 96, 97, 98}, {0, 0, 0, 0}, 470, 1 },    

  // Gate 470 and track occupied (via gate 490) --> 12317
  { AND_GATE, 2, {99, 100}, {1, 0}, 480, 0 },  

  // Buffer for track occupied (493 / 5) to feed more than four gates
  { BUFFER_GATE, 1, {101}, {0}, 490, 0 },

  //-----------------------------------------------------------------------------------------------------------------------

  // TRACK CIRCUIT 817
  // Route 12412 or 12430  
  { OR_GATE, 2, {111, 112}, {0, 0}, 510, 0}, 

  // Route 12426 or 12427 or 12428 or 12429  
  { OR_GATE, 4, {113, 114, 115, 116}, {0, 0, 0, 0}, 520, 0}, 

  // Gate 510 and track occupied --> 12328
  { AND_GATE, 2,  {117, 118}, {0, 0}, 530, 0 },   

  // Gate 520 and track occupied --> 12329
  { AND_GATE, 2,  {119, 120}, {0, 0}, 540, 0 },    

  // None of gate 510 or gate 520
  { NOR_GATE, 2, {121, 122}, {0, 0}, 550, 1 },    

  // Gate 550 and track occupied --> 12327
  { AND_GATE, 2, {123, 124}, {1, 0}, 560, 0 } ,

  //-----------------------------------------------------------------------------------------------------------------------

  // TRACK CIRCUIT 820
  // Route 12406 or 12407 or 12408 or 12419 or 12423 or 12427 
  { OR_GATE, 6, {131, 132, 133, 134, 135, 136}, {0, 0, 0, 0, 0, 0}, 610, 0}, 

  // Route 12403 or 12404 or 12405 or 12420 or 12424 or 12428
  { OR_GATE, 6, {137, 138, 139, 140, 141, 142}, {0, 0, 0, 0, 0, 0}, 620, 0}, 

  // Route 12400 or 12401 or 12402 or 12421 or 12425 or 12429
  { OR_GATE, 6, {143, 144, 145, 146, 147, 148}, {0, 0, 0, 0, 0, 0}, 630, 0}, 

  // Gate 610 and track occupied --> 12333
  { AND_GATE, 2,  {149, 150}, {0, 0}, 640, 0 },   

  // Gate 620 and track occupied --> 12334
  { AND_GATE, 2,  {151, 152}, {0, 0}, 650, 0 },   

  // Gate 630 and track occupied --> 12335
  { AND_GATE, 2,  {153, 154}, {0, 0}, 660, 0 },      

  // None of gate 610, gate 620 or gate 630
  { NOR_GATE, 3, {155, 156, 157}, {0, 0, 0}, 670, 1 },    

  // Gate 670 and track occupied --> 12332
  { AND_GATE, 2, {158, 159}, {1, 0}, 680, 0 } 

};
