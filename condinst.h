/*
  condinsts.h
  Header file for condinsts.c

  Coder: Elias Vonapartis
  Releade Date: July 24 2016
  Updates: July 26, 2016 - Added struct pattern, removed inst declarations
*/

#ifndef CONDINST_H
#define CONDINST_H

/* Definitions */
#define MAXCC 16
#define CCMASK(x) ((x) & 0x107)

struct pattern{
  unsigned short count;       //Count of potential bit patterns
  unsigned short mask[3];     //Array of masks to extract the bits of interest
  unsigned short expected[3]; //Array of expected results
};

/* Global Conditional Counters */
unsigned short then_cnt;
unsigned short else_cnt;

/* Conditional Reset Macro */
#define COND_RESET  {\
                    srptr->COND = CLEAR;\
                    then_cnt = 0;\
                    else_cnt = 0;\
                    }

/* Function Declarations */
void check_CC(unsigned short);
void decisiontable(void);

#endif /* CONDINSTS_H */
