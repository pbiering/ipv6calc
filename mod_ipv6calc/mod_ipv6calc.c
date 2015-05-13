/*
 * Project    : ipv6calc/mod_ipv6calc
 * File       : mod_ipv6calc.c
 * Version    : $Id: mod_ipv6calc.c,v 1.1 2015/05/13 05:51:38 ds6peter Exp $
 * Copyright  : 2015-2015 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc Apache module
 *
 *  Currently supporting:
 *   - client IP address anonymization by setting IPV6CALC_CLIENT_IP_ANON
 *   	options: ipv6calcAnonPreset
 */

// Apache/APR related includes
#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h" 
#include "apr_strings.h"

// ipv6calc related includes
#include "libipv6calc.h"

/* features */
int feature_zeroize = 1; // always supported
int feature_anon    = 1; // always supported
int feature_kp      = 0; // will be checked later


#define MODULENAME "mod_ipv6calc"


/***************************
 * Prototyping
 ***************************/
static const char *set_ipv6calc_enable(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_anonpreset(cmd_parms *cmd, void *dummy, const char *anon_preset, int arg);

long int ipv6calc_debug = 0; // ipv6calc_debug usage


/***************************
 * Definitions
 ***************************/

/* define module name */
module AP_MODULE_DECLARE_DATA ipv6calc_module;


/* define config structure */
typedef struct {
	int enabled;
	int anon_set_default;
	s_ipv6calc_anon_set ipv6calc_anon_set;
} ipv6calc_server_config;


/*
 * Config options (ipv6calc_cmds)
 */
static const command_rec ipv6calc_cmds[] = {
	AP_INIT_FLAG("ipv6calcEnable"    , set_ipv6calc_enable, NULL, OR_FILEINFO, "Turn on mod_ipv6calc"),
	AP_INIT_TAKE1("ipv6calcAnonPreset",  (const char *(*)()) set_ipv6calc_anonpreset, NULL, OR_FILEINFO, "Define mod_ipv6calc anonymization method as ac ap zs zc zp kp"),
	{NULL} 
};


/***************************
 * Functions / Hooks
 ***************************/

/*
 * ipv6calc_cleanup
 */
static apr_status_t ipv6calc_cleanup(void *cfgdata) {
	// cleanup ipv6calc database wrapper
	libipv6calc_db_wrapper_cleanup();
	return APR_SUCCESS;
}

/*
 * ipv6calc_child_init
 */
static void ipv6calc_child_init(apr_pool_t *p, server_rec *s) {
	char string[NI_MAXHOST] = "";
	int result;

	apr_pool_cleanup_register(p, NULL, ipv6calc_cleanup, ipv6calc_cleanup);

	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(s->module_config, &ipv6calc_module);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s: ipv6calc_child_init"
		, MODULENAME
	);

	result = libipv6calc_db_wrapper_init("");
	if (result != 0) {
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
			, "%s: database wrapper initialization failed (disable module now): %d"
			, MODULENAME
			, result
		);

		config->enabled = 0;
	};

	/* check for KeepTypeAsnCC support */
	if ((libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB) == 1) \
	    && (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV6_REQ_DB) == 1)) {
		feature_kp = 1;
	};

	string[0] = '\0';
	libipv6calc_db_wrapper_features(string, sizeof(string));
	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s: features: %s"
		, MODULENAME
		, string
	);

	string[0] = '\0';
	libipv6calc_db_wrapper_capabilities(string, sizeof(string));
	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s: capabilities: %s"
		, MODULENAME
		, string
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s: internal main     library version: %s  API: %s  (%s)"
		, MODULENAME
		, libipv6calc_lib_version_string()
		, libipv6calc_api_version_string()
#ifdef SHARED_LIBRARY
		, "shared"
#else  // SHARED_LIBRARY
		, "built-in"
#endif // SHARED_LIBRARY
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s: internal database library version: %s  API: %s  (%s)"
		, MODULENAME
		, libipv6calc_db_lib_version_string()
		, libipv6calc_db_api_version_string()
#ifdef SHARED_LIBRARY
		, "shared"
#else  // SHARED_LIBRARY
		, "built-in"
#endif // SHARED_LIBRARY
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s: supported anonymization methods:%s%s%s"
		, MODULENAME
		, (feature_zeroize == 1) ? " ANON_ZEROISE" : ""
		, (feature_anon    == 1) ? " ANON_ANONYMIZE" : ""
		, (feature_kp      == 1) ? " ANON_KEEP-TYPE-ASN-CC" : ""
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s: %s anonymization preset: %s%s"
		, MODULENAME
		, (config->anon_set_default == 1) ? "default" : "configured"
		, libipv6calc_anon_method_name(&config->ipv6calc_anon_set)
		, ((feature_kp == 0) && (config->ipv6calc_anon_set.method == ANON_METHOD_KEEPTYPEASNCC)) ? " NOT-SUPPORTED" : ""
	);

	if ((feature_kp == 0) && (config->ipv6calc_anon_set.method == ANON_METHOD_KEEPTYPEASNCC)) {
		// fallback
		libipv6calc_anon_set_by_name(&config->ipv6calc_anon_set, "as"); // anonymize standard

		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
			, "%s: fallback anonymization preset: %s"
			, MODULENAME
			, libipv6calc_anon_method_name(&config->ipv6calc_anon_set)
		);
	};

	return;
};

/*
 * ipv6calc_post_read_request
 */
static int ipv6calc_post_read_request(request_rec *r) {
	// *** definitions
	// Apache/APR related includes
	apr_sockaddr_t *client_addr_p; // structure defined in apr_network_io.h
	ipv6calc_server_config* config;

	// ipv6calc related
	ipv6calc_ipaddr ipaddr;
	ipv6calc_ipv4addr ipv4addr;
#if APR_HAVE_IPV6
	ipv6calc_ipv6addr ipv6addr;
#endif

	// workflow related
	char client_addr_string_anonymized[APRMAXHOSTLEN];
	int result;


	// *** workflow
	// get config	
	config = (ipv6calc_server_config*) ap_get_module_config(r->server->module_config, &ipv6calc_module);

	// check enabled	
	if (!config->enabled) {
		return OK;
	};

	// get client address (aka REMOTE_IP)
	client_addr_p = r->connection->client_addr;

	ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
		, "%s: client IP address (string): %s  family: %d"
		, MODULENAME
		, r->connection->client_ip
		, client_addr_p->family
	);

	libipaddr_clearall(&ipaddr);

	if (client_addr_p->family == APR_INET) {
		// IPv4
		ipv4addr.in_addr = client_addr_p->sa.sin.sin_addr;
		ipv4addr.flag_valid = 1;

		libipv4addr_anonymize(&ipv4addr, config->ipv6calc_anon_set.mask_ipv4, config->ipv6calc_anon_set.method);

		CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);
#if APR_HAVE_IPV6
	} else if (client_addr_p->family == APR_INET6) {
		// IPv6
		ipv6addr_clear(&ipv6addr);
		//memcpy(&ipv6addr.in6_addr, &client_addr_p->sa.sin6.sin6_addr, client_addr_p->salen);
		ipv6addr.in6_addr = client_addr_p->sa.sin6.sin6_addr;
		ipv6addr.scope = ipv6addr_gettype(&ipv6addr);
		ipv6addr.flag_valid = 1;

		if ((ipv6addr.scope & IPV6_ADDR_MAPPED) == IPV6_ADDR_MAPPED) {
			// IPv4 address stored as mapped in IPv6 address
			ipv4addr_clear(&ipv4addr);

			result = libipv6addr_get_included_ipv4addr(&ipv6addr, &ipv4addr, 0);

			ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
				, "%s: mapped IPv4 address found in IPv6 address, status of extract: %d"
				, MODULENAME
				, result
			);

			libipv4addr_anonymize(&ipv4addr, config->ipv6calc_anon_set.mask_ipv4, config->ipv6calc_anon_set.method);

			CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);
		} else {
			libipv6addr_anonymize(&ipv6addr, &config->ipv6calc_anon_set);

			CONVERT_IPV6ADDRP_IPADDR(&ipv6addr, ipaddr);
		};
#endif
	} else {
		// unsupported family, do nothing
		return OK;
	};

	// get address string
	result = libipaddr_ipaddrstruct_to_string(&ipaddr, client_addr_string_anonymized, sizeof(client_addr_string_anonymized), 0); 

	if (result == 0) {
		ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
			, "%s: client IP address anonymized(string): %s"
			, MODULENAME
			, client_addr_string_anonymized
		);
		apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", client_addr_string_anonymized); 
	} else {
		apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", r->connection->client_ip); 
	};

	return OK;
}


/*
 * set_ipv6calc_enable
 */
static const char *set_ipv6calc_enable(cmd_parms *cmd, void *dummy, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	
	if (!config) {
		return NULL;
	};
	
	config->enabled = arg;
	
	return NULL;
}


/*
 * set_ipv6calc_anonpreset
 */
static const char *set_ipv6calc_anonpreset(cmd_parms *cmd, void *dummy, const char *anon_preset, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	int result;
	
	if (!config) {
		return NULL;
	};
	
	result = libipv6calc_anon_set_by_name(&config->ipv6calc_anon_set, apr_pstrdup(cmd->pool, anon_preset));

	if (result != 0) {
		return "Unsupported anonymization preset";

	};

	config->anon_set_default = 0;

	return NULL;
}

/*
 * ipv6calc_create_svr_conf
 */
static void *ipv6calc_create_svr_conf(apr_pool_t* pool, server_rec* svr) {
	ipv6calc_server_config* svr_cfg = apr_pcalloc(pool, sizeof(ipv6calc_server_config));
	
	svr_cfg->enabled = 0;

	libipv6calc_anon_set_by_name(&svr_cfg->ipv6calc_anon_set, "as"); // anonymize standard
	svr_cfg->anon_set_default = 1;

	return svr_cfg ;
}


/*
 * ipv6calc_register_hooks
 */
static void ipv6calc_register_hooks(apr_pool_t *p) {
	ap_hook_child_init(ipv6calc_child_init, NULL, NULL, APR_HOOK_MIDDLE );
	ap_hook_post_read_request(ipv6calc_post_read_request, NULL, NULL, APR_HOOK_MIDDLE );
}


/*
 * mod_ipv6calc API hooks
 */
module AP_MODULE_DECLARE_DATA ipv6calc_module = {
	STANDARD20_MODULE_STUFF, 
	NULL,                        /* create per-dir    config structures */
	NULL,                        /* merge  per-dir    config structures */
	ipv6calc_create_svr_conf,    /* create per-server config structures */
	NULL,                        /* merge  per-server config structures */
	ipv6calc_cmds,               /* table of config file commands       */
	ipv6calc_register_hooks      /* register hooks                      */
};
