/*
  debugger.c
  A simple debugger which allows stepping through the execution and memory
  dumping.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: July 20, 2016 - Added support cache printing.
*/

#include <stdio.h>
#include <stdlib.h>   //Included for the exit function
#include <getopt.h>   //Included for option parsing
#include <string.h>   //Included for string comparing
#include "debugger.h"
#include "emulator.h" //Included for cpos global
#include "machine.h"  //Included for the register file printing
#include "srcontrol.h"//Included for print_SR()
#include "cache.h"

void check_dbg_input(int argc, char* argv[]){
  int c;
  cpos = 2;       //If there is no debug option, make cpos 2 for dev.in file
  dbg = FALSE;    //We do not know whether the debugger option has been set
  trace = FALSE;  //FALSE until coder enter the trace command

  while((c = getopt(argc, argv, "d")) != -1){
    switch (c) {
      case 'd':
      dbg = TRUE;   //Flag for debugging
      cpos = 3; //If there is a debug option set, make cpos 3 for dev.in file
      dbg_set_up(); //Allow user to set breakpoints, or enable stepping
      return;
      case '?': //No need for a debug message. getopt prints one by default
      exit(1);
    }
  }
}
/*
  This function allows for the initial setup of the debugger in the terminal.
  The user can indicate that he wants to trace the program or add break points
  at specific locations in the code.
*/
void dbg_set_up(void){
  char option[BUFFER_SZ];
  char buffer[BUFFER_LEN];
  unsigned short i = 0, k = 0;
  unsigned int value;

  printf("\nThe debugger supports breakpoints, tracing and memory inspection.\n"
  "* To add up to 16 breakpoints type 'break'.\n"
  "* For simply tracing through all instructions type 'trace'.\n"
  "* To exit debugger setup type 'done'.\n"
  "* Memory can be printed at breakpoints with the 'print' command.\n");

  while(TRUE){
    scanf("%s", option);
    if(strcasecmp("break", option) == 0){
      printf("Enter the PC of the breakpoint.\n");
      scanf("%x", &value);
      if(value <= MAX_MEM){
        breakpoints[i] = value;
        printf("PC break at 0x%04x. Remaining breakpoints: %d. "
        "Enter another command.\n", breakpoints[i], (BREAKS - i));
        i++;
      }
      else{
        printf("Invalid PC. The MSP430 accepts PCs only between 0 & 0xFFFE\n"
        "Enter another command.\n");
      }
    }
    else if(strcasecmp("trace", option) == 0){
      printf("Debugger will now step through instructions.\n");
      trace = TRUE;   //flag to print at the end of run_machine()
      return;
    }
    else if(strcasecmp("done", option) == 0){
      return;
    }
    else{
      printf("Unkown debugger command.\n"); //Indicate unknown command, loop
    }
  }
}
//Used in one operand instructions to print instruction information
void oa_dbg_printer(unsigned char* inst, unsigned short bw, unsigned short as,
  unsigned short reg, unsigned short res){
  printf("\n------------------------\n%s INST\n"
  "Reg: %d As: %d Bw: %d\n"
  "Result is %04x\n------------------------\n",
  inst, reg, as, bw, res);
}
//Used in two operand instructions to print instruction information
void ta_dbg_printer(unsigned char* inst, unsigned short sreg,unsigned short ad,
  unsigned short bw, unsigned short as, unsigned short dreg,unsigned short src,
  unsigned short dst, unsigned short res){
  printf("\n------------------------\n%s INST\n"
  "Sreg: %d Ad: %d Bw: %d As: %d Dreg: %d\n"
  "Retrieved SRC: %04x DST: %04x\n"
  "Result is %04x\n------------------------\n",
  inst, sreg, ad, bw, as, dreg, src, dst, res);
}
//Used for printing specific sections of memory
void print_memory(unsigned short base, unsigned short end){
  unsigned int i;
  for(i = base; i <= end; i++){
    printf("Memory[%04x]: %02x\n", i, memory[i]);
  }
}
//Used for printing all register contents
void print_registers(unsigned short low, unsigned short high){
  unsigned short regnum;
  printf("REGISTER CONTENTS\n");
  for(regnum = low; regnum <= high; regnum++){
    printf("reg_file[%d]: %04x\n", regnum, reg_file[regnum]);
  }
}

void check_dbg_breaks(void){
  unsigned short i;
  for(i = 0; i <= BREAKS; i++){         //Loop through array
    if(reg_file[PC] == breakpoints[i]){
      breakpoint_hdlr();                //Call the handler for debug options
    }
  }
}

//This function is called when a break point is encountered in the code.
void breakpoint_hdlr(void){
  char option[BUFFER_SZ];
  unsigned char mem_flag = FALSE, cache_flag = FALSE, reg_flag = FALSE;
  unsigned short low, high, rlow, rhigh;
  char* regs = NULL;

  printf("Break at location %04x. Commands available: 'print', 'next', 'trace'",
  reg_file[PC]);

  if(trace){
    printf(", 'quit' (trace).\n");
  }
  else{
    printf(".\n");                 //Included just for aesthetics
  }

  while(TRUE){
    scanf("%s", option);
    if((strcasecmp("print", option) == 0)||(strcasecmp("p", option) == 0)){

      printf("Print Memory? [Y/n]\n");
      scanf("%s", option);
      if(strcasecmp("Y", option) == 0){
        mem_flag = TRUE;
        printf("Enter range of memory in hex to be examined with a space "
        "between the lower and upper limits.\n");
        scanf("%hx %hx", &low, &high);
      }

      printf("Print Registers? [Y/n]\n");
      scanf("%s", option);
      if(strcasecmp("Y", option) == 0){
        reg_flag = TRUE;
        printf("Enter lower and upper limits\n");
        scanf("%hu %hu", &rlow, &rhigh);
      }

      printf("Print Cache? [Y/n]\n");
      scanf("%s", option);
      if(strcasecmp("Y", option) == 0){
        cache_flag = TRUE;
      }

      if(mem_flag){
        print_memory(low, high);
      }
      if(cache_flag){
        cache_print();
      }
      if(reg_flag){
        print_registers(rlow, rhigh);
        print_SR();
      }
      printf("Enter another command.\n");
    }
    else if((strcasecmp("trace", option) == 0)||(strcasecmp("t", option) == 0)){
      trace = TRUE;
      printf("Enter another command.\n");
    }
    else if((strcasecmp("next", option) == 0)||(strcasecmp("n", option) == 0)){
      return;
    }
    else if((strcasecmp("quit", option) == 0)||(strcasecmp("q", option) == 0)){
      trace = FALSE;
      return;
    }
    else{
      printf("Unknown command. Enter another command.\n");
    }
  }
}
