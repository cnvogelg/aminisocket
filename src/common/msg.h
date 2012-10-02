/* msg.h - define message format passed between lib and server */

#ifndef MSG_H
#define MSG_H

#include <exec/ports.h>

struct msg_s {
    struct Message m_msg;
    UWORD          m_command;
    UWORD          m_result;
    UWORD          m_handle;
    UWORD          m_flags;
    APTR           m_data;
    ULONG          m_size;
};
typedef struct msg_s msg_t;

#define SRV_PORT_NAME           "socksrv"

#define MSG_CMD_HELLO           0
#define MSG_CMD_CREATE_SOCKET   1
#define MSG_CMD_CLOSE_SOCKET    2

#define MSG_RESULT_OK           0
#define MSG_RESULT_NO_SERVER    1

#endif
