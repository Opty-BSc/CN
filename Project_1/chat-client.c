/****************************************************
 *      Computer Networks - 1º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#include "chat-client.h"

/* =============================================================================
 * doSelect
 * =============================================================================
 * This function waits for flow in server socket or stdin.
 * @params serverFd: Server socket's file descriptors.
 * @params mask: Mask which is set with the file descriptors.
*/
void doSelect(int serverFd, fd_set *mask) {

    FD_ZERO(mask);
    FD_SET(STDIN_FD, mask);
    FD_SET(serverFd, mask);

    if (select(serverFd + 1, mask, NULL, NULL, NULL) < 0) {
        error("select");
    }
}


int main(int argc, const char *argv[]) {

    struct sockaddr_in serverAddr;
    struct hostent *host;
    int serverFd;

    if (argc != 3) {
        error("argc");
    }

    /* Filling the server address's structure. */
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    /* Getting server's address. */
    if ((host = gethostbyname(argv[1])) == NULL) {
        error("gethostbyname");
    }
    serverAddr.sin_addr = *(struct in_addr *)host->h_addr;
    serverAddr.sin_port = htons(atoi(argv[2]));

    /* Creating server's socket. */
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("socket");
    }

    /* Connects the server's socket to the server's address */
    if (connect(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        error("connect");
    }

    fd_set mask;
    size_t nBytes;
    size_t nBytesIn = 0;
    char buffer[BUFF_SZ];
    char bufferIn[BUFF_SZ];

    while (TRUE) {

        doSelect(serverFd, &mask);

        if (FD_ISSET(STDIN_FD, &mask)) {
            if (!mkRead(STDIN_FD, bufferIn, nBytesIn)) {
                /* EOF (C^D). */
                break;
            }
            if (bufferIn[nBytesIn++] == '\n') {
                /* All msg read. */
                if (write(serverFd, bufferIn, nBytesIn) != nBytesIn) {
                    error("write");
                }
                nBytesIn = 0;
            }
            if (nBytesIn > MSG_SZ) error("mkRead");
        }

        if (FD_ISSET(serverFd, &mask)) {
            if (!(nBytes = getMsg(serverFd, buffer))) {
                /* Server is Offline. */
                break;
            }

            if (write(STDOUT_FD, buffer, nBytes) != nBytes) {
                error("write");
            }
        }
    }

    close(serverFd);
    exit(EXIT_SUCCESS);
}
