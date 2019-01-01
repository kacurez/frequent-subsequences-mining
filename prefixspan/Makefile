#ORGINAL MAKEFILE
srcdir=./
builddir=build/
OBJECTS= $(srcdir)definitions.o $(srcdir)interface.o $(srcdir)frequentitem.o $(srcdir)algorithm.o $(srcdir)main.o
FLAGS=-Wall -o4
CC=g++
#MYSHELL = c:/command.com /c

mesg:
	@echo "Compiling prefixspan 0.1(level-by-level).."

all:$(mesg) $(OBJECTS)    
	$(CC) $(OBJECTS) $(CFLAGS) -o $(builddir)prefixspan
clean:
	del *.o
       

  
