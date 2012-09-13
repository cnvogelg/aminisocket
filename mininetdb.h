#ifndef MININETDB_H
#define MININETDB_H

#include <netdb.h>

extern struct servent * mini_getservbyname(const char * name, const char * proto);
extern struct servent * mini_getservbyport(int port, const char * proto);
extern struct protoent * mini_getprotobyname(const char * name);
extern struct protoent * mini_getprotobynumber(int proto);

#endif
