#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_pmap.h"

static char *ngx_pmap_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t ngx_pmap_commands[] = {
	{
		ngx_string("port_map"),
		NGX_MAIN_CONF|NGX_CONF_BLOCK|NGX_CONF_NOARGS,
		ngx_pmap_block,
		0,
		0,
		NULL
	},

	ngx_null_command
};

static ngx_core_module_t ngx_pmap_module_ctx = {
	ngx_string("pmap"),
	NULL,
	NULL,
};

ngx_module_t ngx_pmap_module = {
	NGX_MODULE_V1,
	&ngx_pmap_module_ctx,
	ngx_pmap_commands,
	NGX_CORE_MODULE,
	NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
	NGX_MODULE_V1_PADDING
};

static char *
ngx_pmap_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	
}
