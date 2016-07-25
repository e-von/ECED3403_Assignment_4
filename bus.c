/*
  bus.c
  This module contains the bus emulation code for the MSP430. It supports
  transmission of 16 and 32 bit data and calls for special code when device
  mapped memory locations are accessed.

  Code: Provided by the ECED3403 class at Dalhousie University, modified by
        Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#include <stdio.h>
#include <stdlib.h>
#include "emulator.h"
#include "devices.h"
#include "machine.h"
#include "bus.h"

void bus(unsigned short address, unsigned short* data, unsigned char bw,
         unsigned char rw){

  //Check READ and WRITE general cases
  if(rw == READ){
    if(bw == BYTE){
      *data = memory[address];
    }
    else{
      *data = (memory[address + 1] << 8) | memory[address];
    }
  }
  else{ // rw == WRITE
    memory[address] = (*data) & 0xFF;
    if(bw == WORD){
      memory[address + 1] = (*data >> 8) & 0xFF;
    }
  }

  //Now check if we accessed a device, smaller than 32, to change sr bits
  if(address < DEVICE_LIMIT){
    dev_mem_access(address, *data, rw);
  }

  sys_clk += MEM_ACCESS_TIME;   //Simulate time spent waiting for bus
  return;
}
