global tkInterruptReadPort
global tkInterruptWritePort
global tkInterruptLoadIDT

tkInterruptReadPort:
	mov edx, [esp + 4]
	in al, dx	
	ret

tkInterruptWritePort:
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
	out   dx, al  
	ret

tkInterruptLoadIDT:
	mov edx, [esp + 4]
	lidt [edx]
	sti 				;turn on interrupts
	ret

