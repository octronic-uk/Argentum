.global tkKeyboardHandlerASM
.extern tkKeyboardHandler

tkKeyboardHandlerASM:
	call    tkKeyboardHandler
	iret
