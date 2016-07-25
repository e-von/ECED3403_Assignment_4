/*
  addrmodes.c
  Addressing mode and operand handling code:
  - determine mode from As/Ad and Regnum
  - get_operand() obtains operand for manipulation
  - put operand() stores operand after manipulation

  Coder: Elias Vonapartis
         Based on code provided in the ECED3403 class of Dalhousie University.
  Release Date: July 14, 2016
  Latest Updates: None
*/

#include <stdio.h>
#include <stdlib.h>
#include "emulator.h"
#include "machine.h"
#include "cache.h"
#include "addrmodes.h"

/*
 - Source addressing mode - map As and Reg to mode (1..7)
 - Special cases: constant generators 0xC#, #=0, 1, 2, 4, 8, F (-1)

 - Destination address mode - same as source but using Ad not As
 - Ad (0|1) allows modes 1..4
*/
enum addr_modes {REG_DIR = 1, INDEXED = 2, RELATIVE = 3, ABSOLUTE = 4,
  REG_IND = 5, REG_IND_AI = 6, IMM = 7};

unsigned char pc_increment[] = {0, 0, 2, 2, 2, 0, 0, 2};

PRIVATE char sd_am[4][16] = {
  /*00*/ {1, 1, 1, /* Reg direct */
          0xC0, /* Constant generator: #0 - R3 */
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, /* Reg direct */
  /*01*/ {3, /* Relative - PC */
          2, /* Indexed - SP */
          4, /* Absolute */
          0xC1, /* Constant generator: #1 - R3 */
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, /* Indexed */
  /*10*/ {5, 5, /* Reg indirect */
          0xC4, 0xC2, /* Constant generator: #4 - SR, R3; #2 - R3 */
          5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}, /* Reg indirect */
  /*11*/ {7, /* Imm - PC */
          6, /* SP - Reg ind. w/ incr */
          0xC8, 0xCF, /* Constant generator: #8 - SR, R3; #-1 - R3 */
          6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6} /* Reg indirect w/ incr */
};

/*
  This function gets called by the two operand instructions. It returns through
  pointers the source and destination operand values. It was written to minimize
  repeated code.
*/
void retrieve_operands(unsigned short* src,unsigned short* dst,unsigned char bw,
  unsigned short sreg, unsigned short ad, unsigned short as, unsigned short dreg,
  unsigned char inc_src, unsigned char inc_dst){
  unsigned short dummy_val;
  *src = get_operand(as, sreg, bw, inc_src, &dummy_val);
  #ifdef debug
  printf("PC after getting SRC %d\n", reg_file[PC]);
  #endif
  *dst = get_operand(ad, dreg, bw, inc_dst, &dummy_val);
  #ifdef debug
  printf("PC after getting DST %d\n", reg_file[PC]);
  #endif
}

/*
  This function extracts operands based on the received arguments as, ad,
  register and byte or word indication. The caller can indicate whether the PC
  should be incremented or not in the cases of relative and indexed.
*/
unsigned short get_operand(unsigned asd, unsigned regnum, unsigned bw,
  unsigned incr_pc, unsigned short *addr){
  unsigned short operand = 0;
  unsigned short offset;
  unsigned short address;
  unsigned short mode; /* Addr mode obtained from sd_am[][] */

  switch ((mode = sd_am[asd][regnum])){
    case REG_DIR:
    operand = reg_file[regnum];             //EA is the register
    *addr = operand;                        //Used for CALL
    #ifdef debug
    printf("REGISTER DIRECT, FROM: R%d\n", regnum);
    #endif
    break;
    case INDEXED:
    CACHE_RD(reg_file[PC], address, WORD);  //Read base address
    COND_INC_PC(incr_pc);                   //Increment PC if required
    offset = address + reg_file[regnum];    //Calculate EA
    CACHE_RD(offset, operand, bw);            //Read contents of EA
    *addr = offset;                         //Used for CALL
    #ifdef debug
    printf("INDEXED, FROM: %d\n", offset);
    #endif
    break;
    case RELATIVE:
    CACHE_RD(reg_file[PC], offset, WORD);             //Read relative address
    COND_INC_PC(incr_pc);                           //Increment PC if required
    //!incr_pc in the function below allows for proper calculation of the EA
    //when we do not wish to increment PC, since the assembler assumes PC is
    //always incremented
    address = reg_file[PC] + offset + 2*(!incr_pc); //Calculate EA
    CACHE_RD(address, operand, bw);                   //Read contents of EA
    *addr = address;                                //Used for CALL
    #ifdef debug
    printf("RELATIVE, FROM: %d\n", address);
    #endif
    break;
    case ABSOLUTE:
    CACHE_RD(reg_file[PC], address, WORD);    //Read absolute address
    COND_INC_PC(incr_pc);                     //Increment PC if required
    CACHE_RD(address, operand, bw);           //Read contents of address
    *addr = address;                          //Used for CALL
    #ifdef debug
    printf("ABSOLUTE, FROM: %d\n", address);
    #endif
    break;
    case REG_IND:
    case REG_IND_AI:
    CACHE_RD(reg_file[regnum], operand, WORD);  //Read contents of address in reg
    if(mode == REG_IND_AI){                     //If @Rn+ increment the contents
      bw ? (reg_file[regnum]++) : (reg_file[regnum] += WORDINC);
    }
    *addr = operand;                          //Used for CALL
    #ifdef debug
    printf("INDIRECT OR INDIRECT AUTOINCREMENT, @R%d contains address %x which "
    "has contents %x\n", regnum, reg_file[regnum], operand);
    #endif
    break;
    case IMM:
    #ifdef debug
    printf("IMMEDIATE\n");
    #endif
    CACHE_RD(reg_file[PC], operand, WORD);    //Read immediate value
    COND_INC_PC(TRUE);                        //Always increment PC
    *addr = operand;
    break;
    default:
    #ifdef debug
    printf("CONSTANT GENERATOR\n");
    #endif
    operand = mode & 0x0f;                    //Remove 'C' from array entry
    if (operand == 0x0f){                     //Special case of -1
      operand = -1;
    }
    break;
  }
  return operand;
}

/*
  Function places operand into a location retrieved by using the specified
  as or ad and register number.
*/
void put_operand(unsigned asd, unsigned regnum, unsigned short operand,
  unsigned char bw){
  unsigned short mode;
  unsigned short address;

  switch ((mode = sd_am[asd][regnum])){
    case REG_DIR:
    reg_file[regnum] = operand;
    #ifdef debug
    printf("Register direct[%d]: %d\n", regnum, reg_file[regnum]);
    #endif
    break;
    case INDEXED:
    CACHE_RD(reg_file[PC], address, WORD);  //Get base address pointed to by PC
    #ifdef debug
    printf("Base address: %d\n", address);
    #endif
    COND_INC_PC(TRUE);                    //Increment PC
    address += reg_file[regnum];          //Calculate the indexed address
    CACHE_WR(address, operand, bw);       //Write to this location
    break;
    case RELATIVE:
    CACHE_RD(reg_file[PC], address, WORD);    //Read relative address
    #ifdef debug
    printf("Relative address: %d\n", address);
    #endif
    COND_INC_PC(TRUE);                      //Increment PC
    address += reg_file[PC];                //Calculate EA
    CACHE_WR(address, operand, bw);         //Write to EA
    break;
    case ABSOLUTE:
    CACHE_RD(reg_file[PC], address, WORD);    //Read the address of the absolute
    #ifdef debug
    printf("Absolute address: %d\n", address);
    #endif
    COND_INC_PC(TRUE);                      //Increment PC
    CACHE_WR(address, operand, bw);         //Write to the absolute address
    break;
    case REG_IND:     //Illegal usage of addressing modes. This will never be
    case REG_IND_AI:  //called when using the s-records are obtained from the
    case IMM:         //included assembler. We may end up here in emulator
    signalCPUfault(); //malfunction.
    break;
    default:          //Constant generator support
    operand = mode & 0x0f;
    if (operand == 0x0f){
      operand = -1;
    }
    break;
  }
}
/*
  This function increments the PC for instructions which are skipped with
  conditional execution.
*/
void pc_calculator(unsigned as, unsigned ad, unsigned sreg, unsigned dreg,
  unsigned char type){
  unsigned short smode, dmode;

  smode = sd_am[as][sreg];

  if(smode <= IMM){
    printf("Incremented PC for SRC by %d\n", pc_increment[smode]);
    reg_file[PC] += pc_increment[smode];
  }
  else{
    printf("Source constant generator.\n");
  }

  if(type == TWOOP){
    dmode = sd_am[ad][dreg];
    if(dmode <= IMM){
      printf("Incremented PC for DST by %d\n", pc_increment[dmode]);
      reg_file[PC] += pc_increment[dmode];
    }
    else{
      printf("Destination constant generator.\n");
    }
  }
}
