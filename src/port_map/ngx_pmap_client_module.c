#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_pmap.h"

static void *ngx_pmap_client_create_conf(ngx_cycle_t *cycle);
static void *ngx_pmap_client_init_conf(ngx_cycle_t *cycle, void *conf);

static char *ngx_pmap_client_listen(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_pmap_client_set_server_addr(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_pmap_client_set_kcp_addr(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_str_t client_name = ngx_string("pmap_client");


static ngx_command_t ngx_pmap_client_commands[] = {
    { ngx_string("listen"),
      NGX_PMAP_CLIENT_CONF|NGX_CONF_1MORE,
      ngx_pmap_client_listen,
      0,
      0,
      NULL },

    { ngx_string("server_addr"),
      NGX_PMAP_CLIENT_CONF|NGX_CONF_1MORE,
      ngx_pmap_client_set_server_addr,
      0,
      0,
      NULL },
    
    { ngx_string("use_kcp"),
      NGX_PMAP_CLIENT_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      0,
      offsetof(ngx_pmap_client_conf_t, use_kcp),
      NULL },

    { ngx_string("kcp_addr"),
      NGX_PMAP_CLIENT_CONF|NGX_CONF_1MORE,
      ngx_pmap_client_set_kcp_addr,
      0,
      0,
      NULL },

    ngx_null_command
};

static ngx_pmap_module_t ngx_pmap_client_module_ctx = {
    &client_name,

    ngx_pmap_client_create_conf,
    ngx_pmap_client_init_conf,
};

ngx_module_t ngx_pmap_client_module = {
    NGX_MODULE_V1,
    &ngx_pmap_client_module_ctx,
    ngx_pmap_client_commands,
    NGX_PMAP_MODULE,
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static void *
ngx_pmap_client_create_conf(ngx_cycle_t *cycle)
{
    ngx_pmap_client_conf_t *clientcf;

    clientcf = ngx_palloc(cycle->pool, sizeof(ngx_pmap_client_conf_t));
    if (NULL == clientcf) {
        return NULL;
    }

    clientcf->use_kcp = NGX_CONF_UNSET;

    return clientcf;
}

static void *
ngx_pmap_client_init_conf(ngx_cycle_t *cycle, void *conf)
{
    return NGX_CONF_OK;
}

static char *
ngx_pmap_client_listen(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_pmap_client_conf_t *clientcf = conf;
    ngx_pmap_listen_t      *ls = &clientcf->listen;

    return ngx_pmap_parse_listen_addr(cf, ls);
}

static char *
ngx_pmap_client_set_server_addr(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_pmap_client_conf_t *clientcf = (ngx_pmap_client_conf_t *)conf;

    return ngx_pmap_parse_addr(cf, &clientcf->server_addr);
}

static char *
ngx_pmap_client_set_kcp_addr(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    return NGX_CONF_OK;
}
