all:
	make -C client
	make -C server

clean:
	make clean -C client
	make clean -C server

run:
	gnome-terminal -- "./server/bin/main" -t "Server"
	gnome-terminal -- "./client/bin/main" -t "Client 1"
