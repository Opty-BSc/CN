/****************************************************
 *      Computer Networks - 1º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#include "chat.h"

/* =============================================================================
 * error
 * =============================================================================
 * This function displays error msg in stderr and finishes the program.
 * @params msg: An error msg.
*/
void error(const char *msg) {
    perror(msg);
    exit(ERROR_STATUS);
}

/* =============================================================================
 * mkRead
 * =============================================================================
 * This function reads a byte.
 * @params fd: File descriptor of the msg.
 * @params buffer: The buffer to be fill with the byte.
 * @params nBytes: Buffer's index where the byte will be written.
*/
int mkRead(int fd, char *buffer, size_t nBytes) {

    int retRead = read(fd, buffer + nBytes, 1);
    if (retRead < 0 || nBytes == BUFF_SZ) error("read");
    return retRead;
}

/* =============================================================================
 * getMsg
 * =============================================================================
 * This function reads a msg.
 * @params fd: File descriptor of the msg.
 * @params buffer: The buffer to be fill with the msg.
*/
int getMsg(int fd, char *buffer) {

    size_t nBytes = 0;

    do {
        if (!mkRead(fd, buffer, nBytes)) return 0;

    } while (buffer[nBytes++] != '\n');

    buffer[nBytes] = '\0';
    return nBytes;
}
