/*
  condinsts.h
  Header file for condinsts.c

  Coder: Elias Vonapartis
  Releade Date: July 24 2016
*/
#ifndef CONDINST_H
#define CONDINST_H

/* Global Conditional Counters */
unsigned short then_cnt;
unsigned short else_cnt;

/* Function Declarations */
void ifeq(void);
void ifne(void);
void ifcs(void);
void ifcc(void);
void ifmi(void);
void ifpl(void);
void ifvs(void);
void ifvc(void);
void ifvc(void);
void ifhi(void);
void ifls(void);
void ifge(void);
void iflt(void);
void ifgt(void);
void ifle(void);
void ifal(void);
void ifnv(void);
void decisiontable(void);

#endif /* CONDINSTS_H */
