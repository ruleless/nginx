#include "kcp_tunnel.h"

#include "ngx_socket.h"

int
kcp_tunnel_group_init(kcp_tunnel_group_t *group)
{
    ngx_connection_t    *c;
    ngx_socket_t         s;

    s = ngx_socket(AF_INET, SOCK_DGRAM, 0);

    ngx_log_error(NGX_LOG_INFO, group->log, 0,
                  "create kcp tunnel group! UDP socket %d", s);

    if ((ngx_socket_t)-1 == s) {
        ngx_log_error(NGX_LOG_ERR, group->log, ngx_socket_errno,
                      ngx_socket_n " failed");
        return NGX_ERROR;
    }

    c = ngx_get_connection(s, group->log);

    if (NULL == c) {
        if (ngx_close_socket(s) == -1) {
            ngx_log_error(NGX_LOG_ERR, group->log, ngx_socket_errno,
                          ngx_close_socket_n "failed");
        }

        return NGX_ERROR;
    }

    if (ngx_nonblocking(s) == -1) {
        ngx_log_error(NGX_LOG_ALERT, &rec->log, ngx_socket_errno,
                      ngx_nonblocking_n " failed");

        goto failed;
    }

    return NGX_OK;

failed:
    ngx_close_connection(c);
    group->udp_conn = NULL;

    return NGX_ERROR;
}
