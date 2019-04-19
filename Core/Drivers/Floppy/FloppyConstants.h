#pragma once

/*
    https://wiki.osdev.org/Floppy_Disk_Controller
*/

#define FLOPPY_144_SECTORS_PER_TRACK    144

#define FLOPPY_REGISTER_STATUS_A        0x3F0 // read-only
#define FLOPPY_REGISTER_STATUS_B        0x3F1 // read-only
#define FLOPPY_REGISTER_DIGITAL_OUTPUT  0x3F2
#define FLOPPY_REGISTER_TAPE_DRIVE      0x3F3
#define FLOPPY_REGISTER_MAIN_STATUS     0x3F4 // read-only
#define FLOPPY_REGISTER_DATARATE_SELECT 0x3F4 // write-only
#define FLOPPY_REGISTER_DATA_FIFO       0x3F5
#define FLOPPY_REGISTER_DIGITAL_INPUT   0x3F7 // read-only
#define FLOPPY_REGISTER_CONFIGURATION_CONTROL 0x3F7  // write-only

/*
    Digital Output Register 

    Mnemonic	bit number	value	meaning/usage
    MOTD        7	        0x80	Set to turn drive 3's motor ON
    MOTC	    6	        0x40	Set to turn drive 2's motor ON
    MOTB	    5	        0x20	Set to turn drive 1's motor ON
    MOTA	    4	        0x10	Set to turn drive 0's motor ON
    IRQ	        3	        8	    Set to enable IRQs and DMA
    RESET	    2	        4	    Clear = enter reset mode, Set = normal operation
    DSEL1/0     0/1	        3	    "Select" drive number for next access
*/ 
#define FLOPPY_DOR_MOTD  0x80
#define FLOPPY_DOR_MOTC  0x40
#define FLOPPY_DOR_MOTB  0x20
#define FLOPPY_DOR_MOTA  0x10
#define FLOPPY_DOR_IRQ   0x08
#define FLOPPY_DOR_RESET 0x04
#define FLOPPY_DOR_DSEL  0x03

/*
    MSR bitflag definitions

    Mnemonic	bit number	value	meaning/usage
    RQM	        7	        0x80	Set if it's OK (or mandatory) to exchange bytes with the FIFO IO port
    DIO	        6	        0x40	Set if FIFO IO port expects an IN opcode
    NDMA	    5	        0x20	Set in Execution phase of PIO mode read/write commands only.
    CB	        4	        0x10	Command Busy: set when command byte received, cleared at end of Result phase
    ACTD	    3	        8	    Drive 3 is seeking
    ACTC	    2	        4	    Drive 2 is seeking
    ACTB	    1	        2	    Drive 1 is seeking
    ACTA	    0	        1	    Drive 0 is seeking
*/
#define FLOPPY_MSR_RQM  0x80
#define FLOPPY_MSR_DIO  0x40
#define FLOPPY_MSR_NDMA 0x20
#define FLOPPY_MSR_CB   0x10
#define FLOPPY_MSR_ACTD 0x08
#define FLOPPY_MSR_ACTC 0x04
#define FLOPPY_MSR_ACTB 0x02
#define FLOPPY_MSR_ACTA 0x01