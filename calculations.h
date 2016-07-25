/*
  calculations.h
  Header file for calculations.c

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#ifndef CALCULATIONS_H
#define CALCULATIONS_H

/* Extract nibble from byte 'x' by shifting by 'y' (0 or 4) */
#define NIBBLE(x, y)     (((x) >> (y)) & 0x0f)

/* Function Declarations */
unsigned short bcd_add(unsigned short, unsigned short, unsigned short,
  unsigned short*, unsigned short*);
unsigned int bcd_exerciser(unsigned short, unsigned short, unsigned char);

#endif /* CALCULATIONS_H */
