/*
 * Project    : ipv6calc/mod_ipv6calc
 * File       : mod_ipv6calc.c
 * Version    : $Id: mod_ipv6calc.c,v 1.10 2015/05/30 08:05:12 ds6peter Exp $
 * Copyright  : 2015-2015 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc Apache module
 *
 *  Currently supporting:
 *   - client IP address anonymization by setting environment IPV6CALC_CLIENT_IP_ANON
 *   - client IP address country code retrievement by setting environment IPV6CALC_CLIENT_COUNTRYCODE
 *
 *  mode_ipv6calc behavior can be controlled by config, e.g.
 *   ipv6calcActionAnonymize		on
 *   ipv6calcActionCountrycode		on
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

/* options (only used via the option parser) */
struct option longopts[IPV6CALC_MAXLONGOPTIONS];
char   shortopts[NI_MAXHOST] = "";
int    longopts_maxentries = 0;

long int ipv6calc_debug = 0; // ipv6calc_debug usage


/***************************
 * Prototyping
 ***************************/
static const char *set_ipv6calc_enable(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_action_anonymize(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_action_countrycode(cmd_parms *cmd, void *dummy, int arg);
static const char *set_ipv6calc_option(cmd_parms *cmd, void *dummy, const char *name, const char *value, int arg);


/***************************
 * Cache (Last Recently Inserted)
 ***************************/
#define IPV6CALC_CACHE_LRI_SIZE 200

int		ipv6calc_cache_lri_limit = 20; /* optimum ?? */
int		ipv6calc_cache_lri_enable = 1;
long int        ipv6calc_cache_lri_statistics_interval = 10;

static long int ipv6calc_cache_lri_checked[2] = {0, 0};

static int      ipv6calc_cache_lri_max[2] = {0, 0};
static int      ipv6calc_cache_lri_last[2] = {0, 0 };
static long int ipv6calc_cache_lri_statistics[2][IPV6CALC_CACHE_LRI_SIZE];

static char     ipv6calc_cache_lri_value_anon[2][IPV6CALC_CACHE_LRI_SIZE][APRMAXHOSTLEN];
static char     ipv6calc_cache_lri_value_cc[2][IPV6CALC_CACHE_LRI_SIZE][APRMAXHOSTLEN];

static struct in_addr  ipv6calc_cache_lri_ipv4_token[IPV6CALC_CACHE_LRI_SIZE];
#if APR_HAVE_IPV6
static struct in6_addr ipv6calc_cache_lri_ipv6_token[IPV6CALC_CACHE_LRI_SIZE];
#endif

//#define IPV6CALC_CACHE_DEBUG 1


/***************************
 * Definitions
 ***************************/

/* define module name */
module AP_MODULE_DECLARE_DATA ipv6calc_module;


/* define config structure */
typedef struct {
	int enabled;
	int action_anonymize;
	int action_countrycode;
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


/*
 * Config options (ipv6calc_cmds)
 */
static const command_rec ipv6calc_cmds[] = {
	AP_INIT_FLAG("ipv6calcEnable"     , set_ipv6calc_enable   , NULL, OR_FILEINFO, "Turn on mod_ipv6calc"),
	AP_INIT_FLAG("ipv6calcActionAnonymize"  , set_ipv6calc_action_anonymize, NULL, OR_FILEINFO, "Store anonymized IP address in IPV6CALC_CLIENT_IP_ANON"),
	AP_INIT_FLAG("ipv6calcActionCountrycode", set_ipv6calc_action_countrycode, NULL, OR_FILEINFO, "Store Country Code of IP address in IPV6CALC_CLIENT_COUNTRYCODE"),
	AP_INIT_TAKE2("ipv6calcOption",  (const char *(*)()) set_ipv6calc_option, NULL, OR_FILEINFO, "Define ipv6calc option: <key> <value>"),
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
};


/*
 * ipv6calc_child_init
 */
static void ipv6calc_child_init(apr_pool_t *p, server_rec *s) {
	char string[NI_MAXHOST] = "";
	int result, i;

#ifdef SHARED_LIBRARY
	IPV6CALC_LIB_VERSION_CHECK_EXIT(IPV6CALC_PACKAGE_VERSION_NUMERIC, IPV6CALC_PACKAGE_VERSION_STRING)
	IPV6CALC_DB_LIB_VERSION_CHECK_EXIT(IPV6CALC_PACKAGE_VERSION_NUMERIC, IPV6CALC_PACKAGE_VERSION_STRING)
#endif // SHARED_LIBRARY

	apr_pool_cleanup_register(p, NULL, ipv6calc_cleanup, ipv6calc_cleanup);

	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(s->module_config, &ipv6calc_module);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "ipv6calc_child_init"
	);

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "start option handling"
	);

	/* add options */
	ipv6calc_options_add_common_anon(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
	ipv6calc_options_add_common_basic(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);

	ipv6calc_quiet = 1; // be quiet by default

	/* initialize ipv6calc options from list retrieved via APR config parser */
	if (ipv6calc_option_list_entries > 0) {
		for (i = 0; i < ipv6calc_option_list_entries; i++) {
			result = ipv6calc_set_option(longopts, ipv6calc_option_list[i].name, ipv6calc_option_list[i].value, &config->ipv6calc_anon_set);

			ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
				, "ipv6calc option %s: %s=%s"
				, (result == 0) ? "successfully set" : "NOT UNDERSTOOD"
				, ipv6calc_option_list[i].name
				, ipv6calc_option_list[i].value
			);

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

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "start ipv6calc database wrapper init"
	);

	result = libipv6calc_db_wrapper_init("");
	if (result != 0) {
		ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
			, "database wrapper initialization failed (disable module now): %d"
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
		, "features: %s"
		, string
	);

	string[0] = '\0';
	libipv6calc_db_wrapper_capabilities(string, sizeof(string));
	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "capabilities: %s"
		, string
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

	//libipv6calc_db_wrapper_print_db_info(0, "");
	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s
		, "configured module actions: anonymize=%s countrycode=%s"
		, (config->action_anonymize > 0) ? "ON" : "OFF"
		, (config->action_countrycode > 0) ? "ON" : "OFF"
	);

	return;
};


/*
 * ipv6calc_post_read_request
 */
static int ipv6calc_post_read_request(request_rec *r) {
	int i, hit;
	int pi; // proto index (0:IPv4, 1:IPv6

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
	unsigned int data_source;

	int result;

	// *** workflow
	// get config	
	config = (ipv6calc_server_config*) ap_get_module_config(r->server->module_config, &ipv6calc_module);

	// check enabled	
	if (!config->enabled) {
		return OK;
	};

	// get client address (aka REMOTE_IP)
#if (((AP_SERVER_MAJORVERSION_NUMBER == 2) && (AP_SERVER_MINORVERSION_NUMBER >= 4)) || (AP_SERVER_MAJORVERSION_NUMBER > 2))
	client_addr_p = r->connection->client_addr;
#else
	client_addr_p = r->connection->remote_addr;
#endif

	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r
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
		pi = 0;
#if APR_HAVE_IPV6
	} else if (client_addr_p->family == APR_INET6) {
		// IPv6
		pi = 1;
#endif
	} else {
		// unsupported family, do nothing
		return OK;
	};

	/* cache lookup */
	if (ipv6calc_cache_lri_enable == 1) {
		hit = -1;

#define IPV6CALC_COMPARE(entry) \
			     (    client_addr_p->family == APR_INET \
			       && ipv6calc_cache_lri_ipv4_token[entry].s_addr == client_addr_p->sa.sin.sin_addr.s_addr \
			     ) \
			  || (    client_addr_p->family == APR_INET6 \
			       && ipv6calc_cache_lri_ipv6_token[entry].s6_addr32[3] == client_addr_p->sa.sin6.sin6_addr.s6_addr32[3] \
			       && ipv6calc_cache_lri_ipv6_token[entry].s6_addr32[2] == client_addr_p->sa.sin6.sin6_addr.s6_addr32[2] \
			       && ipv6calc_cache_lri_ipv6_token[entry].s6_addr32[1] == client_addr_p->sa.sin6.sin6_addr.s6_addr32[1] \
			       && ipv6calc_cache_lri_ipv6_token[entry].s6_addr32[0] == client_addr_p->sa.sin6.sin6_addr.s6_addr32[0] \
			     )


		if (ipv6calc_cache_lri_max[pi] > 0) {
			ipv6calc_cache_lri_checked[pi]++;

#ifdef IPV6CALC_CACHE_DEBUG
			/* debugging */
			if (client_addr_p->family == APR_INET) {
				ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
					, "IPv4 address to lookup   : %08x"
					, client_addr_p->sa.sin.sin_addr.s_addr
				);
			} else if (client_addr_p->family == APR_INET6) {
				ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
					, "IPv6 address to lookup   : %08x %08x %08x %08x"
					, client_addr_p->sa.sin6.sin6_addr.s6_addr32[0]
					, client_addr_p->sa.sin6.sin6_addr.s6_addr32[1]
					, client_addr_p->sa.sin6.sin6_addr.s6_addr32[2]
					, client_addr_p->sa.sin6.sin6_addr.s6_addr32[3]
				);
			};

			/* cache entries */
			for (i = 0; i < ipv6calc_cache_lri_max[pi]; i++) {
				if (client_addr_p->family == APR_INET) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "IPv4 address in cache %3d: %08x"
						, i
						, ipv6calc_cache_lri_ipv4_token[i].s_addr
					);
				} else if (client_addr_p->family == APR_INET6) {
					ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
						, "IPv6 address in cache %3d: %08x %08x %08x %08x"
						, i
						, ipv6calc_cache_lri_ipv6_token[i].s6_addr32[0]
						, ipv6calc_cache_lri_ipv6_token[i].s6_addr32[1]
						, ipv6calc_cache_lri_ipv6_token[i].s6_addr32[2]
						, ipv6calc_cache_lri_ipv6_token[i].s6_addr32[3]
					);
				};
			};
#endif // IPV6CALC_CACHE_DEBUG

#ifdef IPV6CALC_CACHE_DEBUG
			ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
				, "check for IPv%s address in cache on last inserted entry: %d"
				, (pi == 0) ? "4" : "6"
				, ipv6calc_cache_lri_last[pi] - 1
			);
#endif // IPV6CALC_CACHE_DEBUG

			/* check last seen one first */
			if (IPV6CALC_COMPARE(ipv6calc_cache_lri_last[pi] - 1)) {
				ipv6calc_cache_lri_statistics[pi][0]++;
				hit = ipv6calc_cache_lri_last[pi] - 1;
			};

			/* run backwards to first entry */
			if ((hit < 0) && (ipv6calc_cache_lri_last[pi] > 1)) {
#ifdef IPV6CALC_CACHE_DEBUG
				ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
					, "check for IPv%s address in cache backwards to first entry"
					, (pi == 0) ? "4" : "6"
				);
#endif // IPV6CALC_CACHE_DEBUG

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
#ifdef IPV6CALC_CACHE_DEBUG
				ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
					, "check for IPv%s address in cache round robin from last entry"
					, (pi == 0) ? "4" : "6"
				);
#endif // IPV6CALC_CACHE_DEBUG

				for (i = ipv6calc_cache_lri_max[pi]; i > ipv6calc_cache_lri_last[pi]; i--) {
					if (IPV6CALC_COMPARE(i - 1)) {
						ipv6calc_cache_lri_statistics[pi][ipv6calc_cache_lri_max[pi] - i + ipv6calc_cache_lri_last[pi]]++;
						hit = i - 1;
						break;
					};
				};
			};

			if (hit >= 0) {
				ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r
					, "retrieve data of IPv%s address from cache position: %d"
					, (pi == 0) ? "4" : "6"
					, hit
				);

				if (config->action_countrycode == 1) {
					ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r
						, "client IP country code (from cache): %s"
						, ipv6calc_cache_lri_value_cc[pi][hit]
					);

					apr_table_set(r->subprocess_env
						, "IPV6CALC_CLIENT_COUNTRYCODE"
						, ipv6calc_cache_lri_value_cc[pi][hit]
					); 
				};

				if (config->action_anonymize == 1) {
					ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r
						, "client IP address anonymized (from cache): %s"
						, ipv6calc_cache_lri_value_anon[pi][hit]
					);

					apr_table_set(r->subprocess_env
						, "IPV6CALC_CLIENT_IP_ANON"
						, ipv6calc_cache_lri_value_anon[pi][hit]
					); 
				};

				return OK;
			};

			// print cache statistics
			if ((ipv6calc_cache_lri_checked[pi] % ipv6calc_cache_lri_statistics_interval) == 0) {
				for (i = 0; i < ipv6calc_cache_lri_limit; i++) {
					ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r
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
	if (client_addr_p->family == APR_INET) {
		// IPv4
		ipv4addr_clearall(&ipv4addr);
		ipv4addr.in_addr = client_addr_p->sa.sin.sin_addr;
		ipv4addr.scope = ipv4addr_gettype(&ipv4addr);
		ipv4addr.flag_valid = 1;

		CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);
#if APR_HAVE_IPV6
	} else if (client_addr_p->family == APR_INET6) {
		// IPv6
		ipv6addr_clearall(&ipv6addr);
		ipv6addr.in6_addr = client_addr_p->sa.sin6.sin6_addr;
		ipv6addr.scope = ipv6addr_gettype(&ipv6addr);
		ipv6addr.flag_valid = 1;

		if ((ipv6addr.scope & IPV6_ADDR_MAPPED) == IPV6_ADDR_MAPPED) {
			// IPv4 address stored as mapped in IPv6 address
			ipv4addr_clearall(&ipv4addr);

			result = libipv6addr_get_included_ipv4addr(&ipv6addr, &ipv4addr, 0);

			ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r
				, "mapped IPv4 address found in IPv6 address, status of extract: %d"
				, result
			);

			CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);
		} else {
			CONVERT_IPV6ADDRP_IPADDR(&ipv6addr, ipaddr);
		};
#endif
	};

	/* store in cache */
	if (ipv6calc_cache_lri_enable == 1) {
		if (ipv6calc_cache_lri_max[pi] < ipv6calc_cache_lri_limit) {
			ipv6calc_cache_lri_last[pi]++;
			ipv6calc_cache_lri_max[pi]++;
		} else {
			if (ipv6calc_cache_lri_last[pi] == ipv6calc_cache_lri_limit) {
				ipv6calc_cache_lri_last[pi] = 1;
			} else {
				ipv6calc_cache_lri_last[pi]++;
			};
		};

#ifdef IPV6CALC_CACHE_DEBUG
		ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
			, "store IPv%s address in cache on position: %d"
			, (pi == 0) ? "4" : "6"
			, ipv6calc_cache_lri_last[pi] - 1
		);
#endif // IPV6CALC_CACHE_DEBUG

		if (client_addr_p->family == APR_INET) {
			// store token
			ipv6calc_cache_lri_ipv4_token[ipv6calc_cache_lri_last[pi] - 1] = client_addr_p->sa.sin.sin_addr;
#if APR_HAVE_IPV6
		} else if (client_addr_p->family == APR_INET6) {
			// store token
			ipv6calc_cache_lri_ipv6_token[ipv6calc_cache_lri_last[pi] - 1] = client_addr_p->sa.sin6.sin6_addr;
#endif
		};
	};


	// set country code of IP in environment
	if (config->action_countrycode == 1) {
		result = libipv6calc_db_wrapper_country_code_by_addr(cc, sizeof(cc), &ipaddr, &data_source);

		if ((result == 0) && (strlen(cc) > 0)) {
			ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r
				, "client IP country code: %s (%s)"
				, cc
				, libipv6calc_db_wrapper_get_data_source_name_by_number(data_source)
			);
		} else {
			snprintf(cc, sizeof(cc), "%s", "");
		};

		apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_COUNTRYCODE", cc); 

		if (ipv6calc_cache_lri_enable == 1) {
			// store value
			snprintf(ipv6calc_cache_lri_value_cc[pi][ipv6calc_cache_lri_last[pi] - 1], sizeof(ipv6calc_cache_lri_value_cc[pi][ipv6calc_cache_lri_last[pi] - 1]), "%s", cc);
#ifdef IPV6CALC_CACHE_DEBUG
			ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
				, "store CountryCode of IPv%s address in cache on position: %d"
				, (pi == 0) ? "4" : "6"
				, ipv6calc_cache_lri_last[pi] - 1
			);
#endif // IPV6CALC_CACHE_DEBUG
		};
	};

	// set anonymized IP address in environment
	if (config->action_anonymize == 1) {
		if (client_addr_p->family == APR_INET) {
			libipv4addr_anonymize(&ipv4addr, config->ipv6calc_anon_set.mask_ipv4, config->ipv6calc_anon_set.method);
			CONVERT_IPV4ADDRP_IPADDR(&ipv4addr, ipaddr);
#if APR_HAVE_IPV6
		} else if (client_addr_p->family == APR_INET6) {
			if ((ipv6addr.scope & IPV6_ADDR_MAPPED) == IPV6_ADDR_MAPPED) {
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
			ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r
				, "client IP address anonymized: %s"
				, client_addr_string_anonymized
			);
			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", client_addr_string_anonymized);
			result_anon_p = client_addr_string_anonymized;
		} else {
#if (((AP_SERVER_MAJORVERSION_NUMBER == 2) && (AP_SERVER_MINORVERSION_NUMBER >= 4)) || (AP_SERVER_MAJORVERSION_NUMBER > 2))
			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", r->connection->client_ip); 
			result_anon_p = r->connection->client_ip;
#else
			apr_table_set(r->subprocess_env, "IPV6CALC_CLIENT_IP_ANON", r->connection->remote_ip);
			result_anon_p = r->connection->remote_ip;
#endif
		};

		if (ipv6calc_cache_lri_enable == 1) {
			// store value
			snprintf(ipv6calc_cache_lri_value_anon[pi][ipv6calc_cache_lri_last[pi] - 1], sizeof(ipv6calc_cache_lri_value_anon[pi][ipv6calc_cache_lri_last[pi] - 1]), "%s", result_anon_p);

#ifdef IPV6CALC_CACHE_DEBUG
			ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r
				, "store anonymized IPv%s address in cache on position: %d"
				, (pi == 0) ? "4" : "6"
				, ipv6calc_cache_lri_last[pi] - 1
			);
#endif // IPV6CALC_CACHE_DEBUG
		};
	};

	return OK;
};


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
 * set_ipv6calc_countrycode
 */
static const char *set_ipv6calc_action_countrycode(cmd_parms *cmd, void *dummy, int arg) {
	ipv6calc_server_config *config = (ipv6calc_server_config*) ap_get_module_config(cmd->server->module_config, &ipv6calc_module);
	
	if (!config) {
		return NULL;
	};
	
	config->action_countrycode = arg;
	
	return NULL;
}

;
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

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, cmd->server
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

	libipv6calc_anon_set_by_name(&svr_cfg->ipv6calc_anon_set, "as"); // anonymize standard
	svr_cfg->anon_set_default = 1;

	return svr_cfg ;
}


/*
 * ipv6calc_register_hooks
 */
static void ipv6calc_register_hooks(apr_pool_t *p) {
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
