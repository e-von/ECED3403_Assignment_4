/*
  jumpinst.h
  Header file for jumpinst.c.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#ifndef JUMPINST_H
#define JUMPINST_H

#define OFFMASK      0x03FF   //Masks the offset bits for jumps
#define SIGNEXT      0xFC00
#define SIGN10       0x0200

void jne_jnz(unsigned short);
void jeq_jz(unsigned short);
void jnc_jlo(unsigned short);
void jc_jhs(unsigned short);
void j_n(unsigned short);
void jge(unsigned short);
void jl(unsigned short);
void jmp(unsigned short);
signed short offset10to16(unsigned short);

#endif  /* JUMPINST_H */
