#include <sys/types.h>
#include <string.h>
#include "mininetdb.h"

const static char *null = NULL;

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
