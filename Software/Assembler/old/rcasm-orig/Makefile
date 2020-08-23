PROJECT=rcasm
PROJLNK=rclink
INCLUDE = .
OBJECTS = asm.o support.o mstrings.o asmcmds.o
CC = gcc
LIBS =

all : FLAGS = -I$(INCLUDE) -Wall

dos : FLAGS = -mno-cygwin -DWIN32 -I$(INCLUDE)

.SUFFIXES: .c .o

all: $(PROJECT) $(PROJLNK)

dos: $(PROJECT) $(PROJLNK)

$(PROJECT): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) $(LIBS) -o $(PROJECT)

$(PROJLNK): rclink.o
	$(CC) $(FLAGS) $(LIBS) rclink.o -o rclink

.c.o:
	$(CC) -c $(FLAGS) $<

asm.o: header.h
support.o: header.h

clean:
	-rm *.o
	-rm rcasm
	-rm rclink
	-rm *.prg
	-rm rcasm.exe
	-rm rclink.exe

dist:
	make clean
	-rmdir rcasm
	mkdir rcasm
	cp *.c rcasm
	cp *.h rcasm
	cp Makefile rcasm
	cp *.def rcasm
	cp *.doc rcasm
	tar cvfz rcasm.tgz ./rcasm

install:
	-mkdir /usr/lib/rcasm
	cp *.def /usr/lib/rcasm
	chmod a+rx /usr/lib/rcasm
	chmod a+r /usr/lib/rcasm/*
	cp rcasm /usr/bin
	cp rclink /usr/bin
	chmod a+rx /usr/bin/rcasm
	chmod a+rx /usr/bin/rclink

