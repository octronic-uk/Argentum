/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(8K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (uninitialized) and stack */
	.bss /*BLOCK(1024K) : */ALIGN(4K) :
	{
		*(COMMON)
		*(.bss)
	}

	/* Read-write data (initialized) */
	.data BLOCK(8K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (initialized) */
	.floppy_dma_buffer BLOCK(64K) : ALIGN(32K)
	{
		*(.floppy_dma_buffer)
	}

	/* The compiler may produce other sections, put them in the proper place in
	   in this file, if you'd like to include them in the final kernel. */
}
