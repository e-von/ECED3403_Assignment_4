/*
  calculations.c
  A binary coded decimal addition module.

  Coder: Provided by the ECED3403 class at Dalhousie University, modified by
        Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#include <stdio.h>
#include <stdlib.h>
#include "calculations.h"
#include "srcontrol.h"

unsigned short bcd_add(unsigned short sd, unsigned short dd, unsigned short c,
    unsigned short *sum, unsigned short *carry){

  *sum = sd + dd + c;
  if (*sum >= 10){    //10s carry
    *sum -= 10;       //Sum between 0 and 9
    *carry = 1;       //Carry set
  }else{              //No carry
    *carry = 0;
  }
}

//Add two BCD numbers plus carry from lower order nibbles
unsigned int bcd_exerciser(unsigned short n1, unsigned short n2,
  unsigned char bw){
  unsigned short temp = 0, /*res = 0,*/ i, j, k = 0, carry = 0;
  unsigned int res;

  carry = srptr->C;

  j = bw ? 2 : 4;           //Determine the number of cycles based on BW
  for(i = 1; i <= j; i++){
    bcd_add(NIBBLE(n1, k), NIBBLE(n2, k), carry, &temp, &carry);
    res = res | (temp << k);
    k += 4;            
  }
  res |= (carry << k);    //k is shifted

  printf("%x + %x = %x with c: %d\n", n1, n2, res, carry);

  return res;
}
