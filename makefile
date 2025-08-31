CXXFLAGS = -Wall -g

GPP = g++

MISC = ./misc
TEST = ./test
BUILD = build


DIRECTORY_ASM = ./assembler
DIRECTORY_LINKER = ./linker


ASM_TARGET = asm_exec
LINKER_TARGET = linker_exec


asm: $(ASM_TARGET)
linker: $(LINKER_TARGET)


all: asm linker


ASM_SRCS = $(wildcard $(DIRECTORY_ASM)/*.cpp)
ASM_HEADERS = $(wildcard $(DIRECTORY_ASM)/*.hpp)
ASM_OBJS = $(patsubst $(DIRECTORY_ASM)/%.cpp, $(BUILD)/asm/%.o, $(ASM_SRCS))


LINKER_SRCS = $(wildcard $(DIRECTORY_LINKER)/*.cpp)
LINKER_HEADERS = $(wildcard $(DIRECTORY_LINKER)/*.hpp)
LINKER_OBJS = $(patsubst $(DIRECTORY_LINKER)/%.cpp, $(BUILD)/linker/%.o, $(LINKER_SRCS))


COMMON = ./common
COMMON_SRCS = $(wildcard $(COMMON)/*.cpp)
COMMON_HEADERS = $(wildcard $(COMMON)/*.hpp)
COMMON_OBJS = $(patsubst $(COMMON)/%.cpp, $(BUILD)/%.o, $(COMMON_SRCS))

COMMON_ASM_OBJS = $(patsubst $(COMMON)/%.cpp, $(BUILD)/asm/%.o, $(COMMON_SRCS))

COMMON_LINKER_SRCS = $(COMMON)/binaryrw.cpp
COMMON_LINKER_OBJS = $(patsubst $(COMMON)/%.cpp, $(BUILD)/linker/%.o, $(COMMON_LINKER_SRCS))


INSTRUCTIONS = ./common/instructions
INSTRUCTION_SRCS = $(wildcard $(INSTRUCTIONS)/*.cpp)
INSTRUCTION_HEADERS = $(wildcard $(INSTRUCTIONS)/*.hpp)
INSTRUCTION_ASM_OBJS = $(patsubst $(INSTRUCTIONS)/%.cpp, $(BUILD)/asm/%.o, $(INSTRUCTION_SRCS))


LEXER_OBJ = $(BUILD)/asm/lexer.yy.o
PARSER_OBJ = $(BUILD)/asm/parser.tab.o


ASM_OBJ_FILES = $(LEXER_OBJ) $(PARSER_OBJ) $(INSTRUCTION_ASM_OBJS) $(COMMON_ASM_OBJS) $(ASM_OBJS)

LINKER_OBJ_FILES = $(LINKER_OBJS) $(COMMON_LINKER_OBJS)


$(BUILD)/asm/%.o: $(DIRECTORY_ASM)/%.cpp | $(BUILD)/asm
	$(GPP) -c $< -Icommon -o $@

$(BUILD)/linker/%.o: $(DIRECTORY_LINKER)/%.cpp | $(BUILD)/linker
	$(GPP) -c $< -Icommon -o $@

$(BUILD)/asm/%.o: $(COMMON)/%.cpp | $(BUILD)/asm
	$(GPP) -c $< -Icommon -o $@

$(BUILD)/linker/%.o: $(COMMON)/%.cpp | $(BUILD)/linker
	$(GPP) -c $< -Icommon -o $@

$(BUILD)/asm/%.o: $(INSTRUCTIONS)/%.cpp | $(BUILD)/asm
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)

$(BUILD)/linker/%.o: $(INSTRUCTIONS)/%.cpp | $(BUILD)/linker
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)


$(BUILD)/asm/parser.tab.c $(BUILD)/asm/parser.tab.h: $(MISC)/parser.y | $(BUILD)/asm
	bison -d $< -o $(BUILD)/asm/parser.tab.c

$(BUILD)/asm/lexer.yy.c: $(MISC)/lexer.l $(BUILD)/asm/parser.tab.h | $(BUILD)/asm
	flex -o $@ $<

$(BUILD)/asm/parser.tab.o: $(BUILD)/asm/parser.tab.c
	$(GPP) $(CXXFLAGS) -c $< -o $@

$(BUILD)/asm/lexer.yy.o: $(BUILD)/asm/lexer.yy.c
	$(GPP) $(CXXFLAGS) -c $< -o $@

$(BUILD)/asm: | $(BUILD)
	mkdir -p $@

$(BUILD)/linker: | $(BUILD)
	mkdir -p $@

$(BUILD):
	mkdir -p $@


$(ASM_TARGET): $(ASM_OBJ_FILES)
	$(GPP) $(CXXFLAGS) -Icommon -o $@ $^

$(LINKER_TARGET): $(LINKER_OBJ_FILES)
	$(GPP) $(CXXFLAGS) -Icommon -o $@ $^	


clean:
	rm -rf $(BUILD)
	rm -f *.o
	rm -f *.hex
	rm -f asm_exec
	rm -f linker_exec


.PHONY: all clean asm linker
