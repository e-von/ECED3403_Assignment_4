/*
  machine.c
  The main module controlling the functionality of the machine. Instruction
  fetching, decoding and calls for execution take place here.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#include <stdio.h>
#include <stdlib.h>
#include "machine.h"
#include "emulator.h"
#include "cache.h"
#include "oneaddrinst.h"
#include "twoaddrinst.h"
#include "condinst.h"
#include "jumpinst.h"
#include "srcontrol.h"
#include "devices.h"
#include "debugger.h"
#include "addrmodes.h"

//I initialize SR here, to enable GIE and EX bits by default
unsigned short reg_file[] = {
  0x0000, 0x0000, 0x0408, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  //PC, SP, SR, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15

//Three arrays of pointers to functions, used in the run_machine loop
void (*oa_ptr[])(unsigned, unsigned, unsigned) = {
  rrc, swpb, rra, sxt, push, call, reti};

void (*cond_ptr[])(void) = {
  ifeq, ifne, ifcs, ifcc, ifmi, ifpl, ifvs, ifvc, ifhi, ifls, ifge, iflt,
  ifgt, ifle, ifal, ifnv};

void (*ta_ptr[])(unsigned, unsigned, unsigned, unsigned, unsigned) = {
  mov, add, addc, subc, sub, cmp, dadd, bit, bic, bis, xor, and};

void (*jp_ptr[])(unsigned) = {
  jne_jnz, jeq_jz, jnc_jlo, jc_jhs, j_n, jge, jl, jmp};

unsigned short fetch(unsigned short address){  //Returns item for the loop in
  unsigned short instruction;                  //run_machines
  decisiontable();                             //Decision Table for Conditionals
  CACHE_RD(address, instruction, WORD);
  return instruction;
}

void run_machine(void){
  unsigned short inst_reg;
  unsigned short regnum, i;

  while(inst_reg = fetch(reg_file[PC])){

    if(trace){
      printf("\n------------------------New Fetch------------------------:\n"
      "inst_reg: %04x sys_clk: %ld PC: %04x SR: %04x SP: %04x EX: %d\n",
      inst_reg, sys_clk, reg_file[PC], reg_file[SR], reg_file[SP], srptr->EX);
    }

    if(dbg){                                //Only if user used -d option
      check_dbg_breaks();                   //Loop through breaks
    }

    reg_file[PC] += WORDINC;                //Now increment PC, after dbg check

    switch (OPCODE_TYPE(inst_reg)) {        //Sorts masked instructions
      case ONEOP:
      if(COND_CHK(inst_reg)){
        then_cnt = COND_TC(inst_reg);       //Extract Then Else counters
        else_cnt = COND_FC(inst_reg);
        printf("Εντωπίστηκε Conditional Instruction.\n");
        printf("Thens:%d Elses:%d\n", then_cnt, else_cnt);
        cond_ptr[COND_MASK(inst_reg)]();
      }
      else{
        if(srptr->EX){
          oa_ptr[OA_MASK(inst_reg)](OA_BW(inst_reg), OA_AS(inst_reg),
          OA_REG(inst_reg));
        }
        else{
          pc_calculator(OA_AS(inst_reg), 0, OA_REG(inst_reg), 0, ONEOP);
        }
      }
      break;
      case JUMP:
      if(srptr->EX){
        jp_ptr[JP_MASK(inst_reg)](JP_OFF(inst_reg));
      }
      else{
        reg_file[PC] += WORDINC;    //always increment by 2
      }
      break;
      default: /* double-op */
      if(srptr->EX){
        ta_ptr[TA_MASK(inst_reg)](TA_SREG(inst_reg), TA_AD(inst_reg),
        TA_BW(inst_reg), TA_AS(inst_reg), TA_DREG(inst_reg));
      }
      else{
        pc_calculator(TA_AS(inst_reg), TA_AD(inst_reg), TA_SREG(inst_reg),
        TA_DREG(inst_reg), TWOOP);
      }
      break;
    }

    if(trace){                          //If trace was set at debug setup
      printf("Instruction Executed.\n"
      "sys_clk: %ld PC: %04x SR: %04x SP: %04x EX: %d COND: %d\n",
      sys_clk, reg_file[PC], reg_file[SR], reg_file[SP], srptr->EX, srptr->COND);
      cache_print();
      breakpoint_hdlr();                //Call the handler for debug options
    }
    check_for_status_change();          //Check for interrupts, after inst
  }
  printf("\nInstructions complete. Total CPU cycles: %ld\n", sys_clk);
}

void signalCPUfault(void){
  printf("ERROR: Emulator encountered fatal error at PC: %d.\n", reg_file[PC]);
  exit(1);
}
