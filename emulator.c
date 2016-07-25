/*
  emulator.c
  Main module for the MSP430 emulator. Call initialize, run and termination
  here.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#include <stdio.h>
#include <stdlib.h>
#include "emulator.h"
#include "loader.h"
#include "machine.h"
#include "devices.h"
#include "srcontrol.h"
#include "debugger.h"

int main(int argc, char* argv[]){
  initialize(argc, argv);                       //Initialize emulator
  run_machine();                                //Fetch, decode, execute
  terminate();                                  //Close files and end main(2)
}

void initialize(int argc, char* argv[]){
  check_dbg_input(argc, argv);                  //Set up the debugger if needed
  init_devices(argv);                           //Initialize the devices
  load(argc, argv);                             //Load S-record into memory
  srptr = (struct sr_bits* ) & reg_file[SR];    //Initialize the SR pointer
}

void terminate(void){
  fclose(infile);                               //Close S-records
  fclose(indev);                                //Close device input
  fclose(outdev);                               //Close device output
  printf("Machine has succesfully been terminated.\n");
  exit(0);                                      //Exit without errors
}
