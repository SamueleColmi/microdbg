# DIRECTORIES
BIN_DIR		= ./bin
EXT_DIR		= ./ext
INC_DIR		= ./include
OBJ_DIR		= ./obj
SRC_DIR		= ./src
EXMPL_DIR	= ./examples

# VARIABLES
TARGET  	= microdbg
SRC		= $(wildcard $(SRC_DIR)/*.cpp)
TMP_OBJ 	= $(SRC:%.cpp=%.o)
OBJ		= $(TMP_OBJ:./src/%=./obj/%)
RM 		= rm -r -f
EXMPL_TARGET 	= hello
EXMPL_SRC	= $(wildcard $(EXMPL_DIR)/*.cpp)

# FLAGS
CFLAGS = -Wall -c -I$(INC_DIR)

# BUILD
.PHONY: all

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET) : $(OBJ) | $(BIN_DIR)
	@$(CXX) $^ -o $@
	@echo " Done!"
	@echo
	@echo ">>> COMPILED WITH: " $(CXX)

$(OBJ) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@/bin/echo -n "Compiling microgdb..."
	@$(CXX) $(CFLAGS) -o $@ $<

$(BIN_DIR):
	@mkdir -p $@
#	@echo "Folder ./bin created..."

$(OBJ_DIR):
	@mkdir -p $@
#	@echo "Folder ./obj created..."

# EXAMPLES
.PHONY = examples

examples: $(BIN_DIR)/$(EXMPL_TARGET)

$(BIN_DIR)/$(EXMPL_TARGET) : $(EXMPL_SRC) | $(BIN_DIR)
	@/bin/echo -n "Compiling examples..."
	@$(CXX) -Wall $^ -o $@
	@echo " Done!"
	@echo
	@echo ">>> COMPILED WITH: " $(CXX)

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
