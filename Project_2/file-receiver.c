/****************************************************
 *      Computer Networks - 2º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#include "file-receiver.h"
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

/* =============================================================================
 * setAddr
 * =============================================================================
 * This function sets the serverAddr and clears cliAddr.
 * @params serverAddr: Struct of socket address inet (to be set).
 * @params cliAddr: Struct of socket address inet.
 * @params port: Port in string type.
*/
void setAddr(struct sockaddr_in *serverAddr, struct sockaddr_in *cliAddr, char *port) {

    bzero(serverAddr, sizeof(struct sockaddr_in));
    bzero(cliAddr, sizeof(struct sockaddr_in));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr.s_addr = INADDR_ANY;

    int portN = atoi(port);
    if (portN < 0 || portN > 65535 ||
        (portN == 0 && port[0] != '0')) {
        error("port");
    }
    serverAddr->sin_port = htons(portN);
}

/* =============================================================================
 * recvPkt
 * =============================================================================
 * This function receives a packet from cliAddr.
 * @params recvFd: File descriptor of the receiver's socket.
 * @params win: Window struct (to get window stuff).
 * @params cliAddr: Struct of client's socket address inet.
 * @return: Window movement flag.
*/
char recvPkt(int recvFd, window *win, struct sockaddr_in *cliAddr) {

    static char first = TRUE;

    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    size_t nBytes;
    int32_t index;
    data_pkt_t *data_pkt = (data_pkt_t *)malloc(sizeof(data_pkt_t));

    do {
        if ((nBytes = recvfrom(recvFd, (data_pkt_t *)data_pkt, sizeof(data_pkt_t),
                0, (struct sockaddr *)&addr, &addrLen)) < 0)
            error("recvfrom");

        if (first) {
            first = FALSE;
            /* Sets cliAddr */
            *cliAddr = addr;
        }

        index = ntohl(data_pkt->seq_num) - win->seq_base;
        if (index < 0 || index >= win->sz || win->data[index] != NULL) {
            free(data_pkt);
            return !MOVE_WIN;
        }

    } while (cliAddr->sin_port != addr.sin_port ||
      cliAddr->sin_addr.s_addr != addr.sin_addr.s_addr);

    if (nBytes - sizeof(uint32_t) < CHUNK_SZ) {
        win->lastBytes = nBytes - sizeof(uint32_t);
        win->sz = index + 1;
    }

    win->data[index] = data_pkt;
    if (!index) return MOVE_WIN;

    win->selective_acks |= 1 << (index - 1);
    return !MOVE_WIN;
}

/* =============================================================================
 * sendAck
 * =============================================================================
 * Sends an acknowledge of the received packets.
 * @params recvFd: File descriptor of the receiver's socket.
 * @params cliAddr: Struct of client's socket address inet.
 * @params win: Window struct (to get window stuff).
*/
void sendAck(int recvFd, struct sockaddr_in *cliAddr, window *win) {

    ack_pkt_t ack;
    ack.seq_num = htonl(win->seq_base);
    ack.selective_acks = htonl(win->selective_acks);

    if (sendto(recvFd, &ack, sizeof(ack_pkt_t), 0,
            (struct sockaddr *)cliAddr, sizeof(struct sockaddr_in)) < 0)
        error("sendto");
}

/* =============================================================================
 * moveWin
 * =============================================================================
 * Moves and cleans the window.
 * @params fileFd: File descriptor to write received chunks data.
 * @params win: Window struct (to be updated).
*/
void moveWin(int fileFd, window *win) {

    const uint32_t mask_1 = 1;
    size_t nBytes = CHUNK_SZ;
    uint32_t index = 0;

    char isNotBase = TRUE;
    while (isNotBase) {

        if (win->lastBytes < CHUNK_SZ && !(--win->sz))
            nBytes = win->lastBytes;

        if (write(fileFd, win->data[index]->data, nBytes) < 0)
            error("write");

        free(win->data[index]);
        index++;

        isNotBase = win->selective_acks & mask_1;
        win->selective_acks >>= mask_1;
    }

    win->seq_base += index;

    for (int i = 0; i < win->sz; i++) {
        if (i + index < win->sz) win->data[i] = win->data[i + index];
        else win->data[i] = NULL;
    }
}


int main(int argc, char *argv[]) {

    if (argc != 4)
        error("argc");

    window win;
    win.sz = atoi(argv[3]);

    if (win.sz < MIN_WINDOW_SZ || win.sz > MAX_WINDOW_SZ)
        error("WINDOW_SZ");

    struct sockaddr_in serverAddr, clientAddr;
    setAddr(&serverAddr, &clientAddr, argv[2]);

    int fileFd;
    /* Opens the file to write the chunks data. */
    if ((fileFd = open(argv[1], O_WRONLY | O_CREAT, S_IRWXU)) < 0)
        error("open");

    int receiverFd;
    /* Creating server's socket. */
    if ((receiverFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        error("socket");

    /* Setting socket options. */
    if (setsockopt(receiverFd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        error("setsockopt");

    if (bind(receiverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        error("bind");

    size_t dataSZ = sizeof(data_pkt_t *) * win.sz;
    assert((win.data = (data_pkt_t **)malloc(dataSZ)));
    memset(win.data, 0, dataSZ);
    win.seq_base = 1;
    win.selective_acks = 0;
    win.lastBytes = CHUNK_SZ;

    char winAct;

    /* While there is chunks to be received (...) */
    while (win.sz) {

        winAct = recvPkt(receiverFd, &win, &clientAddr);

        if (winAct == MOVE_WIN) moveWin(fileFd, &win);

        sendAck(receiverFd, &clientAddr, &win);
    }

    free(win.data);
    close(receiverFd);
    close(fileFd);
    exit(EXIT_SUCCESS);
}
