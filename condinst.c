/*
  condinsts.c - Final Version

  This module adds conditional instructions to the the MSP430 emulator. This is
  accomplished by using the decision table shown here which shows when an
  instruction is to be executed. The conditional instructions use the ifarray
  table to determine if their respective condition is met.

  +----------------+--------+--------+--------+--------+--------+
  |                | Rule 1 | Rule 2 | Rule 3 | Rule 4 | Rule 5 |
  +----------------+--------+--------+--------+--------+--------+
  | Condition =    |    T   |    T   |    F   |    F   |    -   |
  +----------------+--------+--------+--------+--------+--------+
  | Then Count > 0 |    T   |    F   |    T   |    F   |    F   |
  +----------------+--------+--------+--------+--------+--------+
  | Else Count > 0 |    -   |    T   |    -   |    T   |    F   |
  +----------------+--------+--------+--------+--------+--------+
  | Then Count --  |    X   |        |    X   |        |        |
  +----------------+--------+--------+--------+--------+--------+
  | Else Count --  |        |    X   |        |    X   |        |
  +----------------+--------+--------+--------+--------+--------+
  | Execute        |    X   |        |        |    X   |    X   |
  +----------------+--------+--------+--------+--------+--------+

  Coder: Elias Vonapartis
  Releade Date: July 24 2016
  Updates: July 26, 2016  - Modified the code to be table driven
                            *addition of struct pattern, ifarray[], check_CC
                            *removal of instruction functions
*/

#include <stdio.h>
#include "emulator.h"     //For debug bit
#include "srcontrol.h"
#include "condinst.h"
#include "machine.h"

struct pattern ifarray[MAXCC] = {
  {1, {0x0002, 0, 0}, {0x0002, 0, 0}},                      //EQ
  {1, {0x0002, 0, 0}, {0x0000, 0, 0}},                      //NE
  {1, {0x0001, 0, 0}, {0x0001, 0, 0}},                      //CS
  {1, {0x0001, 0, 0}, {0x0000, 0, 0}},                      //CC
  {1, {0x0004, 0, 0}, {0x0004, 0, 0}},                      //MI
  {1, {0x0004, 0, 0}, {0x0000, 0, 0}},                      //PL
  {1, {0x0100, 0, 0}, {0x0100, 0, 0}},                      //VS
  {1, {0x0100, 0, 0}, {0x0000, 0, 0}},                      //VC
  {1, {0x0003, 0, 0}, {0x0001, 0, 0}},                      //HI
  {2, {0x0001, 0x0002, 0}, {0x0000, 0x0002, 0}},            //LS
  {2, {0x0104, 0x0104, 0}, {0x0104, 0x0000, 0}},            //GE
  {2, {0x0104, 0x0104, 0}, {0x0004, 0x1000, 0}},            //LT
  {2, {0x0106, 0x0106, 0}, {0x0104, 0x0000, 0}},            //GT
  {3, {0x0002, 0x0104, 0x0104}, {0x0002, 0x0004, 0x0100}},  //LE
  {1, {0, 0, 0}, {0, 0, 0}},                                //AL, checks 0  = 0
  {1, {0, 0, 0}, {0x1, 0, 0}}                               //NV, checks 0 != 1
};

#ifdef debug
char* names[MAXCC] = {"IFEQ", "IFNE", "IFCS", "IFCC", "IFMI", "IFPL", "IFVS",
                      "IFVC", "IFHI", "IFLS", "IFGE", "IFLT", "IFGT", "IFLE",
                      "IFAL", "IFNV"};
#endif /*debug*/

void check_CC(unsigned short cond){
  unsigned short i, srbits;

  srbits = CCMASK(reg_file[SR]);    //Retrieve V, N, Z, C
  srptr->COND = CLEAR;              //If the bits don't match COND will be CLEAR

#ifdef debug
  printf("%s.\n", names[cond]);
  print_SR();
#endif /*debug*/

  for(i = 0; i < ifarray[cond].count; i++){
#ifdef debug
    printf("Anding SR bits %04x with mask %04x, res: %04x.\n",
    srbits, ifarray[cond].mask[i], (((ifarray[cond].mask[i])) & srbits));
    printf("Comparing to %04x.\n", ifarray[cond].expected[i]);
#endif /*debug*/
    if((((ifarray[cond].mask[i])) & srbits) == ifarray[cond].expected[i]){
#ifdef debug
    printf("Succesful comparison.\n\n");
#endif /*debug*/
      srptr->COND = SET;
      return;
    }
  }
}
/*
  This function represents the table included in the module's header.
*/
void decisiontable(void){
  //Rule 5
  if((then_cnt == 0) && (else_cnt == 0)){
#ifdef debug
    printf("Rule 5 applied\n");
#endif
    srptr->EX = SET;
    srptr->COND = CLEAR;            //Reset Condition bit to allow SR changes
  }
  else{
    //Rules 1 and 2
    if(srptr->COND){                //Condition TRUE
      if(then_cnt > 0){             //Rule 1
#ifdef debug
      printf("Rule 1 applied.\n");
#endif /*debug*/
        then_cnt--;                 //Decrement Then Count
        srptr->EX = SET;            //Execute Instruction
      }
      else if(else_cnt > 0){        //Rule 2
#ifdef debug
        printf("Rule 2 applied.\n");
#endif /*debug*/
        else_cnt--;                 //Decrement Else Count
        srptr->EX = CLEAR;          //Do not Execute Instruction
      }
    }
    //Rules 3 and 4
    else{                           //Condition FALSE
      if(then_cnt > 0){             //Rule 3
#ifdef debug
        printf("Rule 3 applied.\n");
#endif /*debug*/
        then_cnt--;                 //Decrement Then Count
        srptr->EX = CLEAR;          //Do not Execute Instruction
      }
      else if(else_cnt > 0){        //Rule 4
#ifdef debug
        printf("Rule 4 applied.\n");
#endif /*debug*/
        else_cnt--;                 //Decrement Else Count
        srptr->EX = SET;            //Execute Instruction
      }
    }
  }
}
