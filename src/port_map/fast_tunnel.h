#ifndef _FAST_TUNNEL_H_INCLUDED_
#define _FAST_TUNNEL_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>

#include "kcp_tunnel.h"

typedef struct fast_tunnel_s    fast_tunnel_t;

struct fast_tunnel_s {
    kcp_tunnel_t        *kcp_tun;
    ngx_connection_t    *ctrl_con;
};

ngx_int_t fast_tunnel_connect();
ngx_int_t fast_tunnel_accept();

ssize_t fast_tunnel_send(const void *data, size_t size);

#endif /* _FAST_TUNNEL_H_INCLUDED_ */
