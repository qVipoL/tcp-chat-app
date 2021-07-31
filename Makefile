all:
	make release -C client
	make release -C server

clean:
	make clean -C client
	make clean -C server

run:
	gnome-terminal -- "./server/bin/server" -t "Server"
	gnome-terminal -- "./client/bin/client" -t "Client 1"
