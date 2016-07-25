/*
  cache.c
  This module contains a cache implementation for the emulator. It supports two
  organization methods, Associative & Direct, and two replacement policies,
  Writeback & Writethrough. Prior to accessing memory the cache is checked,
  effectively emulating its placement between the CPU and primary memory.

  Coder: Elias Vonapartis
  Release Date: N/A
  Updates: None
*/

#include <stdio.h>
#include "cache.h"
#include "bus.h"
#include "machine.h" //Included for sys_clk global

/*
  Emulates the bus between CPU and primary memory.
  Simply determines if the emulator is reading or writing from/to memory and
  calls the resprective functions/
*/
void cache_bus(unsigned short address, unsigned short* data, unsigned char bw,
               unsigned char rw){
  switch (rw) {
    case READ:
    cache_read(address, data, bw);
    break;
    case WRITE:
    cache_write(address, data, bw);
    break;
  }
  sys_clk += CACHE_ACCESS;
  return;
}

/*
  Function which handles CPU reads. Code for associative and direct cache
  organization is included here.
*/
void cache_read(unsigned short address, unsigned short* data, unsigned char bw){
  unsigned char hm, linenum;

  //Check hit or miss in cache and return line number of hit in associative or
  //the line number of hit or miss in direct.
  hm = check_HM(address, &linenum);

  switch (hm) {
    case HIT:
    printf("\nRead hit for address %04x.\n", address);
    *data = data(linenum);            //Cache is word addressable.
#ifdef associative
    update_LRU(linenum);              //If associative update LRU
#endif
    break;

    case MISS:
    printf("\nRead miss for address %04x.\n", address);
    bus(address, data, bw, READ);       //Read contents from memory
#ifdef associative
    linenum = lowest_LRU();             //Find the oldest entry in associative
    update_LRU(linenum);                //Update the LRU of the cache
#endif
#ifdef writeback
    //If the cache line we are replacing has been written to in writeback
    //write it back to primary memory now that it's being replaced.
    if(db(linenum)){
      printf("Cache line %d removed, and updated in primary using bw %d.\n",
      linenum, bw(linenum));
      MEM_WR(addr(linenum), data(linenum), bw(linenum));
      db(linenum) = FALSE;
    }
    bw(linenum) = bw;                   //BW indicator for writeback purposes
#endif
    printf("Adding contents to cache line %d.\n", linenum);
    addr(linenum) = address;            //In associative and direct update cache
    data(linenum) = (*data);            //New data obtained from primary mem
    break;
  }
}

/*
  Function which handles CPU writes in direct and associative cache. Supports
  writethrough and writeback replacement policies.
*/
void cache_write(unsigned short address, unsigned short* data, unsigned char bw){
  unsigned char hm, linenum;

  hm = check_HM(address, &linenum);   //Check if address is in cache memory

  switch (hm) {                       //Switch the hit or miss
    case HIT:
    printf("\nWrite hit for address %04x.\n", address);
#ifdef associative
    update_LRU(linenum);                //If associative update_LRU
#endif /* associative */
#ifdef writeback
    db(linenum) = SET;                  //Indicate the line has been written to
    bw(linenum) = bw;                   //BW indicator for writeback purposes
#endif
    if(bw == BYTE){
      data(linenum) = (*data) & 0xFF;   //In WT & WB update cache
    }
    else{ /* WORD */
      data(linenum) = (*data) & 0xFFFF;
    }
#ifdef writethrough
    printf("Writing through to memory.\n");    //If writethrough update primary
    bus(address, data, bw, WRITE);             //MEM_WR(address, data, bw)
#endif /* writethrough */
    break;

    case MISS:
    printf("\nWriting miss for address %04x.\n", address);
#ifdef associative
    linenum = lowest_LRU();      //If associative update_LRU
    update_LRU(linenum);         //I would combine these two if always the oldest
#else /* direct */
    linenum = hash(address);
    printf("Direct Line#:%x from Primary Address: %x\n", linenum, address);
#endif
#ifdef writeback
    //If writeback enabled & DB set write the line being replaced to primary
    //memory.
    if(db(linenum)){
      printf("Cache line %d removed, and updated in primary.\n", linenum);
      MEM_WR(addr(linenum), data(linenum), bw(linenum));
    }
    db(linenum) = TRUE;               //Mark that data has been written
    bw(linenum) = bw;                 //BW indicator for writeback purposes
#endif
    printf("Adding contents to cache line %d.\n", linenum);
    addr(linenum) = address;         //In writethrough & writeback update cache
    if(bw == BYTE){
      data(linenum) = (*data) & 0xFF;
    }
    else{
      data(linenum) = (*data) & 0xFFFF;
    }
#ifdef writethrough
    printf("Writing to primary memory as well.\n");
    bus(address, data, bw, WRITE);  //In writethrough update memory
#endif
    break;
  }
}

unsigned char check_HM(unsigned short address, unsigned char *linenum){
  unsigned char i;

#ifdef associative
  for(i = 0; i < CACHE_SIZE; i++){
    *linenum = i;
    if(address == addr(i)){
      return HIT;
    }
  }
  return MISS;          //Defined only with associative, return miss if no hits
#else //direct
  *linenum = hash(address);   //For direct in both hit and miss return the line
  return (address == addr(hash(address))) ? HIT : MISS; //Return hit/miss
#endif
}

/* Function to initialize cache values. Called by emulator main. */
void cache_init(void){
  unsigned char i;
  for(i = 0; i < CACHE_SIZE; i++){
    addr(i) = INIT_ADDR;          //Assign an invalid initial address 0xFFFF
    data(i) = CLEAR;              //Contents are clear on start
    bw(i) = WORD;                 //Will later change to reflect actual contents
    db(i) = CLEAR;                //Dirty bit is clear
    LRU(i) = i;                   //Not like this for direct.. Does it matter?
  }
}

#ifdef associative
/*
  This function updates the LRU. It takes the last accessed line as an argument,
  and applies the LRU algorithm from there on. Once all values are decremented
  it assigns the largest LRU to the target.
*/
void update_LRU(unsigned char target){
  unsigned char i;

  for(i = 0; i < CACHE_SIZE; i++){
    if(LRU(i) > LRU(target)){     //Decrement the LRU value for any LRU greater
      LRU(i)--;                   //than the target's LRU
    }
  }
  LRU(target) = YOUNGEST;         //Now give the target the largest LRU
  return;
}

unsigned char lowest_LRU(void){
  unsigned char i;

  for(i = 0; i < CACHE_SIZE; i++){
    if(LRU(i) == OLDEST){
      return i;
    }
  }
  printf("Error finding oldest associative entry.\n");
  return 0;
}

#endif /* associative */

/* This function sinply prints the cache contents depending on the setup */
void cache_print(void){
  unsigned char i;

  printf("\n--------------------Cache Contents--------------------\n");
  for(i = 0; i < CACHE_SIZE; i++){
    #ifdef associative

    #ifdef writeback
    printf("Line %d\tAddress: %04x\tContents: %04x\tBW: %d\tDB: %d\tLRU: %d\n",
    i, addr(i), data(i), bw(i), db(i), LRU(i));

    #else /* writethrough in associative */
    printf("Line %d\tAddress: %04x\tContents: %04x\tLRU: %d\n",
    i, addr(i), data(i), LRU(i));
    #endif /* writeback and writethrough*/

    #else /* direct */
    #ifdef writeback
    printf("Line %d\tAddress: %04x\tContents: %04x\tBW: %d\tDB: %d\n",
    i, addr(i), data(i), bw(i), db(i));

    #else /* writethrough in direct */
    printf("Line %d\tAddress: %04x\tContents: %04x\n",
    i, addr(i), data(i));
    #endif /* writethrough and writeback */

    #endif /* associative and direct*/
  }
  printf("\n\n");
}
