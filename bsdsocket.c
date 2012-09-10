/*
 * bsdsocket.c - library calls
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "debug.h"

__asm __saveds int lib_socket(register __d0 int domain, register __d1 int type, register __d2 int protocol )
{
    D(bug("socket(%d,%d,%d)\n", domain, type, protocol));
    return 0;
}

__asm __saveds int lib_bind(register __d0 int sock, register __a0 struct sockaddr * name, register __d1 int namelen )
{
    D(bug("bind(%d,%p,%d)\n", sock, name, namelen));
    return 0;
}

__asm __saveds int lib_listen(register __d0 int sock, register __d1 int backlog )
{
    D(bug("listen(%d,%d)\n", sock, backlog));
    return 0;
}

__asm __saveds int lib_accept(register __d0 int sock, register __a0 struct sockaddr * addr, register __a1 int addrlen )
{
    D(bug("accept(%d,%p,%d)\n", sock, addr, addrlen));
    return 0;
}

__asm __saveds int lib_connect(register __d0 int sock, register __a0 struct sockaddr * name, register __d1 int namelen )
{
    D(bug("connect(%d,%p,%d)\n", sock, name, namelen));
    return 0;
}

__asm __saveds int lib_sendto(register __d0 int sock, register __a0 const void * buf, register __d1 int len, register __d2 int flags, register __a1 const struct sockaddr * to, register __d3 int tolen )
{
    D(bug("sendto(%d,%p,%d,%d,%p,%d)\n", sock, buf, len, flags, to, tolen));
    return len;
}

__asm __saveds int lib_send(register __d0 int sock, register __a0 const void * buf, register __d1 int len, register __d2 int flags )
{
    D(bug("send(%d,%p,%d,%d)\n", sock, buf, len, flags));
    return 0;
}

__asm __saveds int lib_recvfrom(register __d0 int sock, register __a0 void * buf, register __d1 int len, register __d2 int flags, register __a1 struct sockaddr * addr, register __a2 int addrlen )
{
    D(bug("recvfrom(%d,%p,%d,%d,%p,%d)\n", sock, buf, len, flags, addr, addrlen));
    return len;
}

__asm __saveds int lib_recv(register __d0 int sock, register __a0 void * buf, register __d1 int len, register __d2 int flags )
{
    D(bug("recv(%d,%p,%d,%d)\n", sock, buf, len, flags));
    return len;
}

__asm __saveds int lib_shutdown(register __d0 int sock, register __d1 int how )
{
    D(bug("shutdown(%d,%d)\n", sock, how));
    return 0;
}

__asm __saveds int lib_setsockopt(register __d0 int sock, register __d1 int level, register __d2 int optname, register __a0 void * optval, register __d3 int optlen )
{
    D(bug("setsockopt(%d,%d,%d,%p,%d)\n", sock, level, optname, optval, optlen));
    return 0;
}

__asm __saveds int lib_getsockopt(register __d0 int sock, register __d1 int level, register __d2 int optname, register __a0 void * optval, register __a1 int optlen )
{
    D(bug("getsockopt(%d,%d,%d,%p,%d)\n", sock, level, optname, optval, optlen));
    return 0;
}

__asm __saveds int lib_getsockname(register __d0 int sock, register __a0 struct sockaddr * name, register __a1 int namelen )
{
    D(bug("getsockname(%d,%p,%d)\n", sock, name, namelen));
    return 0;
}

__asm __saveds int lib_getpeername(register __d0 int sock, register __a0 struct sockaddr * name, register __a1 int namelen )
{
    D(bug("getpeername(%d,%p,%d)\n", sock, name, namelen));
    return 0;
}

__asm __saveds int lib_IoctlSocket(register __d0 int sock, register __d1 unsigned long req, register __a0 char * argp )
{
    D(bug("IoctlSocket(%d,%lu,%p)\n", sock, req, argp));
    return 0;
}

__asm __saveds int lib_CloseSocket(register __d0 int sock )
{
    D(bug("CloseSocket(%d)\n", sock));
    return 0;
}

__asm __saveds int lib_WaitSelect(register __d0 int nfds, register __a0 fd_set * read_fds, register __a1 fd_set * write_fds, register __a2 fd_set * except_fds, register __a3 struct timeval * timeout, register __d1 ULONG * signals )
{
    D(bug("WaitSelect(%d,%p,%p%,p,%p,%p)\n", nfds, read_fds, write_fds, except_fds, timeout, signals));
    return 0;
}

__asm __saveds void lib_SetSocketSignals(register __d0 ULONG int_mask, register __d1 ULONG io_mask, register __d2 ULONG urgent_mask )
{
    D(bug("SetSocketSignals(%lu,%lu,%lu)\n", int_mask, io_mask, urgent_mask));
}

__asm __saveds int lib_getdtablesize( void  )
{
    D(bug("getdtablesize()\n"));
    return 4;
}

__asm __saveds LONG lib_ObtainSocket(register __d0 LONG id, register __d1 LONG domain, register __d2 LONG type, register __d3 LONG protocol )
{
    D(bug("ObtainSocket(%lu,%lu,%lu,%lu)\n", id, domain, type, protocol));
    return 0;
}

__asm __saveds LONG lib_ReleaseSocket(register __d0 LONG sock, register __d1 LONG id )
{
    D(bug("ReleaseSocket(%lu,%lu)\n", sock, id));
    return 0;
}

__asm __saveds LONG lib_ReleaseCopyOfSocket(register __d0 LONG sock, register __d1 LONG id )
{
    D(bug("ReleasecCopyOfSocket(%lu,%lu)\n", sock, id));
    return 0;
}

__asm __saveds long lib_Errno( void  )
{
    D(bug("Errno()\n"));
    return 0;
}

__asm __saveds void lib_SetErrnoPtr(register __a0 void * errno_ptr, register __d0 int size )
{
    D(bug("SetErrnoPtr(%p,%d)\n", errno_ptr, size));
}

__asm __saveds char * lib_Inet_NtoA(register __d0 long ip )
{
    D(bug("Inet_NtoA(%ld)\n", ip));
    return NULL;
}

__asm __saveds unsigned long lib_inet_addr(register __a0 const char * cp )
{
    D(bug("inet_addr(%s)\n", cp));
    return INADDR_NONE;
}

__asm __saveds unsigned long lib_Inet_LnaOf(register __d0 unsigned long in )
{
    D(bug("%Inet_LnaOf(%ld)\n", in));
    return INADDR_NONE;
}

__asm __saveds unsigned long lib_Inet_NetOf(register __d0 unsigned long in )
{
    D(bug("Inet_NetOf(%ld)\n", in));
    return INADDR_NONE;
}

__asm __saveds unsigned long lib_Inet_MakeAddr(register __d0 int net, register __d1 int lna )
{
    D(bug("Inet_MakeAddr(%d,%d)\n", net, lna));
    return INADDR_NONE;
}

__asm __saveds unsigned long lib_inet_network(register __a0 const char * cp )
{
    D(bug("inet_network(%s)\n", cp));
    return INADDR_NONE;
}

__asm __saveds struct hostent * lib_gethostbyname(register __a0 const char * name )
{
    D(bug("gethostbyname(%s)\n", name));
    return NULL;
}

__asm __saveds struct hostent * lib_gethostbyaddr(register __a0 const char * addr, register __d0 int len, register __d1 int type )
{
    D(bug("gethostbyaddr(%p,%d,%d)\n", addr, len, type));
    return NULL;
}

__asm __saveds struct netent * lib_getnetbyname(register __a0 const char * name )
{
    D(bug("getnetbyname(%s)\n", name));
    return NULL;
}

__asm __saveds struct netent * lib_getnetbyaddr(register __d0 long net, register __d1 int type )
{
    D(bug("getnetbyaddr(%ld,%d)\n", net, type));
    return NULL;
}

__asm __saveds struct servent * lib_getservbyname(register __a0 const char * name, register __a1 const char * proto )
{
    D(bug("getservbyname(%s,%s)\n", name, proto));
    return NULL;
}

__asm __saveds struct servent * lib_getservbyport(register __d0 int port, register __a0 const char * proto )
{
    D(bug("getservbyport(%d,%s)\n", port, proto));
    return NULL;
}

__asm __saveds struct protoent * lib_getprotobyname(register __a0 const char * name )
{
    D(bug("getprotobyname(%s)\n", name));
    return NULL;
}

__asm __saveds struct protoent * lib_getprotobynumber(register __d0 int proto )
{
    D(bug("getprotobynumber(%d)", proto));
    return NULL;
}

__asm __saveds void lib_vsyslog(register __d0 int pri, register __a0 const char * msg, register __a1 LONG * args )
{
    D(bug("vsyslog(%d,%s,%p)\n", pri, msg, args));
}

__asm __saveds int lib_Dup2Socket(register __d0 int old_socket, register __d1 int new_socket )
{
    D(bug("Dup2Socket(%d,%d)\n", old_socket, new_socket));
    return 0;
}

__asm __saveds int lib_sendmsg(register __d0 int sock, register __a0 const struct msghdr * msg, register __d1 int flags )
{
    D(bug("sendmsg(%d,%p,%d)\n", sock, msg, flags));
    return 0;
}

__asm __saveds int lib_recvmsg(register __d0 int sock, register __a0 struct msghdr * msg, register __d1 int flags )
{
    D(bug("recvmsg(%d,%p,%d)\n", sock, msg, flags));
    return 0;
}

__asm __saveds int lib_gethostname(register __a0 const char * name, register __d0 int namelen )
{
    D(bug("gethostname(%s,%d)\n", name, namelen));
    return 0;
}

__asm __saveds unsigned long lib_gethostid( void  )
{
    D(bug("gethostid()\n"));
    return 0;
}

__asm __saveds ULONG lib_SocketBaseTagList(register __a0 struct TagItem * tags )
{
    D(bug("SocketBaseTagList(%p)\n", tags));
    return 0;
}

__asm __saveds LONG lib_GetSocketEvents(register __a0 ULONG * event_ptr )
{
    D(bug("GetSocketEvents(%p)\n", event_ptr));
    return 0;
}
