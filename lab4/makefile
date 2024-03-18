CC=gcc
CFLAGS=-I. -g -O0 -lpthread

all: server deliver

server.o: server.c packet.h
	$(CC) -c server.c $(CFLAGS)

deliver.o: deliver.c packet.h
	$(CC) -c deliver.c $(CFLAGS)

packet.o: packet.c packet.h
	$(CC) -c packet.c $(CFLAGS)

server: server.o packet.o
	$(CC) -o server server.o packet.o  $(CFLAGS)

deliver: deliver.o packet.o
	$(CC) -o deliver deliver.o packet.o $(CFLAGS)

clean:
	rm -f *.o server deliver
