#ifndef _FAST_TUNNEL_H_INCLUDED_
#define _FAST_TUNNEL_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>

#include "kcp_tunnel.h"


typedef struct fast_tunnel_s    fast_tunnel_t;


struct fast_tunnel_s {
    void                *data;

    ngx_pool_t          *pool;
    ngx_log_t           *log;

    kcp_tunnel_group_t  *kcp_group; /* used to create kcp tunnel */
    
    kcp_tunnel_t        *kcp_tun;
    ngx_connection_t    *ctrl_con;

    /* handler */

    void               (*connect_handler)(fast_tunnel_t *ft);
    void               (*disconnect_handler)(fast_tunnel_t *ft);
    void               (*err_handler)(fast_tunnel_t *ft, ngx_int_t errcode);
    void               (*recv_handler)(fast_tunnel_t *ft, const void *data, size_t size);
};


ngx_int_t fast_tunnel_connect(fast_tunnel_t *ft, const struct sockaddr *addr, socklen_t addrlen);
ngx_int_t fast_tunnel_accept(fast_tunnel_t *ft, ngx_connection_t *c);

ssize_t fast_tunnel_send(fast_tunnel_t *ft, const void *data, size_t size);

#endif /* _FAST_TUNNEL_H_INCLUDED_ */
