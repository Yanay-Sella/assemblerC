assembler: assembler.o pre_assembler.o first_scan.o second_scan.o global.o
	gcc -Wall -ansi -pedantic assembler.o pre_assembler.o first_scan.o second_scan.o global.o -o assembler
assembler.o: assembler.c
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o
pre_assembler.o: pre_assembler.c
	gcc -c -Wall -ansi -pedantic pre_assembler.c -o pre_assembler.o
first_scan.o: first_scan.c
	gcc -c -Wall -ansi -pedantic first_scan.c -o first_scan.o
global.o: global.c
	gcc -c -Wall -ansi -pedantic global.c -o global.o
second_scan.o: second_scan.c
	gcc -c -Wall -ansi -pedantic second_scan.c -o second_scan.o
