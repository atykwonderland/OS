mysh: shell.c interpreter.c shellmemory.c cpu.c kernel.c pcb.c
	gcc -c shell.c interpreter.c shellmemory.c cpu.c kernel.c pcb.c
	gcc -o mysh shell.o interpreter.o shellmemory.o cpu.o kernel.o pcb.o

clean: 
	rm mysh; rm *.o

debug: shell.c interpreter.c shellmemory.c cpu.c kernel.c pcb.c
	gcc -c shell.c interpreter.c shellmemory.c cpu.c kernel.c pcb.c
	gcc -g -o mysh shell.o interpreter.o shellmemory.o cpu.o kernel.o pcb.o