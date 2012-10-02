/* msg.h - define message format passed between lib and server */

#ifndef MSG_H
#define MSG_H

#include <exec/ports.h>

struct msg_s {
    struct Message m_msg;
    UWORD          m_command;
    UWORD          m_result;
    UWORD          m_flags;
    UWORD          m_extra;
    ULONG          m_handle;
    APTR           m_data;
    ULONG          m_size;
    APTR           m_addr_data;
    ULONG          m_addr_size;
};
typedef struct msg_s msg_t;

#define SRV_PORT_NAME           "socksrv"

#define MSG_CMD_CREATE_SOCKET       0
#define MSG_CMD_BIND_SOCKET         1
#define MSG_CMD_LISTEN_SOCKET       2
#define MSG_CMD_ACCEPT_SOCKET       3
#define MSG_CMD_CONNECT_SOCKET      4
#define MSG_CMD_SEND_TO_SOCKET      5
#define MSG_CMD_SEND_SOCKET         6
#define MSG_CMD_RECV_FROM_SOCKET    7
#define MSG_CMD_RECV_SOCKET         8
#define MSG_CMD_SHUTDOWN_SOCKET     9

#define MSG_CMD_CLOSE_SOCKET        10

#define MSG_RESULT_OK           0
#define MSG_RESULT_NO_SERVER    1

#endif
