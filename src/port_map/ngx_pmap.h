#ifndef _NGX_PMAP_H_INCLUDED_
#define _NGX_PMAP_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>

#define NGX_PMAP_MODULE				0x50414D50
#define NGX_PMAP_CONF				0x02000000

#define NGX_PMAP_ENDPOINT_CLIENT	1
#define NGX_PMAP_ENDPOINT_SERVER	2


/* the listening structure */
typedef struct {
    union {
        struct sockaddr sockaddr;
        struct sockaddr_in  sockaddr_in;
#if (NGX_HAVE_INET6)
        struct sockaddr_in6 sockaddr_in6;
#endif
#if (NGX_HAVE_UNIX_DOMAIN)
        struct sockaddr_un sockaddr_un;
#endif
        u_char sockaddr_data[NGX_SOCKADDRLEN];
    } u;

    socklen_t socklen;
    int backlog;
} ngx_pmap_listen_t;


/* pmap module */
typedef struct {
	ngx_str_t *name;

	void *(*create_conf)(ngx_cycle_t *cycle);
	void *(*init_conf)(ngx_cycle_t *cycle, void *conf);
} ngx_pmap_module_t;


/* conf structure of ngx_pmap_core_module */
typedef struct {
	ngx_int_t endpoint;
} ngx_pmap_conf_t;


#endif /* _NGX_PMAP_H_INCLUDED_ */
