#ifndef _KCP_TUNNEL_H_INCLUDED_
#define _KCP_TUNNEL_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>

#include "ngx_pmap.h"
#include "ikcp.h"


typedef struct kcp_tunnel_s         kcp_tunnel_t;
typedef struct kcp_tunnel_group_s   kcp_tunnel_group_t;


/* kcp tunnel */
struct kcp_tunnel_s {
    ngx_rbtree_node_t       node;
    IUINT32                 conv;

    kcp_tunnel_group_t     *group;

    unsigned                addr_settled:1;
    ngx_pmap_addr_t         addr;
};


/* kcp tunnel group */
struct kcp_tunnel_group_s {
    ngx_cycle_t            *cycle;
    ngx_pool_t             *pool;
    ngx_log_t              *log;

    ngx_pmap_addr_t         addr;
    ngx_connection_t       *udp_conn;

    /* rbtree to strore kcp tunnel */
    ngx_rbtree_t            rbtree;
    ngx_rbtree_node_t       sentinel;
};


/* function for kcp tunnel */


/* function for kcp tunnel group */

int kcp_tunnel_group_init(kcp_tunnel_group_t *group);

kcp_tunnel_t *kcp_create_tunnel(kcp_tunnel_group_t *group, IUINT32 conv);
void kcp_destroy_tunnel(kcp_tunnel_group_t *group, kcp_tunnel_t *tunnel);

kcp_tunnel_t *kcp_find_tunnel(kcp_tunnel_group_t *group, IUINT32 conv);

#endif /* _KCP_TUNNEL_H_INCLUDED_ */
