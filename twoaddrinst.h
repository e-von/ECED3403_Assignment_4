/*
  twoaddrinst.h
  Module for the MSP430 emulator containing the execution methods for
  instructions which use two operands. These functions are accessed through an
  array of pointers to functions located in machine.c.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/


#ifndef TWOADDRINST_H
#define TWOADDRINST_H

#include <stdint.h>  //Inclusion of this allows uint16_t in the calculations

/* Macro calculations for two-operand instructions */
#define ONESCOMP(x) (~(x))
#define TWOSCOMP(x) (ONESCOMP(x) + 1)
#define ADD(x, y)   ((x) + (y))
#define SUB(x, y)   ADD((TWOSCOMP(x)), (y))
#define SUBC(x, y)  ADD((ONESCOMP(x)), (y)) //this is what the manual says. 
#define AND(x, y)   ((x) & (y))

/* Linking the external pointer to the SR register bits */
extern struct sr_bits* srptr;

/* Function declarations */
void mov(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void add(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void addc(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void subc(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void sub(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void cmp(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void dadd(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void bit(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void bic(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void bis(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void xor(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
void and(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);
uint16_t and_fn(unsigned short, unsigned short, unsigned short, unsigned short,
  unsigned short);

#endif  /* TWOADDRINST_H */
