CC=g++

WARNING_FLAGS=-Wall -Wextra -Werror -Wshadow -pedantic-errors
CFLAGS= -ggdb $(WARNING_FLAGS)
LDFLAGS= -ggdb

.cpp.o:
	$(CC) -c $(CFLAGS) $<

all: server 

server: message.o communication.o
	$(CC) $(LDFLAGS) -o $@ $^ 

verslag: verslag.tex
	pdflatex verslag.tex
	rm -f *.aux
	rm -f *.log

clean:
	rm -f *.o server

message.o: message.cpp *.h
communication.o: communication.cpp *.h

