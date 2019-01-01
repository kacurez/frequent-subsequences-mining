#ORGINAL MAKEFILE
srcdir=./
builddir=build/
OBJECTS= $(srcdir)cmd_line_options.o $(srcdir)definitions.o $(srcdir)interface.o $(srcdir)algorithm.o $(srcdir)main.o
FLAGS=-Wall -o4
CC=g++
#MYSHELL = c:/command.com /c

mesg:
	@echo "Compiling spade 0.1.."
all:$(mesg) $(OBJECTS)    
	$(CC) $(OBJECTS) $(CFLAGS) -o $(builddir)spade
clean:
	del *.o
       

  
