assembler: assembler.o pre_assembler.o sym_scan.o
	gcc -Wall -ansi -pedantic assembler.o pre_assembler.o -o assembler
assembler.o: assembler.c
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o
pre_assembler.o: pre_assembler.c
	gcc -c -Wall -ansi -pedantic pre_assembler.c -o pre_assembler.o
sym_scan.o: sym_scan.c
	gcc -c -Wall -ansi -pedantic sym_scan.c -o sym_scan.o