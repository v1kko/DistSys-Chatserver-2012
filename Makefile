CC=g++

HD=headers
BIN=bin
OL=objects
DOC=doc

WARNING_FLAGS=-Wall -Wextra -Wshadow -pedantic-errors
CFLAGS= -ggdb -I$(HD) $(WARNING_FLAGS)
LDFLAGS= -ggdb -I$(HD)

VPATH = src:$(DOC)

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $(OL)/$@

all: manager.o database.o connection.o main.o message.o server.o $(BIN)/server verslag.pdf $(HD)/*.h

$(BIN)/server: $(OL)/manager.o $(OL)/database.o $(OL)/message.o $(OL)/connection.o $(OL)/server.o $(OL)/main.o
	$(CC) $(LDFLAGS) -o $@ $^ 

verslag.pdf: verslag.tex
	pdflatex -output-directory $(DOC) $< 
	rm -f */*.aux
	rm -f */*.log

clean:
	rm -f */*.o */server *.o */verslag.pdf

message.o: message.cpp
connection.o: connection.cpp
server.o: server.cpp
main.o: main.cpp
database.o: database.cpp
manager.o: manager.cpp
