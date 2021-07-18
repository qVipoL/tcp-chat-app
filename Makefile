CC := gcc
FLAGS := -g -pthread -Wall
BUILD := bin
SOURCE := src
INCLUDE := include

all: build server

build:
	@mkdir -p $(BUILD)

client: $(SOURCE)/client.c

server: $(SOURCE)/server.c $(INCLUDE)/chatroom/chatroom.c $(INCLUDE)/error/error.c $(INCLUDE)/user/user.c
	$(CC) $(FLAGS) $(SOURCE)/server.c $(INCLUDE)/chatroom/chatroom.c $(INCLUDE)/error/error.c $(INCLUDE)/user/user.c -o $(BUILD)/server

clean:
	rm $(BUILD_DIR)/*