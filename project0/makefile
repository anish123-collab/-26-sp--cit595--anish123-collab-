CC = gcc
CFLAGS = -Wall -g

all: tcpclient multi-tcpserver

tcpclient: tcpclient.c
	$(CC) $(CFLAGS) -o tcpclient tcpclient.c

multi-tcpserver: multi-tcpserver.c
	$(CC) $(CFLAGS) -o multi-tcpserver multi-tcpserver.c -pthread

clean:
	rm -f tcpclient multi-tcpserver