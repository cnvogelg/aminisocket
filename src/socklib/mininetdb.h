#ifndef MININETDB_H
#define MININETDB_H

#include <netdb.h>

extern struct hostent * mini_gethostbyname( const char * name );
extern struct hostent * mini_gethostbyaddr( const char * addr, int len, int type );
extern struct netent * mini_getnetbyname( const char * name );
extern struct netent * mini_getnetbyaddr( long net, int type );
extern struct servent * mini_getservbyname(const char * name, const char * proto);
extern struct servent * mini_getservbyport(int port, const char * proto);
extern struct protoent * mini_getprotobyname(const char * name);
extern struct protoent * mini_getprotobynumber(int proto);

#endif
