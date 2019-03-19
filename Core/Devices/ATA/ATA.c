#include "ATA.h"

#include <Devices/IO/IO.h>
#include <Devices/PCI/PCI.h>
#include <LibC/include/unistd.h>
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

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
        // Check if this device need an IRQ assignment:
        /*
        IO_WritePortLong(
            (1 << 31) | 
            (ata_device->mBus << 16) | 
            (ata_device->mDevice << 11) | 
            (ata_device->mFunction << 8) | 
            0x3C, 0xCF8
        );

        // Change the IRQ field to 0xFE
        IO_WritePortLong(0xFE, 0xCFC); 
        */
        
        PCI_DeviceWriteConfig8b(ata_device, PCI_DEVICE_INTERRUPT_LINE_OFFSET, 0xFE);

/*
        // Read the IRQ Field Again.
        IO_WritePortLong(
            (1<<31) | 
            (ata_device->mBus<<16) | 
            (ata_device->mDevice<<11) | 
            (ata_device->mFunction<<8) | 
            0x3C, 0xCF8
        ); 
*/
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