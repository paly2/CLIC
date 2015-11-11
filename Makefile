CC=g++
CFLAGS=-lpthread -std=c++11
SERVER_FILES=server/*.cpp rsa/*.cpp Blowfish/*.cpp
CLIENT_FILES=client/*.cpp rsa/*.cpp Blowfish/*.cpp

all:
	$(CC) $(SERVER_FILES) $(CFLAGS) -o server/server
	$(CC) $(CLIENT_FILES) $(CFLAGS) -o client/client

clean:
	rm -f server/server client/client

mrproper: clean
