/*
  machine.h
  Header file for machine.c

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#ifndef MACHINE_H
#define MACHINE_H

/*
  The macros below have been written to easily access specific bits from the
  MSP430 instruction set format. OPCODE_TYPE isolates the bits used for
  differentiating One Address (OA), Two Address (TA) and Jump (JP) instructions.
  The repeated code for the TA definitions is to allow for readability in the
  modules.
*/

/* Registers */
#define PC  0
#define SP  1
#define SR  2

/* Magic-Numberless */
#define ONEOP   0
#define JUMP    1
#define WORDINC 2
#define TWOOP   2   //Used for pc calculator conditionals

/* Macros */
#define INC_PC(x) (reg_file[PC] += (x))
#define RSHIFT(x) ((x) >> 1)
#define LSHIFT(x) ((x) << 1)

/* Masks */
#define OPCODE_TYPE(x)  ((x) >> 13)
#define OA_REG(x)       ((x) & 0x0F)
#define OA_AS(x)        (((x) >> 4) & 0x03)
#define OA_BW(x)        (((x) >> 6) & 0x01)
#define OA_MASK(x)      (((x) >> 7) & 0x07)
#define TA_DREG(x)      ((x) & 0x0F)
#define TA_AS(x)        (((x) >> 4) & 0x03)
#define TA_BW(x)        (((x) >> 6) & 0x01)
#define TA_AD(x)        (((x) >> 7) & 0x01)
#define TA_SREG(x)      (((x) >> 8) & 0x0F)
#define TA_MASK(x)      (((x) >> 12) - 4)  //Subtract 4 for correct array index
#define JP_OFF(x)       ((x) & 0x3FF)
#define JP_MASK(x)      (((x) >> 10) & 0x07)

/* Additions to masking for conditional instructions */
#define COND_CHK(x)     (((x) >> 10) & 0x01)  //Separate One Operand from COND
#define COND_MASK(x)    (((x) >> 4) & 0x0F)   //Isolate COND bits
#define COND_TC(x)      (((x) >> 2) & 0x03)   //Mask TC
#define COND_FC(x)      ((x) & 0x03)          //Mask FC

/* Function Declarations */
unsigned short fetch(unsigned short);
void run_machine(void);
void signalCPUfault(void);

/* Globals & Linking */
extern unsigned short reg_file[];
unsigned long sys_clk;

#endif /* MACHINE_H */
