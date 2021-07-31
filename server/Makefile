CC := gcc
FLAGS := -g -pthread -Wall
SRC := src
OBJ := obj
SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

# tcp lib vars
TCP := ../tcp
TCP_SRC := $(TCP)/$(SRC)
TCP_OBJ := $(TCP)/$(OBJ)
TCP_LIB := $(wildcard $(TCP_SRC)/*.c)
TCP_OBJS := $(patsubst $(TCP_SRC)/%.c, $(TCP_OBJ)/%.o, $(TCP_LIB))

BIN_DIR := bin
BIN := $(BIN_DIR)/server

all: dirs $(BIN)

release: FLAGS := -pthread -Wall -O2
release: clean
release: dirs $(BIN)

dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ)
	mkdir -p $(TCP_OBJ)

$(BIN): $(OBJS) $(TCP_OBJS)
	$(CC) $(FLAGS) $(OBJS) $(TCP_OBJS) -o $@

# compile project objs
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(FLAGS) -c $< -o $@

# compile tcp lib ojbs
$(TCP_OBJ)/%.o: $(TCP_SRC)/%.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	$(RM) -r $(BIN_DIR)/* $(OBJ)/* $(TCP_OBJ)/*