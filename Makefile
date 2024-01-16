# DIRECTORIES
BIN_DIR		= ./bin
INC_DIR		= ./include
OBJ_DIR		= ./obj
SRC_DIR		= ./src
EXMPL_DIR	= ./examples
LNNS_DIR	= ./ext/linenoise

# VARIABLES
TARGET  	= microdbg
SRC		= $(wildcard $(SRC_DIR)/*.cpp)
TMP_OBJ 	= $(SRC:%.cpp=%.o)
OBJ		= $(TMP_OBJ:./src/%=./obj/%)

LNNS_TARGET 	= linenoise
LNNS_SRC	= $(LNNS_DIR)/$(LNNS_TARGET).c
LNNS_TMP	= $(LNNS_SRC:%.c=%.o)
LNNS_OBJ	= $(LNNS_TMP:$(LNNS_DIR)/%=./obj/%)

EXMPL_TARGET	= hello
EXMPL_SRC	= $(EXMPL_DIR)/hello.cpp

RM 		= rm -r -f

# FLAGS
CXXFLAGS 	= -Wall -Os -g -c -I$(INC_DIR) -I$(LNNS_DIR)
CFLAGS		= -Wall -Os -g -c -I$(LNNS_DIR)

# BUILD
.PHONY: all

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJ) $(LNNS_OBJ) | $(BIN_DIR)
	@/bin/echo -n "Linking $(@F)..."
	@$(CXX) -o $@ $^
	@echo " Done!"

$(OBJ) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@/bin/echo -n "Compiling $(@F)..."
	@$(CXX) -o $@ $< $(CXXFLAGS)
	@echo " Done"

$(LNNS_OBJ): $(LNNS_SRC) | $(OBJ_DIR)
	@/bin/echo -n "Compiling $(@F)..."
	@$(CC) -o $@ $< $(CFLAGS)
	@echo " Done"

$(BIN_DIR):
	@mkdir -p $@

$(OBJ_DIR):
	@mkdir -p $@

# EXAMPLES
.PHONY = examples

examples: $(BIN_DIR)/$(EXMPL_TARGET)

$(BIN_DIR)/$(EXMPL_TARGET) : $(EXMPL_SRC) | $(BIN_DIR)
	@/bin/echo -n "Compiling $(@F)..."
	@$(CXX) -Wall $^ -o $@
	@echo " Done!"

#CLEAN
.PHONY: clean
clean:
	@$(RM) $(OBJ_DIR) $(BIN_DIR)
	@echo "CLEANED!"

# RUN
.PHONY: run
run:
	@$(BIN_DIR)/$(TARGET) $(arg)

#	$(info $$SRC is [${SRC}])
