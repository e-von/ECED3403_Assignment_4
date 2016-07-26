/*
  oneaddrinst.c
  Module containing functions for all supported one operand instructions.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: July 25, 2016 - Added safeguards in some instructions to avoid
                                  SR alteration in conditional execution.
*/

#include <stdio.h>
#include <stdint.h>
#include "oneaddrinst.h"
#include "emulator.h"
#include "machine.h"
#include "cache.h"
#include "addrmodes.h"
#include "srcontrol.h"
#include "debugger.h"
#include "condinst.h"

//tested, working
void rrc(unsigned short bw, unsigned short as, unsigned short reg){
  unsigned short operand, c, dummy_val;

  operand = get_operand(as, reg, bw, FALSE, &dummy_val);  //Get operand
  c = srptr->C;                                           //Load C for new MSB

  if(!(srptr->COND)){
    srptr->C = LSB(operand);                                //New C is old LSB
  }

  switch (bw) {
    case BYTE:
    operand = RSHIFT(LOWBYTE(operand)) | PLACEBIT(c, MSBBYTE);
    break;
    case WORD:
    operand = RSHIFT(operand) | PLACEBIT(c, MSBWORD);
    break;
  }
  put_operand(as, reg, operand, bw);
  update_sr(operand, 0, bw, ZNV);

  if(trace){
    oa_dbg_printer("RRC", bw, as, reg, operand);
  }
}
//tested, working C->MSB->MSB-1 ... LSB+1->LSB->C
void swpb(unsigned short bw, unsigned short as, unsigned short reg){
  unsigned short operand, swapped, dummy_val;

  operand = get_operand(as, reg, bw, FALSE, &dummy_val);
  swapped = SWAPBYTES(operand);
  put_operand(as, reg, swapped, bw);

  if(trace){
    oa_dbg_printer("SWPB", bw, as, reg, swapped);
  }
}
//tested, working   MSB->MSB, MSB->MSB-1, MSB-1->MSB-2, ... LSB->C
void rra(unsigned short bw, unsigned short as, unsigned short reg){
  unsigned short operand, temp, dummy_val;

  operand = get_operand(as, reg, bw, FALSE, &dummy_val);

  if(!(srptr->COND)){
    srptr->C = LSB(operand);                          //Loading C bit from LSB
  }

  switch (bw) {
    case BYTE:
    temp = MSB_B(operand);                          //Get MSB to retain it
    operand = RSHIFT(LOWBYTE(operand)) | temp;      //OR shifted OP with MSB
    break;
    case WORD:
    temp = MSB_W(operand);                          //Same for WORD
    operand = RSHIFT(operand) | temp;
    break;
  }
  put_operand(as, reg, operand, bw);                //Place the new operand
  update_sr(operand, 0, bw, ZNV);                   //Update bits Z, N, V

  if(trace){
    oa_dbg_printer("RRA", bw, as, reg, operand);
  }
}
//tested, working
void sxt(unsigned short bw, unsigned short as, unsigned short reg){
  uint16_t operand, dummy_val;
  unsigned char shift, bit;

  operand = get_operand(as, reg, bw, FALSE, &dummy_val);   //Get the operand
  if(bit = MSB_B(operand)){                               //Extract sign bit
    operand |= HIGH_BITS;                       //Saves byte and sets 8-15 to 1
  }
  else{
    operand = LOWBYTE(operand);         //Ensures 8 - 15 are 0, by ANDing low
  }

  put_operand(as, reg, operand, bw);    //Place the new operand
  update_sr(operand, 0, bw, ZNV);       //Update Zero, Negative, Overflow

  if(!(srptr->COND)){
    srptr->Z ? CLEAR_C : SET_C;         //Special Carry Case
  }

  if(trace){
    oa_dbg_printer("SXT", bw, as, reg, operand);
  }
}
//tested, working
void push(unsigned short bw, unsigned short as, unsigned short reg){
  uint16_t operand, dummy_val;

  operand = get_operand(as, reg, bw, TRUE, &dummy_val);   //src->tmp
  reg_file[SP] -= WORDINC;                                //SP-2->SP
  CACHE_WR(reg_file[SP], operand, WORD);                  //tmp->@SP

  if(trace){
    oa_dbg_printer("PUSH", bw, as, reg, operand);
  }
}
//tested, working
void call(unsigned short bw, unsigned short as, unsigned short reg){
  uint16_t operand, address;

  COND_RESET;                                             //Reset the Conditions

  operand = get_operand(as, reg, WORD, TRUE, &address);   //dst->tmp
  reg_file[SP] -= WORDINC;                                //SP-2->SP
  CACHE_WR(reg_file[SP], reg_file[PC], WORD);             //PC->@SP
  reg_file[PC] = address;                                 //tmp->PC

  if(trace){
    oa_dbg_printer("CALL", bw, as, reg, address);
  }
}
//tested, working
void reti(void){
  CACHE_RD(reg_file[SP], reg_file[SR], WORD);   //TOS->SR
  reg_file[SP] += WORDINC;                      //SP+2->SP
  CACHE_RD(reg_file[SP], reg_file[PC], WORD);   //TOS->PC
  reg_file[SP] += WORDINC;                      //SP+2->SP

  if(trace){
    printf("\n------------------------\nRETI INST\n------------------------\n");
  }
}
