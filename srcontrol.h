/*
  srcontrol.h
  Header file for srcontrol.c.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: July 25, 2016 - Added two bits to the SR, COND and EX
*/

#ifndef SRCONTROL_H
#define SRCONTROL_H

#define SET   1
#define CLEAR 0

#define POS  1
#define NEG  0
#define SUBTRACTION 0
#define ADDITION    1
#define XOR         2
#define DADD        3

#define MSBITB(x)   ((x) & 0x0080)  //MSB for BYTE, i.e. the sign
#define MSBITW(x)   ((x) & 0x8000)  //MSB for WORD, i.e. the sign
#define NMSBITB(x)  ((x) & 0x00FF)  //Bits 0 to 8
#define NMSBITW(x)  ((x) & 0xFFFF)  //Bits 0 to 15
#define CARRYB(x)   ((x) & 0x0100)  //The carry would be one beyond the byte len
#define CARRYW(x)   ((x) & 0x10000) //32-bit storage used for detecting carry

#define SET_C   (srptr->C = SET)
#define CLEAR_C (srptr->C = CLEAR)
#define SET_Z   (srptr->Z = SET)
#define CLEAR_Z (srptr->Z = CLEAR)
#define SET_N   (srptr->N = SET)
#define CLEAR_N (srptr->N = CLEAR)
#define SET_V   (srptr->V = SET)
#define CLEAR_V (srptr->V = CLEAR)

#define mask_carry(x)    ((x) & 0x1)
#define mask_zero(x)     ((x) & 0x2)
#define mask_negative(x) ((x) & 0x4)
#define mask_overflow(x) ((x) & 0x8)

#define ZNV               0xE
#define CZN               0x7
#define ZN                0x6

enum srbits;
extern struct sr_bits* srptr;

struct sr_bits{
  unsigned char C:1;
  unsigned char Z:1;
  unsigned char N:1;
  unsigned char GIE:1;
  unsigned char C_off:1;
  unsigned char OSC_off:1;
  unsigned char SCG0:1;
  unsigned char SCG1:1;
  unsigned char V:1;
  unsigned char COND: 1;
  unsigned char EX: 1;
  unsigned char reserved:5;
};

/* Function Declarations */
unsigned char check_SR(enum srbits );
unsigned char check_value_carry(unsigned int, unsigned char);
unsigned char check_value_negative(unsigned short, unsigned char);
unsigned char check_value_zero(unsigned short, unsigned char);
void update_sr(unsigned short, unsigned int, unsigned char,
  unsigned short);
void arithm_op_sr(unsigned short, unsigned int, unsigned char, unsigned short,
  unsigned short, unsigned char, unsigned short);
void print_SR(void);

#endif /* SRCONTROL_H */
