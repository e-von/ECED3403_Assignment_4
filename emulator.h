/*
  emulator.h
  Header for the MSP430 emulator header.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#ifndef EMULATOR_H
#define EMULATOR_H

/* Testing */
#define debug

/* Definitions */
#define ON    1
#define OFF   0

#define TRUE  1
#define FALSE 0

#define PRIVATE static

/* Memory Control */
#define WORD  0
#define BYTE  1

/* Maximum Memory Locations */
#define MAX_MEM   65536

/* Globals */
unsigned char memory[MAX_MEM];
struct sr_bits* srptr;
unsigned char cpos;
extern FILE* infile;

/* Function Declarations */
void initialize(int argc, char* argv[]);
void terminate(void);

#endif  /* EMULATOR_H */
