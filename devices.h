#ifndef DEVICES_H
#define DEVICES_H

#define devs

#define DEVICE_NUM    16
#define DEVICE_LIMIT  32      /* SCR:DR pairs - 0 to DEV_LIM-1 (low mem) */
#define VECTOR_BASE   0xFFC0  /* Start of interrupt vector (high mem) */

#define LINE_LEN      80
#define COFF          0x10    /* Mask for C-off bit in device control */

#define MAX_CLK       0xFFFFFFFF

#define addtodev(x)   ((x) >> 1)
#define devscr(x)     ((x) << 1)
#define devdata(x)    (devscr(x) + 1)

#define INPUT         1
#define CLEAR         0
#define SET           1

/* Globals and linking */
FILE* indev;
FILE* outdev;
extern struct sr_bits* srptr;   //Link the SR to this modules

/* Status-control register overlay - common to all devices */
struct scr_olay{
  unsigned ie : 1;
  unsigned io : 1;
  unsigned dba : 1;
  unsigned of : 1;
  unsigned res : 4;
};

/* Device properties - common to all devices (note write-specific fields) */
struct device_properties{
  unsigned in_out : 1;        /* 0 - output | 1 - input */
  unsigned output_active : 1; /* T|F - outputting byte */
  unsigned char in_out_data;  /* byte being transmitted */
  unsigned output_time;       /* Constant - time for byte to xmit */
  unsigned end_time;          /* time proceessing is to finish */
};

struct dev_queue{
  unsigned char queue_data;   /* Data of the device's pending interrupt */
  unsigned long queue_time;   /* Time for next interrupt of the device */
  unsigned char DB;           /* Dirty bit showing if an int has been serviced */
  unsigned char swap;         /* Flags if a pending int has been replaced */
};

/* Private Declarations to this Module */
PRIVATE struct dev_queue devqueue[DEVICE_NUM];
PRIVATE struct device_properties devices[DEVICE_NUM];
PRIVATE unsigned next_dev;
PRIVATE unsigned char next_data;
PRIVATE unsigned long next_time;
PRIVATE unsigned int linecount;
PRIVATE unsigned char flag_wait;

/* Function Declarations */
void init_devices(char* argv[]);
void cause_interrupt(unsigned);
void check_for_status_change(void);
void dev_mem_access(unsigned short, unsigned char, unsigned);
unsigned char rd_next_line(unsigned int);
void printscr(void);
void print_dev_queue(void);

#endif /* DEVICES_H */
