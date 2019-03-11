.global tkKeyboard_HandlerASM
.extern tkKeyboard_Handler

tkKeyboard_HandlerASM:
	call    tkKeyboard_Handler
	iret
