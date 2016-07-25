/*
  bus.h
  Header file for bus.c. Contains some macros and definitions.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#ifndef BUS_H
#define BUS_H

/* Definitions */
#define READ  0
#define WRITE 1

/* Updating system clock on each bus usage (read or write) */
#define MEM_ACCESS_TIME 10
#define DEV_ACCESS_TIME 15

/* Function Declarations */
void bus(unsigned short , unsigned short* , unsigned char , unsigned char );

/* Macros */
#define MEM_RD(address, data, bw) bus(address, &data, bw, READ)
#define MEM_WR(address, data, bw) bus(address, &data, bw, WRITE)

#endif /* BUS_H */
