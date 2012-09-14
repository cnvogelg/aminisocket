#include <sys/types.h>
#include <string.h>
#include "mininetdb.h"

const static char *null = NULL;

/* host adresses */
const static char *localhost_addr[] = {
    "\x7f\x00\x00\x01",
    NULL
};

/* static table of hosts */
const static struct hostent hosts[] = {
    { "localhost", &null, AF_INET, 4, localhost_addr },
    { NULL, &null, AF_INET, 4, &null }
};

/* static table of networks */
const static struct netent networks[] = {
    { "loopback", &null, AF_INET, 127 },
    { NULL, &null, AF_INET, 0 }
};

/* static table of services */
const static struct servent services[] = {
    { "ftp", &null, 21, "tcp" },
    { NULL, &null, 0, NULL }
};

/* static table of protocols */
const static struct protoent protocols[] = {
    { "tcp", &null, 6 },
    { "udp", &null, 17 },
    { NULL, &null, 0 }
};

struct hostent * mini_gethostbyname( const char * name )
{
    const struct hostent *h = hosts;
    while(h->h_name != NULL) {
        if(strcmp(h->h_name, name)==0) {
            return h;
        }
        h++;
    }
    return NULL;
}

struct hostent * mini_gethostbyaddr( const char * addr, int len, int type )
{
    const struct hostent *h = hosts;
    if((len != 4)||(type != AF_INET)) {
        return NULL;
    }
    while(h->h_name != NULL) {
        const char *haddr = h->h_addr_list[0];
        if((haddr[0] == addr[0]) && (haddr[1] == addr[1]) && 
           (haddr[2] == addr[2]) && (haddr[3] == addr[3])) {           
            return h;
        }
        h++;
    }
    
    return NULL;
}

struct netent * mini_getnetbyname( const char * name )
{
    const struct netent *n = networks;
    while(n->n_name != NULL) {
        if(strcmp(n->n_name, name)==0) {
            return n;
        }
        n++;
    }
    return NULL;
}

struct netent * mini_getnetbyaddr( long net, int type )
{
    const struct netent *n = networks;
    if(type != AF_INET) {
        return NULL;
    }
    while(n->n_name != NULL) {
        if(n->n_net == type) {
            return n;
        }
        n++;
    }
    return NULL;
}

struct servent * mini_getservbyname(const char * name, const char * proto)
{
    const struct servent *s = services;
    while(s->s_name != NULL) {
        if((strcmp(s->s_name, name) == 0) && (strcmp(s->s_proto, proto) == 0)) {
            return s;
        }
        s++;
    }
    return NULL;
}

struct servent * mini_getservbyport(int port, const char * proto)
{
    const struct servent *s = services;
    while(s->s_name != NULL) {
        if((s->s_port == port) && (strcmp(s->s_proto, proto) == 0)) {
            return s;
        }
        s++;
    }
    return NULL;
}

struct protoent * mini_getprotobyname(const char * name)
{
    const struct protoent *p = protocols;
    while(p->p_name != NULL) {
        if(strcmp(p->p_name, name) == 0) {
            return p;
        }
        p++;
    }
    return NULL;
}

struct protoent * mini_getprotobynumber(int proto)
{
    const struct protoent *p = protocols;
    while(p->p_name != NULL) {
        if(p->p_proto == proto) {
            return p;
        }
        p++;
    }
    return NULL;
}
