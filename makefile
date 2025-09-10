CXXFLAGS = -Wall -g

GPP = g++

MISC = ./misc
TEST = ./test
BUILD = build


DIRECTORY_ASM = ./assembler
DIRECTORY_LINKER = ./linker
DIRECTORY_EMULATOR = ./emulator


ASM_TARGET = asm_exec
LINKER_TARGET = linker_exec
EMULATOR_TARGET = emulator_exec

all: asm linker emulator

asm: $(ASM_TARGET)
linker: $(LINKER_TARGET)
emulator: $(EMULATOR_TARGET)


ASM_SRCS = $(wildcard $(DIRECTORY_ASM)/*.cpp)
ASM_HEADERS = $(wildcard $(DIRECTORY_ASM)/*.hpp)
ASM_OBJS = $(patsubst $(DIRECTORY_ASM)/%.cpp, $(BUILD)/asm/%.o, $(ASM_SRCS))


LINKER_SRCS = $(wildcard $(DIRECTORY_LINKER)/*.cpp)
LINKER_HEADERS = $(wildcard $(DIRECTORY_LINKER)/*.hpp)
LINKER_OBJS = $(patsubst $(DIRECTORY_LINKER)/%.cpp, $(BUILD)/linker/%.o, $(LINKER_SRCS))


EMULATOR_SRCS = $(wildcard $(DIRECTORY_EMULATOR)/*.cpp)
EMULATOR_HEADERS = $(wildcard $(DIRECTORY_EMULATOR)/*.hpp)
EMULATOR_OBJS = $(patsubst $(DIRECTORY_EMULATOR)/%.cpp, $(BUILD)/emulator/%.o, $(EMULATOR_SRCS))


COMMON = ./common
COMMON_SRCS = $(wildcard $(COMMON)/*.cpp)
COMMON_HEADERS = $(wildcard $(COMMON)/*.hpp)
COMMON_OBJS = $(patsubst $(COMMON)/%.cpp, $(BUILD)/%.o, $(COMMON_SRCS))

# common needed by asm
COMMON_ASM_OBJS = $(patsubst $(COMMON)/%.cpp, $(BUILD)/asm/%.o, $(COMMON_SRCS))
# common needed by linker
COMMON_LINKER_SRCS = $(COMMON)/binaryrw.cpp
COMMON_LINKER_OBJS = $(patsubst $(COMMON)/%.cpp, $(BUILD)/linker/%.o, $(COMMON_LINKER_SRCS))
# common needed by emulator
COMMON_EMULATOR_OBJS = $(patsubst $(COMMON)/%.cpp, $(BUILD)/emulator/%.o, $(COMMON_SRCS))


INSTRUCTIONS = ./common/instructions
INSTRUCTION_SRCS = $(wildcard $(INSTRUCTIONS)/*.cpp)
INSTRUCTION_HEADERS = $(wildcard $(INSTRUCTIONS)/*.hpp)

INSTRUCTION_ASM_OBJS = $(patsubst $(INSTRUCTIONS)/%.cpp, $(BUILD)/asm/%.o, $(INSTRUCTION_SRCS))
INSTRUCTION_EMULATOR_OBJS = $(patsubst $(INSTRUCTIONS)/%.cpp, $(BUILD)/emulator/%.o, $(INSTRUCTION_SRCS))


DEVICES = ./emulator/devices
DEVICES_SRCS = $(wildcard $(DEVICES)/*.cpp)
DEVICES_HEADERS = $(wildcard $(DEVICES)/*.hpp)

ASM_DEVICES_OBJS = $(patsubst $(DEVICES)/%.cpp, $(BUILD)/asm/%.o, $(DEVICES_SRCS))
EMULATOR_DEVICES_OBJS = $(patsubst $(DEVICES)/%.cpp, $(BUILD)/emulator/%.o, $(DEVICES_SRCS))


LEXER_OBJ = $(BUILD)/asm/lexer.yy.o
PARSER_OBJ = $(BUILD)/asm/parser.tab.o


ASM_OBJ_FILES = $(LEXER_OBJ) $(PARSER_OBJ) $(INSTRUCTION_ASM_OBJS) $(COMMON_ASM_OBJS) $(ASM_OBJS) $(ASM_DEVICES_OBJS)

LINKER_OBJ_FILES = $(LINKER_OBJS) $(COMMON_LINKER_OBJS)

EMULATOR_OBJ_FILES = $(EMULATOR_OBJS) $(COMMON_EMULATOR_OBJS) $(INSTRUCTION_EMULATOR_OBJS) $(EMULATOR_DEVICES_OBJS)


$(BUILD)/asm/%.o: $(DIRECTORY_ASM)/%.cpp | $(BUILD)/asm
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)
$(BUILD)/linker/%.o: $(DIRECTORY_LINKER)/%.cpp | $(BUILD)/linker
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)
$(BUILD)/emulator/%.o: $(DIRECTORY_EMULATOR)/%.cpp | $(BUILD)/emulator
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)

$(BUILD)/asm/%.o: $(COMMON)/%.cpp | $(BUILD)/asm
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)
$(BUILD)/linker/%.o: $(COMMON)/%.cpp | $(BUILD)/linker
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)
$(BUILD)/emulator/%.o: $(COMMON)/%.cpp | $(BUILD)/emulator
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)

$(BUILD)/asm/%.o: $(INSTRUCTIONS)/%.cpp | $(BUILD)/asm
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)
#$(BUILD)/linker/%.o: $(INSTRUCTIONS)/%.cpp | $(BUILD)/linker
#	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)
$(BUILD)/emulator/%.o: $(INSTRUCTIONS)/%.cpp | $(BUILD)/emulator
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)

$(BUILD)/asm/%.o: $(DEVICES)/%.cpp | $(BUILD)/asm
	$(GPP) -c $< -Icommon -o $@ $(CXXFLAGS)
$(BUILD)/emulator/%.o: $(DEVICES)/%.cpp | $(BUILD)/emulator
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
$(BUILD)/emulator: | $(BUILD)
	mkdir -p $@

$(BUILD):
	mkdir -p $@


$(ASM_TARGET): $(ASM_OBJ_FILES)
	$(GPP) $(CXXFLAGS) -Icommon -o $@ $^
$(LINKER_TARGET): $(LINKER_OBJ_FILES)
	$(GPP) $(CXXFLAGS) -Icommon -o $@ $^	
$(EMULATOR_TARGET) : $(EMULATOR_OBJ_FILES)
	$(GPP) $(CXXFLAGS) -Icommon -o $@ $^


clean:
	rm -rf $(BUILD)
	rm -f *.o
	rm -f *.hex
	rm -f asm_exec
	rm -f linker_exec
	rm -f emulator_exec


.PHONY: all clean asm linker emulator
