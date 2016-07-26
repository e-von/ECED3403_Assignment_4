/*
  srcontrol.c
  Module which controls bit changes of the status register. Gets called by the
  instruction functions which also tells it through a hex value which bits
  should be altered.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: July 25, 2016 - Added safeguards to avoid SR changes with
                                  conditional executions
*/

#include <stdio.h>
#include "srcontrol.h"
#include "emulator.h"
#include <stdlib.h>

enum srbits {CARRY, ZERO, NEGATIVE, OVERFLOW};

//The main way to update the status register, it uses other smaller functions
//to determine if a bit should be set or cleared.
void update_sr(unsigned short val, unsigned int intval,
  unsigned char bw, unsigned short bits){
  #ifdef debug
  printf("Checking values val: %d intval: %u for bw: %d\n", val, intval, bw);
  #endif

  if(srptr->COND){
    printf("Altering the SR is forbidden in conditional instructions.\n");
    return;
  }

  if(mask_carry(bits)){
    check_value_carry(intval, bw) ? SET_C : CLEAR_C;
  }
  if(mask_zero(bits)){
    check_value_zero(val, bw) ? SET_Z : CLEAR_Z;
  }
  if(mask_negative(bits)){
    check_value_negative(val, bw) ? SET_N : CLEAR_N;
  }
  if(mask_overflow(bits)){
    CLEAR_V;
  }
}
//Called by two-operand instructions, updates the SR using update_sr and
//determines the overflow bit for special cases.
void arithm_op_sr(unsigned short value, unsigned int intval, unsigned char bw,
  unsigned short src, unsigned short dst, unsigned char type,
  unsigned short bits){
  unsigned char src_sign, dst_sign;

  #ifdef debug
  printf("Initial SR values: C:%d Z:%d N:%d V:%d EX:%d COND:%d\n",
  srptr->C, srptr->Z, srptr->N, srptr->V, srptr->EX, srptr->COND);
  #endif

  if(srptr->COND){
    printf("Altering the SR is forbidden in conditional instructions.\n");
    return;
  }

  update_sr(value, intval, bw, bits);    //Updates carry, zero and negative bits
  src_sign = check_value_negative(src, bw);   //Used unnecessarily for DADD
  dst_sign = check_value_negative(dst, bw);

  //check OVERFLOW, same for BYTE or WORD, differs for addition & subtractions
  switch (type) {
    case ADDITION:
    if((!src_sign) && (!dst_sign)){       //if src & dst positive
      (srptr->N) ? SET_V : CLEAR_V;       //and result is negative
    }
    else if((src_sign) && (dst_sign)){    //if src & dst negative
      (!(srptr->N)) ? SET_V : CLEAR_V;    //and result is positive
    }
    break;
    case SUBTRACTION:
    if((src_sign) && (!dst_sign)){        //src negative, dst positive
      (srptr->N) ? SET_V : CLEAR_V;       //and result is negative
    }
    else if((!src_sign) && (dst_sign)){   //src positive, dst negative
      (!(srptr->N)) ? SET_V : CLEAR_V;    //and result is positive
    }
    break;
    case XOR:                         //XOR case
    (srptr->Z) ? CLEAR_C : SET_C;     //If Z has been set we know C is cleared
    if(src_sign && dst_sign){
      SET_V;
    }
    break;
    default: /* DADD inst, undefined V, special C */
    switch (bw) {
      case BYTE:
      intval >= 0x99 ? SET_C : CLEAR_C;  //It's a BCD so we look at the hex val
      break;
      case WORD:
      intval >= 0x9999 ? SET_C : CLEAR_C;
      break;
    }
    break;
  }
  #ifdef debug
  printf("New SR values: C:%d Z:%d N:%d V:%d\n",
  srptr->C, srptr->Z, srptr->N, srptr->V);
  #endif
}

unsigned char check_SR(enum srbits bit){
  unsigned char status;

  #ifdef debug
  printf("SR Values: C:%d Z:%d N:%d V:%d\n",
  srptr->C, srptr->Z, srptr->N, srptr->V);
  #endif

  switch (bit) {
    case CARRY:
    status = srptr->C;
    break;
    case ZERO:
    status = srptr->Z;
    break;
    case NEGATIVE:
    status = srptr->N;
    break;
    case OVERFLOW:
    status = srptr->Z;
    break;
  }
  return status;
}

unsigned char check_value_carry(unsigned int value, unsigned char bw){
  switch (bw) {
    case BYTE:
    return CARRYB(value) ? TRUE : FALSE;    //Need to restructure these
    case WORD:
    return (abs(value) > 0xFFFF) ? TRUE: FALSE;
  }
}

unsigned char check_value_zero(unsigned short value, unsigned char bw){
  switch (bw) {
    case BYTE:
    return !NMSBITB(value);
    case WORD:
    return !NMSBITW(value);
  }
}

unsigned char check_value_negative(unsigned short value, unsigned char bw){
  unsigned char res = FALSE;

  switch (bw) {
    case BYTE:
    res = MSBITB(value) ? TRUE : FALSE;   //Not structured like the previous
    break;                                //conditionals due to comp error
    case WORD:
    res = MSBITW(value) ? TRUE : FALSE;
    break;
  }
  return res;
}

void print_SR(void){
  printf("SR Values: C:%d Z:%d N:%d GIE:%d C-off:%d V:%d COND: %d EX: %d\n",
  srptr->C, srptr->Z, srptr->N, srptr->GIE, srptr->C_off, srptr->V,
  srptr->COND, srptr->EX);
}
