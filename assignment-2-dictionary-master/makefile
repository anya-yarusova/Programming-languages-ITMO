ASM = nasm
FLAGS = -felf64 -g -o
LD = ld

.PHONY: clean, all
all: main
clean:
	rm -rf lib.o dict.o main.o main

main.o: main.asm lib.inc colon.inc words.inc
	$(ASM) $(FLAGS) $@ $<
main: main.o lib.o dict.o
	$(LD) -o $@ $^
lib.o: lib.asm
	$(ASM) $(FLAGS) $@ $<
dict.o: dict.asm lib.inc	
	$(ASM) $(FLAGS) $@ $<