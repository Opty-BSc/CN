/****************************************************
 *      Computer Networks - 2º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#ifndef PROJECT_2_FILE_TRANSFER_H
#define PROJECT_2_FILE_TRANSFER_H
#include <stdint.h>

#define TRUE 1
#define FALSE 0
#define ERROR_STATUS -1
#define CHUNK_SZ 1000
#define MIN_WINDOW_SZ 1
#define MAX_WINDOW_SZ 32

typedef struct __attribute__(( __packed__ )) {
    uint32_t seq_num;
    char data[1000];
} data_pkt_t;

typedef struct __attribute__(( __packed__ )) {
    uint32_t seq_num;
    uint32_t selective_acks;
} ack_pkt_t;

/* =============================================================================
 * error
 * =============================================================================
 * This function displays error msg in stderr and finishes the program.
 * @params function: An error msg.
*/
void error(char *msg);

#endif
