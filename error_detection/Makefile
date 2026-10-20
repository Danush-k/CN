CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: checksum crc checksum_receiver checksum_sender crc_receiver crc_sender

checksum: checksum.c
	$(CC) $(CFLAGS) checksum.c -o checksum

crc: crc.c
	$(CC) $(CFLAGS) crc.c -o crc

checksum_receiver: checksum_receiver.c
	$(CC) $(CFLAGS) checksum_receiver.c -o checksum_receiver

checksum_sender: checksum_sender.c
	$(CC) $(CFLAGS) checksum_sender.c -o checksum_sender

crc_receiver: crc_receiver.c
	$(CC) $(CFLAGS) crc_receiver.c -o crc_receiver

crc_sender: crc_sender.c
	$(CC) $(CFLAGS) crc_sender.c -o crc_sender

clean:
	rm -f checksum crc checksum_receiver checksum_sender crc_receiver crc_sender

.PHONY: all clean
