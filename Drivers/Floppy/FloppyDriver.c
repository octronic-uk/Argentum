#include "FloppyDriver.h"

#include <string.h>
#include <stdio.h>
#include <Objects/Kernel/Kernel.h>
#include <unistd.h>
#include <Drivers/CMOS/CMOSDriver.h>
#include <Drivers/IO/IODriver.h>
#include <Drivers/Interrupt/InterruptDriver.h>
#include <Drivers/PS2/PS2Driver.h>

extern struct Kernel _Kernel;

// https://forum.osdev.org/viewtopic.php?f=1&t=13538

// we statically reserve a totally uncomprehensive amount of memory
// must be large enough for whatever DMA transfer we might desire
// and must not cross 64k borders so easiest thing is to align it
// to 2^N boundary at least as big as the block
//static char FLOPPY_DMA_BUFFER[FLOPPY_DMALEN] __attribute__((aligned(32)));
extern uint8_t fpy_dma_buf[FLOPPY_DMALEN];

bool FloppyDriver_Constructor(struct FloppyDriver* self)
{
    printf("Floppy Driver: Constructing (DMA Buffer @ 0x%x)\n",(uint32_t)&fpy_dma_buf[0]);
    // Setup the IRQ6 Interrupt Handler
    InterruptDriver_SetHandlerFunction(&_Kernel.Interrupt, 6, FloppyDriver_InterruptHandler);

    // Init the driver object
    memset(self,0,sizeof(struct FloppyDriver));
    self->DebugIO = false;
    self->Debug = false;
    self->InterruptWaiting = false;
    // Get CMOS Config
    uint8_t cmosConfig = CMOSDriver_ReadFloppyConfiguration(true);
    if (self->Debug)
    {
        printf("Floppy: CMOS Configuration 0x%x\n",cmosConfig);
    }
    uint8_t master, slave;
    master = (cmosConfig & 0xF0) >> 4;
    slave  = (cmosConfig & 0x0F);

    self->Master = FloppyDriver_GetType(master);
    self->Slave  = FloppyDriver_GetType(slave);

    return FloppyDriver_Reset(self);
}

void FloppyDriver_WriteCommand(struct FloppyDriver* self, enum FloppyCommand cmd) 
{
    if (self->Debug) printf("Floppy: WriteCommand 0x%x\n",cmd);
    int i; // do timeout
    for(i = 0; i < FLOPPY_MAX_RETRIES; i++) 
    {
        usleep(10); // sleep 10 ms
        if(0x80 & FloppyDriver_ReadRegisterMainStatus(self)) 
        {
            return (void) FloppyDriver_WriteRegisterDataFIFO(self, cmd);
        }
    }
    printf("Floppy: Error - Write Command timeout\n");    
}

uint8_t FloppyDriver_ReadData(struct FloppyDriver* self) 
{
    if (self->Debug) printf("Floppy: Read Data\n");
    int i; // do timeout, 60 seconds
    for(i = 0; i < FLOPPY_MAX_RETRIES; i++) 
    {
        usleep(10); // sleep 10 ms
        if(0x80 & FloppyDriver_ReadRegisterMainStatus(self)) 
        {
            return FloppyDriver_ReadRegisterDataFIFO(self);
        }
    }

    printf("Floppy: Read Commnd timeout\n");
    return 0; // not reached
}

void FloppyDriver_CheckInterrupt(struct FloppyDriver* self, int *st0, int *cyl) 
{
    if (self->Debug) printf("Floppy: Check Interrupt\n");
    FloppyDriver_WriteCommand(self, FLOPPY_CMD_SENSE_INTERRUPT);
    *st0 = FloppyDriver_ReadData(self);
    *cyl = FloppyDriver_ReadData(self);
}

// Move to cylinder 0, which calibrates the drive..
bool FloppyDriver_Calibrate(struct FloppyDriver* self) 
{
    if (self->Debug) printf("Floppy: Calibrate\n");
    int i, st0, cyl = -1; // set to bogus cylinder

    FloppyDriver_Motor(self, floppy_motor_on);

    for(i = 0; i < 10; i++)
    {
        if (self->Debug) printf("Floppy: Calibrate Attempt %d\n",i);
        self->InterruptWaiting = false;
        // Attempt to positions head to cylinder 0
        FloppyDriver_WriteCommand(self, FLOPPY_CMD_RECALIBRATE);
        FloppyDriver_WriteCommand(self, 0); // argument is drive, we only support 0

        FloppyDriver_IRQWait(self);
        FloppyDriver_CheckInterrupt(self,  &st0, &cyl);
        if (self->Debug) printf("Floppy: Calibrate st0 == 0x%x\n",st0); 

        if(st0 & 0xC0) 
        {
            static const char * status[] =
            { 0, "error", "invalid", "drive" };
            if (self->Debug) printf("FloppyDriver_Calibrate: status = %s\n", status[st0 >> 6]);
            continue;
        }

        if(!cyl) 
        { // found cylinder 0 ?
            if (self->Debug) printf("Floppy: Calibrate: Found Cylinder 0\n");
            FloppyDriver_Motor(self, floppy_motor_off);
            return true;
        }
    }

    if (self->Debug) printf("Floppy: Calibrate: 10 retries exhausted\n");
    FloppyDriver_Motor(self, floppy_motor_off);
    return false;
}

bool FloppyDriver_Reset(struct FloppyDriver* self) 
{
    if (self->Debug) printf("Floppy: Reset\n");
    self->InterruptWaiting = false;
    FloppyDriver_WriteRegisterDigitalOutput(self, 0x00); // disable controller
    FloppyDriver_WriteRegisterDigitalOutput(self, 0x0C); // enable controller

    FloppyDriver_IRQWait(self); // sleep until interrupt occurs
    {
        int st0, cyl; // ignore these here..
        FloppyDriver_CheckInterrupt(self, &st0, &cyl);
    }

    // set transfer speed 500kb/s
    FloppyDriver_WriteRegisterConfigurationControl(self, 0x00);

    //  - 1st byte is: bits[7:4] = steprate, bits[3:0] = head unload time
    //  - 2nd byte is: bits[7:1] = head load time, bit[0] = no-DMA
    // 
    //  steprate    = (8.0ms - entry*0.5ms)*(1MB/s / xfer_rate)
    //  head_unload = 8ms * entry * (1MB/s / xfer_rate), where entry 0 -> 16
    //  head_load   = 1ms * entry * (1MB/s / xfer_rate), where entry 0 -> 128
    //
    FloppyDriver_WriteCommand(self, FLOPPY_CMD_SPECIFY);
    FloppyDriver_WriteCommand(self, 0xdf); /* steprate = 3ms, unload time = 240ms */
    FloppyDriver_WriteCommand(self, 0x02); /* load time = 16ms, no-DMA = 0 */

    // it could fail...
    return FloppyDriver_Calibrate(self);
}

void FloppyDriver_IRQWait(struct FloppyDriver* self)
{
    do
    {
        usleep(1);
    }
    while (!self->InterruptWaiting);
}

void FloppyDriver_Motor(struct FloppyDriver* self, int onoff) 
{
    if (self->Debug) printf("Floppy: Motor %s\n", onoff ? "On":"Off");
    if(onoff) 
    {
        if(!self->MotorState[0]) 
        {
            // need to turn on
            FloppyDriver_WriteRegisterDigitalOutput(self, 0x1C);
            usleep(500); // wait 500 ms = hopefully enough for modern drives
        }
        self->MotorState[0] = floppy_motor_on;
    } 
    else 
    {
        if(self->MotorState[0] == floppy_motor_wait) 
        {
            if (self->Debug) printf("floppy_motor: strange, fd motor-state already waiting..\n");
        }
        self->MotorTicks[0] = 300; // 3 seconds, see FloppyDriver_Timer() below
        self->MotorState[0] = floppy_motor_wait;
    }
}

void FloppyDriver_MotorKill(struct FloppyDriver* self) 
{
    if (self->Debug) printf("Floppy: Motor Kill\n");
    FloppyDriver_WriteRegisterDigitalOutput(self, 0x0c);
    self->MotorState[0] = floppy_motor_off;
}

//
// THIS SHOULD BE STARTED IN A SEPARATE THREAD.
//
//
void FloppyDriver_Timer(struct FloppyDriver* self) 
{
    while(true) 
    {
        // sleep for 500ms at a time, which gives around half
        // a second jitter, but that's hardly relevant here.
        usleep(500);
        if(self->MotorState[0] == floppy_motor_wait) 
        {
            self->MotorTicks[0] -= 50;
            if(self->MotorTicks[0] <= 0) 
            {
                FloppyDriver_MotorKill(self);
            }
        }
    }
}

// Seek for a given cylinder, with a given head
int FloppyDriver_Seek(struct FloppyDriver* self, unsigned cyli, int head) 
{
    unsigned i, st0, cyl = -1; // set to bogus cylinder

    FloppyDriver_Motor(self, floppy_motor_on);

    for(i = 0; i < 10; i++) 
    {
        self->InterruptWaiting = false;
        // Attempt to position to given cylinder
        // 1st byte bit[1:0] = drive, bit[2] = head
        // 2nd byte is cylinder number
        FloppyDriver_WriteCommand(self, FLOPPY_CMD_SEEK);
        FloppyDriver_WriteCommand(self, head<<2);
        FloppyDriver_WriteCommand(self, cyli);

        FloppyDriver_IRQWait(self);
        FloppyDriver_CheckInterrupt(self, &st0, &cyl);

        if(st0 & 0xC0) 
        {
            static const char * status[] =
            { "normal", "error", "invalid", "drive" };
            if (self->Debug) printf("FloppyDriver_Seek: status = %s\n", status[st0 >> 6]);
            continue;
        }

        if(cyl == cyli) 
        {
            FloppyDriver_Motor(self, floppy_motor_off);
            return 0;
        }

    }

    if (self->Debug) printf("FloppyDriver_Seek: 10 retries exhausted\n");
    FloppyDriver_Motor(self, floppy_motor_off);
    return -1;
}

void FloppyDriver_InterruptHandler()
{
    //printf("Floppy: Interrupt\n");
    struct FloppyDriver* self = &_Kernel.Floppy;
    self->InterruptWaiting = true;
}

static void FloppyDriver_DMAInit(struct FloppyDriver* self, enum FloppyDirection dir) 
{
    union 
    {
        unsigned char b[4]; // 4 bytes
        unsigned long l;    // 1 long = 32-bit
    } a, c; // address and count

    a.l = (unsigned) &fpy_dma_buf[0];
    c.l = (unsigned) FLOPPY_DMALEN - 1; // -1 because of DMA counting

    // check that address is at most 24-bits (under 16MB)
    // check that count is at most 16-bits (DMA limit)
    // check that if we add count and address we don't get a carry
    // (DMA can't deal with such a carry, this is the 64k boundary limit)
    if((a.l >> 24) || (c.l >> 16) || (((a.l&0xffff)+c.l)>>16)) 
    {
        if (self->Debug) printf("floppy_dma_init: static buffer problem\n");
    }

    unsigned char mode;
    switch(dir) 
    {
        // 01:0:0:01:10 = single/inc/no-auto/to-mem/chan2
        case floppy_dir_read:  
            mode = 0x46; 
            break;
        // 01:0:0:10:10 = single/inc/no-auto/from-mem/chan2
        case floppy_dir_write: 
            mode = 0x4a; 
            break;
        default: 
            if (self->Debug) printf("floppy_dma_init: invalid direction\n");
            return; // not reached, please "mode user uninitialized"
    }

    IO_WritePort8b(0x0a, 0x06);   // mask chan 2
    IO_WritePort8b(0x0c, 0xff);   // reset flip-flop
    IO_WritePort8b(0x04, a.b[0]); //  - address low byte
    IO_WritePort8b(0x04, a.b[1]); //  - address high byte
    IO_WritePort8b(0x81, a.b[2]); // external page register
    IO_WritePort8b(0x0c, 0xff);   // reset flip-flop
    IO_WritePort8b(0x05, c.b[0]); //  - count low byte
    IO_WritePort8b(0x05, c.b[1]); //  - count high byte
    IO_WritePort8b(0x0b, mode);   // set mode 
    IO_WritePort8b(0x0a, 0x02);   // unmask chan 2
}

// This monster does full cylinder (both tracks) transfer to
// the specified direction (since the difference is small).
//
// It retries (a lot of times) on all errors except write-protection
// which is normally caused by mechanical switch on the disk.
//
bool FloppyDriver_DoSector(struct FloppyDriver* self, struct CylinderHeadSector chs, enum FloppyDirection dir) 
{
    // transfer command, set below
    unsigned char cmd;

    // Read is MT:MF:SK:0:0:1:1:0, 
    // write   MT:MF:0:0:1:0:1
    // where MT = multitrack, MF = MFM mode, SK = skip deleted
    // 
    // Specify multitrack and MFM mode
    static const int flags = 0xC0;
    switch(dir) 
    {
        case floppy_dir_read:
            cmd = FLOPPY_CMD_READ_DATA | flags;
            break;
        case floppy_dir_write:
            cmd = FLOPPY_CMD_WRITE_DATA | flags;
            break;
        default: 

            if (self->Debug) printf("floppy_do_track: invalid direction\n");
            return false; // not reached, but pleases "cmd used uninitialized"
    }

    // seek both heads
    if(FloppyDriver_Seek(self, chs.Cylinder, chs.Head)) 
    {
        return false;
    }

    /*
    if(FloppyDriver_Seek(self, cyl, 1)) 
    {
        return false;
    }
    */

    int i;
    for(i = 0; i < 20; i++) 
    {
        FloppyDriver_Motor(self, floppy_motor_on);

        // init dma..
        FloppyDriver_DMAInit(self, dir);

        usleep(100); // give some time (100ms) to settle after the seeks

        self->InterruptWaiting = false;
        FloppyDriver_WriteCommand(self, cmd);  // set above for current direction

        FloppyDriver_WriteCommand(self, 0);    // 0:0:0:0:0:HD:US1:US0 = head and drive

        FloppyDriver_WriteCommand(self, chs.Cylinder);  // cylinder
        FloppyDriver_WriteCommand(self, chs.Head  );    // first head (should match with above) 0 == drive number
        FloppyDriver_WriteCommand(self, chs.Sector);    // first sector, strangely counts from 1

        FloppyDriver_WriteCommand(self, 2);    // bytes/sector, 128*2^x (x=2 -> 512)
        FloppyDriver_WriteCommand(self, 18);   // number of tracks to operate on
        FloppyDriver_WriteCommand(self, 0x1b); // GAP3 length, 27 is default for 3.5"
        FloppyDriver_WriteCommand(self, 0xff); // data length (0xff if B/S != 0)
        
        FloppyDriver_IRQWait(self); // don't SENSE_INTERRUPT here!

        // first read status information
        unsigned char st0, st1, st2, rcy, rhe, rse, bps;
        st0 = FloppyDriver_ReadData(self);
        st1 = FloppyDriver_ReadData(self);
        st2 = FloppyDriver_ReadData(self);
        /*
         * These are cylinder/head/sector values, updated with some
         * rather bizarre logic, that I would like to understand.
         *
         */
        rcy = FloppyDriver_ReadData(self);
        rhe = FloppyDriver_ReadData(self);
        rse = FloppyDriver_ReadData(self);
        // bytes per sector, should be what we programmed in
        bps = FloppyDriver_ReadData(self);

        int error = 0;

        if(st0 & 0xC0) 
        {
            static const char * status[] =
            { 0, "error", "invalid command", "drive not ready" };
            if (self->Debug) printf("Floppy: DoTrack Error - status = %s\n", status[st0 >> 6]);
            error = 1;
        }
        if(st1 & 0x80) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - end of cylinder\n");
            error = 1;
        }
        if(st0 & 0x08) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - drive not ready\n");
            error = 1;
        }
        if(st1 & 0x20) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - CRC error\n");
            error = 1;
        }
        if(st1 & 0x10) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - controller timeout\n");
            error = 1;
        }
        if(st1 & 0x04) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - no data found\n");
            error = 1;
        }
        if((st1|st2) & 0x01) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - no address mark found\n");
            error = 1;
        }
        if(st2 & 0x40) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - deleted address mark\n");
            error = 1;
        }
        if(st2 & 0x20) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - CRC error in data\n");
            error = 1;
        }
        if(st2 & 0x10) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - wrong cylinder\n");
            error = 1;
        }
        if(st2 & 0x04) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - uPD765 sector not found\n");
            error = 1;
        }
        if(st2 & 0x02) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - bad cylinder\n");
            error = 1;
        }
        if(bps != 0x2) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - wanted 512B/sector, got %d\n", (1<<(bps+7)));
            error = 1;
        }
        if(st1 & 0x02) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - not writable\n");
            error = 2;
        }

        if(!error) 
        {
            FloppyDriver_Motor(self, floppy_motor_off);
            return true;
        }
        if(error > 1) 
        {
            if (self->Debug) printf("Floppy: DoTrack Error - not retrying..\n");
            FloppyDriver_Motor(self, floppy_motor_off);
            return false;
        }
    }

    if (self->Debug) printf("Floppy: DoTrack Error - 20 retries exhausted\n");
    FloppyDriver_Motor(self, floppy_motor_off);
    return false;

}

bool FloppyDriver_ReadSectorLBA(struct FloppyDriver* self, uint32_t lba, uint8_t* buffer) 
{
    if (FloppyDriver_ReadSector(self, FloppyDriver_LbaToChs(lba)))
    {
        memcpy(buffer, fpy_dma_buf, FAT_SECTOR_SIZE);
        return true;
    }
    return false;
}

bool FloppyDriver_ReadSector(struct FloppyDriver* self, struct CylinderHeadSector chs) 
{
    return FloppyDriver_DoSector(self, chs, floppy_dir_read);
}

bool FloppyDriver_WriteSectorLBA(struct FloppyDriver* self, uint32_t lba, uint8_t* buffer) 
{
    memcpy(fpy_dma_buf, buffer,  FAT_SECTOR_SIZE);
    return FloppyDriver_WriteSector(self,FloppyDriver_LbaToChs(lba));
}

bool FloppyDriver_WriteSector(struct FloppyDriver* self, struct CylinderHeadSector chs) 
{
    return FloppyDriver_DoSector(self, chs, floppy_dir_write);
}

// Helper Functions ================================================================================

enum FloppyType FloppyDriver_GetType(uint8_t data)
{
    switch (data)
    {
        // 5.25" Disks
        case CMOS_FLOPPY_CONF_525_360:
            return FLOPPY_TYPE_525_360;
        case CMOS_FLOPPY_CONF_525_1200:
            return FLOPPY_TYPE_525_1200;
        // 3.5" Disks
        case CMOS_FLOPPY_CONF_35_720:
            return FLOPPY_TYPE_35_720;
        case CMOS_FLOPPY_CONF_35_1440:
            return FLOPPY_TYPE_35_1440;
        case CMOS_FLOPPY_CONF_35_2880:
            return FLOPPY_TYPE_35_2880;
        // None
        default:
        case CMOS_FLOPPY_CONF_NO_DRIVE:
            return FLOPPY_TYPE_NO_DRIVE;
    }
}

bool FloppyDriver_MasterPresent(struct FloppyDriver* self)
{
    return self->Master != FLOPPY_TYPE_NO_DRIVE;
}

bool FloppyDriver_SlavePresent(struct FloppyDriver* self)
{
    return self->Slave != FLOPPY_TYPE_NO_DRIVE;
}

struct CylinderHeadSector FloppyDriver_LbaToChs(uint32_t lba)
{
    struct CylinderHeadSector chs;
    chs.Cylinder = lba / (2 * FLOPPY_144_NUM_SECTORS);
    chs.Head     = ((lba % (2 * FLOPPY_144_NUM_SECTORS)) / FLOPPY_144_NUM_SECTORS);
    chs.Sector   = ((lba % (2 * FLOPPY_144_NUM_SECTORS)) % FLOPPY_144_NUM_SECTORS + 1);
    return chs;
}

// Register Pokers =================================================================================

uint8_t FloppyDriver_ReadRegisterStatusA(struct FloppyDriver* self) 
{
    uint8_t data = IO_ReadPort8b(FLOPPY_REGISTER_STATUS_A);
    if (self->DebugIO) printf("Floppy: Reading Status A 0x%x\n",data);
    return data;
}

uint8_t FloppyDriver_ReadRegisterStatusB(struct FloppyDriver* self) 
{
    uint8_t data = IO_ReadPort8b(FLOPPY_REGISTER_STATUS_B);
    if (self->DebugIO) printf("Floppy: Reading Status B 0x%x\n",data);
    return data;
}

uint8_t FloppyDriver_ReadRegisterDigitalOutput(struct FloppyDriver* self)
{
    uint8_t data = IO_ReadPort8b(FLOPPY_REGISTER_DIGITAL_OUTPUT);
    if (self->DebugIO) printf("Floppy: Reading Digital Output 0x%x\n",data);
    return data;
}

void FloppyDriver_WriteRegisterDigitalOutput(struct FloppyDriver* self, uint8_t data)
{
    if (self->DebugIO) printf("Floppy: Writing Digital Output 0x%x\n",data);
    IO_WritePort8b(FLOPPY_REGISTER_DIGITAL_OUTPUT, data);
}

uint8_t FloppyDriver_ReadRegisterTapeDrive(struct FloppyDriver* self)
{
    uint8_t data = IO_ReadPort8b(FLOPPY_REGISTER_TAPE_DRIVE);
    if (self->DebugIO) printf("Floppy: Reading Tape Register 0x%x\n",data);
    return data;
}

void FloppyDriver_WriteRegisterTapeDrive(struct FloppyDriver* self, uint8_t data)
{
    if (self->DebugIO) printf("Floppy: Writing Tape Register 0x%x\n",data);
    IO_WritePort8b(FLOPPY_REGISTER_TAPE_DRIVE, data);
}

uint8_t FloppyDriver_ReadRegisterMainStatus(struct FloppyDriver* self)
{
    uint8_t status = IO_ReadPort8b(FLOPPY_REGISTER_MAIN_STATUS);
    if (self->DebugIO) printf("Floppy: Read Main Status Register 0x%x\n",status);
    return status;
}

void FloppyDriver_WriteRegisterDataRateSelect(struct FloppyDriver* self, uint8_t data)
{
    if (self->DebugIO) printf("Floppy: Writing Data Rate Select 0x%x\n",data);
    IO_WritePort8b(FLOPPY_REGISTER_DATARATE_SELECT,data);
}

uint8_t FloppyDriver_ReadRegisterDataFIFO(struct FloppyDriver* self)
{
    uint8_t data = IO_ReadPort8b(FLOPPY_REGISTER_DATA_FIFO);
    if (self->DebugIO) printf("Floppy: Reading FIFO 0x%x\n",data);
    return data;
}

void FloppyDriver_WriteRegisterDataFIFO(struct FloppyDriver* self, uint8_t data)
{
    if (self->DebugIO) printf("Floppy: Writing FIFO 0x%x\n",data);
    IO_WritePort8b(FLOPPY_REGISTER_DATA_FIFO, data);
}

uint8_t FloppyDriver_ReadRegisterDigitalInput(struct FloppyDriver* self)
{
    uint8_t data = IO_ReadPort8b(FLOPPY_REGISTER_DIGITAL_INPUT);
    if (self->DebugIO) printf("Floppy: Reading Digital Input 0x%x\n",data);
    return data;
}

void FloppyDriver_WriteRegisterConfigurationControl(struct FloppyDriver* self, uint8_t data)
{
    if (self->DebugIO) printf("Floppy: Reading Config Control 0x%x\n",data);
    IO_WritePort8b(FLOPPY_REGISTER_CONFIGURATION_CONTROL, data);
}

void FloppyDriver_DebugDMABuffer(struct FloppyDriver* self)
{
    int i;
    printf("----------------------------------------");
    for (i=0; i < 128; i++)
    {
        if (i % 16 == 0)
        {
            printf("\n0x%02x: ",i);
        }
        printf("%02x ",fpy_dma_buf[i]);
    }
    printf("\n----------------------------------------\n");
}

uint8_t* FloppyDriver_GetDMABuffer(struct FloppyDriver* self)
{
    return &fpy_dma_buf[0];
}