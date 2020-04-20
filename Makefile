#Makefile for "Text Editor" C++ application
#Created by Aniswar Krishnan on 20/4/20

PROG = textor
CC = g++
CPPFLAGS = -g -std=c++17
OBJS = main.o fileio.o find.o input.o output.o rowops.o syntaxHighlight.o terminal.o
HEADER = editorConfig.h editorHighlight.h editorKey.h editorSyntax.h find.h fileio.h input.h output.h rowops.h syntaxHighlight.h terminal.h 

$(PROG) : $(OBJS)
		$(CC) -o $(PROG) $(OBJS)
main.o : $(HEADER)
		$(CC) $(CPPFLAGS) -c main.cpp
fileio.o : $(HEADER)
		$(CC) $(CPPFLAGS) -c fileio.cpp
find.o : $(HEADER)
		$(CC) $(CPPFLAGS) -c find.cpp
input.o : $(HEADER)
		$(CC) $(CPPFLAGS) -c input.cpp
output.o : $(HEADER)
		$(CC) $(CPPFLAGS) -c output.cpp
rowops.o : $(HEADER)
		$(CC) $(CPPFLAGS) -c rowops.cpp
syntaxHighlight.o : $(HEADER)
		$(CC) $(CPPFLAGS) -c syntaxHighlight.cpp
terminal.o : $(HEADER)
		$(CC) $(CPPFLAGS) -c terminal.cpp
clean: 
		rm -f core $(PROG) $(OBJS)
