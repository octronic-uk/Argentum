#include "ATA.h"

#include <Drivers/IO/IO.h>
#include <Drivers/PCI/PCI.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

uint8_t ATA_Debug;
ATA_Channel ATA_Channels[2];

unsigned char ATA_IDEBuffer[2048] = {0};
unsigned char ATA_IDEIrqInvoked = 0;
unsigned char ATA_ATAPIPacket[12] = {0};
unsigned char ATA_ATAPIPackage[16] = {0};

void ATA_Constructor()
{
    ATA_Debug = 0;
    printf("ATA: Constructing\n");
    memset(ATA_IDEBuffer,0,2048);
    memset(ATA_Channels,0,sizeof(ATA_Channel)*2);
    memset(ATA_IDEDevices,0,sizeof(ATA_IDEDevice)*4);
    memset(ATA_ATAPIPacket,0,12);
    ATA_ATAPIPacket[0] = 0xA8;
    PCI_ConfigHeader* ata_device = PCI_GetATADevice();
    if (ata_device)
    {
        uint8_t readInterruptLine = PCI_DeviceReadConfig8b(ata_device, PCI_DEVICE_INTERRUPT_LINE_OFFSET);
        if (ATA_Debug)
        {
            printf("ATA: Current Interrupt Line 0x%x\n",readInterruptLine);
        }
        PCI_DeviceWriteConfig8b(ata_device, PCI_DEVICE_INTERRUPT_LINE_OFFSET, 0xFE);
        readInterruptLine = PCI_DeviceReadConfig8b(ata_device,PCI_DEVICE_INTERRUPT_LINE_OFFSET);

        // This Device needs IRQ assignment.
        if (readInterruptLine != 0xFE)
        {
                printf("ATA: Couldn't set interrupt line, device needs IRQ Assignment (NOT IMPLEMENTED)\n");
                printf("\t---[ BAIL OUT ]---\n");
                return;
        }
        // The Device doesn't use IRQs, check if this is an Parallel IDE:
        else
        {
            if (ATA_Debug)
            {
                printf("ATA: Device does not need IRQ Assignment\n");
                printf("ATA: ProgIF 0x%x\n",ata_device->mProgIF);
            }

            // This is a Parallel IDE Controller which use IRQ 14 and IRQ 15.
            if (ata_device->mClassCode == 0x01 &&
                ata_device->mSubclassCode == 0x01 &&
                (ata_device->mProgIF == 0x8A || ata_device->mProgIF == 0x80)
                )
            {
                if (ATA_Debug)
                {
                    printf("ATA:\t- Bus IS Parallel IDE Controller using IRQ 14/15\n");
                }
            }
        }


        uint32_t b0,b1,b2,b3;
        /* IMPORTANT!
            BAR0: Base address of primary channel (I/O space), if it is 0x0 or 0x1, the port is 0x1F0.
            BAR1: Base address of primary channel control port (I/O space), if it is 0x0 or 0x1, the port is 0x3F6.
            BAR2: Base address of secondary channel (I/O space), if it is 0x0 or 0x1, the port is 0x170.
            BAR3: Base address of secondary channel control port, if it is 0x0 or 0x1, the port is 0x376.
            BAR4: Bus Master IDE; refers to the base of I/O range consisting of 16 ports. Each 8 ports controls DMA on the primary and secondary channel respectively.
        */

        if (ata_device->mBAR0 == 0x00 || ata_device->mBAR0 == 0x01)
        {
            b0 = 0x1F0;
        }
        if (ata_device->mBAR1 == 0x00 || ata_device->mBAR1 == 0x01)
        {
            b1 = 0x3f4;
        }
        if (ata_device->mBAR2 == 0x00 || ata_device->mBAR2 == 0x01)
        {
            b2 = 0x170;
        }
        if (ata_device->mBAR3 == 0x00 || ata_device->mBAR3 == 0x01)
        {
            b3 = 0x374;
        }

        ATA_IDEInit(b0, b1, b2, b3, ata_device->mBAR4);
    }
    else
    {
        printf("ATA: No ATA Device Found in PCI Config Space\n");
    }
}

void ATA_IDEInit( unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3,  unsigned int BAR4)
{
    if (ATA_Debug) 
    {
        printf("ATA: Initialising\n");
        printf("\tB0:0x%x \n\tB1:0x%x \n\tB2:0x%x \n\tB3:0x%x \n\tB4:0x%x\n",
               BAR0, BAR1, BAR2, BAR3, BAR4
               );
    }
    int i, j, k, count = 0;

    // 1- Detect I/O Ports which interface IDE Controller:
    ATA_Channels[ATA_PRIMARY].base  = (BAR0 &= 0xFFFFFFFC) + 0x1F0*(!BAR0);
    ATA_Channels[ATA_PRIMARY].ctrl  = (BAR1 &= 0xFFFFFFFC) + 0x3F4*(!BAR1);
    ATA_Channels[ATA_PRIMARY].bmide = (BAR4 &= 0xFFFFFFFC) + 0; // Bus Master IDE

    ATA_Channels[ATA_SECONDARY].base  = (BAR2 &= 0xFFFFFFFC) + 0x170*(!BAR2);
    ATA_Channels[ATA_SECONDARY].ctrl  = (BAR3 &= 0xFFFFFFFC) + 0x374*(!BAR3);
    ATA_Channels[ATA_SECONDARY].bmide = (BAR4 &= 0xFFFFFFFC) + 8; // Bus Master IDE

    if (ATA_Debug)
    {
        printf("ATA Ports:\n");
        printf("\tprimary base    0x%x\n",ATA_Channels[ATA_PRIMARY].base);
        printf("\tprimary ctrl    0x%x\n",ATA_Channels[ATA_PRIMARY].ctrl);
        printf("\tprimary bmide   0x%x\n",ATA_Channels[ATA_PRIMARY].bmide);
        printf("\tsecondary base  0x%x\n",ATA_Channels[ATA_SECONDARY].base);
        printf("\tsecondary crtl  0x%x\n",ATA_Channels[ATA_SECONDARY].ctrl);
        printf("\tsecondary bmide 0x%x\n",ATA_Channels[ATA_SECONDARY].bmide);
    }
    // 2- Disable IRQs:
    if (ATA_Debug) printf("ATA: Disabling IRQs\n");
    ATA_IDEWrite(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
    ATA_IDEWrite(ATA_SECONDARY, ATA_REG_CONTROL, 2);
    // 3- Detect ATA-ATAPI Devices:
    if (ATA_Debug) printf("ATA: Detecting\n");
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            unsigned char err = 0, type = IDE_ATA, status;
            ATA_IDEDevices[count].reserved   = 0; // Assuming that no drive here.

            // (I) Select Drive:
            if (ATA_Debug) printf("ATA: Selecting %d\n",i);
            ATA_IDEWrite(i, ATA_REG_HDDEVSEL, 0xA0 | (j<<4)); // Select Drive.
            usleep(20); // Wait 1ms for drive select to work.

            // (II) Send ATA Identify Command:
            if (ATA_Debug) printf("ATA: Sending identify command to %d\n",i);
            ATA_IDEWrite(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
            usleep(20); // This function should be implemented in your OS. which waits for 1 ms. it is based on System Timer Device Driver.

            // (III) Polling:
            if (ATA_Debug) printf("ATA: Polling %d,%d\n",i,j);
            if (!(ATA_IDERead(i, ATA_REG_STATUS)))
            {
                if (ATA_Debug) printf("ATA: Polling %d,%d - No Device\n",i,j);
                continue; // If Status = 0, No Device.
            }

            while(1)
            {
                status = ATA_IDERead(i, ATA_REG_STATUS);
                if ( (status & ATA_SR_ERR))
                {
                    if (ATA_Debug) printf("ATA: Polling %d,%d Error status\n",i,j);
                    err = 1;
                    break;
                } // If Err, Device is not ATA.
                if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
                {
                    if (ATA_Debug) printf("ATA: Polling %d,%d Not ATA\n",i,j);
                    break; // Everything is right.
                }
            }

            // (IV) Probe for ATAPI Devices:

            if (ATA_Debug) printf("ATA: Probing for ATAPI %d,%d\n",i,j);
            if (err)
            {
                unsigned char cl = ATA_IDERead(i,ATA_REG_LBA1);
                unsigned char ch = ATA_IDERead(i,ATA_REG_LBA2);

                if   (cl == 0x14 && ch ==0xEB)
                {
                    type = IDE_ATAPI;
                }
                else if   (cl == 0x69 && ch ==0x96)
                {
                    type = IDE_ATAPI;
                }
                else
                {
                    continue; // Unknown Type (And always not be a device).
                }

                ATA_IDEWrite(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
                usleep(20);
            }

            // (V) Read Identification Space of the Device:
            ATA_IDEReadBuffer(i, ATA_REG_DATA, (unsigned int) ATA_IDEBuffer, 128);

            // (VI) Read Device Parameters:
            ATA_IDEDevices[count].reserved = 1;
            ATA_IDEDevices[count].type = type;
            ATA_IDEDevices[count].channel = i;
            ATA_IDEDevices[count].drive = j;
            ATA_IDEDevices[count].sign = ((unsigned short *) (ATA_IDEBuffer + ATA_IDENT_DEVICETYPE   ))[0];
            ATA_IDEDevices[count].capabilities = ((unsigned short *) (ATA_IDEBuffer + ATA_IDENT_CAPABILITIES   ))[0];
            ATA_IDEDevices[count].commandsets = ((unsigned int   *) (ATA_IDEBuffer + ATA_IDENT_COMMANDSETS   ))[0];

            // (VII) Get Size:
            if (ATA_IDEDevices[count].commandsets & (1<<26))
            {
                // Device uses 48-Bit Addressing:
                ATA_IDEDevices[count].size   = ((unsigned int*) (ATA_IDEBuffer + ATA_IDENT_MAX_LBA_EXT))[0];
            }
            else
            {
                // Device uses CHS or 28-bit Addressing:
                ATA_IDEDevices[count].size   = ((unsigned int*) (ATA_IDEBuffer + ATA_IDENT_MAX_LBA))[0];
            }

            // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
            for(k = ATA_IDENT_MODEL; k < (ATA_IDENT_MODEL+40); k+=2)
            {
                ATA_IDEDevices[count].model[k - ATA_IDENT_MODEL] = ATA_IDEBuffer[k+1];
                ATA_IDEDevices[count].model[(k+1) - ATA_IDENT_MODEL] = ATA_IDEBuffer[k];
            }
            ATA_IDEDevices[count].model[40] = 0; // Terminate String.

            count++;
        }
    }
    // 4- Print Summary:
    for (i = 0; i < 4; i++)
    {
        if (ATA_IDEDevices[i].reserved == 1)
        {
            printf("\t%d: Found %s Drive %d MB - %s\n", i, 
                   (const char *[]){"ATA", "ATAPI"}[ATA_IDEDevices[i].type],         /* Type */
                    ATA_IDEDevices[i].size/1024/2,               /* Size */
                    ATA_IDEDevices[i].model);
        }
        else
        {
            if (ATA_Debug) printf("\t%d: No summary for device\n",i);
        }
    }
}

unsigned char ATA_IDERead(unsigned char channel, unsigned char reg)
{

    if (ATA_Debug) 
    {
        printf("ATA: Read ch:%d reg:%d\n",channel,reg);
    }
    unsigned char result;
    if   (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channel, ATA_REG_CONTROL, 0x80 | ATA_Channels[channel].nIEN);
    }

    if (reg < 0x08)
    {
        result = IO_ReadPort8b(ATA_Channels[channel].base  + reg - 0x00);
    }
    else if (reg < 0x0C)
    {
        result = IO_ReadPort8b(ATA_Channels[channel].base  + reg - 0x06);
    }
    else if (reg < 0x0E)
    {
        result = IO_ReadPort8b(ATA_Channels[channel].ctrl  + reg - 0x0A);
    }
    else if (reg < 0x16)
    {
        result = IO_ReadPort8b(ATA_Channels[channel].bmide + reg - 0x0E);
    }

    if (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channel, ATA_REG_CONTROL, ATA_Channels[channel].nIEN);
    }

    return result;
}

void ATA_IDEWrite(unsigned char channel, unsigned char reg, unsigned char data)
{
    if (ATA_Debug) 
    {
        printf("ATA: Write ch:%d reg: 0x%x data: 0x%x\n",channel,reg,data);
    }

    if (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channel, ATA_REG_CONTROL, 0x80 | ATA_Channels[channel].nIEN);
    }

    if (reg < 0x08)
    {
        IO_WritePort8b(ATA_Channels[channel].base  + reg - 0x00, data);
    }
    else if (reg < 0x0C)
    {
        IO_WritePort8b(ATA_Channels[channel].base  + reg - 0x06, data);
    }
    else if (reg < 0x0E)
    {
        IO_WritePort8b(ATA_Channels[channel].ctrl  + reg - 0x0A, data);
    }
    else if (reg < 0x16)
    {
        IO_WritePort8b(ATA_Channels[channel].bmide + reg - 0x0E, data);
    }

    if (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channel, ATA_REG_CONTROL, ATA_Channels[channel].nIEN);
    }
}

void ATA_IDEReadBuffer(unsigned char channel, unsigned char reg, unsigned int buffer, unsigned int quads)
{
    if (ATA_Debug)
    {
        printf("ATA: ReadBuffer reg:0x%x quads:%d\n",reg,quads);
    }
    if   (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channel, ATA_REG_CONTROL, 0x80 | ATA_Channels[channel].nIEN);
    }

    asm("pushw %es; movw %ds, %ax; movw %ax, %es");

    if   (reg < 0x08)
    {
        IO_ReadPort32bString(ATA_Channels[channel].base  + reg - 0x00, (void*)buffer, quads);
    }
    else if   (reg < 0x0C)
    {
        IO_ReadPort32bString(ATA_Channels[channel].base  + reg - 0x06, (void*)buffer, quads);
    }
    else if   (reg < 0x0E)
    {
        IO_ReadPort32bString(ATA_Channels[channel].ctrl  + reg - 0x0A, (void*)buffer, quads);
    }
    else if   (reg < 0x16)
    {
        IO_ReadPort32bString(ATA_Channels[channel].bmide + reg - 0x0E, (void*)buffer, quads);
    }

    asm("popw %es;");

    if   (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channel, ATA_REG_CONTROL, ATA_Channels[channel].nIEN);
    }
}

unsigned char ATA_IDEPolling(unsigned char channel, unsigned int advanced_check)
{

    if (ATA_Debug)
    {
        printf("ATA: Polling\n");
    }
    // (I) Delay 400 nanosecond for BSY to be set:
    // -------------------------------------------------
    ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
    ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
    ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
    ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.

    // (II) Wait for BSY to be cleared:
    // -------------------------------------------------
    if (ATA_Debug) printf("ATA: Waiting for busy zero\n");
    while (ATA_IDERead(channel, ATA_REG_STATUS) & ATA_SR_BSY); // Wait for BSY to be zero.

    if (advanced_check)
    {
        if (ATA_Debug) printf("ATA: Advanced check\n");
        unsigned char state = ATA_IDERead(channel, ATA_REG_STATUS); // Read Status Register.

        // (III) Check For Errors:
        // -------------------------------------------------
        if (state & ATA_SR_ERR) return 2; // Error.

        // (IV) Check If Device fault:
        // -------------------------------------------------
        if (state & ATA_SR_DF ) return 1; // Device Fault.

        // (V) Check DRQ:
        // -------------------------------------------------
        // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
        if (!(state & ATA_SR_DRQ)) return 3; // DRQ should be set

    }

    return 0; // No Error.
}

unsigned char ATA_IDEPrintError(unsigned int drive, unsigned char err)
{
    if (err == 0)
    {
        return err;
    }

    printf("ATA IDE Error: ");
    if (err == 1)
    {
        printf("- Device Fault\n");
        err = 19;
    }

    else if (err == 2)
    {
        unsigned char st = ATA_IDERead(ATA_IDEDevices[drive].channel, ATA_REG_ERROR);
        if (st & ATA_ERR_AMNF)
        {
            printf("- No Address Mark Found\n");
            err = 7;
        }
        if (st & ATA_ERR_TKZNF)
        {
            printf("- No Media or Media Error\n");
            err = 3;
        }
        if (st & ATA_ERR_ABRT)
        {
            printf("- Command Aborted\n");
            err = 20;
        }
        if (st & ATA_ERR_MCR)
        {
            printf("- No Media or Media Error\n");
            err = 3;
        }
        if (st & ATA_ERR_IDNF)
        {
            printf("- ID mark not Found\n");
            err = 21;
        }
        if (st & ATA_ERR_MC)
        {
            printf("- No Media or Media Error\n");
            err = 3;
        }
        if (st & ATA_ERR_UNC)
        {
            printf("- Uncorrectable Data Error\n");
            err = 22;
        }
        if (st & ATA_ERR_BBK)
        {
            printf("- Bad Sectors\n");
            err = 13;
        }
    }
    else  if (err == 3)
    {
        printf("- Reads Nothing\n"); err = 23;
    }
    else  if (err == 4)
    {
        printf("- Write Protected\n"); err = 8;
    }
    printf("- [%s %s] %s\n",
           (const char *[]){"Primary","Secondary"}[ATA_IDEDevices[drive].channel],
            (const char *[]){"Master", "Slave"}[ATA_IDEDevices[drive].drive],
            ATA_IDEDevices[drive].model
            );

    return err;
}

unsigned char ATA_IDEAccess
(unsigned char direction, unsigned char drive, unsigned int lba,
unsigned char numsects, unsigned short selector, unsigned int edi)
{
    /* 0: CHS, 1:LBA28, 2: LBA48 */
    unsigned char lba_mode;
    /* 0: No DMA, 1: DMA */
    unsigned char dma;
    unsigned char cmd;
    unsigned char lba_io[6];
    unsigned char head;
    unsigned char sect;
    unsigned char err;
    // Read the Channel.
    unsigned int  channel = ATA_IDEDevices[drive].channel;
    // Read the Drive [Master/Slave]
    unsigned int  slavebit = ATA_IDEDevices[drive].drive;
    // The Bus Base, like [0x1F0] which is also data port.
    unsigned int  bus = ATA_Channels[channel].base;
    // Approximatly all ATA-Drives has sector-size of 512-byte.
    unsigned int  words = 256;
    unsigned short cyl;
    unsigned short i;
    //unsigned char ATA_IDEIrqInvoked;
    ATA_IDEWrite(channel, ATA_REG_CONTROL, ATA_Channels[channel].nIEN = (ATA_IDEIrqInvoked = 0x0) + 0x02);

    // (I) Select one from LBA28, LBA48 or CHS;
    if (lba >= 0x10000000)
    { // Sure Drive should support LBA in this case, or you are giving a wrong LBA.
        // LBA48:
        if (ATA_Debug) printf("ATA: Using LBA48\n");
        lba_mode = 2;
        lba_io[0] = (lba & 0x000000FF)>> 0;
        lba_io[1] = (lba & 0x0000FF00)>> 8;
        lba_io[2] = (lba & 0x00FF0000)>>16;
        lba_io[3] = (lba & 0xFF000000)>>24;
        lba_io[4] = 0; // We said that we lba is integer, so 32-bit are enough to access 2TB.
        lba_io[5] = 0; // We said that we lba is integer, so 32-bit are enough to access 2TB.
        head = 0; // Lower 4-bits of HDDEVSEL are not used here.
    }
    else if (ATA_IDEDevices[drive].capabilities & 0x200)
    { // Drive supports LBA?
        // LBA28:
        if (ATA_Debug) printf("ATA: Using LBA28\n");
        lba_mode = 1;
        lba_io[0] = (lba & 0x00000FF)>> 0;
        lba_io[1] = (lba & 0x000FF00)>> 8;
        lba_io[2] = (lba & 0x0FF0000)>>16;
        lba_io[3] = 0; // These Registers are not used here.
        lba_io[4] = 0; // These Registers are not used here.
        lba_io[5] = 0; // These Registers are not used here.
        head = (lba & 0xF000000)>>24;
    }
    else
    {
        // CHS:
        if (ATA_Debug) printf("ATA: Using CHS\n");
        lba_mode  = 0;
        sect = (lba % 63) + 1;
        cyl = (lba + 1  - sect)/(16*63);
        lba_io[0] = sect;
        lba_io[1] = (cyl>>0) & 0xFF;
        lba_io[2] = (cyl>>8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = (lba + 1  - sect)%(16*63)/(63); // Head number is written to HDDEVSEL lower 4-bits.
    }

    // (II) See if Drive Supports DMA or not;
    dma = 0; // Supports or doesn't, we don't support !!!

    // (III) Wait if the drive is busy;
    while (ATA_IDERead(channel, ATA_REG_STATUS) & ATA_SR_BSY); // Wait if Busy.

    // (IV) Select Drive from the controller;
    if (lba_mode == 0)
    {
        ATA_IDEWrite(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit<<4) | head);   // Select Drive CHS.
    }
    else
    {
        ATA_IDEWrite(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit<<4) | head);   // Select Drive LBA.
    }

    // (V) Write Parameters;
    if (lba_mode == 2)
    {
        ATA_IDEWrite(channel, ATA_REG_SECCOUNT1,   0);
        ATA_IDEWrite(channel, ATA_REG_LBA3,   lba_io[3]);
        ATA_IDEWrite(channel, ATA_REG_LBA4,   lba_io[4]);
        ATA_IDEWrite(channel, ATA_REG_LBA5,   lba_io[5]);
    }
    ATA_IDEWrite( channel, ATA_REG_SECCOUNT0,   numsects);
    ATA_IDEWrite( channel, ATA_REG_LBA0,   lba_io[0]);
    ATA_IDEWrite( channel, ATA_REG_LBA1,   lba_io[1]);
    ATA_IDEWrite( channel, ATA_REG_LBA2,   lba_io[2]);

    // (VI) Select the command and send it;
    // Routine that is followed:
    // If ( DMA & LBA48)   DO_DMA_EXT;
    // If ( DMA & LBA28)   DO_DMA_LBA;
    // If ( DMA & LBA28)   DO_DMA_CHS;
    // If (!DMA & LBA48)   DO_PIO_EXT;
    // If (!DMA & LBA28)   DO_PIO_LBA;
    // If (!DMA & !LBA#)   DO_PIO_CHS;

    // Read Modes
    if (lba_mode == 0 && dma == 0 && direction == 0)      cmd = ATA_CMD_READ_PIO;
    else if (lba_mode == 1 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
    else if (lba_mode == 2 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO_EXT;
    else if (lba_mode == 0 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
    else if (lba_mode == 1 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
    else if (lba_mode == 2 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA_EXT;
    // Write Modes
    else if (lba_mode == 0 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
    else if (lba_mode == 1 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
    else if (lba_mode == 2 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO_EXT;
    else if (lba_mode == 0 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
    else if (lba_mode == 1 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
    else if (lba_mode == 2 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA_EXT;

    ATA_IDEWrite(channel, ATA_REG_COMMAND, cmd);               // Send the Command.

    if (dma)
    {
        if (direction == 0)
        {
            // DMA Read.
        }
        else
        {
            // DMA Write.
        }
    }
    else
    {
        if (direction == 0)
        {
            // PIO Read.
            for (i = 0; i < numsects; i++)
            {
                err = ATA_IDEPolling(channel, 1);
                if (err)
                {
                    return err; // Polling, then set error and exit if there is.
                }
                asm ("pushw %es");
                asm ("mov %%ax, %%es"::"a"(selector));
                asm ("rep insw"::"c"(words), "d"(bus), "D"(edi)); // Receive Data.
                asm ("popw %es");
                edi += (words*2);
            }
        }
        else
        {
            // PIO Write.
            for (i = 0; i < numsects; i++)
            {
                ATA_IDEPolling(channel, 0); // Polling.
                asm ("pushw %ds");
                asm ("mov %%ax, %%ds"::"a"(selector));
                asm ("rep outsw"::"c"(words), "d"(bus), "S"(edi)); // Send Data
                asm ("popw %ds");
                edi += (words*2);
            }
            ATA_IDEWrite(channel, ATA_REG_COMMAND,
                (char [])
                {
                    ATA_CMD_CACHE_FLUSH,
                    ATA_CMD_CACHE_FLUSH,
                    ATA_CMD_CACHE_FLUSH_EXT
                }[lba_mode]
            );
            ATA_IDEPolling(channel, 0); // Polling.
        }

        return 0;
    }
}

void ATA_IDEWaitForIrq() 
{
   while (!ATA_IDEIrqInvoked);
   ATA_IDEIrqInvoked = 0;
}


//when an IRQ happens, the following function should be executed by ISR:
void ATA_IDEIrq() 
{
   ATA_IDEIrqInvoked = 1;
}

unsigned char ATA_IDEAtapiRead(unsigned char drive, unsigned int lba, unsigned char numsects, unsigned short selector, unsigned int edi) 
{
    unsigned int channel = ATA_IDEDevices[drive].channel;
    unsigned int slavebit = ATA_IDEDevices[drive].drive;
    unsigned int bus = ATA_Channels[channel].base;
    unsigned int words = 2048 / 2; // Sector Size in Words, Almost All ATAPI Drives has a sector size of 2048 bytes.
    unsigned char err; 
    int i;

      // Enable IRQs:
   ATA_IDEWrite(channel, ATA_REG_CONTROL, ATA_Channels[channel].nIEN = ATA_IDEIrqInvoked = 0x0);
   // (I): Setup SCSI Packet:
   // ------------------------------------------------------------------
   ATA_ATAPIPacket[ 0] = ATAPI_CMD_READ;
   ATA_ATAPIPacket[ 1] = 0x0;
   ATA_ATAPIPacket[ 2] = (lba>>24) & 0xFF;
   ATA_ATAPIPacket[ 3] = (lba>>16) & 0xFF;
   ATA_ATAPIPacket[ 4] = (lba>> 8) & 0xFF;
   ATA_ATAPIPacket[ 5] = (lba>> 0) & 0xFF;
   ATA_ATAPIPacket[ 6] = 0x0;
   ATA_ATAPIPacket[ 7] = 0x0;
   ATA_ATAPIPacket[ 8] = 0x0;
   ATA_ATAPIPacket[ 9] = numsects;
   ATA_ATAPIPacket[10] = 0x0;
   ATA_ATAPIPacket[11] = 0x0;
    // (II): Select the Drive:
   // ------------------------------------------------------------------
   ATA_IDEWrite(channel, ATA_REG_HDDEVSEL, slavebit<<4);
   // (III): Delay 400 nanosecond for select to complete:
   // ------------------------------------------------------------------
   ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
   ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
   ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
   ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
    // (IV): Inform the Controller that we use PIO mode:
   // ------------------------------------------------------------------
   ATA_IDEWrite(channel, ATA_REG_FEATURES, 0);         // PIO mode.


   // (V): Tell the Controller the size of buffer:
   // ------------------------------------------------------------------
   ATA_IDEWrite(channel, ATA_REG_LBA1, (words * 2) & 0xFF);   // Lower Byte of Sector Size.
   ATA_IDEWrite(channel, ATA_REG_LBA2, (words * 2)>>8);   // Upper Byte of Sector Size.


   // (VI): Send the Packet Command:
   // ------------------------------------------------------------------
   ATA_IDEWrite(channel, ATA_REG_COMMAND, ATA_CMD_PACKET);      // Send the Command.


   // (VII): Waiting for the driver to finish or invoke an error:
   // ------------------------------------------------------------------
   if (err = ATA_IDEPolling(channel, 1)) return err;         // Polling and return if error.


   // (VIII): Sending the packet data:
   // ------------------------------------------------------------------
   asm("rep   outsw"::"c"(6), "d"(bus), "S"(ATA_ATAPIPacket));   // Send Packet Data

   // (IX): Recieving Data:
   // ------------------------------------------------------------------
   for (i = 0; i < numsects; i++) 
   {
      ATA_IDEWaitForIrq();                  // Wait for an IRQ.
      if (err = ATA_IDEPolling(channel, 1)) return err;      // Polling and return if error.
      asm("pushw %es");
      asm("mov %%ax, %%es"::"a"(selector));
      asm("rep insw"::"c"(words), "d"(bus), "D"(edi));// Receive Data.
      asm("popw %es");
      edi += (words*2);
   }

   // (X): Waiting for an IRQ:
   // ------------------------------------------------------------------
   ATA_IDEWaitForIrq();

   // (XI): Waiting for BSY & DRQ to clear:
   // ------------------------------------------------------------------
   while (ATA_IDERead(channel, ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ));

   return 0; // Easy, ... Isn't it?
}

void ATA_IDEReadSectors
(unsigned char drive, unsigned char numsects, unsigned int lba, unsigned short es, unsigned int edi) 
{
   // 1: Check if the drive presents:
   // ==================================
   if (drive > 3 || ATA_IDEDevices[drive].reserved == 0) 
   {
       ATA_ATAPIPackage[0] = 0x1;      // Drive Not Found!
   }

   // 2: Check if inputs are valid:
   // ==================================
   else if (((lba + numsects) > ATA_IDEDevices[drive].size) && (ATA_IDEDevices[drive].type == IDE_ATA))
   {
      ATA_ATAPIPackage[0] = 0x2;                     // Seeking to invalid position.
   }
   // 3: Read in PIO Mode through Polling & IRQs:
   // ============================================
   else 
   {
      unsigned char err;
      if (ATA_IDEDevices[drive].type == IDE_ATA)
      {
         err = ATA_IDEAccess(ATA_READ, drive, lba, numsects, es, edi);
      }
      else if (ATA_IDEDevices[drive].type == IDE_ATAPI)
      {
         int i;
        for (i = 0; i < numsects; i++)
        {
            err = ATA_IDEAtapiRead(drive, lba + i, 1, es, edi + (i*2048));
        }
      }
      ATA_ATAPIPackage[0] = ATA_IDEPrintError(drive, err);
   }
}
// ATA_ATAPIPackage[0] is an entry of array, this entry specifies the Error Code, you can replace that.

void ATA_IDEWriteSectors
(unsigned char drive, unsigned char numsects, unsigned int lba, unsigned short es, unsigned int edi) 
{
   // 1: Check if the drive presents:
   // ==================================
   if (drive > 3 || ATA_IDEDevices[drive].reserved == 0) ATA_ATAPIPackage[0] = 0x1;      // Drive Not Found!
   // 2: Check if inputs are valid:
   // ==================================
   else if (((lba + numsects) > ATA_IDEDevices[drive].size) && (ATA_IDEDevices[drive].type == IDE_ATA))
      ATA_ATAPIPackage[0] = 0x2;                     // Seeking to invalid position.
   // 3: Read in PIO Mode through Polling & IRQs:
   // ============================================
   else 
   {
      unsigned char err;
      if (ATA_IDEDevices[drive].type == IDE_ATA)
      {
         err = ATA_IDEAccess(ATA_WRITE, drive, lba, numsects, es, edi);
      }
      else if (ATA_IDEDevices[drive].type == IDE_ATAPI)
      {
         err = 4; // Write-Protected.
      }
      ATA_ATAPIPackage[0] = ATA_IDEPrintError(drive, err);
   }
}

void ATA_IDEAtapiEject(unsigned char drive) 
{
   unsigned int   channel      = ATA_IDEDevices[drive].channel;
   unsigned int   slavebit      = ATA_IDEDevices[drive].drive;
   unsigned int   bus      = ATA_Channels[channel].base;
   unsigned int   words      = 2048 / 2;               // Sector Size in Words.
   unsigned char  err = 0;
   ATA_IDEIrqInvoked = 0;

   // 1: Check if the drive presents:
   // ==================================
   if (drive > 3 || ATA_IDEDevices[drive].reserved == 0) 
   {
       ATA_ATAPIPackage[0] = 0x1;      // Drive Not Found!
   }
   // 2: Check if drive isn't ATAPI:
   // ==================================
   else if (ATA_IDEDevices[drive].type == IDE_ATA) 
   {
       ATA_ATAPIPackage[0] = 20;         // Command Aborted.
   }
   // 3: Eject ATAPI Driver:
   // ============================================
   else 
   {
      // Enable IRQs:
      ATA_IDEWrite(channel, ATA_REG_CONTROL, ATA_Channels[channel].nIEN = ATA_IDEIrqInvoked = 0x0);

      // (I): Setup SCSI Packet:
      // ------------------------------------------------------------------
      ATA_ATAPIPacket[ 0] = ATAPI_CMD_EJECT;
      ATA_ATAPIPacket[ 1] = 0x00;
      ATA_ATAPIPacket[ 2] = 0x00;
      ATA_ATAPIPacket[ 3] = 0x00;
      ATA_ATAPIPacket[ 4] = 0x02;
      ATA_ATAPIPacket[ 5] = 0x00;
      ATA_ATAPIPacket[ 6] = 0x00;
      ATA_ATAPIPacket[ 7] = 0x00;
      ATA_ATAPIPacket[ 8] = 0x00;
      ATA_ATAPIPacket[ 9] = 0x00;
      ATA_ATAPIPacket[10] = 0x00;
      ATA_ATAPIPacket[11] = 0x00;

      // (II): Select the Drive:
      // ------------------------------------------------------------------
      ATA_IDEWrite(channel, ATA_REG_HDDEVSEL, slavebit<<4);

      // (III): Delay 400 nanosecond for select to complete:
      // ------------------------------------------------------------------
      ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
      ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
      ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
      ATA_IDERead(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.

      // (IV): Send the Packet Command:
      // ------------------------------------------------------------------
      ATA_IDEWrite(channel, ATA_REG_COMMAND, ATA_CMD_PACKET);      // Send the Command.

      // (V): Waiting for the driver to finish or invoke an error:
      // ------------------------------------------------------------------
      if (err = ATA_IDEPolling(channel, 1));            // Polling and stop if error.

      // (VI): Sending the packet data:
      // ------------------------------------------------------------------
      else 
      {
         asm("rep   outsw"::"c"(6), "d"(bus), "S"(ATA_ATAPIPacket));// Send Packet Data
         ATA_IDEWaitForIrq();                  // Wait for an IRQ.
         err = ATA_IDEPolling(channel, 1);            // Polling and get error code.
         if (err == 3) err = 0; // DRQ is not needed here.
      }
      ATA_ATAPIPackage[0] = ATA_IDEPrintError(drive, err); // Return;
   }
}

uint8_t ATA_DeviceUsesLBA48(ATA_IDEDevice device)
{
    // Device uses 48-Bit Addressing:
    if (device.commandsets & (1<<26))
    {
        return 1;
    }
    // Device uses CHS or 28-bit Addressing:
    else
    {
        return 0;
    }
}