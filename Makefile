CC := gcc
FLAGS := -g -pthread -Wall
BUILD := bin
SOURCE := src

all: build server client 

build:
	@mkdir -p $(BUILD)

client: $(SOURCE)/client.c $(SOURCE)/tcp/tcp.c
	$(CC) $(FLAGS) $^ -o $(BUILD)/client

server: $(SOURCE)/server.c $(SOURCE)/chatroom/chatroom.c $(SOURCE)/user/user.c $(SOURCE)/tcp/tcp.c
	$(CC) $(FLAGS) $^ -o $(BUILD)/server

run:
	gnome-terminal -- "./bin/server" -t "Server"
	gnome-terminal -- "./bin/client" -t "Client 1"

clean:
	rm $(BUILD_DIR)/*