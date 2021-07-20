CC := gcc
FLAGS := -g -pthread -Wall
BUILD := bin
SOURCE := src

all: build main 

build:
	@mkdir -p $(BUILD)

main: $(SOURCE)/main.c $(SOURCE)/tcp.c $(SOURCE)/user.c $(SOURCE)/chatroom.c
	$(CC) $(FLAGS) $^ -o $(BUILD)/main

clean:
	rm $(BUILD)/*