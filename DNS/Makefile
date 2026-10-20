CC = gcc
CFLAGS = -Wall -Wextra

all: dns_server dns_client

dns_server: dns_server.c
	$(CC) $(CFLAGS) dns_server.c -o dns_server

dns_client: dns_client.c
	$(CC) $(CFLAGS) dns_client.c -o dns_client

clean:
	rm -f dns_server dns_client

.PHONY: all clean
