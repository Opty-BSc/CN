TARGETS = file-receiver file-sender

CC = gcc

CFLAGS = -Wall -Werror -O3

all: $(TARGETS)

file-receiver: file-receiver.c file-transfer.c
file-sender: file-sender.c file-transfer.c

$(TARGETS):
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

