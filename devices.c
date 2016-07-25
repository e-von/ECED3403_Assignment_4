/*
 Device file
 - Emulates up to 16 devices
 - All have same register structure:
   n: Reserved [4], OF [1], DBA [1], IO [1], IE [1]
   n+1: Data (I/O)
 - I/O controlled by input file

  Coder: Elias Vonapartis, with code from ECED3403 class at Dalhousie University
  Release Date: July 14, 2016
  Latest Updates: None
*/
#include <stdio.h>
#include <stdlib.h>
#include "emulator.h"
#include "machine.h"
#include "devices.h"
#include "srcontrol.h"
#include "bus.h"
#include "debugger.h"

void init_devices(char* argv[]){
  char devbuffer[LINE_LEN];     //buffers info from the dev.in file
  unsigned dev;                 //device number
  unsigned in_out;              //indicates i|o of device
  unsigned int_time;            //time at which an interrupt occurs
  unsigned output_time;         //time to complete an interrupt
  unsigned char in_out_data;    //data to be transmitted

  if((indev = fopen(argv[cpos], "r")) == NULL){
    printf("File %s could not be opened.\n", argv[cpos]);
    exit(1);
  }
  if((outdev = fopen("dev.out", "w")) == NULL){
    printf("Output file could not be created.\n");
    exit(1);
  }

  cpos--; //Decrement the global argc pos, to allow opening of srecords file.

  /*
    Read first 16 records for input/output and processing time.
    Application program sets or clears interrupts, loaded into memory.
  */
  for(dev = 0; dev < DEVICE_NUM; dev++){
    fgets(devbuffer, LINE_LEN, indev);
    sscanf(devbuffer, "%d %d", &in_out, &output_time);
    devices[dev].in_out = in_out;      //1 for input, 0 for output
    devices[dev].output_time = output_time;
    devices[dev].output_active = FALSE;
    devqueue[dev].DB = TRUE;          //List of pending devices marked as free
  }                                   //for use

  #ifdef devs
  for(dev = 0; dev < DEVICE_NUM; dev++){
    printf("dev[%d]\t%d %d\n",dev,devices[dev].in_out,devices[dev].output_time);
  }
  #endif

  linecount = DEVICE_NUM;             //Assign the global linecount to line 12

  //Read input file for first status change - next_time, next_dev, next_data
  if(!rd_next_line(linecount)){
    printf("\nNo input interrupts have been set. Code execution proceeding.\n");
    next_time = MAX_CLK; //Max long int value, not reached in this assignment
  }
}

/*
  This function generates interrupts. It does so by saving the machine's state,
  i.e. the PC, and SR. It then assigns the value in the interrupt vector of the
  device to the PC to execute the ISR.
*/
void cause_interrupt(unsigned device){
  printf(">>>>>>INTERRUPTING NOW<<<<<< dev%d\n", device);

  #ifdef devs
  printf("PC value %x\n", reg_file[PC]);
  printf("SP value %x\n", reg_file[SP]);
  printf("SR value %x\n", reg_file[SR]);
  #endif

  //Save the machines state SR, PC
  reg_file[SP] -= WORDINC;                    //Decrement SP
  MEM_WR(reg_file[SP], reg_file[PC], WORD);   //Store PC val in TOS
  reg_file[SP] -= WORDINC;                    //Decrement SP
  MEM_WR(reg_file[SP], reg_file[SR], WORD);   //Store SR
  reg_file[SR] = 0;                           //Clear SR
  reg_file[PC] = (memory[VECTOR_BASE + devscr(device)] |  //Assign new PC
                  memory[VECTOR_BASE + devscr(device) + 1] << 8);

  #ifdef devs
  printf("Final PC value %d\n", reg_file[PC]);
  printf("Final SP value %d\n", reg_file[SP]);
  printf("Final SR value %d\n", reg_file[SR]);
  #endif
}

/*
  This function gets called after an instruction is executed. It checks to see
  for input status changes, based on the input file interrupt times. If
  interrupts are enabled it calls cause_interrupt to generate interrupts.
  It then checks if output devices have been written to (dev_mem_access) and if
  so it outputs to the dev.out file. If ints have been enabled it calls
  cause_interrupt. It handles input device priority but not output.
*/
void check_for_status_change(void){
  struct scr_olay *scr;
  unsigned char devbuffer[LINE_LEN], flag_found_next = FALSE;
  unsigned short i;
  unsigned dev;
  unsigned int linenum;

  #ifdef devs
  printf("\n\n\nChecking for: "
  "next_time: %ld next_dev: %d next_data: %d\n", next_time, next_dev, next_data);
  #endif

  //Check if at this sys_clk we are expecting an interrupt
  if(sys_clk >= next_time){
    printf(">>>>>>>>>>We have entered an interrupt time period.<<<<<<<<<<\n");

    //Obtain the status control register bits for the device that is now going
    //cause the interrupt.
    scr = (struct scr_olay *) & (memory[devscr(next_dev)]);
    flag_wait = FALSE; //Show that we can read the next line of dev.in

    #ifdef devs
    printf("The control memory location[%d] contains %02x\n",
    devscr(next_dev), memory[devscr(next_dev)]);
    printf("IE: %d IO: %d DBA: %d OF: %d\n",scr->ie,scr->io,scr->dba,scr->of);
    #endif

    //Check for overflow (unread data register), and set the bit accordingly
    if(scr->dba == SET){
      //Swap shows if a device with a pending interrupt has had another interrupt
      //request, overwritting the last one pending.
      if(!devqueue[next_dev].swap){
        printf("No overflow in this case; interrupt by dev%d is pending.\n",
        next_dev);
      }
      else{
        printf("We have overflow, DBA was already set for the device.\n");
        devqueue[next_dev].swap = FALSE;
        scr->of = SET;    //Set the overflow bit
      }
    }

    scr->dba = SET; //Indicate the new input byte through the scr
    printf("Set DBA for dev%d. New SCR bits:\n", next_dev);
    printf("IE: %d IO: %d DBA: %d OF: %d\n",scr->ie,scr->io,scr->dba,scr->of);

    if(scr->ie){            //If the device has been set up to interrupt
      if(srptr->GIE){       //and general interrupts are enabled
        //Add the next_data to memory of the device being handled
        devices[next_dev].in_out_data = memory[devdata(next_dev)] = next_data;

        #ifdef debug
        printf("devices[next_dev] | devices[%d] = in_out_data = %c\n",
        next_dev, devices[next_dev].in_out_data);
        #endif
        //Start ISR of dev with highest priority as determined last check
        cause_interrupt(next_dev);
        printf("Making DB true for dev%d\n", next_dev);
        devqueue[next_dev].DB = TRUE; //Indicates serviced interrupt
      }
      else{ //GIE = 0
        printf("Dev%d requested interrupt but GIE is clear.\n", next_dev);
        print_dev_queue();
      }
    }

    printf("Going to read next line.\n");

    if(!rd_next_line(linecount)){
      printf("No more interrupts in the input file. Checking Pending...\n");
      next_time = MAX_CLK;        //Make it MAX_CLK if no other ints are found
      for(i = 0; i < DEVICE_NUM; i++){
        if(devqueue[i].DB == FALSE){
          printf("Found pending interrupt from dev%d.\n", i);
          next_dev = i;
          next_time = devqueue[i].queue_time;
          next_data = devqueue[i].queue_data;
        }
      }
    }
    else{
      if(next_time > sys_clk){
        printf("The interrupt is expected to happen in the future.\n");
        flag_wait = TRUE;
        linecount--; //Decrement the line number to re-read the same next time
      }
      if(!flag_wait){
        printf("Adding read interrupt to pending position for itself.\n");
        (!(devqueue[next_dev].DB)) ? (devqueue[next_dev].swap = TRUE) : 0;
        devqueue[next_dev].queue_time = next_time;
        devqueue[next_dev].queue_data = next_data;
        devqueue[next_dev].DB = FALSE;

        for(i = 0; i <= next_dev; i++){     //Check for highest priority pending
          if(!devqueue[i].DB){              //Non-serviced
            printf("Found higher priority non-serviced interrupt dev%d.\n", i);
            next_time = devqueue[i].queue_time;
            next_data = devqueue[i].queue_data;
            print_dev_queue();
            break;                          //Stop searching when found
          }
        }
      }
      else{ //Flag wait set, i.e. the interrupt is expected at a later time
        for(i = 0; i < DEVICE_NUM; i++){     //Check for highest priority pending
          if(!devqueue[i].DB){               //DB shows non-serviced
            printf("Next in line interrupt is dev%d.\n", i);
            next_time = devqueue[i].queue_time;
            next_data = devqueue[i].queue_data;
            flag_found_next;
            print_dev_queue();
            break;                          //Stop searching when found
          }
        }
        if(!flag_found_next){
          devqueue[next_dev].queue_time = next_time;
          devqueue[next_dev].queue_data = next_data;
          devqueue[next_dev].DB = FALSE;
        }
      }
    }
  } // Not interrupt time yet, or interrupt has been executed

  /* Output device(s) finished writing? (Slow and dirty solution) */
  for (dev = 0; dev < DEVICE_NUM; dev++){

    /* Check for output active device and time expired */
    if (devices[dev].output_active && sys_clk >= devices[dev].end_time){
      /* Output status change has occurred */
      printf("Output dev%d end_time %d \n", dev, devices[dev].end_time);
      //Obtain scr bits for output dev
      scr = (struct scr_olay *) & memory[devscr(dev)];

      //I have followed the subsequent conditional structure, for
      //easier debugging and adding other features in the future if I want

      if(scr->ie){                         //Check if dev's interrupts enabled
        if(srptr->GIE){                    //Check if general interrupts enabled
          scr -> dba = CLEAR;                  //Buffer is now empty
          devices[dev].output_active = FALSE; //Output is inactive
          cause_interrupt(dev);               //Start the ISR
        }
        else{                                 //GIE = 0
          printf("Pending Output but GIE bit clear.\n");
          return;                             //To avoid print below
        }
      }
      else{                                   //IE = 0
        printf("Device's IE bit is clear.\n");
        return;                               //To avoid print below
      }

      /* WRITE sys_clk, dev, output_data to output file */
      fprintf(outdev, "sys_clk: %ld\tdev: %d\tdata: %c\n",
      sys_clk, dev, devices[dev].in_out_data);
    }
  }
}

/*
 - Device memory has been read or written
 - Check changes
 - Update device or memory or both
 - Write to SCR will not stop output
*/
void dev_mem_access(unsigned short address, unsigned char data, unsigned rw){
  unsigned dev, scr_addr;
  struct scr_olay *scr;

  dev = addtodev(address);  /* 0:1, 2:3, .. 30:31 -> 0, 1, .. 15 */
  scr_addr = devscr(dev);   /* SCR address for this device */
  scr = (struct scr_olay *) & memory[scr_addr];

  printf("dev_mem_access dev%d with data %x\n", dev, data);
  printf("IE: %d IO: %d DBA: %d OF: %d\n", scr->ie, scr->io, scr->dba, scr->of);
  sys_clk += DEV_ACCESS_TIME; //Added to the start to avoid repeated code

  /* Accessed sc register and ensuring io bit remains unchanged */
  if(scr_addr == address){
    if(rw == WRITE){   //Ignore read case, no possibility of changing the io bit
      printf("Writing to control\n");
      scr->io = devices[dev].in_out;
      printf("The io bit of dev[%d] is %d\n", dev, scr->io);
    }
    return;
  }
  /*
   Address is data register
   - Read from input dr: clear OF and DBA
   - Write to output dr: check DBA for OF, start output
  */
  if(devices[dev].in_out == INPUT){     //1 for input, 0 for output
    /* Input device */
    if(rw == READ){
      printf("Reading from input data register\n");
      scr->dba = CLEAR;
      scr->of = CLEAR;
    }
    else{ /* Write to "read-only" register - restore value */
      printf("Wrote to input device, restoring its contents now.\n");
      memory[scr_addr + 1] = devices[dev].in_out_data;
    }
  }
  else{ /* Output device in_out == 0 */
    if (rw == WRITE){
      if (scr->dba == SET){
        printf("Overflow in output device\n");
        scr->of = SET;   /* Overflow - lose current byte */
      }
      printf("Writing to output device\n");
      scr->dba = SET; /* Write active */
      devices[dev].output_active = TRUE;
      devices[dev].in_out_data = data;
      devices[dev].end_time = sys_clk + devices[dev].output_time;
      printf("Wrote data %x\n", data);
    }
  }
  printf("IE: %d IO: %d DBA: %d OF: %d\n", scr->ie, scr->io, scr->dba, scr->of);
}

unsigned char rd_next_line(unsigned int linenum){
  char devbuffer[LINE_LEN];
  unsigned int count = 0;

  rewind(indev);            //Resets pointer in file

  while(fgets(devbuffer, LINE_LEN, indev) != NULL){
    if(count == linenum){
      sscanf(devbuffer, "%ld %d %c", &next_time, &next_dev, &next_data);
      printf("next_time: %ld next_dev: %d next_data: %d\n",
      next_time, next_dev, next_data);
      linecount++;
      return TRUE;
    }
    else{
      count++;
    }
  }
  return FALSE;
}

void print_dev_queue(void){
  unsigned short i;
  for(i = 0; i < DEVICE_NUM; i++){
    printf("queue_dev: %d queue_data %d queue_time: %ld DB %d\n",
    i, devqueue[i].queue_data, devqueue[i].queue_time, devqueue[i].DB);
  }
}
