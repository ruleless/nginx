#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_pmap.h"

static char *ngx_pmap_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *ngx_pmap_core_create_conf(ngx_cycle_t *cycle);
static void *ngx_pmap_core_init_conf(ngx_cycle_t *cycle, void *conf);

static ngx_uint_t ngx_pmap_max_module;


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


static ngx_str_t pmap_core_name = ngx_string("port_map");

static ngx_command_t ngx_pmap_core_commands[] = {
	{
		ngx_string("endpoint"),
		NGX_PMAP_CONF|NGX_CONF_TAKE1,
		ngx_conf_set_num_slot,
		0,
		offsetof(ngx_pmap_conf_t, endpoint),
		NULL
	},

	ngx_null_command
};

ngx_pmap_module_t ngx_pmap_core_module_ctx = {
	&pmap_core_name,

	ngx_pmap_core_create_conf,
	ngx_pmap_core_init_conf,
};

ngx_module_t ngx_pmap_core_module = {
	NGX_MODULE_V1,
	&ngx_pmap_core_module_ctx,
	ngx_pmap_core_commands,
	NGX_PMAP_MODULE,
	NULL,                                  /* init master */
    NULL,                 				   /* init module */
    NULL,					               /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static char *
ngx_pmap_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	char *rv;
	ngx_int_t i, mi;
	void ***ctx;
	ngx_pmap_module_t *m;
	ngx_conf_t pcf;
	
	if (*(void **)conf) {
		return "is duplicate";
	}

	/* count the number of the pmap modules and set up their indices */
	ngx_pmap_max_module = ngx_count_modules(cf->cycle, NGX_PMAP_MODULE);

	ctx = ngx_pcalloc(cf->pool, sizeof(void *));
	if (NULL == ctx) {
		return NGX_CONF_ERROR;
	}

	*ctx = ngx_pcalloc(cf->pool, ngx_pmap_max_module*sizeof(void *));
	if (NULL == *ctx) {
		return NGX_CONF_ERROR;
	}

	*(void **)conf = ctx;

	for (i = 0; cf->cycle->modules[i]; ++i) {
		if (cf->cycle->modules[i]->type != NGX_PMAP_MODULE) {
			continue;
		}
		
		m = cf->cycle->modules[i]->ctx;
		mi = cf->cycle->modules[i]->ctx_index;
		
		if (m->create_conf) {
			(*ctx)[mi] = m->create_conf(cf->cycle);
			if (NULL == (*ctx)[mi]) {
				return NGX_CONF_ERROR;
			}
		}
	}

	pcf = *cf;
	cf->ctx = ctx;
	cf->module_type = NGX_PMAP_MODULE;
	cf->cmd_type = NGX_PMAP_CONF;

	rv = ngx_conf_parse(cf, NULL);

	*cf = pcf;

	if (rv != NGX_CONF_OK) {
		return rv;
	}

	for (i = 0; cf->cycle->modules[i]; ++i) {
		if (cf->cycle->modules[i]->type != NGX_PMAP_MODULE) {
			continue;
		}

		m = cf->cycle->modules[i]->ctx;
		mi = cf->cycle->modules[i]->ctx_index;

		if (m->init_conf) {
			rv = m->init_conf(cf->cycle, (*ctx)[mi]);
			if (rv != NGX_CONF_OK) {
				return rv;
			}
		}		
	}

	return NGX_CONF_OK;
}

static void *
ngx_pmap_core_create_conf(ngx_cycle_t *cycle)
{
	ngx_pmap_conf_t *pmapcf;

	pmapcf = ngx_palloc(cycle->pool, sizeof(ngx_pmap_conf_t));
	if (NULL == pmapcf) {
		return NULL;
	}

	pmapcf->endpoint = NGX_CONF_UNSET;

	return pmapcf;
}

static void *
ngx_pmap_core_init_conf(ngx_cycle_t *cycle, void *conf)
{
	return NGX_CONF_OK;
}
