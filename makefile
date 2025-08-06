CFLAGS = -Wall
CXXFLAGS = -std=c++11 -I./inc

MISC = ./misc
SRC = ./src
INC = ./inc
TEST = ./test

parser: $(MISC)/lexer.c $(MISC)/parser.tab.c
	gcc -c $(MISC)/parser.tab.c -o $(MISC)/parser.tab.o
	gcc -c $(MISC)/lexer.c -o $(MISC)/lexer.o
	g++ -c $(SRC)/assembler.cpp -o $(SRC)/assembler.o $(CXXFLAGS)
	g++ -o parser $(MISC)/parser.tab.o $(MISC)/lexer.o $(SRC)/assembler.o -lfl

$(MISC)/lexer.c: $(MISC)/lexer.l
	flex -o $@ $<

$(MISC)/parser.tab.c $(MISC)/parser.tab.h: $(MISC)/parser.y
	bison -d -t -o $(MISC)/parser.tab.c $<

clean:
	rm -f $(MISC)/*.tab.c $(MISC)/*.tab.h $(MISC)/lexer.c $(MISC)/*.o $(SRC)/*.o parser $(TEST)/*txt

.PHONY: all clean