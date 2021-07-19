CC := gcc
FLAGS := -g -pthread -Wall
BUILD := bin
SOURCE := src
INCLUDE := include

all: build server client 

build:
	@mkdir -p $(BUILD)

client: $(SOURCE)/client.c $(SOURCE)/error/error.c
	$(CC) $(FLAGS) $(SOURCE)/client.c $(SOURCE)/error/error.c -o $(BUILD)/client

server: $(SOURCE)/server.c $(SOURCE)/chatroom/chatroom.c $(SOURCE)/error/error.c $(SOURCE)/user/user.c
	$(CC) $(FLAGS) $(SOURCE)/server.c $(SOURCE)/chatroom/chatroom.c $(SOURCE)/error/error.c $(SOURCE)/user/user.c -o $(BUILD)/server

clean:
	rm $(BUILD_DIR)/*