/****************************************************
 *      Computer Networks - 1º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#ifndef PROJECT_1_CHAT_H
#define PROJECT_1_CHAT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define TRUE 1
#define FALSE 0
#define ERROR_STATUS -1

#define MAX_PORT_SZ 5
#define MAX_ID_SZ (INET6_ADDRSTRLEN + 1 + MAX_PORT_SZ)
#define MSG_SZ (4096 + 1)
#define BUFF_SZ (MAX_ID_SZ + 1 + MSG_SZ)

/* =============================================================================
 * error
 * =============================================================================
 * This function displays error msg in stderr and finishes the program.
 * @params msg: An error msg.
*/
void error(const char *msg);

/* =============================================================================
 * mkRead
 * =============================================================================
 * This function reads a byte.
 * @params fd: File descriptor of the msg.
 * @params buffer: The buffer to be fill with the byte.
 * @params nBytes: Buffer's index where the byte will be written.
*/
int mkRead(int fd, char *buffer, size_t nBytes);

/* =============================================================================
 * getMsg
 * =============================================================================
 * This function reads a msg.
 * @params fd: File descriptor of the msg.
 * @params buffer: The buffer to be fill with the msg.
*/
int getMsg(int fd, char *buffer);

#endif
