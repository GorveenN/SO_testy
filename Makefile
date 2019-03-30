OBJECTS = main.o euron.o
CC = g++
CFLAGS = -Wall -Werror -c -O3 -std=c++17
AS = nasm
ASFLAGS = -DN=1000 -f elf64 -F dwarf -g
LINKFLAGS = -lpthread -fno-pie -no-pie

all: $(OBJECTS)
	$(CC) $(LINKFLAGS) $(OBJECTS) -o euronmain -lstdc++fs
	rm -rf *.o

run: all
	./euronmain

%.o: %.cpp
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) euronmain
