#ifndef CACHE_H
#define CACHE_H

/* Definitions */
#define associative
//#define direct
//#define writeback
#define writethrough

#define CACHE_SIZE 32
#define YOUNGEST   31
#define OLDEST     0
#define HIT   1
#define MISS  0
#define CLEAR 0
#define SET   1
#define INIT_ADDR 0xFFFF
#define CACHE_ACCESS 5

//Only temporary
#define BYTE  1
#define WORD  0
#define TRUE  1
#define FALSE 0
#define READ  0
#define WRITE 1

/* Globals */
struct cache_line{
  unsigned short address;   //address       :16 bits
  unsigned short contents;  //data          :16 bits
  unsigned char db;         //a dirty bit   :1 bit
  unsigned char LRU;        //an LRU        :5 bits
  unsigned char bw;         //Indicate whether we have written a byte or word
};                          //total bandwidth: 38 bits

struct cache_line cache_mem[CACHE_SIZE];

/* Array Structure Entry Macros */
#define addr(x)         (cache_mem[x].address)
#define data(x)         (cache_mem[x].contents)
#define db(x)           (cache_mem[x].db)
#define LRU(x)          (cache_mem[x].LRU)
#define bw(x)           (cache_mem[x].bw)
#define hash(x)         ((x)%CACHE_SIZE)
#define direct_addr(x)  (addr(hash(x)))

/* Macros for Cache Usage from within the CPU */
#define CACHE_RD(address, data, bw) cache_bus(address, &data, bw, READ)
#define CACHE_WR(address, data, bw) cache_bus(address, &data, bw, WRITE)

/* Function Declarations */
void cache_bus(unsigned short, unsigned short*, unsigned char, unsigned char);
void cache_read(unsigned short, unsigned short*, unsigned char);
void cache_write(unsigned short, unsigned short*, unsigned char);
void cache_init(void);
void cache_print(void);
void update_LRU(unsigned char);
unsigned char lowest_LRU(void);
unsigned char check_HM(unsigned short, unsigned char *);

#endif /* CACHE_H */
