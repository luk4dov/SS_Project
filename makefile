CXXFLAGS = -Wall -g

GPP = g++

MISC = ./misc
SRC = ./src
TEST = ./test
BUILD = build
ASM_TARGET = assembler

asm: $(ASM_TARGET)

SRC = ./src
SRCS = $(wildcard $(SRC)/*.cpp)
OBJS = $(patsubst $(SRC)/%.cpp, $(BUILD)/%.o, $(SRCS))

COMMON = ./common
COMMON_SRCS = $(wildcard $(COMMON)/*.cpp)
COMMON_HEADERS = $(wildcard $(COMMON)/*.hpp)
COMMON_OBJS = $(patsubst $(COMMON)/%.cpp, $(BUILD)/%.o, $(COMMON_SRCS))

INSTRUCTIONS = ./common/instructions
INSTRUCTION_SRCS = $(wildcard $(INSTRUCTIONS)/*.cpp)
INSTRUCTION_HEADERS = $(wildcard $(INSTRUCTIONS)/*.hpp)
INSTRUCTION_OBJS = $(patsubst $(INSTRUCTIONS)/%.cpp, $(BUILD)/%.o, $(INSTRUCTION_SRCS))

ALL_OBJ_FILES = $(LEXER_OBJ) $(PARSER_OBJ) $(INSTRUCTION_OBJS) $(COMMON_OBJS) $(OBJS)

LEXER_OBJ = $(BUILD)/lexer.yy.o
PARSER_OBJ = $(BUILD)/parser.tab.o

$(BUILD)/%.o: $(SRC)/%.cpp | $(BUILD)
	$(GPP) -c $< -Icommon -o $@

$(BUILD)/%.o: $(COMMON)/%.cpp | $(BUILD)
	$(GPP) -c $< -Icommon -o $@

$(BUILD)/%.o: $(INSTRUCTIONS)/%.cpp | $(BUILD)
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)


$(BUILD)/parser.tab.c $(BUILD)/parser.tab.h: $(MISC)/parser.y | $(BUILD)
	bison -d $< -o $(BUILD)/parser.tab.c

$(BUILD)/lexer.yy.c: $(MISC)/lexer.l $(BUILD)/parser.tab.h | $(BUILD)
	flex -o $@ $<

$(BUILD)/parser.tab.o: $(BUILD)/parser.tab.c
	$(GPP) $(CXXFLAGS) -c $< -o $@

$(BUILD)/lexer.yy.o: $(BUILD)/lexer.yy.c
	$(GPP) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $@

$(ASM_TARGET): $(ALL_OBJ_FILES)
	$(GPP) $(CXXFLAGS) -Icommon -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all clean asm
