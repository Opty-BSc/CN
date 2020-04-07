/****************************************************
 *      Computer Networks - 2º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#include "file-sender.h"
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

/* =============================================================================
 * setAddr
 * =============================================================================
 * This function sets the socket address inet
 * according to the given IP and Port.
 * @params addr: Struct of socket address inet (to be set).
 * @params ip: IP in string type.
 * @params port: Port in string type.
*/
void setAddr(struct sockaddr_in *addr, char *ip, char *port) {

    /* Filling the server address's structure. */
    bzero(addr, sizeof(struct sockaddr_in));

    struct hostent *host;
    /* Getting server's address. */
    if ((host = gethostbyname(ip)) == NULL)
        error("gethostbyname");

    addr->sin_family = AF_INET;
    addr->sin_addr = *(struct in_addr *)host->h_addr;

    int portN = atoi(port);
    if (portN < 0 || portN > 65535 ||
            (portN == 0 && port[0] != '0')) {
        error("port");
    }
    addr->sin_port = htons(portN);
}

/* =============================================================================
 * createChunk
 * =============================================================================
 * This function creates a chunk.
 * @params seq_num: Sequence number of the chunk.
 * @params fileFd: File descriptor to extract the chunk data.
 * @params nBytes: Bytes number of data extraction (to be set).
 * @return: New chunk.
*/
data_pkt_t *createChunk(uint32_t seq_num, int fileFd, size_t *nBytes) {

    data_pkt_t *chunk;
    assert((chunk = (data_pkt_t *)malloc(sizeof(data_pkt_t))));

    chunk->seq_num = htonl(seq_num);

    if ((*nBytes = read(fileFd, chunk->data, CHUNK_SZ)) < 0)
        error("read");

    return chunk;
}

/* =============================================================================
 * moveWin
 * =============================================================================
 * Moves and fills the window with new chunks.
 * @params win: Window struct (to be updated).
 * @params newChunks: Number of new chunks to add (override
 *                    when there is no more chunks).
 * @params fileFd: File descriptor to extract new chunks data.
*/
void moveWin(window *win, int32_t *newChunks, int fileFd) {

    uint32_t index;

    for (index = 0; index < win->sz - *newChunks; index++) {
        win->chunks[index] = win->chunks[index + *newChunks];
    }

    if (win->lastBytes < CHUNK_SZ) {
        win->sz -= *newChunks;
        *newChunks = 0;
        return;
    }

    for (; index < win->sz; index++) {
        win->chunks[index] = createChunk(win->seq_base + index, fileFd, &win->lastBytes);

        if (win->lastBytes < CHUNK_SZ) {
            *newChunks -= win->sz - (index + 1);
            win->sz = index + 1;
            return;
        }
    }
}

/* =============================================================================
 * sendChunks
 * =============================================================================
 * Sends some chunks in the window.
 * @params win: Window struct (to get window stuff).
 * @params senderFd: File descriptor of the sender's socket.
 * @params serverAddr: Receiver's address.
*/
void sendChunks(window *win, int32_t newChunks, int senderFd, struct sockaddr_in *serverAddr) {

    size_t nBytes = CHUNK_SZ;
    for (uint32_t i = win->sz - newChunks; i < win->sz; i++) {

        if (win->chunks[i] == NULL) continue;

        if (win->sz == i + 1) nBytes = win->lastBytes;

        if (sendto(senderFd, win->chunks[i], sizeof(uint32_t) + nBytes, 0,
                (struct sockaddr *)serverAddr, sizeof(struct sockaddr_in)) < 0)
            error("sendto");
    }
}

/* =============================================================================
 * getAck
 * =============================================================================
 * Waits for receiver's acknowledge.
 * @params ack: ACK Packet (to be set).
 * @params win: Window struct (to get window stuff).
 * @params senderFd: File descriptor of the sender's socket.
 * @params serverAddr: Receiver's address.
*/
void getAck(ack_pkt_t *ack, window *win, int senderFd, struct sockaddr_in *serverAddr) {

    int nFails = 0;
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(struct sockaddr_in);

    do {
        while (recvfrom(senderFd, ack, sizeof(ack_pkt_t), 0,
                (struct sockaddr *)&addr, &addrLen) < 0) {

            if (++nFails == MAX_FAILS)
                error("recvfrom");

            if (errno == EAGAIN || errno == EWOULDBLOCK)
                sendChunks(win, win->sz, senderFd, serverAddr);
        }
    } while (serverAddr->sin_port != addr.sin_port ||
      serverAddr->sin_addr.s_addr != addr.sin_addr.s_addr);
}

/* =============================================================================
 * freeChunks
 * =============================================================================
 * Free's all received chunks according selective acknowledges.
 * @params win: Window struct (to be updated).
 * @params sel_acks: Selective acknowledges.
 * @params newChunks: Movement padding of the window.
*/
void freeChunks(window *win, uint32_t sel_acks, int32_t newChunks) {

    const uint32_t mask_1 = 1;

    for (int32_t index = 0; index < newChunks; index++) {

        if (win->chunks[index] != NULL) {
            free(win->chunks[index]);
            win->chunks[index] = NULL;
        }
    }

    for (int32_t index = newChunks + 1; sel_acks && index < win->sz; index++, sel_acks >>= mask_1) {

        if (sel_acks & mask_1 && win->chunks[index] != NULL) {
            free(win->chunks[index]);
            win->chunks[index] = NULL;
        }
    }
}


int main(int argc, char *argv[]) {

    if (argc != 5)
        error("argc");

    window win;
    bzero(&win, sizeof(window));
    win.sz = atoi(argv[4]);

    if (win.sz < MIN_WINDOW_SZ || win.sz > MAX_WINDOW_SZ)
        error("WINDOW_SZ");

    struct sockaddr_in serverAddr;
    setAddr(&serverAddr, argv[2], argv[3]);

    int senderFd, fileFd;
    /* Creating server's socket. */
    if ((senderFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        error("socket");

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    /* Setting socket options. */
    if (setsockopt(senderFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
            sizeof(struct timeval)) < 0)
        error("setsockopt");

    /* Opens the file to read the chunks data. */
    if ((fileFd = open(argv[1], O_RDONLY)) < 0)
        error("open");

    /* Setting window struct. */
    size_t chunksSZ = sizeof(data_pkt_t *) * win.sz;
    assert((win.chunks = (data_pkt_t **)malloc(chunksSZ)));
    memset(win.chunks, 0, chunksSZ);
    win.seq_base = 1;
    win.lastBytes = CHUNK_SZ;

    ack_pkt_t ack;
    int32_t newChunks = win.sz;

    moveWin(&win, &newChunks, fileFd);

    /* While there is chunks to be sent (...) */
    while (win.sz) {

        sendChunks(&win, newChunks, senderFd, &serverAddr);

        do {
            do {
                getAck(&ack, &win, senderFd, &serverAddr);
                newChunks = ntohl(ack.seq_num) - win.seq_base;
            } while (newChunks < 0);

            freeChunks(&win, ntohl(ack.selective_acks), newChunks);

            /* While the window is not moved (...) */
        } while (!newChunks);

        win.seq_base += newChunks;
        moveWin(&win, &newChunks, fileFd);
    }

    free(win.chunks);
    close(fileFd);
    close(senderFd);
    exit(EXIT_SUCCESS);
}
