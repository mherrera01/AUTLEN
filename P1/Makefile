#Macros definition
CC = gcc
CFLAGS = -g -Wall
OBJECTS = main.o transforma.o debugger.o list.o int.o estado.o

#We specify that the files are not related with the rules, so that the makefile will not search for a file called as a rule
.PHONY: all clean runv

#Rules declaration
all: clean practica-1

practica-1: $(OBJECTS)
	$(CC) $(CFLAGS) -o$@ $(OBJECTS) -L. lib_afnd_64.a

#The $< symbol refers to the name of the .c to compile
main.o: main.c
	$(CC) $(CFLAGS) -c $<

transforma.o: transforma.c transforma.h 
	$(CC) $(CFLAGS) -c $<

debugger.o: debugger.c debugger.h
	$(CC) $(CFLAGS) -c $<

list.o: list.c list.h 
	$(CC) $(CFLAGS) -c $<

int.o: int.c int.h 
	$(CC) $(CFLAGS) -c $<

estado.o: estado.c estado.h 
	$(CC) $(CFLAGS) -c $<

#Clean all the generated files
clean:
	rm -rf *.o practica-1 DEBUG.txt

runv:
	valgrind --leak-check=full ./practica-1
