/****************************************************
 *      Computer Networks - 2º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#ifndef PROJECT_2_FILE_SENDER_H
#define PROJECT_2_FILE_SENDER_H
#include "file-transfer.h"
#include <netinet/in.h>

#define MAX_FAILS 3

typedef struct {
    size_t sz;
    uint32_t seq_base;
    size_t lastBytes;
    data_pkt_t **chunks;
} window;

/* =============================================================================
 * setAddr
 * =============================================================================
 * This function sets the socket address inet
 * according to the given IP and Port.
 * @params addr: Struct of socket address inet (to be set).
 * @params ip: IP in string type.
 * @params port: Port in string type.
*/
void setAddr(struct sockaddr_in *addr, char *ip, char *port);

/* =============================================================================
 * createChunk
 * =============================================================================
 * This function creates a chunk.
 * @params seq_num: Sequence number of the chunk.
 * @params fileFd: File descriptor to extract the chunk data.
 * @params nBytes: Bytes number of data extraction (to be set).
 * @return: New chunk.
*/
data_pkt_t *createChunk(uint32_t seq_num, int fileFd, size_t *nBytes);

/* =============================================================================
 * moveWin
 * =============================================================================
 * Moves and fills the window with new chunks.
 * @params win: Window struct (to be updated).
 * @params newChunks: Number of new chunks to add (override
 *                    when there is no more chunks).
 * @params fileFd: File descriptor to extract new chunks data.
*/
void moveWin(window *win, int32_t *newChunks, int fileFd);

/* =============================================================================
 * sendChunks
 * =============================================================================
 * Sends some chunks in the window.
 * @params win: Window struct (to get window stuff).
 * @params senderFd: File descriptor of the sender's socket.
 * @params serverAddr: Receiver's address.
*/
void sendChunks(window *win, int32_t newChunks, int senderFd, struct sockaddr_in *serverAddr);

/* =============================================================================
 * getAck
 * =============================================================================
 * Waits for receiver's acknowledge.
 * @params ack: ACK Packet (to be set).
 * @params win: Window struct (to get window stuff).
 * @params senderFd: File descriptor of the sender's socket.
 * @params serverAddr: Receiver's address.
*/
void getAck(ack_pkt_t *ack, window *win, int senderFd, struct sockaddr_in *serverAddr);

/* =============================================================================
 * freeChunks
 * =============================================================================
 * Free's all received chunks according selective acknowledges.
 * @params win: Window struct (to be updated).
 * @params sel_acks: Selective acknowledges.
 * @params newChunks: Movement padding of the window.
*/
void freeChunks(window *win, uint32_t sel_acks, int32_t newChunks);

#endif
