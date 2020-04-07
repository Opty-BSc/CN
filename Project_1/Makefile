TARGETS = chat-client chat-server

CC = gcc

CFLAGS = -Wall -Werror -O3

all: $(TARGETS)

chat-client: chat-client.c chat.c
chat-server: chat-server.c chat.c

$(TARGETS):

	$(CC) $(CFLAGS) -o $@ $^


	
clean:
	rm -f $(TARGETS)

