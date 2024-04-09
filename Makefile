assembler: assembler.o pre_assembler.o first_scan.o
	gcc -Wall -ansi -pedantic assembler.o pre_assembler.o -o assembler
assembler.o: assembler.c
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o
pre_assembler.o: pre_assembler.c
	gcc -c -Wall -ansi -pedantic pre_assembler.c -o pre_assembler.o
first_scan.o: first_scan.c
	gcc -c -Wall -ansi -pedantic first_scan.c -o first_scan.o