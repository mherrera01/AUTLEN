#Macros definition
CC = gcc
CFLAGS = -g -Wall
OBJECTS = main.o transforma.o

#We specify that the files are not related with the rules, so that the makefile will not search for a file called as a rule
.PHONY: all clean

#Rules declaration
all: clean practica-1

practica-1: $(OBJECTS)
	$(CC) $(CFLAGS) -o$@ $(OBJECTS) -L. lib_afnd_64.a

#The $< symbol refers to the name of the .c to compile
main.o: main.c
	$(CC) $(CFLAGS) -c $<

transforma.o: transforma.c transforma.h 
	$(CC) $(CFLAGS) -c $<

#Clean all the generated files
clean:
	rm -rf *.o practica-1