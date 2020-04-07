/****************************************************
 *      Computer Networks - 1º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#include "chat-server.h"

/* =============================================================================
 * doSelect
 * =============================================================================
 * This function waits for flow in the server socket or clients sockets.
 * @params chatR: Contains the sockets file descriptors.
 * @params mask: Mask which is set with the sockets file descriptors.
*/
void doSelect(chatRoom *chatR, fd_set *mask) {

    FD_ZERO(mask);
    FD_SET(chatR->serverFd, mask);

    for (int i = 0; i < chatR->cliNum; i++) {
        FD_SET(chatR->clients[i].fd, mask);
    }

    if (select(chatR->maxFd + 1, mask, NULL, NULL, NULL) < 0) {
        error("select");
    }
}

/* =============================================================================
 * newClient
 * =============================================================================
 * This functions creates a new client at the clients vector first's position
 * that is not taken. Sets id = "%s:%d", client's IP, client's PORT.
 * Updates chatRoom max file descriptor.
 * @params chatR: Contains the clients vector and the server socket
 *                which the new client is connecting for.
*/
void newClient(chatRoom *chatR) {

    client *clientPtr = chatR->clients + chatR->cliNum++;
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    if ((clientPtr->fd = accept(chatR->serverFd, (struct sockaddr *)&clientAddr, &clientLen)) < 0) {
        error("accept");
    }

    char buffer[INET6_ADDRSTRLEN + 1];

   setIP(&clientAddr, buffer);

    if (sprintf(clientPtr->id, "%s:%d", buffer, ntohs(clientAddr.sin_port)) < 0) {
       error("sprintf");
    }

   if (clientPtr->fd > chatR->maxFd) {
       chatR->maxFd = clientPtr->fd;
   }
}

/* =============================================================================
 * notify
 * =============================================================================
 * This functions notifies all clients at the client's vector,
 * including or not the client who sent the msg, according the notifyAll flag.
 * notification = "%s %s", client's id, client's msg.
 * @params chatR: Contains the clients.
 * @params index: Index of the client who sent the msg in the clients vector.
 * @params msg: The msg that the client sent.
 * @params notifyAll: Flag that indicates if the notify will be sent for
 *                    the client himself who sent the msg.
*/
void notify(chatRoom *chatR, int index, char *msg, int notifyAll) {

    char buffer[BUFF_SZ + 1];
    size_t nBytes;

    if ((nBytes = sprintf(buffer, "%s %s", chatR->clients[index].id, msg)) < 0) {
        error("sprintf");
    }

    for (int i = 0; i < chatR->cliNum; i++) {
        if (!notifyAll && index == i) continue;
        if (write(chatR->clients[i].fd, buffer, nBytes) != nBytes) {
            error("write");
        }
    }
}

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
void delClient(chatRoom *chatR, int index) {

    close(chatR->clients[index].fd);
    chatR->clients[index] = chatR->clients[--chatR->cliNum];

    chatR->maxFd = chatR->serverFd;
    for (int i = 0; i < chatR->cliNum; ++i) {

        if (chatR->clients[i].fd > chatR->maxFd) {
            chatR->maxFd = chatR->clients[i].fd;
        }
    }
}

/* =============================================================================
 * setIP
 * =============================================================================
 * This functions fills the buffer with the address's IP.
 * @params addr: A socket address.
 * @params buffer: The buffer to be fill with the address's IP.
*/
void setIP(struct sockaddr_in *addr, char *buffer) {

    if (inet_ntop(addr->sin_family, &addr->sin_addr, buffer, INET6_ADDRSTRLEN + 1) == NULL) {
        error("inet_ntop");
    }
}


int main(int argc, const char *argv[]) {

    chatRoom chatR;
    struct sockaddr_in serverAddr;

    if (argc != 2) {
        error("argc");
    }

    /* Filling the server address's structure. */
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(atoi(argv[1]));

    /* Creating server's socket. */
    if ((chatR.serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("socket");
    }

    chatR.cliNum = 0;
    chatR.maxFd = chatR.serverFd;

    if (setsockopt(chatR.serverFd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        error("setsockopt");
    }

    /* Assigns the server's address to the server's socket. */
    if (bind(chatR.serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
        error("bind");
    }

    /* Defines the max length of the pending connections queue. */
    if (listen(chatR.serverFd, MAX_CLIENTS)) {
        error("listen");
    }

    fd_set mask;
    size_t nBytes;
    char buffer[BUFF_SZ + 1];

    while (TRUE) {

        doSelect(&chatR, &mask);

        if (FD_ISSET(chatR.serverFd, &mask)) {
            /* Connect request by a new client. */
            newClient(&chatR);
            notify(&chatR, chatR.cliNum - 1, JOIN_MSG, TRUE);
        }

        for (int i = 0; i < chatR.cliNum; i++) {
            client *clientPtr = &chatR.clients[i];
            if (FD_ISSET(clientPtr->fd, &mask)) {
                /* Client sent a notification. */
                if (!(nBytes = getMsg(clientPtr->fd, buffer))) {
                    /* Client is Offline. */
                    notify(&chatR, i, LEFT_MSG, FALSE);
                    delClient(&chatR, i);
                    continue;
                }
                if (nBytes > MSG_SZ) error("getMsg");
                notify(&chatR, i, buffer, FALSE);
            }
        }
    }
}
