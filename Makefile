
# MAKEFILE

CC := gcc
CFLAGS := -g

YACC := bison
YFLAGS := -t

LEX := flex

SRC_FOLDER := src
BUILD_FOLDER := build
INCLUDE_FOLDER := include
TARGET := bin/server
INCLUDES := -I $(INCLUDE_FOLDER)

SOURCES := $(shell find $(SRC_FOLDER) -type f -name *.c)
LEX_SOURCES := $(shell find $(SRC_FOLDER) -type f -name *.l)
YACC_SOURCES := $(shell find $(SRC_FOLDER) -type f -name *.y)

SOURCES_OBJECTS := $(patsubst $(SRC_FOLDER)/%,$(BUILD_FOLDER)/%,$(SOURCES:.c=.o))
LEX_OBJECTS := $(patsubst $(SRC_FOLDER)/%,$(BUILD_FOLDER)/%,$(LEX_SOURCES:.l=.yy.o))
YACC_OBJECTS := $(patsubst $(SRC_FOLDER)/%,$(BUILD_FOLDER)/%,$(YACC_SOURCES:.y=.tab.o))
OBJECTS := $(SOURCES_OBJECTS) $(YACC_OBJECTS) $(LEX_OBJECTS)

# main targets
all: build
	make clean

build: $(OBJECTS)
	gcc -o $(TARGET) $(OBJECTS)

clean:
	rm -rf $(BUILD_FOLDER)
	rm -f $(SRC_FOLDER)/*.tab.c $(SRC_FOLDER)/*.yy.c $(INCLUDE_FOLDER)/*.tab.h

run:
	./$(TARGET)

# minor targets
$(BUILD_FOLDER)/%.o: $(SRC_FOLDER)/%.c $(INCLUDE_FOLDER)/%.h
	mkdir -p $(BUILD_FOLDER)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(SRC_FOLDER)/parserLex.yy.c: $(SRC_FOLDER)/parserLex.l
	mkdir -p $(BUILD_FOLDER)
	$(LEX) -o $@ $<

$(SRC_FOLDER)/parserYacc.tab.c: $(SRC_FOLDER)/parserYacc.y
	mkdir -p $(BUILD_FOLDER)
	$(YACC) $(YFLAGS) --defines=$(INCLUDE_FOLDER)/$(patsubst $(SRC_FOLDER)/%,include/%,$(<F:.y=.tab.h)) -o $@ $<

# prerequisits
$(BUILD_FOLDER)/main.o: $(INCLUDE_FOLDER)/logger.h $(INCLUDE_FOLDER)/parser.h
$(BUILD_FOLDER)/main.o: $(INCLUDE_FOLDER)/process.h $(INCLUDE_FOLDER)/request.h
$(BUILD_FOLDER)/main.o: $(INCLUDE_FOLDER)/response.h
$(BUILD_FOLDER)/logger.o: $(INCLUDE_FOLDER)/utils.h
$(BUILD_FOLDER)/parser.o: $(INCLUDE_FOLDER)/parserYacc.tab.h
$(BUILD_FOLDER)/process.o: $(INCLUDE_FOLDER)/utils.h

$(BUILD_FOLDER)/parserLex.yy.o: $(INCLUDE_FOLDER)/parserYacc.tab.h $(INCLUDE_FOLDER)/request.h
$(BUILD_FOLDER)/parserYacc.tab.o: $(INCLUDE_FOLDER)/request.h

$(INCLUDE_FOLDER)/parserYacc.tab.h: $(SRC_FOLDER)/parserYacc.tab.c
$(INCLUDE_FOLDER)/parserLex.yy.h:

$(INCLUDE_FOLDER)/main.h:
$(INCLUDE_FOLDER)/logger.h: $(INCLUDE_FOLDER)/request.h $(INCLUDE_FOLDER)/response.h
$(INCLUDE_FOLDER)/parser.h: $(INCLUDE_FOLDER)/request.h
$(INCLUDE_FOLDER)/process.h: $(INCLUDE_FOLDER)/request.h $(INCLUDE_FOLDER)/response.h
$(INCLUDE_FOLDER)/request.h: $(INCLUDE_FOLDER)/header.h
$(INCLUDE_FOLDER)/response.h: $(INCLUDE_FOLDER)/header.h

.PHONY: all build clean run
