/****************************************************
 *      Computer Networks - 1º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#ifndef PROJECT_1_CHAT_SERVER_H
#define PROJECT_1_CHAT_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "chat.h"

#define MAX_CLIENTS 1000

#define JOIN_MSG "joined.\n"
#define LEFT_MSG "left.\n"

typedef struct {
    int fd;
    char id[MAX_ID_SZ + 1];
} client;


typedef struct {
    int serverFd;
    int maxFd;
    int cliNum;
    client clients[MAX_CLIENTS];
} chatRoom;

/* =============================================================================
 * doSelect
 * =============================================================================
 * This function waits for flow in the server socket or clients sockets.
 * @params chatR: Contains the sockets file descriptors.
 * @params mask: Mask which is set with the sockets file descriptors.
*/
void doSelect(chatRoom *chatR, fd_set *mask);

/* =============================================================================
 * newClient
 * =============================================================================
 * This functions creates a new client at the clients vector first's position
 * that is not taken. Sets id = "IP:PORT". Updates chatRoom max file descriptor.
 * @params chatR: Contains the clients vector and the server's socket
 *                which the new client is connecting for.
*/
void newClient(chatRoom *chatR);

/* =============================================================================
 * notify
 * =============================================================================
 * This functions notifies all clients at the client's vector,
 * including or not the client who sent the msg, according the notifyAll flag.
 * notification = "%s %s", client's id, client's msg.
 * @params chatR: Contains the clients vector.
 * @params index: Index of the client who sent the msg in the clients vector.
 * @params msg: The msg that the client sent.
 * @params notifyAll: Flag that indicates if the notify will be sent for
 *                    the client himself who sent the msg.
*/
void notify(chatRoom *chatR, int index, char *msg, int notifyAll);

/* =============================================================================
 * delClient
 * =============================================================================
 * This functions deletes a client.
 * Closes the client's file descriptor and removes it of the clients vector.
 * Updates chatRoom max file descriptor.
 * @params chatR: Contains the server, the clients and
 *                the max file descriptor of the chatRoom.
 * @params index: Index of the client which will be deleted.
*/
void delClient(chatRoom *chatR, int index);

/* =============================================================================
 * setIP
 * =============================================================================
 * This functions fills the buffer with the address's IP.
 * @params addr: A socket address.
 * @params buffer: The buffer to be fill with the address's IP.
*/
void setIP(struct sockaddr_in *addr, char *buffer);

#endif
