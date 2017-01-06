#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>

#include "fast_tunnel.h"


static void fast_tunnel_on_recv(ngx_event_t *rev);
static void fast_tunnel_on_connected(ngx_event_t *wev);


ngx_int_t
fast_tunnel_connect(fast_tunnel_t *ft, const struct sockaddr *addr, socklen_t addrlen)
{
    int                rc;
    ngx_int_t          event;
    ngx_err_t          err;
    ngx_uint_t         level;
    ngx_socket_t       s;
    ngx_event_t       *rev, *wev;
    ngx_connection_t  *c;

    /* create socket and get connection */

    s = ngx_socket(AF_INET, SOCK_STREAM, 0);

    ngx_log_error(NGX_LOG_DEBUG, ft->log, 0,
                  "fast_tunnel_connect() TCP socket %d", s);

    if (s == (ngx_socket_t) -1) {
        ngx_log_error(NGX_LOG_ALERT, ft->log, ngx_socket_errno,
                      ngx_socket_n " failed");
        return NGX_ERROR;
    }

    c = ngx_get_connection(s, ft->log);

    if (c == NULL) {
        if (ngx_close_socket(s) == -1) {
            ngx_log_error(NGX_LOG_ALERT, ft->log, ngx_socket_errno,
                          ngx_close_socket_n "failed");
        }

        return NGX_ERROR;
    }

    if (ngx_nonblocking(s) == -1) {
        ngx_log_error(NGX_LOG_ALERT, ft->log, ngx_socket_errno,
                      ngx_nonblocking_n " failed");

        goto failed;
    }

    /* set event handler */

    rev = c->read;
    wev = c->write;

    rev->log = ft->log;
    wev->log = ft->log;

    rev->handler = fast_tunnel_on_recv;
    wev->handler = fast_tunnel_on_connected;
    
    ft->ctrl_con = c;
    ft->pool     = c->pool;

    c->data   = ft;
    c->number = ngx_atomic_fetch_add(ngx_connection_counter, 1);

    if (ngx_add_conn) {
        if (ngx_add_conn(c) == NGX_ERROR) {
            goto failed;
        }
    }

    /* connect */

    rc = connect(s, addr, addrlen);

    if (rc == -1) {
        err = ngx_socket_errno;

        if (err != NGX_EINPROGRESS
#if (NGX_WIN32)
            /* Winsock returns WSAEWOULDBLOCK (NGX_EAGAIN) */
            && err != NGX_EAGAIN
#endif
            )
        {
            if (err == NGX_ECONNREFUSED
#if (NGX_LINUX)
                /*
                 * Linux returns EAGAIN instead of ECONNREFUSED
                 * for unix sockets if listen queue is full
                 */
                || err == NGX_EAGAIN
#endif
                || err == NGX_ECONNRESET
                || err == NGX_ENETDOWN
                || err == NGX_ENETUNREACH
                || err == NGX_EHOSTDOWN
                || err == NGX_EHOSTUNREACH)
            {
                level = NGX_LOG_ERR;
            } else {
                level = NGX_LOG_CRIT;
            }

            ngx_log_error(level, c->log, err, "fast_tunnel_connect() failed!");

            ngx_close_connection(c);
            ft->ctrl_con = NULL;

            return NGX_ERROR;
        }
    }

    if (ngx_add_conn) {
        if (rc == -1) {

            /* NGX_EINPROGRESS */

            return NGX_AGAIN;
        }

        wev->ready = 1;

        return NGX_OK;
    }

    if (ngx_event_flags & NGX_USE_CLEAR_EVENT) {

        /* kqueue */

        event = NGX_CLEAR_EVENT;

    } else {

        /* select, poll, /dev/poll */

        event = NGX_LEVEL_EVENT;
    }

    if (ngx_add_event(rev, NGX_READ_EVENT, event) != NGX_OK) {
        goto failed;
    }

    if (rc == -1) {

        /* NGX_EINPROGRESS */

        if (ngx_add_event(wev, NGX_WRITE_EVENT, event) != NGX_OK) {
            goto failed;
        }

        return NGX_AGAIN;
    }

    wev->ready = 1;

    return NGX_OK;

failed:

    ngx_close_connection(c);
    ft->ctrl_con = NULL;

    return NGX_ERROR;
}

ngx_int_t
fast_tunnel_accept(fast_tunnel_t *ft, ngx_connection_t *c)
{
}

ssize_t
fast_tunnel_send(fast_tunnel_t *ft, const void *data, size_t size)
{
}
