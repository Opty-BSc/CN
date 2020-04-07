/****************************************************
 *      Computer Networks - 1º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#ifndef PROJECT_1_CHAT_CLIENT_H
#define PROJECT_1_CHAT_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include "chat.h"

#define STDIN_FD 0
#define STDOUT_FD 1

/* =============================================================================
 * doSelect
 * =============================================================================
 * This function waits for flow in server socket or stdin.
 * @params serverFd: Server socket's file descriptors.
 * @params mask: Mask which is set with the file descriptors.
*/
void doSelect(int serverFd, fd_set *mask);

#endif
