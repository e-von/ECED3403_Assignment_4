/*
  loader.h
  Header file for loader.c.

  Coder: Elias Vonapartis
  Release Date: July 14, 2016
  Latest Updates: None
*/

#ifndef LOADER_H
#define LOADER_H

/* Definitions */
#define SREC_LEN  78
#define WORD_LEN  2
#define NUL       '\0'
#define START     1
#define DATAPOS   8
#define S9        9

/* Globals */
FILE* infile;

/* Function Declarations */
void load(int argc, char* argv[]);
unsigned char debugger(char* );

#endif /* LOADER_H */
