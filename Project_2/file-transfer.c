/****************************************************
 *      Computer Networks - 2º Project
 *
 *  Authors:
 *  Sara Machado, 86923
 *  Rafael Figueiredo, 90770
 *  Ricardo Grade, 90774
 */
#include "file-transfer.h"
#include <stdlib.h>
#include <stdio.h>

/* =============================================================================
 * error
 * =============================================================================
 * This function displays error msg in stderr and finishes the program.
 * @params function: An error msg.
*/
void error(char *msg) {
    perror(msg);
    exit(ERROR_STATUS);
}
