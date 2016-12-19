#ifndef _KCP_TUNNEL_H_INCLUDED_
#define _KCP_TUNNEL_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>

#include "alg_cache.h"
#include "ngx_pmap.h"
#include "ikcp.h"


typedef struct kcp_arg_s            kcp_arg_t;
typedef struct kcp_tunnel_s         kcp_tunnel_t;
typedef struct kcp_tunnel_group_s   kcp_tunnel_group_t;


/* kcp arg */
struct kcp_arg_s
{
    int nodelay;    /* 是否启用 nodelay模式，0不启用；1启用 */
    int interval;   /* 协议内部工作的 interval，单位毫秒，比如 10ms或者 20ms */
    int resend;     /* 快速重传模式，默认0关闭，可以设置2(2次ACK跨越将会直接重传) */
    int nc;         /* 是否关闭流控，默认是0代表不关闭，1代表关闭 */
    int mtu;
};


/* kcp tunnel */
struct kcp_tunnel_s {
    ngx_rbtree_node_t       node;
    
    IUINT32                 conv;
    ikcpcb                 *kcpcb;

    kcp_tunnel_group_t     *group;

    alg_cache_t            *cache;

    unsigned                addr_settled:1;
    ngx_pmap_addr_t         addr; /* peer addr */
};


/* kcp tunnel group */
struct kcp_tunnel_group_s {
    ngx_cycle_t            *cycle;
    ngx_pool_t             *pool;
    ngx_log_t              *log;

    ngx_int_t               ep; /* endpoint(server of client) */

    ngx_pmap_addr_t         addr;
    ngx_connection_t       *udp_conn;

    /* rbtree to strore kcp tunnel */
    ngx_rbtree_t            rbtree;
    ngx_rbtree_node_t       sentinel;
};


/* function for kcp tunnel */
ssize_t kcp_send(kcp_tunnel_t *tunnel, const void *data, size_t size);


/* function for kcp tunnel group */

int kcp_group_init(kcp_tunnel_group_t *group);

kcp_tunnel_t *kcp_create_tunnel(kcp_tunnel_group_t *group, IUINT32 conv);
void kcp_destroy_tunnel(kcp_tunnel_group_t *group, kcp_tunnel_t *tunnel);

kcp_tunnel_t *kcp_find_tunnel(kcp_tunnel_group_t *group, IUINT32 conv);

#endif /* _KCP_TUNNEL_H_INCLUDED_ */
