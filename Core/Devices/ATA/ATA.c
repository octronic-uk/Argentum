#include "ATA.h"

#include <Devices/IO/IO.h>
#include <Devices/PCI/PCI.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

uint8_t ATA_Debug = 0;
ATA_Channel ATA_Channels[2];
ATA_IDEDevice ATA_IDEDevices[4];

unsigned char ATA_IDEBuffer[2048] = {0};
unsigned static char ATA_IDEIrqInvoked = 0;
unsigned static char ATA_ATAPIPacket[12] = {0};

void ATA_Constructor()
{
    printf("ATA: Constructing\n");
    memset(ATA_IDEBuffer,0,2048);
    memset(ATA_Channels,0,sizeof(ATA_Channel)*2);
    memset(ATA_IDEDevices,0,sizeof(ATA_IDEDevice)*4);
    memset(ATA_ATAPIPacket,0,12);
    ATA_ATAPIPacket[0] = 0xA8;
    PCI_ConfigHeader* ata_device = PCI_GetATADevice();
    if (ata_device)
    {
        PCI_DeviceWriteConfig8b(ata_device, PCI_DEVICE_INTERRUPT_LINE_OFFSET, 0xFE);
        uint8_t readInterruptLine = PCI_DeviceReadConfig8b(ata_device,PCI_DEVICE_INTERRUPT_LINE_OFFSET);

        // This Device needs IRQ assignment.
        if (readInterruptLine != 0xFE)
        {
            if (ATA_Debug)
            {
                printf("ATA: Couldn't set interrupt line, device needs IRQ Assignment (NOT IMPLEMENTED)\n");
            }
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

        ATA_IDEInit(
                    ATA_IDEBuffer, ATA_IDEDevices, ATA_Channels,
                    b0,b1,b2,b3,ata_device->mBAR4
                    );
    }
    else
    {
        printf("ATA: No ATA Device Found in PCI Config Space\n");
    }
}

void ATA_IDEInit
(
        unsigned char* ide_buf, ATA_IDEDevice* ide_devices, ATA_Channel* channels,
        unsigned int BAR0, unsigned int BAR1,
        unsigned int BAR2, unsigned int BAR3,
        unsigned int BAR4
        )
{
    if (ATA_Debug) {
        printf("ATA: Initialising\n");
        printf("\tB0:0x%x \n\tB1:0x%x \n\tB2:0x%x \n\tB3:0x%x \n\tB4:0x%x\n",
               BAR0, BAR1, BAR2, BAR3, BAR4
               );
    }
    int i, j, k, count = 0;

    // 1- Detect I/O Ports which interface IDE Controller:
    channels[ATA_PRIMARY].base  = (BAR0 &= 0xFFFFFFFC) + 0x1F0*(!BAR0);
    channels[ATA_PRIMARY].ctrl  = (BAR1 &= 0xFFFFFFFC) + 0x3F4*(!BAR1);
    channels[ATA_PRIMARY].bmide = (BAR4 &= 0xFFFFFFFC) + 0; // Bus Master IDE

    channels[ATA_SECONDARY].base  = (BAR2 &= 0xFFFFFFFC) + 0x170*(!BAR2);
    channels[ATA_SECONDARY].ctrl  = (BAR3 &= 0xFFFFFFFC) + 0x374*(!BAR3);
    channels[ATA_SECONDARY].bmide = (BAR4 &= 0xFFFFFFFC) + 8; // Bus Master IDE

    if (ATA_Debug)
    {
        printf("ATA Ports:\n");
        printf("\tprimary base    0x%x\n",channels[ATA_PRIMARY].base);
        printf("\tprimary ctrl    0x%x\n",channels[ATA_PRIMARY].ctrl);
        printf("\tprimary bmide   0x%x\n",channels[ATA_PRIMARY].bmide);
        printf("\tsecondary base  0x%x\n",channels[ATA_SECONDARY].base);
        printf("\tsecondary crtl  0x%x\n",channels[ATA_SECONDARY].ctrl);
        printf("\tsecondary bmide 0x%x\n",channels[ATA_SECONDARY].bmide);
    }
    // 2- Disable IRQs:
    if (ATA_Debug) printf("ATA: Disabling IRQs\n");
    ATA_IDEWrite(channels, ATA_PRIMARY  , ATA_REG_CONTROL, 2);
    ATA_IDEWrite(channels, ATA_SECONDARY, ATA_REG_CONTROL, 2);
    // 3- Detect ATA-ATAPI Devices:
    if (ATA_Debug) printf("ATA: Detecting\n");
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            unsigned char err = 0, type = IDE_ATA, status;
            ide_devices[count].reserved   = 0; // Assuming that no drive here.

            // (I) Select Drive:
            if (ATA_Debug) printf("ATA: Selecting %d\n",i);
            ATA_IDEWrite(channels, i, ATA_REG_HDDEVSEL, 0xA0 | (j<<4)); // Select Drive.
            usleep(20); // Wait 1ms for drive select to work.

            // (II) Send ATA Identify Command:
            if (ATA_Debug) printf("ATA: Sending identify command to %d\n",i);
            ATA_IDEWrite(channels, i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
            usleep(20); // This function should be implemented in your OS. which waits for 1 ms. it is based on System Timer Device Driver.

            // (III) Polling:
            if (ATA_Debug) printf("ATA: Polling %d,%d\n",i,j);
            if (!(ATA_IDERead(channels, i, ATA_REG_STATUS)))
            {
                if (ATA_Debug) printf("ATA: Polling %d,%d - No Device\n",i,j);
                continue; // If Status = 0, No Device.
            }
            if (ATA_Debug) printf("ATA: After Polling\n");
            while(1)
            {
                status = ATA_IDERead(channels, i, ATA_REG_STATUS);
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
                unsigned char cl = ATA_IDERead(channels, i,ATA_REG_LBA1);
                unsigned char ch = ATA_IDERead(channels, i,ATA_REG_LBA2);

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

                ATA_IDEWrite(channels, i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
                usleep(20);
            }

            // (V) Read Identification Space of the Device:
            ATA_IDEReadBuffer(channels, i, ATA_REG_DATA, (unsigned int) ide_buf, 128);

            // (VI) Read Device Parameters:
            ide_devices[count].reserved = 1;
            ide_devices[count].type = type;
            ide_devices[count].channel = i;
            ide_devices[count].drive = j;
            ide_devices[count].sign = ((unsigned short *) (ide_buf + ATA_IDENT_DEVICETYPE   ))[0];
            ide_devices[count].capabilities = ((unsigned short *) (ide_buf + ATA_IDENT_CAPABILITIES   ))[0];
            ide_devices[count].commandsets = ((unsigned int   *) (ide_buf + ATA_IDENT_COMMANDSETS   ))[0];

            // (VII) Get Size:
            if (ide_devices[count].commandsets & (1<<26))
            {
                // Device uses 48-Bit Addressing:
                ide_devices[count].size   = ((unsigned int   *) (ide_buf + ATA_IDENT_MAX_LBA_EXT   ))[0];
                // Note that Quafios is 32-Bit Operating System, So last 2 Words are ignored.
            }
            else
            {
                // Device uses CHS or 28-bit Addressing:
                ide_devices[count].size   = ((unsigned int   *) (ide_buf + ATA_IDENT_MAX_LBA   ))[0];
            }

            // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
            for(k = ATA_IDENT_MODEL; k < (ATA_IDENT_MODEL+40); k+=2)
            {
                ide_devices[count].model[k - ATA_IDENT_MODEL] = ide_buf[k+1];
                ide_devices[count].model[(k+1) - ATA_IDENT_MODEL] = ide_buf[k];
            }
            ATA_IDEDevices[count].model[40] = 0; // Terminate String.

            count++;
        }
    }
    if (ATA_Debug) printf("ATA: After for loops\n");
    // 4- Print Summary:
    for (i = 0; i < 4; i++)
    {
        if (ide_devices[i].reserved == 1)
        {
            printf("\tFound %s Drive %d MB - %s\n",
                   (const char *[]){"ATA", "ATAPI"}[ide_devices[i].type],         /* Type */
                    ide_devices[i].size/1024/2,               /* Size */
                    ide_devices[i].model);
        }
        else
        {
            if (ATA_Debug) printf("ATA: No summary for device %d\n",i);
        }
    }
}

unsigned char ATA_IDERead
(ATA_Channel* channels, unsigned char channel, unsigned char reg)
{

    if (ATA_Debug) {
        printf("ATA: Read ch:%d reg:%d\n",channel,reg);
    }
    unsigned char result;
    if   (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channels, channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }

    if (reg < 0x08)
    {
        result = IO_ReadPort8b(channels[channel].base  + reg - 0x00);
    }
    else if (reg < 0x0C)
    {
        result = IO_ReadPort8b(channels[channel].base  + reg - 0x06);
    }
    else if (reg < 0x0E)
    {
        result = IO_ReadPort8b(channels[channel].ctrl  + reg - 0x0A);
    }
    else if (reg < 0x16)
    {
        result = IO_ReadPort8b(channels[channel].bmide + reg - 0x0E);
    }

    if (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channels, channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }

    return result;
}

void ATA_IDEWrite(ATA_Channel* channels, unsigned char channel, unsigned char reg, unsigned char data)
{
    if (ATA_Debug) {
        printf("ATA: Write ch:%d reg: 0x%x data: 0x%x\n",channel,reg,data);
    }

    if (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channels, channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }

    if (reg < 0x08)
    {
        IO_WritePort8b(channels[channel].base  + reg - 0x00, data);
    }
    else if (reg < 0x0C)
    {
        IO_WritePort8b(channels[channel].base  + reg - 0x06, data);
    }
    else if (reg < 0x0E)
    {
        IO_WritePort8b(channels[channel].ctrl  + reg - 0x0A, data);
    }
    else if (reg < 0x16)
    {
        IO_WritePort8b(channels[channel].bmide + reg - 0x0E, data);
    }

    if (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channels, channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }
}

void ATA_IDEReadBuffer(ATA_Channel* channels, unsigned char channel, unsigned char reg, unsigned int buffer, unsigned int quads)
{
    if (ATA_Debug)
    {
        printf("ATA: ReadBuffer reg:0x%x quads:%d\n",reg,quads);
    }
    if   (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channels, channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }

    asm("pushw %es; movw %ds, %ax; movw %ax, %es");

    if   (reg < 0x08)
    {
        IO_ReadPort32bString(channels[channel].base  + reg - 0x00, (void*)buffer, quads);
    }
    else if   (reg < 0x0C)
    {
        IO_ReadPort32bString(channels[channel].base  + reg - 0x06, (void*)buffer, quads);
    }
    else if   (reg < 0x0E)
    {
        IO_ReadPort32bString(channels[channel].ctrl  + reg - 0x0A, (void*)buffer, quads);
    }
    else if   (reg < 0x16)
    {
        IO_ReadPort32bString(channels[channel].bmide + reg - 0x0E, (void*)buffer, quads);
    }

    asm("popw %es;");

    if   (reg > 0x07 && reg < 0x0C)
    {
        ATA_IDEWrite(channels, channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }
}

unsigned char ATA_IDEPolling(ATA_Channel* channels, unsigned char channel, unsigned int advanced_check)
{

    if (ATA_Debug)
    {
        printf("ATA: Polling\n");
    }
    // (I) Delay 400 nanosecond for BSY to be set:
    // -------------------------------------------------
    ATA_IDERead(channels, channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
    ATA_IDERead(channels, channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
    ATA_IDERead(channels, channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
    ATA_IDERead(channels, channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.

    // (II) Wait for BSY to be cleared:
    // -------------------------------------------------
    if (ATA_Debug) printf("ATA: Waiting for busy zero\n");
    while (ATA_IDERead(channels, channel, ATA_REG_STATUS) & ATA_SR_BSY); // Wait for BSY to be zero.

    if (advanced_check)
    {
        if (ATA_Debug) printf("ATA: Advanced check\n");
        unsigned char state = ATA_IDERead(channels, channel, ATA_REG_STATUS); // Read Status Register.

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

unsigned char ATA_IDEPrintError(ATA_Channel* channels, ATA_IDEDevice* ide_devices, unsigned int drive, unsigned char err)
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
        unsigned char st = ATA_IDERead(channels, ide_devices[drive].channel, ATA_REG_ERROR);
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
           (const char *[]){"Primary","Secondary"}[ide_devices[drive].channel],
            (const char *[]){"Master", "Slave"}[ide_devices[drive].drive],
            ide_devices[drive].model
            );

    return err;
}

/**
 * ATA/ATAPI Read/Write Modes:
* ++++++++++++++++++++++++++++++++
*  Addressing Modes:
*  ================
*   - LBA28 Mode.
*   - LBA48 Mode.
*   - CHS.
*  Reading Modes:
*  ================
*   - PIO Modes (0 : 6) - Slower than DMA, but not a problem.
*   - Single Word DMA Modes (0, 1, 2).
*   - Double Word DMA Modes (0, 1, 2).
*   - Ultra DMA Modes (0 : 6).
*  Polling Modes:
*  ================
*   - IRQs
*   - Polling Status - Suitable for Singletasking
*
* @brief This Function reads/writes sectors from ATA-Drive.
* @param direction if == 0 we are reading, else we are writing.
* @param drive, is drive number which can be from 0 to 3.
* @param lba, is the LBA Address which allows us to access disks up to 2TB.
* @param numsects, number of sectors to be read, it is a char, as reading more than 256 sector immediately may cause the OS to hang. notice that if numsects = 0, controller will know that we want 256 sectors.
* @param selector segment selector to read from, or write to.
* @param edi offset in the segment.
*/

unsigned char ATA_IDEAccess
(
        ATA_IDEDevice* ide_devices, ATA_Channel* channels,
        unsigned char direction, unsigned char drive, unsigned int lba,
        unsigned char numsects, unsigned short selector, unsigned int edi
        )
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
    unsigned int  channel = ide_devices[drive].channel;
    // Read the Drive [Master/Slave]
    unsigned int  slavebit = ide_devices[drive].drive;
    // The Bus Base, like [0x1F0] which is also data port.
    unsigned int  bus = channels[channel].base;
    // Approximatly all ATA-Drives has sector-size of 512-byte.
    unsigned int  words = 256;
    unsigned short cyl;
    unsigned short i;
    unsigned char ide_irq_invoked;
    ATA_IDEWrite(channels, channel, ATA_REG_CONTROL,
                 channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);

    // (I) Select one from LBA28, LBA48 or CHS;
    if (lba >= 0x10000000)
    { // Sure Drive should support LBA in this case, or you are giving a wrong LBA.
        // LBA48:
        lba_mode = 2;
        lba_io[0] = (lba & 0x000000FF)>> 0;
        lba_io[1] = (lba & 0x0000FF00)>> 8;
        lba_io[2] = (lba & 0x00FF0000)>>16;
        lba_io[3] = (lba & 0xFF000000)>>24;
        lba_io[4] = 0; // We said that we lba is integer, so 32-bit are enough to access 2TB.
        lba_io[5] = 0; // We said that we lba is integer, so 32-bit are enough to access 2TB.
        head = 0; // Lower 4-bits of HDDEVSEL are not used here.
    }
    else if (ide_devices[drive].capabilities & 0x200)
    { // Drive supports LBA?
        // LBA28:
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
    while (ATA_IDERead(channels, channel, ATA_REG_STATUS) & ATA_SR_BSY); // Wait if Busy.

    // (IV) Select Drive from the controller;
    if (lba_mode == 0)
    {
        ATA_IDEWrite(channels, channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit<<4) | head);   // Select Drive CHS.
    }
    else
    {
        ATA_IDEWrite(channels, channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit<<4) | head);   // Select Drive LBA.
    }

    // (V) Write Parameters;
    if (lba_mode == 2)
    {
        ATA_IDEWrite(channels, channel, ATA_REG_SECCOUNT1,   0);
        ATA_IDEWrite(channels, channel, ATA_REG_LBA3,   lba_io[3]);
        ATA_IDEWrite(channels, channel, ATA_REG_LBA4,   lba_io[4]);
        ATA_IDEWrite(channels, channel, ATA_REG_LBA5,   lba_io[5]);
    }
    ATA_IDEWrite(channels, channel, ATA_REG_SECCOUNT0,   numsects);
    ATA_IDEWrite(channels, channel, ATA_REG_LBA0,   lba_io[0]);
    ATA_IDEWrite(channels, channel, ATA_REG_LBA1,   lba_io[1]);
    ATA_IDEWrite(channels, channel, ATA_REG_LBA2,   lba_io[2]);

    // (VI) Select the command and send it;
    // Routine that is followed:
    // If ( DMA & LBA48)   DO_DMA_EXT;
    // If ( DMA & LBA28)   DO_DMA_LBA;
    // If ( DMA & LBA28)   DO_DMA_CHS;
    // If (!DMA & LBA48)   DO_PIO_EXT;
    // If (!DMA & LBA28)   DO_PIO_LBA;
    // If (!DMA & !LBA#)   DO_PIO_CHS;
    if (lba_mode == 0 && dma == 0 && direction == 0)
    {
        cmd = ATA_CMD_READ_PIO;
    }
    else if (lba_mode == 1 && dma == 0 && direction == 0)
    {
        cmd = ATA_CMD_READ_PIO;
    }
    else if (lba_mode == 2 && dma == 0 && direction == 0)
    {
        cmd = ATA_CMD_READ_PIO_EXT;
    }
    else if (lba_mode == 0 && dma == 1 && direction == 0)
    {
        cmd = ATA_CMD_READ_DMA;
    }
    else if (lba_mode == 1 && dma == 1 && direction == 0)
    {
        cmd = ATA_CMD_READ_DMA;
    }
    else if (lba_mode == 2 && dma == 1 && direction == 0)
    {
        cmd = ATA_CMD_READ_DMA_EXT;
    }
    else if (lba_mode == 0 && dma == 0 && direction == 1)
    {
        cmd = ATA_CMD_WRITE_PIO;
    }
    else if (lba_mode == 1 && dma == 0 && direction == 1)
    {
        cmd = ATA_CMD_WRITE_PIO;
    }
    else if (lba_mode == 2 && dma == 0 && direction == 1)
    {
        cmd = ATA_CMD_WRITE_PIO_EXT;
    }
    else if (lba_mode == 0 && dma == 1 && direction == 1)
    {
        cmd = ATA_CMD_WRITE_DMA;
    }
    else if (lba_mode == 1 && dma == 1 && direction == 1)
    {
        cmd = ATA_CMD_WRITE_DMA;
    }
    else if (lba_mode == 2 && dma == 1 && direction == 1)
    {
        cmd = ATA_CMD_WRITE_DMA_EXT;
    }

    ATA_IDEWrite(channels, channel, ATA_REG_COMMAND, cmd);               // Send the Command.

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
                err = ATA_IDEPolling(channels, channel, 1);
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
                ATA_IDEPolling(channels, channel, 0); // Polling.
                asm ("pushw %ds");
                asm ("mov %%ax, %%ds"::"a"(selector));
                asm ("rep outsw"::"c"(words), "d"(bus), "S"(edi)); // Send Data
                asm ("popw %ds");
                edi += (words*2);
            }
            ATA_IDEWrite(
                        channels,
                        channel,
                        ATA_REG_COMMAND,
                        (char [])
            {
                            ATA_CMD_CACHE_FLUSH,
                            ATA_CMD_CACHE_FLUSH,
                            ATA_CMD_CACHE_FLUSH_EXT
                        }[lba_mode]
                        );
            ATA_IDEPolling(channels, channel, 0); // Polling.
        }

        return 0; // Easy, ... Isn't it?
    }
}

