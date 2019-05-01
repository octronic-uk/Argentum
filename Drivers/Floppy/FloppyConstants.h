#pragma once

/*
    https://wiki.osdev.org/Floppy_Disk_Controller
*/

#define FLOPPY_144_NUM_SIDES 2
#define FLOPPY_144_NUM_TRACKS 80
#define FLOPPY_144_NUM_SECTORS 18
#define FLOPPY_144_TOTAL_SECTORS ( FLOPPY_144_NUM_SIDES * FLOPPY_144_NUM_TRACKS * FLOPPY_144_NUM_SECTORS )

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

#define FLOPPY_BUFFER_SIZE 32

/*
    Command Options
    Bit MT
        Value = 0x80. 
        Multitrack mode. The controller will switch automatically from Head 0 to Head 1 at the end 
        of the track. This allows you to read/write twice as much data with a single command.

    Bit MF
        Value = 0x40. 
        "MFM" magnetic encoding mode. Always set it for read/write/format/verify 
        operations.

    Bit SK
    Value = 0x20. 
    Skip mode. Ignore this bit and leave it cleared, unless you have a really good reason not to.
*/
#define FLOPPY_CMD_OPT_MT 0x80
#define FLOPPY_CMD_OPT_MF 0x40
#define FLOPPY_CMD_OPT_SK 0x20

#define FLOPPY_MAX_RETRIES 100 
#define FLOPPY_SLEEP_TIME 10

enum FloppyType
{
    FLOPPY_TYPE_NO_DRIVE,

    FLOPPY_TYPE_525_360,
    FLOPPY_TYPE_525_1200,

    FLOPPY_TYPE_35_720,
    FLOPPY_TYPE_35_1440,
    FLOPPY_TYPE_35_2880,
};

/*
    The ones that you actually will use are marked with a * and a comment.
*/

enum FloppyCommand
{
   FLOPPY_CMD_READ_TRACK =         2,  // generates IRQ6
   FLOPPY_CMD_SPECIFY =            3,  // * set drive parameters
   FLOPPY_CMD_SENSE_DRIVE_STATUS = 4,
   FLOPPY_CMD_WRITE_DATA =         5,  // * write to the disk
   FLOPPY_CMD_READ_DATA =          6,  // * read from the disk
   FLOPPY_CMD_RECALIBRATE =        7,  // * seek to cylinder 0
   FLOPPY_CMD_SENSE_INTERRUPT =    8,  // * ack IRQ6, get status of last command
   FLOPPY_CMD_WRITE_DELETED_DATA = 9,
   FLOPPY_CMD_READ_ID =            10,	// generates IRQ6
   FLOPPY_CMD_READ_DELETED_DATA =  12,
   FLOPPY_CMD_FORMAT_TRACK =       13, // *
   FLOPPY_CMD_DUMPREG =            14,
   FLOPPY_CMD_SEEK =               15, // * seek both heads to cylinder X
   FLOPPY_CMD_VERSION =            16, // * used during initialization, once
   FLOPPY_CMD_SCAN_EQUAL =         17,
   FLOPPY_CMD_PERPENDICULAR_MODE = 18,	// * used during initialization, once, maybe
   FLOPPY_CMD_CONFIGURE =          19, // * set controller parameters
   FLOPPY_CMD_LOCK =               20, // * protect controller params from a reset
   FLOPPY_CMD_VERIFY =             22,
   FLOPPY_CMD_SCAN_LOW_OR_EQUAL =  25,
   FLOPPY_CMD_SCAN_HIGH_OR_EQUAL = 29
};

enum FloppyMotorState
{ 
    floppy_motor_off = 0, 
    floppy_motor_on, 
    floppy_motor_wait 
};

// Used by floppy_dma_init and floppy_do_track to specify direction
//#define FLOPPY_DMALEN 0x4800
#define FLOPPY_DMALEN 512

enum FloppyDirection 
{
    floppy_dir_read = 1,
    floppy_dir_write = 2
};
