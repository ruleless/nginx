#include "kcp_tunnel.h"

#include <ngx_socket.h>

static void kcp_tunnel_group_on_recv(ngx_event_t *rev);

int
kcp_tunnel_group_init(kcp_tunnel_group_t *group, ngx_cycle_t *cycle)
{
    ngx_connection_t    *c;
    ngx_event_t         *rev, wev;
    ngx_pmap_conf_t     *pcf;
    ngx_socket_t         s;
    ngx_int_t            event;

    s = ngx_socket(AF_INET, SOCK_DGRAM, 0);

    ngx_log_error(NGX_LOG_INFO, group->log, 0,
                  "create kcp tunnel group! UDP socket %d", s);

    if ((ngx_socket_t)-1 == s) {
        ngx_log_error(NGX_LOG_ERR, group->log, ngx_socket_errno,
                      ngx_socket_n " failed");
        return NGX_ERROR;
    }

    /* get connection */

    c = ngx_get_connection(s, group->log);

    if (NULL == c) {
        if (ngx_close_socket(s) == -1) {
            ngx_log_error(NGX_LOG_ERR, group->log, ngx_socket_errno,
                          ngx_close_socket_n "failed");
        }

        return NGX_ERROR;
    }

    if (ngx_nonblocking(s) == -1) {
        ngx_log_error(NGX_LOG_ERR, &rec->log, ngx_socket_errno,
                      ngx_nonblocking_n " failed");

        goto failed;
    }

    group->udp_conn = c;

    /* bind address for server */
    
    pcf = ngx_pmap_get_conf(cycle->conf_ctx, ngx_pmap_core_module);
    if (NGX_PMAP_ENDPOINT_SERVER == pcf->endpoint) {
        
        if (bind(s, &group->addr.u.sockaddr, group->addr.socklen) < 0) {
            
            ngx_log_error(NGX_LOG_ERR, group->log, ngx_socket_errno,
                          ngx_close_socket_n "failed");
            
            goto failed;
        }
    }       

    /* register read event */

    rev = c->read;
    wev = c->write;

    rev->handler = kcp_tunnel_group_on_recv;
    rev->log = group->log;
            
    wev->ready = 1; /* UDP sockets are always ready to write */
    wev->log = group->log;

    c->number = ngx_atomic_fetch_add(ngx_connection_counter, 1);
    c->data = group;

    event = (ngx_event_flags & NGX_USE_CLEAR_EVENT) ?
            NGX_CLEAR_EVENT: /* kqueue, epoll */
            NGX_LEVEL_EVENT; /* select, poll, /dev/poll */

    if (ngx_add_event(rev, NGX_READ_EVENT, event) != NGX_OK) {
        goto failed;
    }

    /* init rbtree(it's used to store kcp tunnel) */
    
    ngx_rbtree_init(&group->rbtree, &group->sentinel, ngx_rbtree_insert_value);

    return NGX_OK;

failed:
    ngx_close_connection(c);
    group->udp_conn = NULL;

    return NGX_ERROR;
}

kcp_tunnel_t *
kcp_create_tunnel(kcp_tunnel_group_t *group, IUINT32 conv)
{}

void
kcp_destroy_tunnel(kcp_tunnel_group_t *group, kcp_tunnel_t *tunnel)
{}
