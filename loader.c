/*
  loader.c
  Module containing code for extracting opcode from the s-records.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#include <stdio.h>
#include <stdlib.h>
#include "loader.h"
#include "emulator.h"
#include "machine.h"
#include "bus.h"
#include "debugger.h"

void load(int argc, char* argv[]){
  char srec[SREC_LEN];
  char buffer[WORD_LEN];
  unsigned int length;
  unsigned int address;
  unsigned short byte;
  unsigned int type;
  unsigned short pos;
  unsigned short i, k;
  unsigned char S9flag = TRUE;

  /* The following ensures file accessibility */
  if (argc < 3){
    printf("Format: ./emulator '(-options)' 'srecord' 'dev file'\n");
    exit(1);
  }
  if((infile = fopen(argv[cpos], "r")) == NULL){
    printf("File %s could not be opened\n", argv[cpos]);
  }
  else{
    while(fgets(srec, SREC_LEN, infile) != NULL){
      i = k = 0;
      pos = DATAPOS;
      srec[SREC_LEN] = NUL;

      sscanf(&srec[START], "%1d%2x%4x", &type, &length, &address);

      if(type == S9){           // The S9 record, if it exists, contains the
        reg_file[PC] = address; // starting addr. So we initialize the PC here.
        S9flag = FALSE;         // Else we use the default starting address
      }

      #ifdef debug
      printf("\nSREC: %s", srec);
      printf("Type: %d\n", type);
      printf("Length: %02x\n", length);
      printf("Address: %04x\n", address);
      #endif /* debug */

      while(i < (length - 3)){
        sscanf(&srec[pos], "%2hx", &byte);  //Uses bus in CPU; increment cycles
        MEM_WR((address + i), byte, BYTE);  //in the emulator, don't use cache
        #ifdef debug
        printf("*loader* memory[%04x]: %02x\n",(address + i),memory[address + i]);
        #endif
        pos += WORDINC;
        i++;
      }
    }
    if(S9flag){
      printf("WARNING: Missing starting address in the assembly code. "
      "Execution will start at address 0x0000.\n");
    }
  }
  return;
}
