CC=g++

HD=headers
BIN=bin
OL=objects
DOC=doc

WARNING_FLAGS=-Wall -Wextra -Werror -Wshadow -pedantic-errors
CFLAGS= -ggdb -I$(HD) $(WARNING_FLAGS)
LDFLAGS= -ggdb -I$(HD)

VPATH = src:$(DOC)

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $(OL)/$@

all: message.o communication.o $(BIN)/server verslag.pdf $(HD)/*.h

$(BIN)/server: $(OL)/message.o $(OL)/communication.o
	$(CC) $(LDFLAGS) -o $@ $^ 

verslag.pdf: verslag.tex
	pdflatex -output-directory $(DOC) $< 
	rm -f */*.aux
	rm -f */*.log

clean:
	rm -f */*.o */server *.o */verslag.pdf

message.o: message.cpp
communication.o: communication.cpp

