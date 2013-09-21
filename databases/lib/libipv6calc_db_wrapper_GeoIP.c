/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_GeoIP.c
 * Version    : $Id: libipv6calc_db_wrapper_GeoIP.c,v 1.19 2013/09/21 17:25:56 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc GeoIP database wrapper
 *    - decoupling databases from main binary
 *    - optional support of dynamic library loading (based on config.h)
 */

#include <stdio.h>
#include <dlfcn.h>

#include "config.h"

#include "libipv6calcdebug.h"

#include "libipv6calc_db_wrapper.h"

#ifdef SUPPORT_GEOIP

#include "libipv6calc_db_wrapper_GeoIP.h"

uint32_t wrapper_features_GeoIP = 0;

char geoip_db_dir[NI_MAXHOST] = GEOIP_DB;

#ifdef SUPPORT_GEOIP_DYN
char geoip_lib_file[NI_MAXHOST] = GEOIP_DYN_LIB;
static const char* wrapper_geoip_info = "dyn-load";
static int wrapper_geoip_ipv6_support = GEOIP_IPV6_SUPPORT_UNKNOWN;
static int wrapper_geoip_support      = GEOIP_SUPPORT_UNKNOWN;

/* define status and dynamic load functions */
static int dl_status_GeoIP_open = IPV6CALC_DL_STATUS_UNKNOWN;
static GeoIP* (*dl_GeoIP_open)(const char * filename, int flags) = NULL;

static int dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_UNKNOWN;
static GeoIP* (*dl_GeoIP_open_type)(int type, int flags) = NULL;

static int dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_UNKNOWN;
static int (*dl_GeoIP_db_avail)(int type) = NULL;

static int dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_UNKNOWN;
static unsigned char (*dl_GeoIP_database_edition)(GeoIP* gi) = NULL;

static int dl_status_GeoIP_delete = IPV6CALC_DL_STATUS_UNKNOWN;
static void (*dl_GeoIP_delete)(GeoIP* gi) = NULL;

static int dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_database_info)(GeoIP* gi) = NULL;

static int dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_country_code_by_addr)(GeoIP* gi, const char *addr) = NULL;

static int dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_country_name_by_addr)(GeoIP* gi, const char *addr) = NULL;

static int dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_name_by_addr)(GeoIP* gi, const char *addr) = NULL;

static int dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
static GeoIPRecord* (*dl_GeoIP_record_by_addr)(GeoIP* gi, const char *addr) = NULL;

static int dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_UNKNOWN;
static void (*dl_GeoIPRecord_delete)(GeoIPRecord *gir) = NULL;

static int dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_country_code_by_addr_v6)(GeoIP* gi, const char *addr) = NULL;

static int dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_country_name_by_addr_v6)(GeoIP* gi, const char *addr) = NULL;

static int dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_name_by_addr_v6)(GeoIP* gi, const char *addr) = NULL;

static int dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_lib_version)() = NULL;

static int dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_UNKNOWN;
static int (*dl_GeoIP_cleanup)() = NULL;

static int dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_UNKNOWN;
static void (*dl_GeoIP_setup_custom_directory)(char *dir) = NULL;

static const char **dl_GeoIPDBDescription = NULL;
static char ***dl_GeoIPDBFileName_ptr = NULL;

/* prototyping of feature check dl_load */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6 (void);
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6 (void);
static void libipv6calc_db_wrapper_dl_load_GeoIP_lib_version (void);

#ifndef SUPPORT_GEOIP_IPV6_STRUCT
// workaround in case of GeoIP.h is too old, but dynamic load should support IPv6
#include <netinet/in.h>
typedef struct in6_addr geoipv6_t;
#endif // SUPPORT_GEOIP_IPV6_STRUCT

static int dl_status_GeoIP_country_name_by_ipnum_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_country_name_by_ipnum_v6)(GeoIP* gi, geoipv6_t ipnum);

static int dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
static char* (*dl_GeoIP_country_code_by_ipnum_v6)(GeoIP* gi, geoipv6_t ipnum) = NULL;

#else // SUPPORT_GEOIP_DYN
static const char* wrapper_geoip_info = "built-in";
#if defined (SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6) && defined (SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6)
static int wrapper_geoip_ipv6_support = GEOIP_IPV6_SUPPORT_FULL;
#else
#ifdef SUPPORT_GEOIP_V6
static int wrapper_geoip_ipv6_support = GEOIP_IPV6_SUPPORT_COMPAT;
#endif
#endif
#endif // SUPPORT_GEOIP_DYN

static int geoip_country_v4 = 0;
static int geoip_country_v6 = 0;
static int geoip_asnum_v4 = 0;
static int geoip_asnum_v6 = 0;
static int geoip_city_v4 = 0;
static int geoip_city_v6 = 0;

static unsigned int geoip_num_db_types = 0;

static void *dl_GeoIP_handle = NULL;

char ***libipv6calc_db_wrapper_GeoIPDBFileName_ptr = NULL;
const char **libipv6calc_db_wrapper_GeoIPDBDescription = NULL;

/*
 * function initialise the GeoIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_GeoIP_wrapper_init(void) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_GEOIP_DYN

	char *error;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Load library: %s\n", __FILE__, __func__, geoip_lib_file);
	};

	dl_GeoIP_handle = dlopen(geoip_lib_file, RTLD_NOW | RTLD_LOCAL);

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "%s/%s: Loading of library failed: %s\n", __FILE__, __func__, geoip_lib_file);
		return(1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Loaded library successful: %s\n", __FILE__, __func__, geoip_lib_file);
	};

	libipv6calc_db_wrapper_GeoIP_cleanup();

	/* GeoIPDBDescription */
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, "GeoIPDBDescription");
	};
	dlerror();    /* Clear any existing error */
	*(void **) (&dl_GeoIPDBDescription) = dlsym(dl_GeoIP_handle, "GeoIPDBDescription");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		return(1);
	};
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, "GeoIPDBDescription");
	};
	libipv6calc_db_wrapper_GeoIPDBDescription = dl_GeoIPDBDescription;

#ifdef GEOIP_WORKAROUND_NUM_DB_TYPES
	// workaround to determine NUM_DB_TYPES until GeoIP API provides a function
	// UNTIL now, no workaround found :-(
	geoip_num_db_types = 0;
#else
	geoip_num_db_types = 0;
#endif

#else
	libipv6calc_db_wrapper_GeoIPDBDescription = GeoIPDBDescription;
	geoip_num_db_types = NUM_DB_TYPES;
#endif

	// <= 1.5.1 misses an API function for detecting NUM_DB_TYPES on dynamic load
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		int i;
		if (geoip_num_db_types > 0) {
			for (i = 0; i < geoip_num_db_types; i++) {
				fprintf(stderr, "%s/%s: GeoIPDBDescription Entry #%d: %s\n", __FILE__, __func__, i, libipv6calc_db_wrapper_GeoIPDBDescription[i]);
			};
		} else {
			fprintf(stderr, "%s/%s: GeoIPDBDescription Entries can't be displayed, number of entries can't be retrieved (missing support)\n", __FILE__, __func__);
		};
	};

#ifdef SUPPORT_GEOIP_DYN
	//int r = libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION); // dummy call to trigger _GeoIP_setup_dbfilename
	int r = libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION_V6); // dummy call to trigger _GeoIP_setup_dbfilename
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: result of dummy GeoIP_db_avail call: %d\n", __FILE__, __func__, r);
	};

	/* GeoIPDBFFileName */
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, "GeoIPDBFileName");
	};

	dlerror();    /* Clear any existing error */

	dl_GeoIPDBFileName_ptr = dlsym(dl_GeoIP_handle, "GeoIPDBFileName");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		return(1);
	};

	libipv6calc_db_wrapper_GeoIPDBFileName_ptr = dl_GeoIPDBFileName_ptr;

	/* check for IPv6 compat support */
	libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6();
	if (dl_status_GeoIP_country_code_by_ipnum_v6 == IPV6CALC_DL_STATUS_OK) {
		wrapper_geoip_ipv6_support = GEOIP_IPV6_SUPPORT_COMPAT;
	};

	/* check for IPv6 full support */
	libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6();
	if (dl_status_GeoIP_country_name_by_addr_v6 == IPV6CALC_DL_STATUS_OK) {
		wrapper_geoip_ipv6_support = GEOIP_IPV6_SUPPORT_FULL;
	};

	/* check for version */
	libipv6calc_db_wrapper_dl_load_GeoIP_lib_version();
	if (dl_status_GeoIP_lib_version == IPV6CALC_DL_STATUS_OK) {
		wrapper_geoip_support = GEOIP_SUPPORT_FULL;
	};

#else
	libipv6calc_db_wrapper_GeoIPDBFileName_ptr = &GeoIPDBFileName;
#endif

	if (libipv6calc_db_wrapper_GeoIPDBFileName_ptr == NULL) {
		fprintf(stderr, "%s/%s: libipv6calc_db_wrapper_GeoIPDBFileName_ptr == NULL (unexpected)\n", __FILE__, __func__);
		exit(1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Call libipv6calc_db_wrapper_GeoIP_setup_custom_directory\n", __FILE__, __func__);
	};

	libipv6calc_db_wrapper_GeoIP_setup_custom_directory(geoip_db_dir);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Returned from libipv6calc_db_wrapper_GeoIP_setup_custom_directory\n", __FILE__, __func__);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Check for standard GeoIP databases\n", __FILE__, __func__);
	};

	/* check required databases for resolution */
	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION) == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: GeoIP database GEOIP_COUNTRY_EDITION available\n", __FILE__, __func__);
		};
		geoip_country_v4 = 1;
		wrapper_features_GeoIP |= IPV6CALC_DB_IPV4_TO_CC;
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION_V6) == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: GeoIP database GEOIP_COUNTRY_EDITION_V6 available\n", __FILE__, __func__);
		};
		geoip_country_v6 = 1;
		wrapper_features_GeoIP |= IPV6CALC_DB_IPV6_TO_CC;
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_ASNUM_EDITION) == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: GeoIP database GEOIP_ASNUM_EDITION available\n", __FILE__, __func__);
		};
		geoip_asnum_v4 = 1;
		wrapper_features_GeoIP |= IPV6CALC_DB_IPV4_TO_AS;
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_ASNUM_EDITION_V6) == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: GeoIP database GEOIP_ASNUM_EDITION_V6 available\n", __FILE__, __func__);
		};
		geoip_asnum_v6 = 1;
		wrapper_features_GeoIP |= IPV6CALC_DB_IPV6_TO_AS;
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_CITY_EDITION_REV1) == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: GeoIP database GEOIP_CITY_EDITION_REV1 available\n", __FILE__, __func__);
		};
		geoip_city_v4 = 1;
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_CITY_EDITION_REV1_V6) == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: GeoIP database GEOIP_CITY_EDITION_REV1_V6 available\n", __FILE__, __func__);
		};
		geoip_city_v6 = 1;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Version of linked library: %s / IPv6 support: %s / custom directory: %s\n", __FILE__, __func__, libipv6calc_db_wrapper_GeoIP_lib_version(), libipv6calc_db_wrapper_GeoIP_IPv6_support[wrapper_geoip_ipv6_support].token, geoip_db_dir);
	};

	wrapper_features |= wrapper_features_GeoIP;

	return 0;
};


/*
 * function cleanup the GeoIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_GeoIP_wrapper_cleanup(void) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_GEOIP
	libipv6calc_db_wrapper_GeoIP_cleanup();
#endif

	dl_GeoIP_handle = NULL; // disable handle

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return 0;
};


/*
 * function info of GeoIP wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_GeoIP_wrapper_info(char* string, const size_t size) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_GEOIP
	snprintf(string, size, "GeoIP available databases: Country4=%d Country6=%d ASN4=%d ASN6=%d City4=%d City6=%d", geoip_country_v4, geoip_country_v6, geoip_asnum_v4, geoip_asnum_v6, geoip_city_v4, geoip_city_v6);
#else
	snprintf(string, size, "No GeoIP support built-in");
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return;
};

/*
 * function print database info of GeoIP wrapper
 *
 * in : (void)
 * out: (void)
 */
void libipv6calc_db_wrapper_GeoIP_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	GeoIP *gi;
	int i;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	printf("%sGeoIP: features: 0x%08x\n", prefix, wrapper_features_GeoIP);

#ifdef SUPPORT_GEOIP
	printf("%sGeoIP: info of available databases in directory: %s\n", prefix, geoip_db_dir);
	// TODO: replace hardcoded NUM_DB_TYPES by a function of GeoIP library
	for (i = 0; i < NUM_DB_TYPES; i++) {
		if (libipv6calc_db_wrapper_GeoIP_db_avail(i)) {
			// GeoIP returned that database is available
			gi = libipv6calc_db_wrapper_GeoIP_open_type(i, 0);
			if (gi == NULL) {
				if ((i == GEOIP_CITY_EDITION_REV0) || (i == GEOIP_CITY_EDITION_REV0_V6) || (i == GEOIP_LARGE_COUNTRY_EDITION) || (i == GEOIP_LARGE_COUNTRY_EDITION_V6))  {
					// silently skip REV0 and LARGE, if not existing
					continue;
				};
				printf("%sGeoIP: %-33s: %-40s (CAN'T OPEN)\n", prefix, libipv6calc_db_wrapper_GeoIPDBDescription[i], (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
			} else {
				printf("%sGeoIP: %-33s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_GeoIPDBDescription[i], (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i], libipv6calc_db_wrapper_GeoIP_database_info(gi));
				libipv6calc_db_wrapper_GeoIP_delete(gi);
			};
		} else {
			if (geoip_num_db_types > 0) {
				if ((*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i] == NULL) {
					continue;
				};

				if (level_verbose == LEVEL_VERBOSE2) {
					// printf("%sGeoIP: %-33s: %-40s (MISSING FILE)\n", prefix, libipv6calc_db_wrapper_GeoIPDBDescription[i], (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
				} else {
					continue;
				};
			 };
		};
	};
	if ((geoip_num_db_types == 0) && (level_verbose == LEVEL_VERBOSE2)) {
		printf("%sGeoIP: other possible databases can't be displayed, number of entries can't be retrieved (missing support)\n", prefix);
	};
#else
	snprintf(string, size, "%sNo GeoIP support built-in", prefix);
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return;
};


#ifdef SUPPORT_GEOIP

/*******************************
 * Wrapper functions for GeoIP
 *******************************/

/*
 * function cleanup the GeoIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_GeoIP_cleanup(void) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	int r = 1;

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_cleanup";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_cleanup == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_cleanup) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_cleanup == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	r = (int) (*dl_GeoIP_cleanup)();

END_libipv6calc_db_wrapper:
#else
	r = GeoIP_cleanup();
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return (r);
};



/*
 * wrapper: GeoIP_lib_version
 */
const char * libipv6calc_db_wrapper_GeoIP_lib_version(void) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char *result_GeoIP_lib_version = "unknown";
	char *error;
	libipv6calc_db_wrapper_dl_load_GeoIP_lib_version();

	if (dl_status_GeoIP_lib_version != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	result_GeoIP_lib_version = (*dl_GeoIP_lib_version)();

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_lib_version);
#else
#ifdef SUPPORT_GEOIP_LIB_VERSION
	return(GeoIP_lib_version());
#else
	return("unknown");
#endif
#endif
};


/*
 * wrapper: GeoIP_setup_custom_directory
 */
void libipv6calc_db_wrapper_GeoIP_setup_custom_directory(char *dir) {
	int r;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s with dir: %s\n", __FILE__, __func__, wrapper_geoip_info, dir);
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_setup_custom_directory";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_setup_custom_directory == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_setup_custom_directory) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_setup_custom_directory == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Call libipv6calc_db_wrapper_GeoIP_cleanup\n", __FILE__, __func__);
	};

	libipv6calc_db_wrapper_GeoIP_cleanup(); // free old stuff

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Returned from libipv6calc_db_wrapper_GeoIP_cleanup\n", __FILE__, __func__);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Call dl_GeoIP_setup_custom_directory\n", __FILE__, __func__);
	};
	(*dl_GeoIP_setup_custom_directory)(dir);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
#else
	libipv6calc_db_wrapper_GeoIP_cleanup(); // free old stuff

	GeoIP_setup_custom_directory(dir);
#endif
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Call libipv6calc_db_wrapper_GeoIP_db_avail (to trigger _GeoIP_setup_dbfilename)\n", __FILE__, __func__);
	};

	r = libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION); // dummy call to trigger _GeoIP_setup_dbfilename

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Returned from libipv6calc_db_wrapper_GeoIP_db_avail\n", __FILE__, __func__);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: result of dummy GeoIP_db_avail call: %d\n", __FILE__, __func__, r);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);

		if (geoip_num_db_types > 0) {
			int i;
			for (i = 0; i < geoip_num_db_types; i++) {
				fprintf(stderr, "%s/%s: GeoIPDBFileName Entry #%d: %s\n", __FILE__, __func__, i, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
			};
		} else {
			fprintf(stderr, "%s/%s: GeoIPDBDescription Entries can't be displayed, number of entries can't be retrieved (missing support)\n", __FILE__, __func__);
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished: %s with dir: %s\n", __FILE__, __func__, wrapper_geoip_info, dir);
	};
	return;
};


/*
 * wrapper: GeoIP_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_GeoIP_db_avail(int type) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s type=%d (desc: %s)\n", __FILE__, __func__, wrapper_geoip_info, type, libipv6calc_db_wrapper_GeoIPDBDescription[type]);
	};

#ifdef SUPPORT_GEOIP_DYN
	int result_GeoIP_db_avail = 0;
	const char *dl_symbol = "GeoIP_db_avail";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_db_avail == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_db_avail) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_db_avail == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_db_avail = (*dl_GeoIP_db_avail)(type);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Call to dl_GeoIP_db_avail results in: %d\n", __FILE__, __func__, result_GeoIP_db_avail);
	};

	if (result_GeoIP_db_avail == 0) {
		goto END_libipv6calc_db_wrapper;
	};

	dlerror();    /* Clear any existing error */
/*
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, "GeoIPDFilename");
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		if ((*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[type] == NULL) {
			fprintf(stderr, "%s/%s: Finished: %s type=%d (still unknown)\n", __FILE__, __func__, wrapper_geoip_info, type);
		} else {
			fprintf(stderr, "%s/%s: Finished: %s type=%d (file: %s)\n", __FILE__, __func__, wrapper_geoip_info, type, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[type]);
		};
	};
*/

END_libipv6calc_db_wrapper:
	return(result_GeoIP_db_avail);
#else
	int r = GeoIP_db_avail(type);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		if ((*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[type] == NULL) {
			fprintf(stderr, "%s/%s: Finished: %s type=%d (still unknown) (r=%d)\n", __FILE__, __func__, wrapper_geoip_info, type, r);
		} else {
			fprintf(stderr, "%s/%s: Finished: %s type=%d (%s) (r=%d)\n", __FILE__, __func__, wrapper_geoip_info, type, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[type], r);
		};
	};

	return(r);
#endif
};


/*
 * wrapper: GeoIP_open_type
 */
GeoIP* libipv6calc_db_wrapper_GeoIP_open_type(int type, int flags) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s type=%d (%s)\n", __FILE__, __func__, wrapper_geoip_info, type, libipv6calc_db_wrapper_GeoIPDBDescription[type]);
	};

#ifdef SUPPORT_GEOIP_DYN
	GeoIP *gi = NULL;
	const char *dl_symbol = "GeoIP_open_type";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_open_type == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_open_type) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_open_type == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	gi = (*dl_GeoIP_open_type)(type, flags);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(gi);
#else
	return(GeoIP_open_type(type, GEOIP_STANDARD));
#endif
};


/*
 * wrapper: GeoIP_open
 */
GeoIP* libipv6calc_db_wrapper_GeoIP_open(const char * filename, int flags) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s filename=%s\n", __FILE__, __func__, wrapper_geoip_info, filename);
	};

#ifdef SUPPORT_GEOIP_DYN
	GeoIP *gi = NULL;
	const char *dl_symbol = "GeoIP_open";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_open == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_open) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_open = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_open = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_open == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	gi = (*dl_GeoIP_open)(filename, flags);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(gi);
#else
	return(GeoIP_open(filename, GEOIP_STANDARD));
#endif
};


/*
 * wrapper: GeoIP_database_edition
 */
unsigned char libipv6calc_db_wrapper_GeoIP_database_edition (GeoIP* gi) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	unsigned char result_GeoIP_database_edition = '\0';
	const char *dl_symbol = "GeoIP_database_edition";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_database_edition == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_database_edition) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_database_edition == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_database_edition = (*dl_GeoIP_database_edition)(gi);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_database_edition);
#else
	return(GeoIP_database_edition(gi));
#endif
};


/*
 * wrapper: GeoIP_database_info
 */
char* libipv6calc_db_wrapper_GeoIP_database_info (GeoIP* gi) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_database_info = NULL;
	const char *dl_symbol = "GeoIP_database_info";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_database_info == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_database_info) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_database_info == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_database_info = (*dl_GeoIP_database_info)(gi);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_database_info);
#else
	return(GeoIP_database_info(gi));
#endif
};


/*
 * wrapper: GeoIP_delete
 */ 
void libipv6calc_db_wrapper_GeoIP_delete(GeoIP* gi) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_delete";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_delete == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_delete) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_delete = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_delete = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_delete == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	(*dl_GeoIP_delete)(gi);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
#else
	GeoIP_delete(gi);
#endif

	return;
};


/*
 * wrapper: GeoIP_country_code_by_addr
 */
const char* libipv6calc_db_wrapper_GeoIP_country_code_by_addr (GeoIP* gi, const char *addr) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_code_by_addr = NULL;
	const char *dl_symbol = "GeoIP_country_code_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_country_code_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_addr) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_code_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_country_code_by_addr = (*dl_GeoIP_country_code_by_addr)(gi, addr);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_code_by_addr);
#else
	return(GeoIP_country_code_by_addr(gi, addr));
#endif
};


/*
 * wrapper: GeoIP_country_name_by_addr
 */
const char* libipv6calc_db_wrapper_GeoIP_country_name_by_addr (GeoIP* gi, const char *addr) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_name_by_addr = NULL;
	const char *dl_symbol = "GeoIP_country_name_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_country_name_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_addr) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_name_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_country_name_by_addr = (*dl_GeoIP_country_name_by_addr)(gi, addr);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_name_by_addr);
#else
	return(GeoIP_country_name_by_addr(gi, addr));
#endif
};

/*
 * wrapper: GeoIP_record_by_addr
 */
GeoIPRecord*  libipv6calc_db_wrapper_GeoIP_record_by_addr (GeoIP* gi, const char *addr) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	GeoIPRecord* result_GeoIP_record_by_addr = NULL;
	const char *dl_symbol = "GeoIP_record_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_record_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_record_by_addr) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_record_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_record_by_addr = (*dl_GeoIP_record_by_addr)(gi, addr);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_record_by_addr);
#else
	return(GeoIP_record_by_addr(gi, addr));
#endif
};


/*
 * wrapper: GeoIPRecord_delete
 */
void libipv6calc_db_wrapper_GeoIPRecord_delete (GeoIPRecord *gir) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIPRecord_delete";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIPRecord_delete == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIPRecord_delete) = dlsym(dl_GeoIP_handle, "GeoIPRecord_delete");

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIPRecord_delete == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	(*dl_GeoIPRecord_delete)(gir);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return;
#else
	GeoIPRecord_delete(gir);
#endif
};


/*
 * wrapper: GeoIP_country_code_by_addr_v6
 */
const char *  libipv6calc_db_wrapper_GeoIP_country_code_by_addr_v6 (GeoIP* gi, const char *addr) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_code_by_addr_v6 = NULL;
	const char *dl_symbol = "GeoIP_country_code_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_country_code_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_addr_v6) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_code_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_country_code_by_addr_v6 = (*dl_GeoIP_country_code_by_addr_v6)(gi, addr);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_code_by_addr_v6);
#else
	return(GeoIP_country_code_by_addr_v6(gi, addr));
#endif
};


/*
 * wrapper: GeoIP_country_name_by_addr_v6
 */
const char *  libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6 (GeoIP* gi, const char *addr) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_name_by_addr_v6 = NULL;
	char *error;

	libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6();

	if (dl_status_GeoIP_country_name_by_addr_v6 != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	result_GeoIP_country_name_by_addr_v6 = (*dl_GeoIP_country_name_by_addr_v6)(gi, addr);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_name_by_addr_v6);
#else
	return(GeoIP_country_name_by_addr_v6(gi, addr));
#endif
};


/*
 * wrapper: GeoIP_country_name_by_ipnum_v6
 */
const char *  libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum_v6 (GeoIP* gi, geoipv6_t ipnum) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_name_by_ipnum_v6 = NULL;
	const char *dl_symbol = "GeoIP_country_name_by_ipnum_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_country_name_by_ipnum_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_ipnum_v6) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_ipnum_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_country_name_by_ipnum_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_name_by_ipnum_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_country_name_by_ipnum_v6 = (*dl_GeoIP_country_name_by_ipnum_v6)(gi, ipnum);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_name_by_ipnum_v6);
#else
	return(GeoIP_country_name_by_ipnum_v6(gi, ipnum));
#endif
};


/*
 * wrapper: GeoIP_country_code_by_ipnum_v6
 */
const char *  libipv6calc_db_wrapper_GeoIP_country_code_by_ipnum_v6 (GeoIP* gi, geoipv6_t ipnum) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_code_by_ipnum_v6 = NULL;
	char *error;
	libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6();

	if (dl_status_GeoIP_country_code_by_ipnum_v6 != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	result_GeoIP_country_code_by_ipnum_v6 = (*dl_GeoIP_country_code_by_ipnum_v6)(gi, ipnum);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_code_by_ipnum_v6);
#else
	return(GeoIP_country_code_by_ipnum_v6(gi, ipnum));
#endif
};


/*
 * wrapper: GeoIP_name_by_addr
 */
char* libipv6calc_db_wrapper_GeoIP_name_by_addr (GeoIP* gi, const char *addr) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_name_by_addr = NULL;
	const char *dl_symbol = "GeoIP_name_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_name_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_name_by_addr) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_name_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_name_by_addr = (*dl_GeoIP_name_by_addr)(gi, addr);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_name_by_addr);
#else
	return(GeoIP_name_by_addr(gi, addr));
#endif
};


/*
 * wrapper: GeoIP_name_by_addr_v6
 */
char* libipv6calc_db_wrapper_GeoIP_name_by_addr_v6 (GeoIP* gi, const char *addr) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_name_by_addr_v6 = NULL;
	const char *dl_symbol = "GeoIP_name_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_name_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_name_by_addr_v6) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_name_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_name_by_addr_v6 = (*dl_GeoIP_name_by_addr_v6)(gi, addr);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_name_by_addr_v6);
#else
	return(GeoIP_name_by_addr_v6(gi, addr));
#endif
};


/********************************************************
 * particular dynamic loader functions for feature checks
 *  avoiding duplicate code
 ********************************************************/
#ifdef SUPPORT_GEOIP_DYN
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6 (void) {
	const char *dl_symbol = "GeoIP_country_code_by_ipnum_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_code_by_ipnum_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_ipnum_v6) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};

	} else if (dl_status_GeoIP_country_code_by_ipnum_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};


static void libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6 (void) {
	const char *dl_symbol = "GeoIP_country_name_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_name_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_addr_v6) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_name_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};


static void libipv6calc_db_wrapper_dl_load_GeoIP_lib_version (void) {
	const char *dl_symbol = "GeoIP_lib_version";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "dl_GeoIP handle not defined\n");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_lib_version == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_lib_version) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_lib_version == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};
#endif


#endif  // GEOIP


/*********************************************
 * Abstract functions
 * *******************************************/

/* function query for feature set
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_GeoIP_has_features(uint32_t features) {
	int result = -1;
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called with feature value to test: 0x%08x\n", __FILE__, __func__, features);
	};

	if ((wrapper_features_GeoIP & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Return with result: %d\n", __FILE__, __func__, result);
	};
	return(result);
};

/* country_code */
const char * libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr (const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	const char *GeoIP_result_ptr;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called with addr=%s proto=%d\n", __FILE__, __func__, addr, proto);
	};

	if (proto == 4) {
		GeoIP_type = GEOIP_COUNTRY_EDITION;
	} else if (proto == 6) {
		GeoIP_type = GEOIP_COUNTRY_EDITION_V6;
	} else {
		return (NULL);
	};

	gi = libipv6calc_db_wrapper_GeoIP_open_type(GeoIP_type, 0);
	if (gi == NULL) {
		return (NULL);
	};

	if (proto == 4) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_code_by_addr(gi, addr);
	} else if (proto == 6) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_code_by_addr_v6(gi, addr);
	};

	if (GeoIP_result_ptr == NULL) {
		return (NULL);
	};

	if (strlen(GeoIP_result_ptr) > 2) {
		return (NULL);
	};

	libipv6calc_db_wrapper_GeoIP_delete(gi);

	return(GeoIP_result_ptr);
};

/* asnum */
char * libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr (const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	char *GeoIP_result_ptr;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called with addr=%s proto=%d\n", __FILE__, __func__, addr, proto);
	};

	if (proto == 4) {
		GeoIP_type = GEOIP_ASNUM_EDITION;
	} else if (proto == 6) {
		GeoIP_type = GEOIP_ASNUM_EDITION_V6;
	} else {
		return (NULL);
	};

	gi = libipv6calc_db_wrapper_GeoIP_open_type(GeoIP_type, 0);
	if (gi == NULL) {
		return (NULL);
	};

	if (proto == 4) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_name_by_addr(gi, addr);
	} else if (proto == 6) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_name_by_addr_v6(gi, addr);
	};

	if (GeoIP_result_ptr == NULL) {
		return (NULL);
	};

	libipv6calc_db_wrapper_GeoIP_delete(gi);

	return(GeoIP_result_ptr);
};

#endif
