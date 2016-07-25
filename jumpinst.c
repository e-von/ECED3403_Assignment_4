/*
  jumpinst.c
  Module containing functions for all supported jump instructions.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#include <stdio.h>
#include "machine.h"
#include "jumpinst.h"
#include "srcontrol.h"
#include "debugger.h"     //Included for trace flag
#include "condinst.h"    //Included for counter reset

enum srbits {C, Z, N, V};

/*
  All the functions below simply follow the structure of jump executions. If the
  required bit is set or clear (depending on the jump type) they change the PC
  to the offset*2.
*/
void jne_jnz(unsigned short offset){
  if(trace){
    printf("jne_jnz with half offset %x\n", offset);
  }
  if(!(check_SR(Z))){
    INC_PC(offset10to16(offset));
    COND_RESET;
  }
}

void jeq_jz(unsigned short offset){
  if(trace){
    printf("jeq_jz with half offset %x\n", offset);
  }
  if(check_SR(Z)){
    INC_PC(offset10to16(offset));
    COND_RESET;
  }
}

void jnc_jlo(unsigned short offset){
  if(trace){
    printf("jnc_jlo with half offset %x\n", offset);
  }
  if(!(check_SR(C))){
    INC_PC(offset10to16(offset));
    COND_RESET;
  }
}

void jc_jhs(unsigned short offset){
  if(trace){
    printf("jc_jhs with half offset %x\n", offset);
  }
  if(check_SR(C)){
    INC_PC(offset10to16(offset));
    COND_RESET;
  }
}

void j_n(unsigned short offset){         //named like this due to C function jn
  if(trace){
    printf("jn with half offset %x\n", offset);
  }
  if(check_SR(N)){
    INC_PC(offset10to16(offset));
    COND_RESET;
  }
}

void jge(unsigned short offset){
  if(trace){
    printf("jge with half offset %x\n", offset);
  }
  if(!(check_SR(N) ^ check_SR(V))){
    INC_PC(offset10to16(offset));
    COND_RESET;
  }
}

void jl(unsigned short offset){
  if(trace){
    printf("jl with half offset %x\n", offset);
  }
  if(check_SR(N) ^ check_SR(V)){
    INC_PC(offset10to16(offset));
    COND_RESET;
  }
}

void jmp(unsigned short offset){
  if(trace){
    printf("jmp with half offset %x\n", offset);
  }
  INC_PC(offset10to16(offset));
  COND_RESET;
}

signed short offset10to16(unsigned short off10){
  /* Convert 10-bit signed to 16-bit signed */
  if (off10 & SIGN10){
    return LSHIFT(SIGNEXT | (off10 & OFFMASK)); /* Negative */
  }
  else{
    return LSHIFT(off10 & OFFMASK);             /* Positive */
  }
}
