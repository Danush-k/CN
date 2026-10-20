CC = gcc
CFLAGS = -Wall -Wextra

# Locate OpenSSL (Homebrew openssl@3 or standard paths)
OPENSSL_PREFIX = $(shell brew --prefix openssl@3 2>/dev/null || echo /opt/homebrew/opt/openssl)
SSL_CFLAGS = -I$(OPENSSL_PREFIX)/include
SSL_LDFLAGS = -L$(OPENSSL_PREFIX)/lib -lssl -lcrypto

all: http_client https_client

http_client: http_client.c
	$(CC) $(CFLAGS) http_client.c -o http_client

https_client: https_client.c
	$(CC) $(CFLAGS) $(SSL_CFLAGS) $(SSL_LDFLAGS) https_client.c -o https_client

clean:
	rm -f http_client https_client downloaded_*

.PHONY: all clean
