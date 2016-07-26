/*
  addrmodes.h
  Header file for addrmodes.c. Contains function declarations.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: July 25, 2016 - pc_calculator declaration
*/

#ifndef ADDRMODES_H
#define ADDRMODES_H

#define COND_INC_PC(x) (x) ? reg_file[PC] += WORDINC : 0

/* Function Declarations */
void retrieve_operands(unsigned short*, unsigned short*, unsigned char,
  unsigned short, unsigned short, unsigned short, unsigned short, unsigned char,
  unsigned char);
unsigned short get_operand(unsigned, unsigned, unsigned, unsigned,
  unsigned short *);
void put_operand(unsigned, unsigned, unsigned short, unsigned char);
void pc_calculator(unsigned, unsigned, unsigned, unsigned, unsigned char);

#endif /* ADDRMODES_H */
