/*
  condinsts.c - Final Version

  This module adds conditional instructions to the the MSP430 emulator. It
  then follows the following decision table based on the emulator bits
  condition and execution.

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
*/

#include <stdio.h>
#include "srcontrol.h"
#include "condinst.h"

//Condition met if Z set, tested working
void ifeq(void){
  printf("IFEQ\n");
  srptr->COND = srptr->Z;
}

//Condition met if Z clear, tested working
void ifne(void){
  printf("IFNE\n");
  srptr->COND = !(srptr->Z);
}

//Condition met if C set
void ifcs(void){
  printf("IFCS\n");
  srptr->COND = srptr->C;
}

//Condition met if C clear
void ifcc(void){
  printf("IFCC\n");
  srptr->COND = !(srptr->C);
}

//Condition met if N set
void ifmi(void){
  printf("IFMI\n");
  srptr->COND = srptr->N;
}

//Condition met if N clear
void ifpl(void){
  printf("IFPL\n");
  srptr->COND = !(srptr->N);
}

//Condition met if V set
void ifvs(void){
  printf("IFVS\n");
  srptr->COND = srptr->V;
}

//Condition met if V clear
void ifvc(void){
  printf("IFVC\n");
  srptr->COND = !(srptr->V);
}

//Condition met if C set and Z clear, tested working
void ifhi(void){
  printf("IFHI\n");
  srptr->COND = ((srptr->C) && !(srptr->Z)) ? SET : CLEAR;
}

//Condition met if C clear or Z set
void ifls(void){
  printf("IFLS\n");
  srptr->COND = (!(srptr->C) && (srptr->Z)) ? SET : CLEAR;
}

//Condition met if N and V set or N and V clear
void ifge(void){
  printf("IFGE\n");
  if(((srptr->N) && (srptr->V)) || (!(srptr->N) && !(srptr->Z))){
    srptr->COND = SET;
  }
  else{
    srptr->COND = CLEAR;
  }
}

//Condition met if N set and V clear or N clear and V set
void iflt(void){
  printf("IFLT\n");
  if(((srptr->N) && !(srptr->V)) || (!(srptr->N) && (srptr->V))){
    srptr->COND = SET;
  }
  else{
    srptr->COND = CLEAR;
  }
}

//Condition met if Z clear and either (N set and V set) or (N clear and V clear)
void ifgt(void){
  printf("IFGT\n");
  if(!(srptr->Z)&&(((srptr->N)&&(srptr->V))||((!(srptr->N))&&(!(srptr->V))))){
    srptr->COND = SET;
  }
  else{
    srptr->COND = CLEAR;
  }
}

//Condition met if Z set or N set and V clear or N clear and V set
void ifle(void){
  printf("IFLE\n");
  if(((srptr->Z) || (srptr->N)) && (!(srptr->V) || (srptr->N)) && srptr->V){
    srptr->COND = SET;
  }
  else{
    srptr->COND = CLEAR;
  }
}

//Always execute "true"
void ifal(void){
  printf("IFAL\n");
  srptr->COND = SET;
}

//Always execute "false"
void ifnv(void){
  printf("IFNV\n");
  srptr->COND = CLEAR;
}

void decisiontable(void){
  //Rule 5
  if((then_cnt == 0) && (else_cnt == 0)){
    printf("Rule 5 applied\n");
    srptr->EX = SET;
    srptr->COND = CLEAR;            //Reset Condition bit to allow SR changes
  }
  else{
    //Rules 1 and 2
    if(srptr->COND){                //Condition TRUE
      if(then_cnt > 0){             //Rule 1
        printf("Rule 1 applied\n");
        then_cnt--;                 //Decrement Then Count
        srptr->EX = SET;            //Execute Instruction
      }
      else if(else_cnt > 0){        //Rule 2
        printf("Rule 2 applied\n");
        else_cnt--;                 //Decrement Else Count
        srptr->EX = CLEAR;          //Do not Execute Instruction
      }
    }
    //Rules 3 and 4
    else{                           //Condition FALSE
      if(then_cnt > 0){             //Rule 3
        printf("Rule 3 applied\n");
        then_cnt--;                 //Decrement Then Count
        srptr->EX = CLEAR;          //Do not Execute Instruction
      }
      else if(else_cnt > 0){        //Rule 4
        printf("Rule 4 applied\n");
        else_cnt--;                 //Decrement Else Count
        srptr->EX = SET;            //Execute Instruction
      }
    }
  }
}
