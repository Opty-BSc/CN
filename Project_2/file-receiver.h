/****************************************************
 *      Computer Networks - 2º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#ifndef PROJECT_2_FILE_RECEIVER_H
#define PROJECT_2_FILE_RECEIVER_H
#include "file-transfer.h"
#include <stdlib.h>
#include <netinet/in.h>

#define MOVE_WIN 1

typedef struct {
    uint32_t sz;
    uint32_t seq_base;
    uint32_t selective_acks;
    size_t lastBytes;
    data_pkt_t **data;
} window;

/* =============================================================================
 * setAddr
 * =============================================================================
 * This function sets the serverAddr and clears cliAddr.
 * @params serverAddr: Struct of socket address inet (to be set).
 * @params cliAddr: Struct of socket address inet.
 * @params port: Port in string type.
*/
void setAddr(struct sockaddr_in *serverAddr, struct sockaddr_in *cliAddr, char *port);

/* =============================================================================
 * recvPkt
 * =============================================================================
 * This function receives a packet from cliAddr.
 * @params recvFd: File descriptor of the receiver's socket.
 * @params win: Window struct (to get window stuff).
 * @params cliAddr: Struct of client's socket address inet.
 * @return: Window movement flag.
*/
char recvPkt(int recvFd, window *win, struct sockaddr_in *cliAddr);

/* =============================================================================
 * sendAck
 * =============================================================================
 * Sends an acknowledge of the received packets.
 * @params recvFd: File descriptor of the receiver's socket.
 * @params cliAddr: Struct of client's socket address inet.
 * @params win: Window struct (to get window stuff).
*/
void sendAck(int receiverFd, struct sockaddr_in *cliAddr, window *win);

/* =============================================================================
 * moveWin
 * =============================================================================
 * Moves and cleans the window.
 * @params fileFd: File descriptor to write received chunks data.
 * @params win: Window struct (to be updated).
*/
void moveWin(int fileFd, window *win);

#endif
