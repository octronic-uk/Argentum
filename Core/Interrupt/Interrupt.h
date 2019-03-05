#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

struct tkInterruptDescriptorTableEntry
{
	unsigned short int mOffsetLowerBits;
	unsigned short int mSelector;
	unsigned char mZero;
	unsigned char mTypeAttribute;
	unsigned short int mOffsetHigherBits;
};

struct tkInterruptDescriptorTableEntry InterruptDescriptorTable[IDT_SIZE];

extern unsigned char tkInterruptReadPort(unsigned char port);
extern void tkInterruptWritePort(unsigned char port, unsigned char data);
extern void tkInterruptLoadIDT(unsigned long* idt);

void tkInterruptInitialiseDescriptorTable();