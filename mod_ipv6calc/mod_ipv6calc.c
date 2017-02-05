/*
 * Project    : ipv6calc/mod_ipv6calc
 * File       : mod_ipv6calc.c
 * Version    : $Id$
 * Copyright  : 2015-2015 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc Apache module
 *
 *  Currently supporting:
 *   - client IP address anonymization by setting environment IPV6CALC_CLIENT_IP_ANON
 *   - client IP address country code retrievement by setting environment IPV6CALC_CLIENT_COUNTRYCODE
 *   - client IP address ASN retrievement by setting environment IPV6CALC_CLIENT_ASN
 *   - client IP address registry retrievement by setting environment IPV6CALC_CLIENT_REGISTRY
 *   - anonymization method by setting IPV6CALC_ANON_METHOD
 *
 *  mode_ipv6calc behavior can be controlled by config, e.g.
 *   ipv6calcActionAnonymize		on
 *   ipv6calcActionCountrycode		on
 *   ipv6calcActionAsn			on
 *   ipv6calcActionRegistry		on
 *   ipv6calcCache			off (default: on)
 *   ipv6calcCacheLimit			>= IPV6CALC_CACHE_LRI_LIMIT_MIN
 *   ipv6calcCacheStatisticsInterval	0:disable 
 *   ipv6calcDebuglevel			>0 (see defines below)
 *
 *  ipv6calc behavior can be controlled by config, e.g
 *   ipv6calcOption debug                   0x8
 *   ipv6calcOption anonymize-preset        keep-type-asn-cc
 *   ipv6calcOption disable-external        yes
 *   ipv6calcOption disable-ip2location     yes
 *   ipv6calcOption mask-ipv4               16
 *   ipv6calcOption mask-ipv6               32
 *
 *   see also
 *    ipv6calc -h
 *    ipv6calc -A anonymize -h
 */

// Apache/APR related includes
#include <httpd.h>
#include <http_config.h>
#include <http_log.h>
#include <http_protocol.h>
#include <apr_strings.h>

// ipv6calc related includes
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_URL
#undef PACKAGE_VERSION
#include "config.h"
#include "libipv6calc.h"

/* features */
int feature_zeroize = 1; // always supported
int feature_anon    = 1; // always supported
int feature_kp      = 0; // will be checked later


/***************************
 * Module copyright
 ***************************/
/* global program related definitions */
#define PROGRAM_NAME "mod_ipv6calc"
#define PROGRAM_COPYRIGHT "(P) & (C) 2015-" COPYRIGHT_YEAR " by Peter Bieringer <pb (at) bieringer.de>"


/***************************
 * Module debugging
 ***************************/
#define IPV6CALC_DEBUG_MAP_DEBUG_TO_NOTICE	0x0001

#define IPV6CALC_DEBUG_CACHE_LOOKUP		0x0010
#define IPV6CALC_DEBUG_CACHE_ENTRIES		0x0020
#define IPV6CALC_DEBUG_CACHE_STORE		0x0040
#define IPV6CALC_DEBUG_RETRIEVE_DATA		0x0080

#define IPV6CALC_DEBUG_SHOW_DB_INFO		0x0100


/***************************
 * ipv6calc library debugging and option handling
 ***************************/
long int ipv6calc_debug = 0; // ipv6calc_debug usage (possible set via option)

/* options (only used via the option parser) */
struct option longopts[IPV6CALC_MAXLONGOPTIONS];
char   shortopts[NI_MAXHOST] = "";
int    longopts_maxentries = 0;


/***************************
 * Prototyping
 ***************************/
static const char *set_ipv6calc_enable(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_no_fallback(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_cache(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_cache_limit(cmd_parms *cmd, void *dummy, const char *value, int arg);
static const char *set_ipv6calc_cache_statistics_interval(cmd_parms *cmd, void *dummy, const char *value, int arg);
static const char *set_ipv6calc_debuglevel(cmd_parms *cmd, void *dummy, const char *value, int arg);

static const char *set_ipv6calc_action_anonymize(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_action_countrycode(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_action_asn(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_action_registry(cmd_parms *cmd, void *dummy, int arg);

static const char *set_ipv6calc_option(cmd_parms *cmd, void *dummy, const char *name, const char *value, int arg);


/***************************
 * Cache (Last Recently Inserted)
 ***************************/
#define IPV6CALC_CACHE_LRI_SIZE 200
#define IPV6CALC_CACHE_LRI_LIMIT_MIN 20

static long int ipv6calc_cache_lri_checked[2] = {0, 0};

static int      ipv6calc_cache_lri_max[2] = {0, 0};
static int      ipv6calc_cache_lri_last[2] = {0, 0 };
static long int ipv6calc_cache_lri_statistics[2][IPV6CALC_CACHE_LRI_SIZE];

static char     ipv6calc_cache_lri_value_anon[2][IPV6CALC_CACHE_LRI_SIZE][APRMAXHOSTLEN];
static char     ipv6calc_cache_lri_value_cc[2][IPV6CALC_CACHE_LRI_SIZE][APRMAXHOSTLEN];
static char     ipv6calc_cache_lri_value_asn[2][IPV6CALC_CACHE_LRI_SIZE][APRMAXHOSTLEN];
static char     ipv6calc_cache_lri_value_registry[2][IPV6CALC_CACHE_LRI_SIZE][APRMAXHOSTLEN];

static struct in_addr  ipv6calc_cache_lri_ipv4_token[IPV6CALC_CACHE_LRI_SIZE];
#if APR_HAVE_IPV6
static struct in6_addr ipv6calc_cache_lri_ipv6_token[IPV6CALC_CACHE_LRI_SIZE];
#endif


/***************************
 * Static values
 ***************************/
static const char *anon_method_name = "-";


/***************************
 * Definitions
 ***************************/

/* define module name */
module AP_MODULE_DECLARE_DATA ipv6calc_module;


/* define config structure */
typedef struct {
	int enabled;

	int no_fallback;

	int cache;
	int cache_limit;
	unsigned long int cache_statistics_interval;

	uint32_t debuglevel;

	int action_anonymize;
	int action_countrycode;
	int action_asn;
	int action_registry;

	int anon_set_default;
	s_ipv6calc_anon_set ipv6calc_anon_set;
} ipv6calc_server_config;


/* module options forwarded to ipv6calc during init */
typedef struct {
	char name[NI_MAXHOST];
	char value[NI_MAXHOST];
} ipv6calc_option;

#define mod_ipv6calc_options_max IPV6CALC_MAXLONGOPTIONS
ipv6calc_option ipv6calc_option_list[mod_ipv6calc_options_max]; 
int ipv6calc_option_list_entries = 0;

#define mod_ipv6calc_pi_IPV4	0
#define mod_ipv6calc_pi_IPV6	1


/*
 * Config options (ipv6calc_cmds)
 */
static const command_rec ipv6calc_cmds[] = {
	AP_INIT_FLAG("ipv6calcEnable", set_ipv6calc_enable, NULL, OR_FILEINFO, "Turn on mod_ipv6calc"),
	AP_INIT_FLAG("ipv6calcNoFallback", set_ipv6calc_no_fallback, NULL, OR_FILEINFO, "Do not fallback in case of issues with mod_ipv6calc"),
	AP_INIT_FLAG("ipv6calcCache", set_ipv6calc_cache, NULL, OR_FILEINFO, "Turn off mod_ipv6calc cache"),
	AP_INIT_TAKE1("ipv6calcCacheLimit",  (const char *(*)()) set_ipv6calc_cache_limit, NULL, OR_FILEINFO, "mod_ipv6calc cache limit: <value>"),
	AP_INIT_TAKE1("ipv6calcCacheStatisticsInterval",  (const char *(*)()) set_ipv6calc_cache_statistics_interval, NULL, OR_FILEINFO, "mod_ipv6calc cache statistics interval: <value> (0=disabled)"),
	AP_INIT_TAKE1("ipv6calcDebuglevel",  (const char *(*)()) set_ipv6calc_debuglevel, NULL, OR_FILEINFO, "Debug level of module (binary or'ed): <value>"),
	AP_INIT_FLAG("ipv6calcActionAnonymize", set_ipv6calc_action_anonymize, NULL, OR_FILEINFO, "Store anonymized IP address in IPV6CALC_CLIENT_IP_ANON"),
	AP_INIT_FLAG("ipv6calcActionCountrycode", set_ipv6calc_action_countrycode, NULL, OR_FILEINFO, "Store Country Code of IP address in IPV6CALC_CLIENT_COUNTRYCODE"),
	AP_INIT_FLAG("ipv6calcActionAsn", set_ipv6calc_action_asn, NULL, OR_FILEINFO, "Store ASN of IP address in IPV6CALC_CLIENT_COUNTRYCODE"),
	AP_INIT_FLAG("ipv6calcActionRegistry", set_ipv6calc_action_registry, NULL, OR_FILEINFO, "Store Registry of IP address in IPV6CALC_CLIENT_COUNTRYCODE"),
	AP_INIT_TAKE2("ipv6calcOption",  (const char *(*)()) set_ipv6calc_option, NULL, OR_FILEINFO, "Define ipv6calc option: <key> <value>"),
	{NULL} 
};


/***************************
 * Support functions
 ***************************/
static int ipv6calc_support_init(server_rec *s) {
	int i, result;
	static int ipv6calc_options_added = 0;

	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(s->module_config, &ipv6calc_module);

	int mod_ipv6calc_APLOG_DEBUG = (config->debuglevel & IPV6CALC_DEBUG_MAP_DEBUG_TO_NOTICE) ? APLOG_NOTICE : APLOG_DEBUG;

	// check enabled	
	if (config->enabled == 0) {
		ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
			, "module is NOT enabled (check for 'ipv6calcEnable on')"
		);
		return 0;
	};

	ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
		, "start ipv6calc initialization"
	);

	/* add options */
	if (ipv6calc_options_added == 0) {
		ipv6calc_options_add_common_anon(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
		ipv6calc_options_add_common_basic(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
		ipv6calc_options_added = 1;
	};

	ipv6calc_quiet = 1; // be quiet by default

	/* initialize ipv6calc options from list retrieved via APR config parser */
	ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
		, "apply ipv6calc options"
	);

	if (ipv6calc_option_list_entries > 0) {
		for (i = 0; i < ipv6calc_option_list_entries; i++) {
			result = ipv6calc_set_option(longopts, ipv6calc_option_list[i].name, ipv6calc_option_list[i].value, &config->ipv6calc_anon_set);

			if (result == 0) {
				ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
					, "ipv6calc option %s: %s=%s"
					, "successfully set"
					, ipv6calc_option_list[i].name
					, ipv6calc_option_list[i].value
				);
			} else {
				ap_log_error(APLOG_MARK, APLOG_WARNING, 0, s
					, "ipv6calc option %s: %s=%s"
					, "NOT UNDERSTOOD"
					, ipv6calc_option_list[i].name
					, ipv6calc_option_list[i].value
				);
			};

			if ((result == 0) &&
			  (
			       (strcmp(ipv6calc_option_list[i].name, "anonymize-preset") == 0)
			    || (strcmp(ipv6calc_option_list[i].name, "anonymize-method") == 0)
			  )
			) {
				config->anon_set_default = 0;
			};
		};
	};

	ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
		, "start ipv6calc database wrapper initialization"
	);

	result = libipv6calc_db_wrapper_init("");
	if (result != 0) {
		config->enabled = 0;

		if (config->no_fallback) {
			ap_log_error(APLOG_MARK, APLOG_ERR, 0, s
				, "database wrapper initialization failed (NO-FALLBACK activated, STOP NOW): %d"
				, result
			);

			return(1);
		} else {
			ap_log_error(APLOG_MARK, APLOG_WARNING, 0, s
				, "database wrapper initialization failed (disable module now): %d"
				, result
			);

			return(0);
		};
	} else {
		ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
			, "ipv6calc database wrapper initialization finished"
		);
	};

	/* check for KeepTypeAsnCC support */
	if ((libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB) == 1) \
	    && (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV6_REQ_DB) == 1)) {
		feature_kp = 1;
	};

	return(0);
};


/***************************
 * Hooks functions
 ***************************/

/*
 * ipv6calc_cleanup
 */
static apr_status_t ipv6calc_cleanup(void *cfgdata) {
	// cleanup ipv6calc database wrapper
	libipv6calc_db_wrapper_cleanup();
	return APR_SUCCESS;
};

/*
 * ipv6calc_post_config
 */
static int ipv6calc_post_config(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s) {
	char string[NI_MAXHOST] = "";
	int result;

	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(s->module_config, &ipv6calc_module);

	int mod_ipv6calc_APLOG_DEBUG = (config->debuglevel & IPV6CALC_DEBUG_MAP_DEBUG_TO_NOTICE) ? APLOG_NOTICE : APLOG_DEBUG;

	// check enabled	
	if (config->enabled == 0) {
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
			, "module is NOT enabled (check for 'ipv6calcEnable on')"
		);
		return 0;
	};

#ifdef SHARED_LIBRARY
	IPV6CALC_LIB_VERSION_CHECK_EXIT(IPV6CALC_PACKAGE_VERSION_NUMERIC, IPV6CALC_PACKAGE_VERSION_STRING)
	IPV6CALC_DB_LIB_VERSION_CHECK_EXIT(IPV6CALC_PACKAGE_VERSION_NUMERIC, IPV6CALC_PACKAGE_VERSION_STRING)
#endif // SHARED_LIBRARY

	ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
		, "ipv6calc_post_config"
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "internal main     library version: %s  API: %s  (%s)"
		, libipv6calc_lib_version_string()
		, libipv6calc_api_version_string()
#ifdef SHARED_LIBRARY
		, "shared"
#else  // SHARED_LIBRARY
		, "built-in"
#endif // SHARED_LIBRARY
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "internal database library version: %s  API: %s  (%s)"
		, libipv6calc_db_lib_version_string()
		, libipv6calc_db_api_version_string()
#ifdef SHARED_LIBRARY
		, "shared"
#else  // SHARED_LIBRARY
		, "built-in"
#endif // SHARED_LIBRARY
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s module actions: anonymize=%s countrycode=%s asn=%s registry=%s"
		, ((config->action_anonymize + config->action_countrycode) == 0) ? "default" : "configured"
		, (config->action_anonymize > 0) ? "ON" : "OFF"
		, (config->action_countrycode > 0) ? "ON" : "OFF"
		, (config->action_asn > 0) ? "ON" : "OFF"
		, (config->action_registry > 0) ? "ON" : "OFF"
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "%s module debug level: 0x%08x (%d)"
		, (config->debuglevel == 0) ? "default" : "configured"
		, config->debuglevel
		, config->debuglevel
	);

	if (config->cache == 0) {
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
			, "module cache: OFF (configured)"
		);
	} else {
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
			, "module cache: ON (default)  limit=%d (%s)  statistics_interval=%lu (%s)"
			, config->cache_limit
			, (config->cache_limit == IPV6CALC_CACHE_LRI_LIMIT_MIN) ? "default/minimum" : "configured"
			, config->cache_statistics_interval
			, (config->cache_statistics_interval == 0) ? "default" : "configured"
		);
	};

	result = ipv6calc_support_init(s);

	if (result != 0) {
		return(1);
	};

	string[0] = '\0';
	libipv6calc_db_wrapper_features(string, sizeof(string));
	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "features: %s"
		, string
	);

	string[0] = '\0';
	libipv6calc_db_wrapper_capabilities(string, sizeof(string));
	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "capabilities: %s"
		, string
	);

	if (config->debuglevel && IPV6CALC_DEBUG_SHOW_DB_INFO) {
		libipv6calc_db_wrapper_print_db_info(0, "");
	};

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "supported anonymization methods:%s%s%s"
		, (feature_zeroize == 1) ? " ANON_ZEROISE" : ""
		, (feature_anon    == 1) ? " ANON_ANONYMIZE" : ""
		, (feature_kp      == 1) ? " ANON_KEEP-TYPE-ASN-CC" : ""
	);

	if (config->ipv6calc_anon_set.method != ANON_METHOD_KEEPTYPEASNCC) {
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
			, "%s anonymization method: %s mask_ipv4=%d mask_ipv6=%d mask_eui64=%d mask_mac=%d mask_autoadjust=%s"
			, (config->anon_set_default == 1) ? "default" : "configured"
			, libipv6calc_anon_method_name(&config->ipv6calc_anon_set)
			, config->ipv6calc_anon_set.mask_ipv4
			, config->ipv6calc_anon_set.mask_ipv6
			, config->ipv6calc_anon_set.mask_eui64
			, config->ipv6calc_anon_set.mask_mac
			, (config->ipv6calc_anon_set.mask_autoadjust > 0) ? "yes" : "no"
		);
	} else {
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
			, "%s anonymization method: %s%s"
			, (config->anon_set_default == 1) ? "default" : "configured"
			, libipv6calc_anon_method_name(&config->ipv6calc_anon_set)
			, ((feature_kp == 0) && (config->ipv6calc_anon_set.method == ANON_METHOD_KEEPTYPEASNCC)) ? " NOT-SUPPORTED" : ""
		);

		if (feature_kp == 0) {
			if (config->no_fallback) {
				ap_log_error(APLOG_MARK, APLOG_ERR, 0, s
					, "%s anonymization method: %s NOT-SUPPORTED, NO-FALLBACK activated - STOP NOW"
					, (config->anon_set_default == 1) ? "default" : "configured"
					, libipv6calc_anon_method_name(&config->ipv6calc_anon_set)
				);
				return(1);
			};

			// fallback
			libipv6calc_anon_set_by_name(&config->ipv6calc_anon_set, "as"); // anonymize standard

			ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
				, "fallback anonymization method: %s mask_ipv4=%d mask_ipv6=%d mask_eui64=%d mask_mac=%d mask_autoadjust=%s"
				, libipv6calc_anon_method_name(&config->ipv6calc_anon_set)
				, config->ipv6calc_anon_set.mask_ipv4
				, config->ipv6calc_anon_set.mask_ipv6
				, config->ipv6calc_anon_set.mask_eui64
				, config->ipv6calc_anon_set.mask_mac
				, (config->ipv6calc_anon_set.mask_autoadjust > 0) ? "yes" : "no"
			);
		};
	};

	anon_method_name = libipv6calc_anon_method_name(&config->ipv6calc_anon_set);

	return(0);
};



/*
 * ipv6calc_child_init
 */
static void ipv6calc_child_init(apr_pool_t *p, server_rec *s) {

	apr_pool_cleanup_register(p, NULL, ipv6calc_cleanup, ipv6calc_cleanup);

	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(s->module_config, &ipv6calc_module);

	int mod_ipv6calc_APLOG_DEBUG = (config->debuglevel & IPV6CALC_DEBUG_MAP_DEBUG_TO_NOTICE) ? APLOG_NOTICE : APLOG_DEBUG;

	// check enabled	
	if (config->enabled == 0) {
		ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
			, "module is NOT enabled (check for 'ipv6calcEnable on')"
		);
		return;
	};

	ap_log_error(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, s
		, "ipv6calc_child_init"
	);

	ipv6calc_support_init(s);

	/* check for KeepTypeAsnCC support */
	if ((libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB) == 1) \
	    && (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV6_REQ_DB) == 1)) {
		feature_kp = 1;
	};

	if (config->ipv6calc_anon_set.method != ANON_METHOD_KEEPTYPEASNCC) {
		// nothing to do
	} else {
		if (feature_kp == 0) {
			// fallback
			libipv6calc_anon_set_by_name(&config->ipv6calc_anon_set, "as"); // anonymize standard
		};
	};

	return;
};


/*
 * ipv6calc_post_read_request  (ACTION CODE)
 */
static int ipv6calc_post_read_request(request_rec *r) {
	int i, hit;
	int pi; // proto index (0:IPv4, 1:IPv6
	int p_mapped; // proto mapped (IPv6 in IPv4)

	int mod_ipv6calc_APLOG_DEBUG = APLOG_DEBUG;

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
	char cc[APRMAXHOSTLEN];
	char asn[APRMAXHOSTLEN];
	char registry[APRMAXHOSTLEN];
	unsigned int data_source;

	int result;

	// *** workflow
	// get config	
	config = (ipv6calc_server_config*) ap_get_module_config(r->server->module_config, &ipv6calc_module);

	// check enabled	
	if (config->enabled == 0) {
		return OK;
	};

	if (config->debuglevel & IPV6CALC_DEBUG_MAP_DEBUG_TO_NOTICE) {
		mod_ipv6calc_APLOG_DEBUG = APLOG_NOTICE;
	};

	// get client address (aka REMOTE_IP)
#if (((AP_SERVER_MAJORVERSION_NUMBER == 2) && (AP_SERVER_MINORVERSION_NUMBER >= 4)) || (AP_SERVER_MAJORVERSION_NUMBER > 2))
	client_addr_p = r->connection->client_addr;
#else
	client_addr_p = r->connection->remote_addr;
#endif

	ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
		, "client IP address: %s  family: %d"
#if (((AP_SERVER_MAJORVERSION_NUMBER == 2) && (AP_SERVER_MINORVERSION_NUMBER >= 4)) || (AP_SERVER_MAJORVERSION_NUMBER > 2))
		, r->connection->client_ip
#else
		, r->connection->remote_ip
#endif
		, client_addr_p->family
	);

	// convert address into ipv6calc structure
	libipaddr_clearall(&ipaddr);

	/* store address */
	if (client_addr_p->family == APR_INET) {
		// IPv4
		pi = mod_ipv6calc_pi_IPV4;
		p_mapped = 0;
#if APR_HAVE_IPV6
	} else if (client_addr_p->family == APR_INET6) {
		// IPv6
		pi = mod_ipv6calc_pi_IPV6;

		// check for compat/mapped
		if (	client_addr_p->sa.sin6.sin6_addr.s6_addr32[0] == 0
		    &&	client_addr_p->sa.sin6.sin6_addr.s6_addr32[1] == 0
		    &&	client_addr_p->sa.sin6.sin6_addr.s6_addr32[2] == 0xffff0000
		) {
			pi = mod_ipv6calc_pi_IPV4;
			p_mapped = 1;
		} else {
			p_mapped = 0;
		};
#endif
	} else {
		// unsupported family, do nothing
		return OK;
	};

	/* cache lookup */
	if (config->cache == 1) {
		hit = -1;

#if APR_HAVE_IPV6
#define IPV6CALC_COMPARE(entry) \
			     (    pi == mod_ipv6calc_pi_IPV4 \
			       && ( \
				      (p_mapped == 0 && ipv6calc_cache_lri_ipv4_token[entry].s_addr == client_addr_p->sa.sin.sin_addr.s_addr) \
				   || (p_mapped == 1 && ipv6calc_cache_lri_ipv4_token[entry].s_addr == client_addr_p->sa.sin6.sin6_addr.s6_addr32[3]) \
				  ) \
			     ) \
			  || (    pi == mod_ipv6calc_pi_IPV6 \
			       && ipv6calc_cache_lri_ipv6_token[entry].s6_addr32[3] == client_addr_p->sa.sin6.sin6_addr.s6_addr32[3] \
			       && ipv6calc_cache_lri_ipv6_token[entry].s6_addr32[2] == client_addr_p->sa.sin6.sin6_addr.s6_addr32[2] \
			       && ipv6calc_cache_lri_ipv6_token[entry].s6_addr32[1] == client_addr_p->sa.sin6.sin6_addr.s6_addr32[1] \
			       && ipv6calc_cache_lri_ipv6_token[entry].s6_addr32[0] == client_addr_p->sa.sin6.sin6_addr.s6_addr32[0] \
			     )
#else
#define IPV6CALC_COMPARE(entry) \
			     (    pi == mod_ipv6calc_pi_IPV4 \
			       && ipv6calc_cache_lri_ipv4_token[entry].s_addr == client_addr_p->sa.sin.sin_addr.s_addr \
			     )
#endif


		if (ipv6calc_cache_lri_max[pi] > 0) {
			ipv6calc_cache_lri_checked[pi]++;

			if (config->debuglevel & IPV6CALC_DEBUG_CACHE_LOOKUP) {
				if (pi == mod_ipv6calc_pi_IPV4) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "IPv4 address to lookup in cache: %08x"
						, (p_mapped == 0) ? client_addr_p->sa.sin.sin_addr.s_addr : client_addr_p->sa.sin6.sin6_addr.s6_addr32[3]
					);
				} else if (pi == mod_ipv6calc_pi_IPV6) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "IPv6 address to lookup in cache: %08x %08x %08x %08x"
						, client_addr_p->sa.sin6.sin6_addr.s6_addr32[0]
						, client_addr_p->sa.sin6.sin6_addr.s6_addr32[1]
						, client_addr_p->sa.sin6.sin6_addr.s6_addr32[2]
						, client_addr_p->sa.sin6.sin6_addr.s6_addr32[3]
					);
				};
			};

			if (config->debuglevel & IPV6CALC_DEBUG_CACHE_ENTRIES) {
				for (i = 0; i < ipv6calc_cache_lri_max[pi]; i++) {
					if (pi == mod_ipv6calc_pi_IPV4) {
						ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
							, "IPv4 address in cache       %3d: %08x"
							, i
							, ipv6calc_cache_lri_ipv4_token[i].s_addr
						);
					} else if (pi == mod_ipv6calc_pi_IPV6) {
						ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
							, "IPv6 address in cache       %3d: %08x %08x %08x %08x"
							, i
							, ipv6calc_cache_lri_ipv6_token[i].s6_addr32[0]
							, ipv6calc_cache_lri_ipv6_token[i].s6_addr32[1]
							, ipv6calc_cache_lri_ipv6_token[i].s6_addr32[2]
							, ipv6calc_cache_lri_ipv6_token[i].s6_addr32[3]
						);
					};
				};
			};

			if (config->debuglevel & IPV6CALC_DEBUG_CACHE_LOOKUP) {
				ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
					, "check for IPv%s address in cache on last inserted entry: %d"
					, (pi == 0) ? "4" : "6"
					, ipv6calc_cache_lri_last[pi] - 1
				);
			};

			/* check last seen one first */
			if (IPV6CALC_COMPARE(ipv6calc_cache_lri_last[pi] - 1)) {
				ipv6calc_cache_lri_statistics[pi][0]++;
				hit = ipv6calc_cache_lri_last[pi] - 1;
			};

			/* run backwards to first entry */
			if ((hit < 0) && (ipv6calc_cache_lri_last[pi] > 1)) {
				if (config->debuglevel & IPV6CALC_DEBUG_CACHE_LOOKUP) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "check for IPv%s address in cache backwards to first entry"
						, (pi == 0) ? "4" : "6"
					);
				};

				for (i = ipv6calc_cache_lri_last[pi] - 1; i > 0; i--) {
					if (IPV6CALC_COMPARE(i - 1)) {
						ipv6calc_cache_lri_statistics[pi][ipv6calc_cache_lri_last[pi] - i]++;
						hit = i - 1;
						break;
					};
				};
			};

			/* round robin */ 
			if ((hit < 0) && (ipv6calc_cache_lri_last[pi] < ipv6calc_cache_lri_max[pi])) {
				if (config->debuglevel & IPV6CALC_DEBUG_CACHE_LOOKUP) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "check for IPv%s address in cache round robin from last entry"
						, (pi == 0) ? "4" : "6"
					);
				};

				for (i = ipv6calc_cache_lri_max[pi]; i > ipv6calc_cache_lri_last[pi]; i--) {
					if (IPV6CALC_COMPARE(i - 1)) {
						ipv6calc_cache_lri_statistics[pi][ipv6calc_cache_lri_max[pi] - i + ipv6calc_cache_lri_last[pi]]++;
						hit = i - 1;
						break;
					};
				};
			};

			if (hit >= 0) {
				ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
					, "retrieve data of IPv%s address from cache position: %d"
					, (pi == 0) ? "4" : "6"
					, hit
				);

				if (config->action_countrycode == 1) {
					ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
						, "client IP country code (from cache): %s"
						, ipv6calc_cache_lri_value_cc[pi][hit]
					);

					apr_table_set(r->subprocess_env
						, "IPV6CALC_CLIENT_COUNTRYCODE"
						, ipv6calc_cache_lri_value_cc[pi][hit]
					); 
				};

				if (config->action_asn == 1) {
					ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
						, "client IP ASN (from cache): %s"
						, ipv6calc_cache_lri_value_asn[pi][hit]
					);

					apr_table_set(r->subprocess_env
						, "IPV6CALC_CLIENT_ASN"
						, ipv6calc_cache_lri_value_asn[pi][hit]
					); 
				};

				if (config->action_registry == 1) {
					ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
						, "client IP Registry (from cache): %s"
						, ipv6calc_cache_lri_value_registry[pi][hit]
					);

					apr_table_set(r->subprocess_env
						, "IPV6CALC_CLIENT_REGISTRY"
						, ipv6calc_cache_lri_value_registry[pi][hit]
					); 
				};

				if (config->action_anonymize == 1) {
					ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
						, "client IP address anonymized (from cache): %s"
						, ipv6calc_cache_lri_value_anon[pi][hit]
					);

					apr_table_set(r->subprocess_env
						, "IPV6CALC_CLIENT_IP_ANON"
						, ipv6calc_cache_lri_value_anon[pi][hit]
					); 

					apr_table_set(r->subprocess_env, "IPV6CALC_ANON_METHOD", anon_method_name);
				};

				return OK;
			};

			// print cache statistics
			if (	config->cache_statistics_interval > 0
			    &&  ((ipv6calc_cache_lri_checked[pi] % config->cache_statistics_interval) == 0)
			) {
				for (i = 0; i < config->cache_limit; i++) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "cache hit statistics for IPv%s: distance %3d: %lu / %lu (%2lu%%)"
						, (pi == 0) ? "4" : "6"
						, i
						, ipv6calc_cache_lri_statistics[pi][i]
						, ipv6calc_cache_lri_checked[pi]
						, (ipv6calc_cache_lri_statistics[pi][i] * 100) / ipv6calc_cache_lri_checked[pi]
					);
				};
			};
		};
	};

	/* post cache lookup */
	if (pi == mod_ipv6calc_pi_IPV4) {
		// IPv4
		ipv4addr_clearall(&ipv4addr);

		if (p_mapped == 0) {
			ipv4addr.in_addr = client_addr_p->sa.sin.sin_addr;
		} else {
			ipv4addr.in_addr.s_addr = client_addr_p->sa.sin6.sin6_addr.s6_addr32[3];
		};
		ipv4addr_settype(&ipv4addr, 1);
		ipv4addr.flag_valid = 1;

		CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);
#if APR_HAVE_IPV6
	} else if (pi == mod_ipv6calc_pi_IPV6) {
		// IPv6
		ipv6addr_clearall(&ipv6addr);
		ipv6addr.in6_addr = client_addr_p->sa.sin6.sin6_addr;
		ipv6addr_settype(&ipv6addr, 1);
		ipv6addr.flag_valid = 1;

		CONVERT_IPV6ADDRP_IPADDR(&ipv6addr, ipaddr);
#endif
	};

	/* store address in cache */
	if (config->cache == 1) {
		if (ipv6calc_cache_lri_max[pi] < config->cache_limit) {
			ipv6calc_cache_lri_last[pi]++;
			ipv6calc_cache_lri_max[pi]++;
		} else {
			if (ipv6calc_cache_lri_last[pi] == config->cache_limit) {
				ipv6calc_cache_lri_last[pi] = 1;
			} else {
				ipv6calc_cache_lri_last[pi]++;
			};
		};

		if (config->debuglevel & IPV6CALC_DEBUG_CACHE_STORE) {
			ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
				, "store IPv%s address in cache on position: %d"
				, (pi == 0) ? "4" : "6"
				, ipv6calc_cache_lri_last[pi] - 1
			);
		};

		if (pi == mod_ipv6calc_pi_IPV4) {
			// store token
			if (p_mapped == 0) {
				ipv6calc_cache_lri_ipv4_token[ipv6calc_cache_lri_last[pi] - 1] = client_addr_p->sa.sin.sin_addr;
			} else {
				ipv6calc_cache_lri_ipv4_token[ipv6calc_cache_lri_last[pi] - 1].s_addr = client_addr_p->sa.sin6.sin6_addr.s6_addr32[3];
			};
#if APR_HAVE_IPV6
		} else if (pi == mod_ipv6calc_pi_IPV6) {
			// store token
			ipv6calc_cache_lri_ipv6_token[ipv6calc_cache_lri_last[pi] - 1] = client_addr_p->sa.sin6.sin6_addr;
#endif
		};
	};

	// retrieve data
	int result_cc = -1;
	int result_registry = -1;
	const char *data_source_string = "-";
	uint32_t asn_num = 0;

	if (	(config->action_countrycode == 1)
	     ||	(config->action_asn == 1)
	     ||	(config->action_registry == 1)
	) {
		int retrieve_cc = 0;
		int retrieve_asn = 0;
		int retrieve_registry = 0;

		if ((pi == mod_ipv6calc_pi_IPV6) && (ipv6addr.typeinfo & IPV6_ADDR_HAS_PUBLIC_IPV4)) {
			// extract IPv4 address and retrieve country code of that particular address
			result = libipv6addr_get_included_ipv4addr(&ipv6addr, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);

			if (result == 0) {
				if (ipv4addr.typeinfo & IPV4_ADDR_GLOBAL) {
					CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);

					// retrieve country code only for global addresses
					retrieve_cc = 1;

					// retrieve ASN only for global addresses
					retrieve_asn = 1;
				};

				retrieve_registry = 1;
			};
		} else if (	((pi == mod_ipv6calc_pi_IPV4) && (ipv4addr.typeinfo & IPV4_ADDR_GLOBAL))
			    ||	((pi == mod_ipv6calc_pi_IPV6) && (ipv6addr.typeinfo & IPV6_ADDR_GLOBAL))
		) {
			// retrieve country code only for global addresses
			retrieve_cc = 1;

			// retrieve ASN only for global addresses
			retrieve_asn = 1;

			retrieve_registry = 1;
		} else {
			retrieve_registry = 1;
		};

		if (config->debuglevel & IPV6CALC_DEBUG_RETRIEVE_DATA) {
			ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
				, "potential data retrieve_cc=%d retrieve_asn=%d retrieve_registry=%d"
				, retrieve_cc
				, retrieve_asn
				, retrieve_registry
			);
		};

		// set country code of IP in environment
		if (config->action_countrycode == 1) {
			if (retrieve_cc != 0) {
				result_cc = libipv6calc_db_wrapper_country_code_by_addr(cc, sizeof(cc), &ipaddr, &data_source);

				if ((result_cc == 0) && (strlen(cc) > 0)) {
					data_source_string = libipv6calc_db_wrapper_get_data_source_name_by_number(data_source);
				} else {
					snprintf(cc, sizeof(cc), "%s", "-");
				};

				ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
					, "client IP address country code: %s (%s)"
					, cc
					, data_source_string
				);
			} else {
				snprintf(cc, sizeof(cc), "%s", "-");
			};

			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_COUNTRYCODE", cc); 

			if (config->cache == 1) {
				// store value
				snprintf(ipv6calc_cache_lri_value_cc[pi][ipv6calc_cache_lri_last[pi] - 1], sizeof(ipv6calc_cache_lri_value_cc[pi][ipv6calc_cache_lri_last[pi] - 1]), "%s", cc);
				if (config->debuglevel & IPV6CALC_DEBUG_CACHE_STORE) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "store CountryCode of IPv%s address in cache on position: %d"
						, (pi == 0) ? "4" : "6"
						, ipv6calc_cache_lri_last[pi] - 1
					);
				};
			};
		};

		// set ASN of IP in environment
		if (config->action_asn == 1) {
			if (retrieve_asn != 0) {
				asn_num = libipv6calc_db_wrapper_as_num32_by_addr(&ipaddr);

				snprintf(asn, sizeof(asn), "%u", asn_num);

				ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
					, "client IP address ASN: %s"
					, asn
				);
			} else {
				snprintf(asn, sizeof(asn), "-");
			};

			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_ASN", asn); 

			if (config->cache == 1) {
				// store value
				snprintf(ipv6calc_cache_lri_value_asn[pi][ipv6calc_cache_lri_last[pi] - 1], sizeof(ipv6calc_cache_lri_value_asn[pi][ipv6calc_cache_lri_last[pi] - 1]), "%s", asn);
				if (config->debuglevel & IPV6CALC_DEBUG_CACHE_STORE) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "store ASN of IPv%s address in cache on position: %d"
						, (pi == 0) ? "4" : "6"
						, ipv6calc_cache_lri_last[pi] - 1
					);
				};
			};
		};

		// set Registry of IP in environment
		if (config->action_registry == 1) {
			if (retrieve_registry != 0) {
				result_registry = libipv6calc_db_wrapper_registry_string_by_ipaddr(&ipaddr, registry, sizeof(registry));

				if (((result_registry == 0) || (result_registry == 2)) && (strlen(registry) > 0)) {
					// everything ok
				} else {
					snprintf(registry, sizeof(registry), "%s", "-");
				};

				ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
					, "client IP address registry: %s"
					, registry
				);
			} else {
				snprintf(registry, sizeof(registry), "%s", "-");
			};

			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_REGISTRY", registry); 

			if (config->cache == 1) {
				// store value
				snprintf(ipv6calc_cache_lri_value_registry[pi][ipv6calc_cache_lri_last[pi] - 1], sizeof(ipv6calc_cache_lri_value_asn[pi][ipv6calc_cache_lri_last[pi] - 1]), "%s", registry);
				if (config->debuglevel & IPV6CALC_DEBUG_CACHE_STORE) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "store Registry of IPv%s address in cache on position: %d"
						, (pi == 0) ? "4" : "6"
						, ipv6calc_cache_lri_last[pi] - 1
					);
				};
			};
		};
	};

	// set special value if not enabled
	if (config->action_countrycode == 0) {
		apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_COUNTRYCODE", "disabled"); 
	};

	if (config->action_asn == 0) {
		apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_ASN", "disabled"); 
	};

	if (config->action_registry == 0)  {
		apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_REGISTRY", "disabled"); 
	};


	// set anonymized IP address in environment
	if (config->action_anonymize == 1) {
		if (pi == mod_ipv6calc_pi_IPV4) {
			libipv4addr_anonymize(&ipv4addr, config->ipv6calc_anon_set.mask_ipv4, config->ipv6calc_anon_set.method);
			CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);
#if APR_HAVE_IPV6
		} else if (pi == mod_ipv6calc_pi_IPV6) {
			if ((ipv6addr.typeinfo & IPV6_ADDR_MAPPED) == IPV6_ADDR_MAPPED) {
				libipv4addr_anonymize(&ipv4addr, config->ipv6calc_anon_set.mask_ipv4, config->ipv6calc_anon_set.method);
				CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);
			} else {
				libipv6addr_anonymize(&ipv6addr, &config->ipv6calc_anon_set);
				CONVERT_IPV6ADDRP_IPADDR(&ipv6addr, ipaddr);
			};
#endif
		};

		// get address string
		result = libipaddr_ipaddrstruct_to_string(&ipaddr, client_addr_string_anonymized, sizeof(client_addr_string_anonymized), 0); 

		char *result_anon_p;

		if (result == 0) {
			ap_log_rerror(APLOG_MARK, mod_ipv6calc_APLOG_DEBUG, 0, r
				, "client IP address anonymized: %s"
				, client_addr_string_anonymized
			);
			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", client_addr_string_anonymized);
			result_anon_p = client_addr_string_anonymized;

			apr_table_set(r->subprocess_env, "IPV6CALC_ANON_METHOD", anon_method_name);
		} else {
#if (((AP_SERVER_MAJORVERSION_NUMBER == 2) && (AP_SERVER_MINORVERSION_NUMBER >= 4)) || (AP_SERVER_MAJORVERSION_NUMBER > 2))
			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", r->connection->client_ip); 
			result_anon_p = r->connection->client_ip;
#else
			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", r->connection->remote_ip);
			result_anon_p = r->connection->remote_ip;
#endif
		};

		if (config->cache == 1) {
			// store value
			snprintf(ipv6calc_cache_lri_value_anon[pi][ipv6calc_cache_lri_last[pi] - 1], sizeof(ipv6calc_cache_lri_value_anon[pi][ipv6calc_cache_lri_last[pi] - 1]), "%s", result_anon_p);

			if (config->debuglevel & IPV6CALC_DEBUG_CACHE_STORE) {
				ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
					, "store anonymized IPv%s address in cache on position: %d"
					, (pi == 0) ? "4" : "6"
					, ipv6calc_cache_lri_last[pi] - 1
				);
			};
		};
	} else {
		apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", "disabled"); 
		apr_table_set(r->subprocess_env, "IPV6CALC_ANON_METHOD", "disabled");
	};

	return OK;
};


/***************************
 * Module config option handlers
 ***************************/

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
};


/*
 * set_ipv6calc_no_fallback
 */
static const char *set_ipv6calc_no_fallback(cmd_parms *cmd, void *dummy, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	
	if (!config) {
		return NULL;
	};
	
	config->no_fallback = arg;
	
	return NULL;
};


/*
 * set_ipv6calc_cache
 */
static const char *set_ipv6calc_cache(cmd_parms *cmd, void *dummy, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	
	if (!config) {
		return NULL;
	};
	
	config->cache = arg;
	
	return NULL;
};


/*
 * set_ipv6calc_cache_limit
 */
static const char *set_ipv6calc_cache_limit(cmd_parms *cmd, void *dummy, const char *value, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);

	if (!config) {
		return NULL;
	};

	if (atoi(value) < IPV6CALC_CACHE_LRI_LIMIT_MIN) {
		ap_log_error(APLOG_MARK, APLOG_WARNING, 0, cmd->server
			, "given cache limit below minimum (%d), skip: %s"
			, IPV6CALC_CACHE_LRI_LIMIT_MIN
			, value
		);

		return NULL;
	};

	if (atoi(value) > IPV6CALC_CACHE_LRI_SIZE) {
		ap_log_error(APLOG_MARK, APLOG_WARNING, 0, cmd->server
			, "given cache limit below maximum (%d), skip: %s"
			, IPV6CALC_CACHE_LRI_SIZE
			, value
		);

		return NULL;
	};

	ap_log_error(APLOG_MARK, APLOG_INFO, 0, cmd->server
		, "set cache limit: %s"
		, value
	);

	config->cache_limit = atoi(value);

	return NULL;
};


/*
 * set_ipv6calc_cache_statistics_interval
 */
static const char *set_ipv6calc_cache_statistics_interval(cmd_parms *cmd, void *dummy, const char *value, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);

	if (!config) {
		return NULL;
	};

	if (atoi(value) < 0) {
		ap_log_error(APLOG_MARK, APLOG_WARNING, 0, cmd->server
			, "given cache statistics interval below minimum (%d), skip: %s"
			, 0
			, value
		);

		return NULL;
	};

	ap_log_error(APLOG_MARK, APLOG_INFO, 0, cmd->server
		, "set cache statistics interval: %s"
		, value
	);

	config->cache_statistics_interval = atoi(value);

	return NULL;
};

/*
 * set_ipv6calc_debuglevel
 */
static const char *set_ipv6calc_debuglevel(cmd_parms *cmd, void *dummy, const char *value, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);

	if (!config) {
		return NULL;
	};

	long int debuglevel = strtol(value, NULL, 0);

	if (debuglevel < -1 || debuglevel > 0xffff) {
		ap_log_error(APLOG_MARK, APLOG_WARNING, 0, cmd->server
			, "given debug level is out-of-range (-1|0-65535), skip: %s"
			, value
		);
	} else {
		if (debuglevel == -1) {
			debuglevel = 0xffff;
		};

		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, cmd->server
			, "set module debug level: %s"
			, value
		);
	};

	config->debuglevel = (int) debuglevel;

	return NULL;
};


/*
 * set_ipv6calc_countrycode
 */
static const char *set_ipv6calc_action_countrycode(cmd_parms *cmd, void *dummy, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	
	if (!config) {
		return NULL;
	};
	
	config->action_countrycode = arg;
	
	return NULL;
};


/*
 * set_ipv6calc_asn
 */
static const char *set_ipv6calc_action_asn(cmd_parms *cmd, void *dummy, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	
	if (!config) {
		return NULL;
	};
	
	config->action_asn = arg;
	
	return NULL;
};


/*
 * set_ipv6calc_registry
 */
static const char *set_ipv6calc_action_registry(cmd_parms *cmd, void *dummy, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	
	if (!config) {
		return NULL;
	};
	
	config->action_registry = arg;
	
	return NULL;
};


/*
 * set_ipv6calc_anonymize
 */
static const char *set_ipv6calc_action_anonymize(cmd_parms *cmd, void *dummy, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	
	if (!config) {
		return NULL;
	};
	
	config->action_anonymize = arg;
	
	return NULL;
};


/*
 * set_ipv6calc_option
 *  set generic ipv6calc option
 */
static const char *set_ipv6calc_option(cmd_parms *cmd, void *dummy, const char *name, const char *value, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);

	if (!config) {
		return NULL;
	};

	ap_log_error(APLOG_MARK, APLOG_INFO, 0, cmd->server
		, "store ipv6calc config option for module initialization: %s=%s"
		, name
		, value
	);

	if (ipv6calc_option_list_entries < mod_ipv6calc_options_max) {
		snprintf(ipv6calc_option_list[ipv6calc_option_list_entries].name, sizeof(ipv6calc_option_list[0].name), "%s", name);
		snprintf(ipv6calc_option_list[ipv6calc_option_list_entries].value, sizeof(ipv6calc_option_list[0].value), "%s", value);
		ipv6calc_option_list_entries++;
	} else {
		return "Too many ipv6calc options (limit reached)";
	};

	return NULL;
};


/*
 * ipv6calc_create_svr_conf
 */
static void *ipv6calc_create_svr_conf(apr_pool_t* pool, server_rec* svr) {
	ipv6calc_server_config* svr_cfg = apr_pcalloc(pool, sizeof(ipv6calc_server_config));
	
	svr_cfg->enabled = 0;

	svr_cfg->no_fallback = 0;

	// cache settings
	svr_cfg->cache = 1; // default: on
	svr_cfg->cache_limit = IPV6CALC_CACHE_LRI_LIMIT_MIN; /* optimum ?? */
	svr_cfg->cache_statistics_interval = 0; // disabled

	svr_cfg->debuglevel = 0;

	svr_cfg->action_anonymize = 0;
	svr_cfg->action_countrycode = 0;

	libipv6calc_anon_set_by_name(&svr_cfg->ipv6calc_anon_set, "as"); // anonymize standard
	svr_cfg->anon_set_default = 1;

	return svr_cfg ;
}


/*
 * ipv6calc_register_hooks
 */
static void ipv6calc_register_hooks(apr_pool_t *p) {
	ap_hook_post_config(ipv6calc_post_config, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_child_init(ipv6calc_child_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_read_request(ipv6calc_post_read_request, NULL, NULL, APR_HOOK_MIDDLE);
};


/*
 * mod_ipv6calc API hooks
 */
#if (((AP_SERVER_MAJORVERSION_NUMBER == 2) && (AP_SERVER_MINORVERSION_NUMBER >= 4)) || (AP_SERVER_MAJORVERSION_NUMBER > 2))
AP_DECLARE_MODULE(ipv6calc) = {
#else
module AP_MODULE_DECLARE_DATA ipv6calc_module = {
#endif
	STANDARD20_MODULE_STUFF, 
	NULL,                        /* create per-dir    config structures */
	NULL,                        /* merge  per-dir    config structures */
	ipv6calc_create_svr_conf,    /* create per-server config structures */
	NULL,                        /* merge  per-server config structures */
	ipv6calc_cmds,               /* table of config file commands       */
	ipv6calc_register_hooks      /* register hooks                      */
};


/*
 * ipv6calc copyright (to satisfy dynamic library load)
 */
void printcopyright(void) {
	fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

/* 
 * ipv6calc version (to satisfy dynamic library load)
 */
void printversion(void) {
	char resultstring[NI_MAXHOST] = "";

	libipv6calc_db_wrapper_features(resultstring, sizeof(resultstring));

	fprintf(stderr, "%s: version %s", PROGRAM_NAME, PACKAGE_VERSION);

	fprintf(stderr, " %s", resultstring);

	if (feature_zeroize == 1) {
		fprintf(stderr, " ANON_ZEROISE");
	};

	if (feature_anon == 1) {
		fprintf(stderr, " ANON_ANONYMIZE");
	};

	if (feature_kp == 1) {
		fprintf(stderr, " ANON_KEEP-TYPE-ASN-CC");
	};

	fprintf(stderr, "\n");
};
