/*
  debugger.h
  Header file for debugger.c containg declarations and definitions.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#ifndef DEBUGGER_H
#define DEBUGGER_H

/* Definitions */
#define BUFFER_LEN  20
#define BREAKS      15
#define BUFFER_SZ   5

/* Globals */
unsigned char dbg;                  //Indicator that the debugger has been set
unsigned char trace;                //Indicator that user wants to trace code
unsigned short breakpoints[BREAKS]; //Array of breakpoints

/* Function Declarations */
void check_dbg_input(int argc, char* argv[]);
void dbg_set_up(void);
void oa_dbg_printer(unsigned char*, unsigned short, unsigned short,
  unsigned short, unsigned short);
void ta_dbg_printer(unsigned char*, unsigned short, unsigned short,
  unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short, unsigned short);
void print_memory(unsigned short, unsigned short);
void print_registers(unsigned short, unsigned short);
void check_dbg_breaks(void);
void breakpoint_hdlr(void);

#endif /* DEBUGGER_H */
