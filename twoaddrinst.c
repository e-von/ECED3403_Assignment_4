/*
  twoaddrinst.c
  Module for the MSP430 emulator containing the execution methods for
  instructions which use two operands. These functions are accessed through an
  array of pointers to functions located in machine.c.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: July 25, 2016 - Added safeguards to avoid SR alteration in
                                  conditional execution.
*/

#include <stdio.h>
#include "twoaddrinst.h"
#include "emulator.h"
#include "machine.h"
#include "addrmodes.h"
#include "srcontrol.h"
#include "calculations.h"
#include "debugger.h"

//tested, working
void mov(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src;
  uint16_t dummy_val;

  src = get_operand(as, sreg, bw, TRUE, &dummy_val);   //Get operand and increment PC
  put_operand(ad, dreg, src, bw);

  if(trace){
    ta_dbg_printer("MOV", sreg, ad, bw, as, dreg, src, 0, 0);
  }
}
//tested, working
void add(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t res, src, dst;   //Forced unsigned 16 bit, for wrap-around calcs
  uint32_t tmp;             //Forced unsigned 32 bit, used for carry detection

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  res = tmp = ADD(src, dst);  //Equating a short to an int returns correct value
  put_operand(ad, dreg, res, bw);
  arithm_op_sr(res, tmp, bw, src, dst, ADDITION, CZN);

  if(trace){
    ta_dbg_printer("ADD", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, working
void addc(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;     //Forced unsigned 16 bit, for wrap-around calcs
  uint32_t tmp;               //Forced unsigned 32 bit, used for carry detection

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  res = tmp = ADD(src, (dst + srptr->C));
  put_operand(ad, dreg, res, bw);
  arithm_op_sr(res, tmp, bw, src, dst, ADDITION, CZN);

  if(trace){
    ta_dbg_printer("ADDC", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, need to test carry
void subc(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;     //Forced unsigned 16 bit, for wrap-around calcs
  uint32_t tmp;               //Forced unsigned 32 bit, used for carry detection

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  printf("Carry bit is now %d with %d\n", srptr->C, (srptr->C) + dst);
  res = tmp = SUBC(src, (dst + srptr->C));
  arithm_op_sr(res, tmp, bw, src, dst, SUBTRACTION, CZN);
  put_operand(ad, dreg, res, bw);

  if(trace){
    ta_dbg_printer("SUBC", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, need to test carry
void sub(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;     //Forced unsigned 16 bit, for wrap-around calcs
  uint32_t tmp;               //Forced unsigned 32 bit, used for carry detection

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  res = tmp = SUB(src, dst);
  arithm_op_sr(res, tmp, bw, src, dst, SUBTRACTION, CZN);
  put_operand(ad, dreg, res, bw);

  if(trace){
    ta_dbg_printer("SUB", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, working
void cmp(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;
  uint32_t tmp;
  //In this function we increment PC after retrieving the second operand since
  //there is no storing of operands following the ALU access.
  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, TRUE);
  res = tmp = SUB(src, dst);
  arithm_op_sr(res, tmp, bw, src, dst, SUBTRACTION, CZN);

  if(trace){
    ta_dbg_printer("CMP", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, working
void dadd(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;
  uint32_t tmp;

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  res = tmp = bcd_exerciser(src, dst, bw);
  arithm_op_sr(res, tmp, bw, src, dst, DADD, ZN);
  put_operand(ad, dreg, res, bw);

  if(trace){
    ta_dbg_printer("DADD", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, working
void bit(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, TRUE);
  res = AND(src, dst);

  if(!(srptr->COND)){
    check_value_negative(res, bw) ? SET_N : CLEAR_N;
    if(check_value_zero(res, bw)){
      SET_Z; CLEAR_C;
    }
    else{
      CLEAR_Z; SET_C;
    }
    CLEAR_V;
  }

  if(trace){
    ta_dbg_printer("BIT", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, working
void bic(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  res = AND(ONESCOMP(src), dst);
  put_operand(ad, dreg, res, bw);

  if(trace){
    ta_dbg_printer("BIC", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, working
void bis(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  res = src | dst;                  //No need to macro this, no reuse
  put_operand(ad, dreg, res, bw);

  if(trace){
    ta_dbg_printer("BIS", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, working
void xor(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;

  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  res = src ^ dst;
  put_operand(ad, dreg, res, bw);
  arithm_op_sr(res, 0, bw, src, dst, XOR, ZN);

  if(trace){
    ta_dbg_printer("XOR", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
//tested, working
void and(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t res;
  res = and_fn(sreg, ad, bw, as, dreg);
  put_operand(ad, dreg, res, bw);
}
//common code for bit & and
uint16_t and_fn(unsigned short sreg, unsigned short ad, unsigned short bw,
  unsigned short as, unsigned short dreg){
  uint16_t src, dst, res;
  retrieve_operands(&src, &dst, bw, sreg, ad, as, dreg, TRUE, FALSE);
  res = AND(src, dst);

  if(!(srptr->COND)){
    check_value_negative(res, bw) ? SET_N : CLEAR_N;
    if(check_value_zero(res, bw)){
      SET_Z; CLEAR_C;
    }
    else{
      CLEAR_Z; SET_C;
    }
    CLEAR_V;
  }

  if(trace){
    ta_dbg_printer("AND", sreg, ad, bw, as, dreg, src, dst, res);
  }
}
