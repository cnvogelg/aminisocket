#ifndef DEVICES_PARBLK_H
#define DEVICES_PARBLK_H

#ifndef   EXEC_IO_H
#include "exec/io.h"
#endif /* EXEC_IO_H */

struct IOExtParBlk {
    struct IOStdReq  IOParBlk;
};

#endif /* DEVICES_PARBLK_H */
