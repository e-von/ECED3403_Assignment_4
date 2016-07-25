/*
  oneaddrinst.h
  Header file for oneaddrinst.c.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#ifndef ONEADDRINST_H
#define ONEADDRINST_H

/* Definitions */
#define MSBWORD         15
#define MSBBYTE         7
#define HIGH_BITS       0xFF00
#define LOW_BITS        0x00FF

/* Macros */
#define HIGHBYTE(x)     ((x) & 0xFF00)
#define LOWBYTE(x)      ((x) & 0x00FF)
#define MSB_B(x)        ((x) & 0x0080)
#define MSB_W(x)        ((x) & 0x8000)
#define LSB(x)          ((x) & 0x0001)
#define PLACEBIT(x, y)  ((x) << (y))
#define RBYTESHIFT(x)   ((x) >> 8)
#define LBYTESHIFT(x)   ((x) << 8)
#define SWAPBYTES(x)    (RBYTESHIFT(HIGHBYTE(x)) | LBYTESHIFT(LOWBYTE(x)))

/* Linking the SR pointer to this module */
extern struct sr_bits* srptr;

/* Functions Declarations */
void rrc(unsigned short, unsigned short, unsigned short);
void swpb(unsigned short, unsigned short, unsigned short);
void rra(unsigned short, unsigned short, unsigned short);
void sxt(unsigned short, unsigned short, unsigned short);
void push(unsigned short, unsigned short, unsigned short);
void call(unsigned short, unsigned short, unsigned short);
void reti(void);

#endif  /* ONEADDRINST_H */
