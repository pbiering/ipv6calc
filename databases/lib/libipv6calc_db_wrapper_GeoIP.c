/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_GeoIP.c
 * Version    : $Id: libipv6calc_db_wrapper_GeoIP.c,v 1.50 2013/11/05 06:32:16 ds6peter Exp $
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
#include "libipv6addr.h"

#include "libipv6calc_db_wrapper.h"

#ifdef SUPPORT_GEOIP
#include "libipv6calc_db_wrapper_GeoIP.h"

#define GEOIP_WORKAROUND_NUM_DB_TYPES 1

uint32_t wrapper_features_GeoIP = 0;
uint32_t lib_features_GeoIP = 0;

char geoip_db_dir[NI_MAXHOST] = GEOIP_DB;

#ifdef SUPPORT_GEOIP_DYN
char geoip_lib_file[NI_MAXHOST] = GEOIP_DYN_LIB;
static const char* wrapper_geoip_info = "dyn-load";

/* define status and dynamic load functions */
static int dl_status_GeoIP_open = IPV6CALC_DL_STATUS_UNKNOWN;
//static GeoIP* (*dl_GeoIP_open)(const char * filename, int flags) = NULL;
typedef GeoIP *(*dl_GeoIP_open_t)(const char * filename, int flags);
static union { dl_GeoIP_open_t func; void * obj; } dl_GeoIP_open;

static int dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_UNKNOWN;
//static GeoIP* (*dl_GeoIP_open_type)(int type, int flags) = NULL;
typedef GeoIP *(*dl_GeoIP_open_type_t)(int type, int flags);
static union { dl_GeoIP_open_type_t func; void * obj; } dl_GeoIP_open_type;

static int dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_UNKNOWN;
//static int (*dl_GeoIP_db_avail)(int type) = NULL;
typedef int (*dl_GeoIP_db_avail_t)(int type);
static union { dl_GeoIP_db_avail_t func; void * obj; } dl_GeoIP_db_avail;

static int dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_UNKNOWN;
//static unsigned char (*dl_GeoIP_database_edition)(GeoIP* gi) = NULL;
typedef unsigned char (*dl_GeoIP_database_edition_t)(GeoIP* gi);
static union { dl_GeoIP_database_edition_t func; void * obj; } dl_GeoIP_database_edition;

static int dl_status_GeoIP_delete = IPV6CALC_DL_STATUS_UNKNOWN;
//static void (*dl_GeoIP_delete)(GeoIP* gi) = NULL;
typedef void (*dl_GeoIP_delete_t)(GeoIP* gi);
static union { dl_GeoIP_delete_t func; void * obj; } dl_GeoIP_delete;

static int dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_database_info)(GeoIP* gi) = NULL;
typedef char *(*dl_GeoIP_database_info_t)(GeoIP* gi);
static union { dl_GeoIP_database_info_t func; void * obj; } dl_GeoIP_database_info;

static int dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_country_code_by_addr)(GeoIP* gi, const char *addr) = NULL;
typedef char *(*dl_GeoIP_country_code_by_addr_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_country_code_by_addr_t func; void * obj; } dl_GeoIP_country_code_by_addr;

static int dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_country_name_by_addr)(GeoIP* gi, const char *addr) = NULL;
typedef char *(*dl_GeoIP_country_name_by_addr_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_country_name_by_addr_t func; void * obj; } dl_GeoIP_country_name_by_addr;

static int dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_name_by_addr)(GeoIP* gi, const char *addr) = NULL;
typedef char *(*dl_GeoIP_name_by_addr_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_name_by_addr_t func; void * obj; } dl_GeoIP_name_by_addr;

static int dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
//static GeoIPRecord* (*dl_GeoIP_record_by_addr)(GeoIP* gi, const char *addr) = NULL;
typedef GeoIPRecord *(*dl_GeoIP_record_by_addr_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_record_by_addr_t func; void * obj; } dl_GeoIP_record_by_addr;

static int dl_status_GeoIP_record_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
typedef GeoIPRecord *(*dl_GeoIP_record_by_addr_v6_t)(GeoIP *gi, const char *addr);
static union { dl_GeoIP_record_by_addr_v6_t func; void * obj; } dl_GeoIP_record_by_addr_v6;

static int dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_UNKNOWN;
//static void (*dl_GeoIPRecord_delete)(GeoIPRecord *gir) = NULL;
typedef void (*dl_GeoIPRecord_delete_t)(GeoIPRecord *gir);
static union { dl_GeoIPRecord_delete_t func; void * obj; } dl_GeoIPRecord_delete;

static int dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_country_code_by_addr_v6)(GeoIP* gi, const char *addr) = NULL;
typedef char *(*dl_GeoIP_country_code_by_addr_v6_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_country_code_by_addr_v6_t func; void * obj; } dl_GeoIP_country_code_by_addr_v6;

static int dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_country_name_by_addr_v6)(GeoIP* gi, const char *addr) = NULL;
typedef char *(*dl_GeoIP_country_name_by_addr_v6_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_country_name_by_addr_v6_t func; void * obj; } dl_GeoIP_country_name_by_addr_v6;

static int dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_name_by_addr_v6)(GeoIP* gi, const char *addr) = NULL;
typedef char *(*dl_GeoIP_name_by_addr_v6_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_name_by_addr_v6_t func; void * obj; } dl_GeoIP_name_by_addr_v6;

static int dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_lib_version)() = NULL;
typedef char *(*dl_GeoIP_lib_version_t)(void);
static union { dl_GeoIP_lib_version_t func; void * obj; } dl_GeoIP_lib_version;

static int dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_UNKNOWN;
//static int (*dl_GeoIP_cleanup)() = NULL;
typedef int (*dl_GeoIP_cleanup_t)(void);
static union { dl_GeoIP_cleanup_t func; void * obj; } dl_GeoIP_cleanup;

static int dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_UNKNOWN;
//static void (*dl_GeoIP_setup_custom_directory)(char *dir) = NULL;
typedef void (*dl_GeoIP_setup_custom_director_t)(char *dir);
static union { dl_GeoIP_setup_custom_director_t func; void * obj; } dl_GeoIP_setup_custom_directory;

//static const char **dl_GeoIPDBDescription = NULL;
typedef const char **(dl_GeoIPDBDescription_t);
static union { dl_GeoIPDBDescription_t val; void * obj; } dl_GeoIPDBDescription;

static char ***dl_GeoIPDBFileName_ptr = NULL;

/* prototyping of feature check dl_load */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6 (void);
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_ipnum_v6 (void);
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_addr_v6 (void);
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6 (void);
static void libipv6calc_db_wrapper_dl_load_GeoIP_lib_version (void);

#ifndef SUPPORT_GEOIP_IPV6_STRUCT
// workaround in case of GeoIP.h is too old, but dynamic load should support IPv6
#include <netinet/in.h>
typedef struct in6_addr geoipv6_t;
#endif // SUPPORT_GEOIP_IPV6_STRUCT

static int dl_status_GeoIP_country_name_by_ipnum_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_country_name_by_ipnum_v6)(GeoIP* gi, geoipv6_t ipnum);
typedef char *(*dl_GeoIP_country_name_by_ipnum_v6_t)(GeoIP* gi, geoipv6_t ipnum);
static union { dl_GeoIP_country_name_by_ipnum_v6_t func; void * obj; } dl_GeoIP_country_name_by_ipnum_v6;

static int dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
//static char* (*dl_GeoIP_country_code_by_ipnum_v6)(GeoIP* gi, geoipv6_t ipnum) = NULL;
typedef char *(*dl_GeoIP_country_code_by_ipnum_v6_t)(GeoIP* gi, geoipv6_t ipnum);
static union { dl_GeoIP_country_code_by_ipnum_v6_t func; void * obj; } dl_GeoIP_country_code_by_ipnum_v6;

#else // SUPPORT_GEOIP_DYN
static const char* wrapper_geoip_info = "built-in";
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

/* database usage map */
#define GEOIP_DB_MAX_BLOCKS_32	2	// 0-63
static uint32_t geoip_db_usage_map[GEOIP_DB_MAX_BLOCKS_32];

#define GEOIP_DB_USAGE_MAP_TAG(db)	if (db < (32 * GEOIP_DB_MAX_BLOCKS_32)) { \
						DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Tag usage for db: %d", db); \
						geoip_db_usage_map[db / 32] |= 1 << (db % 32); \
					} else { \
						fprintf(stderr, "FIXME: unsupported db value (exceed limit): %d (%d)\n", db, 32 * GEOIP_DB_MAX_BLOCKS_32 - 1); \
						exit(1); \
					};

char geoip_db_usage_string[NI_MAXHOST] = "";

/*
 * function initialise the GeoIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_GeoIP_wrapper_init(void) {
	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called");

#ifdef SUPPORT_GEOIP_DYN
	char *error;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Load library: %s\n", __FILE__, __func__, geoip_lib_file);
	};

	dl_GeoIP_handle = dlopen(geoip_lib_file, RTLD_NOW | RTLD_LOCAL);

	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "GeoIP dynamic library load failed: %s\n", dlerror());
		return(1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Loaded library successful: %s\n", __FILE__, __func__, geoip_lib_file);
	};

	libipv6calc_db_wrapper_GeoIP_cleanup();

	/* GeoIPDBDescription */
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, "GeoIPDBDescription");
	};
	dlerror();    /* Clear any existing error */
	*(void **) (&dl_GeoIPDBDescription.obj) = dlsym(dl_GeoIP_handle, "GeoIPDBDescription");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		return(1);
	};
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called dlsym successful: %s", "GeoIPDBDescription");
	libipv6calc_db_wrapper_GeoIPDBDescription = dl_GeoIPDBDescription.val;

	/* check for version */
	libipv6calc_db_wrapper_dl_load_GeoIP_lib_version();
	if (dl_status_GeoIP_lib_version == IPV6CALC_DL_STATUS_OK) {
		lib_features_GeoIP |= GEOIP_LIB_FEATURE_LIB_VERSION;
	};

	/* check for IPv6 compat support */
	libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6();
	if (dl_status_GeoIP_country_code_by_ipnum_v6 == IPV6CALC_DL_STATUS_OK) {
		lib_features_GeoIP |= GEOIP_LIB_FEATURE_IPV6_CC_BY_IPNUM;
	};

	libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_ipnum_v6();
	if (dl_status_GeoIP_country_name_by_ipnum_v6 == IPV6CALC_DL_STATUS_OK) {
		lib_features_GeoIP |= GEOIP_LIB_FEATURE_IPV6_CN_BY_IPNUM;
	};

	/* check for IPv6 full support */
	libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_addr_v6();
	if (dl_status_GeoIP_country_code_by_addr_v6 == IPV6CALC_DL_STATUS_OK) {
		lib_features_GeoIP |= GEOIP_LIB_FEATURE_IPV6_CC_BY_ADDR;
	};

	libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6();
	if (dl_status_GeoIP_country_name_by_addr_v6 == IPV6CALC_DL_STATUS_OK) {
		lib_features_GeoIP |= GEOIP_LIB_FEATURE_IPV6_CN_BY_ADDR;
	};

	/* GeoIPDBFFileName */
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call dlsym: %s", "GeoIPDBFileName");
	dlerror();    /* Clear any existing error */
	dl_GeoIPDBFileName_ptr = dlsym(dl_GeoIP_handle, "GeoIPDBFileName");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		return(1);
	};
	libipv6calc_db_wrapper_GeoIPDBFileName_ptr = dl_GeoIPDBFileName_ptr;
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Assigned dlsym: %s", "GeoIPDBFileName");

#ifdef GEOIP_WORKAROUND_NUM_DB_TYPES
	// workaround to determine NUM_DB_TYPES until GeoIP API provides a function
	
	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Try to estimate geoip_num_db_types on dyn-load");

	if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_LIB_VERSION) != 0) {
		geoip_num_db_types = 31 + 1; // >= 1.4.7
	} else if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CC_BY_IPNUM) != 0) {
		geoip_num_db_types = 12 + 1; // >= 1.4.5
	} else {
		geoip_num_db_types = 11 + 1; // <= 1.45
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Estimation of geoip_num_db_types on dyn-load: %d", geoip_num_db_types - 1);
#else
	geoip_num_db_types = 0; // FUTURE: call function
#endif

#else // SUPPORT_GEOIP_DYN
	libipv6calc_db_wrapper_GeoIPDBDescription = GeoIPDBDescription;
	libipv6calc_db_wrapper_GeoIPDBFileName_ptr = &GeoIPDBFileName;
	geoip_num_db_types = NUM_DB_TYPES;
#endif // SUPPORT_GEOIP_DYN

	if ( (ipv6calc_debug & (DEBUG_libipv6addr_db_wrapper_GeoIP | DEBUG_libipv6addr_db_wrapper_GeoIP_verbose)) != 0 ) {
		int i;
		if (geoip_num_db_types > 0) {
			for (i = 0; i < geoip_num_db_types; i++) {
				if ((ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP_verbose) != 0) {
					fprintf(stderr, "GeoIP(verbose): GeoIPDBDescription Entry #%d: %s\n", i, libipv6calc_db_wrapper_GeoIPDBDescription[i]);
				} else {
					DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "GeoIPDBDescription Entry #%d: %s", i, libipv6calc_db_wrapper_GeoIPDBDescription[i]);
				};
			};
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "GeoIPDBDescription Entries can't be displayed, number of entries can't be retrieved (missing support)");
		};
	};

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_setup_custom_directory");

	libipv6calc_db_wrapper_GeoIP_setup_custom_directory(geoip_db_dir);

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Returned from libipv6calc_db_wrapper_GeoIP_setup_custom_directory");

#ifdef SUPPORT_GEOIP_DYN
	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION)");

	int r = libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION); // dummy call to trigger _GeoIP_setup_dbfilename

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Result of dummy GeoIP_db_avail call: %d", r);

#else // SUPPORT_GEOIP_DYN

#ifdef SUPPORT_GEOIP_LIB_VERSION
		lib_features_GeoIP |= GEOIP_LIB_FEATURE_LIB_VERSION;
#endif // SUPPORT_GEOIP_LIB_VERSION

#if defined SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6 && defined SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6
		lib_features_GeoIP |= (GEOIP_LIB_FEATURE_IPV6_CC_BY_ADDR | GEOIP_LIB_FEATURE_IPV6_CN_BY_ADDR);
#else // SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6 && SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6
#ifdef SUPPORT_GEOIP_V6
		lib_features_GeoIP |= (GEOIP_LIB_FEATURE_IPV6_CC_BY_IPNUM | GEOIP_LIB_FEATURE_IPV6_CN_BY_IPNUM);
#endif // SUPPORT_GEOIP_V6
#endif // SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6 && SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6
#endif // SUPPORT_GEOIP_DYN

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "GeoIP library features: 0x%04x", lib_features_GeoIP);

	if (libipv6calc_db_wrapper_GeoIPDBFileName_ptr == NULL) {
		fprintf(stderr, "%s/%s: libipv6calc_db_wrapper_GeoIPDBFileName_ptr == NULL (unexpected)\n", __FILE__, __func__);
		exit(1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Check for standard GeoIP databases\n", __FILE__, __func__);
	};

	/* check required databases for resolution */
	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION) == 1) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "GeoIP database GEOIP_COUNTRY_EDITION available");
		geoip_country_v4 = 1;
		wrapper_features_GeoIP |= IPV6CALC_DB_IPV4_TO_CC;
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION_V6) == 1) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "GeoIP database GEOIP_COUNTRY_EDITION_V6 available");
		geoip_country_v6 = 1;
		wrapper_features_GeoIP |= IPV6CALC_DB_IPV6_TO_CC;
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_ASNUM_EDITION) == 1) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "GeoIP database GEOIP_ASNUM_EDITION available");
		geoip_asnum_v4 = 1;
		wrapper_features_GeoIP |= IPV6CALC_DB_IPV4_TO_AS;
	};

	if ((lib_features_GeoIP & (GEOIP_LIB_FEATURE_IPV6_CC_BY_ADDR | GEOIP_LIB_FEATURE_IPV6_CC_BY_ADDR)) != 0) {
		if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_ASNUM_EDITION_V6) == 1) {
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "GeoIP database GEOIP_ASNUM_EDITION_V6 available");
			geoip_asnum_v6 = 1;
			wrapper_features_GeoIP |= IPV6CALC_DB_IPV6_TO_AS;
		};

		if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_CITY_EDITION_REV1_V6) == 1) {
			if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
				fprintf(stderr, "%s/%s: GeoIP database GEOIP_CITY_EDITION_REV1_V6 available\n", __FILE__, __func__);
			};
			geoip_city_v6 = 1;
		};
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_CITY_EDITION_REV1) == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: GeoIP database GEOIP_CITY_EDITION_REV1 available\n", __FILE__, __func__);
		};
		geoip_city_v4 = 1;
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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_GEOIP
	libipv6calc_db_wrapper_GeoIP_cleanup();
#endif

	dl_GeoIP_handle = NULL; // disable handle

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_GEOIP
	snprintf(string, size, "GeoIP available databases: Country4=%d Country6=%d ASN4=%d ASN6=%d City4=%d City6=%d", geoip_country_v4, geoip_country_v6, geoip_asnum_v4, geoip_asnum_v6, geoip_city_v4, geoip_city_v6);
#else
	snprintf(string, size, "No GeoIP support built-in");
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
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
	int i, count = 0;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called");

	printf("%sGeoIP: features: 0x%08x\n", prefix, wrapper_features_GeoIP);

#ifdef SUPPORT_GEOIP

#ifdef SUPPORT_GEOIP_DYN
	if (dl_GeoIP_handle == NULL) {
		printf("%sGeoIP: info of available databases in directory: %s LIBRARY-NOT-LOADED\n", prefix, geoip_db_dir);
	} else {
#endif // SUPPORT_GEOIP_DYN

	printf("%sGeoIP: info of available databases in directory: %s (max: %d)\n", prefix, geoip_db_dir, geoip_num_db_types - 1);

	for (i = 0; i < geoip_num_db_types; i++) {
		if (libipv6calc_db_wrapper_GeoIP_db_avail(i)) {
			// GeoIP returned that database is available
			gi = libipv6calc_db_wrapper_GeoIP_open_type(i, 0);
			if (gi == NULL) {
				if (i == GEOIP_CITY_EDITION_REV0) { continue; };

#ifdef SUPPORT_GEOIP_LIB_VERSION // >= 1.4.7
				if (i == GEOIP_LARGE_COUNTRY_EDITION) { continue; };
				if (i == GEOIP_CITY_EDITION_REV0_V6) { continue; };
				if (i == GEOIP_LARGE_COUNTRY_EDITION_V6) { continue; };
#endif

				if (level_verbose == LEVEL_VERBOSE2) {
					printf("%sGeoIP: %-33s:[%2d] %-40s (CAN'T OPEN)\n", prefix, libipv6calc_db_wrapper_GeoIPDBDescription[i], i, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
				};
			} else {
				count++;
				printf("%sGeoIP: %-33s:[%2d] %-40s (%s)\n", prefix, libipv6calc_db_wrapper_GeoIPDBDescription[i], i, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i], libipv6calc_db_wrapper_GeoIP_database_info(gi));
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
#ifdef SUPPORT_GEOIP_DYN
	};
#endif

	if (count == 0) {
		printf("%sGeoIP: NO available databases found in directory: %s\n", prefix, geoip_db_dir);
	};

#else
	snprintf(string, size, "%sNo GeoIP support built-in", prefix);
#endif

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Finished");
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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	int r = 1;

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_cleanup";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_cleanup == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_cleanup.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_ERROR;
			// fprintf(stderr, "%s\n", error); // >= 1.4.8
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_OK;

		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);

	} else if (dl_status_GeoIP_cleanup == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	r = (int) (*dl_GeoIP_cleanup.func)();

END_libipv6calc_db_wrapper:
#else
#ifdef SUPPORT_GEOIP_CLEANUP
	r = GeoIP_cleanup();
#else
	r = 0;
#endif
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return (r);
};



/*
 * wrapper: GeoIP_lib_version
 */
const char *libipv6calc_db_wrapper_GeoIP_lib_version(void) {
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char *result_GeoIP_lib_version = "unknown";
	char *error;

	if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_LIB_VERSION) != 0) {
		// see below
	} else if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CC_BY_IPNUM) != 0) {
		result_GeoIP_lib_version = "1.4.5/1.4.6";
	} else { 
		result_GeoIP_lib_version = "<=1.4.4";
	};

        if (dl_GeoIP_handle == NULL) {
                result_GeoIP_lib_version = "LIBRARY-NOT-LOADED";
	} else {
		libipv6calc_db_wrapper_dl_load_GeoIP_lib_version();

		if (dl_status_GeoIP_lib_version != IPV6CALC_DL_STATUS_OK) {
			goto END_libipv6calc_db_wrapper;
		};

		result_GeoIP_lib_version = (*dl_GeoIP_lib_version.func)();

		if ((error = dlerror()) != NULL)  {
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_lib_version);
#else // SUPPORT_GEOIP_DYN
#ifdef SUPPORT_GEOIP_LIB_VERSION
	return(GeoIP_lib_version());
#else
	return("unsupported(< 1.4.7)");
#endif
#endif
};


/*
 * wrapper: GeoIP_setup_custom_directory
 */
void libipv6calc_db_wrapper_GeoIP_setup_custom_directory(char *dir) {
	int r;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s with dir: %s\n", __FILE__, __func__, wrapper_geoip_info, dir);
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_setup_custom_directory";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_setup_custom_directory == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_setup_custom_directory.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s (unsupported too old library)\n", error);
			exit(1); // must have
		};

		dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_setup_custom_directory == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Call libipv6calc_db_wrapper_GeoIP_cleanup\n", __FILE__, __func__);
	};

	libipv6calc_db_wrapper_GeoIP_cleanup(); // free old stuff

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Returned from libipv6calc_db_wrapper_GeoIP_cleanup\n", __FILE__, __func__);
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call dl_GeoIP_setup_custom_directory: %s", dir);

	(*dl_GeoIP_setup_custom_directory.func)(dir);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
#else // SUPPORT_GEOIP_DYN
	libipv6calc_db_wrapper_GeoIP_cleanup(); // free old stuff

	GeoIP_setup_custom_directory(dir);
#endif // SUPPORT_GEOIP_DYN

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_db_avail (to trigger _GeoIP_setup_dbfilename) for db type: %d", GEOIP_COUNTRY_EDITION);

	r = libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION); // dummy call to trigger _GeoIP_setup_dbfilename

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Returned from libipv6calc_db_wrapper_GeoIP_db_avail with result: %d", r);

	if ( (ipv6calc_debug & (DEBUG_libipv6addr_db_wrapper_GeoIP | DEBUG_libipv6addr_db_wrapper_GeoIP_verbose)) != 0 ) {
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "List now available GeoIPDBFilename Entries (max:%d)", geoip_num_db_types - 1);

		if (geoip_num_db_types > 0) {
			int i;
			for (i = 0; i < geoip_num_db_types; i++) {
				if ((ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP_verbose) != 0) {
					fprintf(stderr, "GeoIP(verbose): GeoIPDBFileName Entry #%d: %s\n", i, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
				} else {
					DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "GeoIPDBFileName Entry #%d: %s", i, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
				};
			};
		} else {
			fprintf(stderr, "%s/%s: GeoIPDBDescription Entries can't be displayed, number of entries can't be retrieved (missing support)\n", __FILE__, __func__);
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Finished: %s with dir: %s\n", wrapper_geoip_info, dir);
	return;
};


/*
 * wrapper: GeoIP_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_GeoIP_db_avail(int type) {
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s type=%d (desc: %s)", wrapper_geoip_info, type, libipv6calc_db_wrapper_GeoIPDBDescription[type]);

#ifdef SUPPORT_GEOIP_DYN
	int result_GeoIP_db_avail = 0;
	const char *dl_symbol = "GeoIP_db_avail";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_db_avail == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_db_avail.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_db_avail == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_db_avail = (*dl_GeoIP_db_avail.func)(type);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Call to dl_GeoIP_db_avail results in: %d\n", __FILE__, __func__, result_GeoIP_db_avail);
	};

	if (result_GeoIP_db_avail == 0) {
		goto END_libipv6calc_db_wrapper;
	};

	dlerror();    /* Clear any existing error */
/*
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, "GeoIPDFilename");
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
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
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call: GeoIP_db_avail type=%d", type);
	int r = GeoIP_db_avail(type);
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Returned from GeoIP_db_avail result=%d", r);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
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
GeoIP *libipv6calc_db_wrapper_GeoIP_open_type(int type, int flags) {
	GeoIP *gi = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s type=%d (%s)", wrapper_geoip_info, type, libipv6calc_db_wrapper_GeoIPDBDescription[type]);

	if (libipv6calc_db_wrapper_GeoIP_db_avail(type) != 1) {
		return(NULL);
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_open_type";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_open_type == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_open_type.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_ERROR;
			NONQUIETPRINT_WA("%s", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_OK;

		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_open_type == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	gi = (*dl_GeoIP_open_type.func)(type, flags);

	if ((error = dlerror()) != NULL)  {
		NONQUIETPRINT_WA("%s", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
#else
	gi = GeoIP_open_type(type, GEOIP_STANDARD);
#endif
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Result: gi returned pointer: %s", (gi != NULL) ? "successful" : "NULL");
	return(gi);
};


/*
 * wrapper: GeoIP_open
 */
GeoIP* libipv6calc_db_wrapper_GeoIP_open(const char * filename, int flags) {
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s filename=%s", wrapper_geoip_info, filename);

#ifdef SUPPORT_GEOIP_DYN
	GeoIP *gi = NULL;
	const char *dl_symbol = "GeoIP_open";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_open == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_open.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_open = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_open = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_open == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	gi = (*dl_GeoIP_open.func)(filename, flags);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	unsigned char result_GeoIP_database_edition = '\0';
	const char *dl_symbol = "GeoIP_database_edition";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_database_edition == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_database_edition.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_database_edition == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_database_edition = (*dl_GeoIP_database_edition.func)(gi);

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
char *libipv6calc_db_wrapper_GeoIP_database_info(GeoIP *gi) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_database_info = NULL;
	const char *dl_symbol = "GeoIP_database_info";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_database_info == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_database_info.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_database_info == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_database_info = (*dl_GeoIP_database_info.func)(gi);

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
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_GeoIP_wrapper_db_info_used(void) {
	int db;
	GeoIP *gi;
	char tempstring[NI_MAXHOST];
	char *info;

	for (db = 0; db < 32 * GEOIP_DB_MAX_BLOCKS_32; db++) {
		if ((geoip_db_usage_map[db / 32] & (1 << (db % 32))) != 0) {
			DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "DB used: %d", db);

			gi = libipv6calc_db_wrapper_GeoIP_open_type(db, 0);
			info = libipv6calc_db_wrapper_GeoIP_database_info(gi);

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "DB info: %s", info);

			if (strlen(geoip_db_usage_string) > 0) {
				if (strstr(geoip_db_usage_string, info) != NULL) { continue; }; // string already included
				snprintf(tempstring, sizeof(tempstring), "%s / %s", geoip_db_usage_string, info);
			} else {
				snprintf(tempstring, sizeof(tempstring), "%s", info);
			};

			snprintf(geoip_db_usage_string, sizeof(geoip_db_usage_string), "%s", tempstring);
		};
	};

	return(geoip_db_usage_string);
};


/*
 * wrapper: GeoIP_delete
 */ 
void libipv6calc_db_wrapper_GeoIP_delete(GeoIP* gi) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_delete";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_delete == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
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

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_delete == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	(*dl_GeoIP_delete.func)(gi);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_code_by_addr = NULL;
	const char *dl_symbol = "GeoIP_country_code_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_country_code_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_addr.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_code_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_country_code_by_addr = (*dl_GeoIP_country_code_by_addr.func)(gi, addr);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_name_by_addr = NULL;
	const char *dl_symbol = "GeoIP_country_name_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_country_name_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_addr.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_name_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_country_name_by_addr = (*dl_GeoIP_country_name_by_addr.func)(gi, addr);

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
GeoIPRecord *libipv6calc_db_wrapper_GeoIP_record_by_addr(GeoIP *gi, const char *addr) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	GeoIPRecord* result_GeoIP_record_by_addr = NULL;
	const char *dl_symbol = "GeoIP_record_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_record_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_record_by_addr.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_record_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_record_by_addr = (*dl_GeoIP_record_by_addr.func)(gi, addr);

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
 * wrapper: GeoIP_record_by_addr_v6
 */
GeoIPRecord *libipv6calc_db_wrapper_GeoIP_record_by_addr_v6(GeoIP *gi, const char *addr) {
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	GeoIPRecord* result_GeoIP_record_by_addr_v6 = NULL;
	const char *dl_symbol = "GeoIP_record_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_record_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call dlsym: %s\n", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_record_by_addr_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_record_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_record_by_addr_v6 = IPV6CALC_DL_STATUS_OK;

		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_record_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	result_GeoIP_record_by_addr_v6 = (*dl_GeoIP_record_by_addr_v6.func)(gi, addr);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_GeoIP_record_by_addr_v6);
#else
	return(GeoIP_record_by_addr_v6(gi, addr));
#endif
};


/*
 * wrapper: GeoIPRecord_delete
 */
void libipv6calc_db_wrapper_GeoIPRecord_delete (GeoIPRecord *gir) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIPRecord_delete";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIPRecord_delete == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIPRecord_delete.obj) = dlsym(dl_GeoIP_handle, "GeoIPRecord_delete");

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIPRecord_delete == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	(*dl_GeoIPRecord_delete.func)(gir);

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
 * return: NULL: no result
 */
const char *libipv6calc_db_wrapper_GeoIP_country_code_by_addr_v6(GeoIP *gi, const char *addr) {
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char *result_GeoIP_country_code_by_addr_v6 = NULL;

	libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_addr_v6();

	if (dl_status_GeoIP_country_code_by_addr_v6 != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call dl_GeoIP_country_code_by_addr_v6.func");

	result_GeoIP_country_code_by_addr_v6 = (*dl_GeoIP_country_code_by_addr_v6.func)(gi, addr);

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Returned from dl_GeoIP_country_code_by_addr_v6.func");

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_code_by_addr_v6);
#else
#ifdef SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6
	return(GeoIP_country_code_by_addr_v6(gi, addr));
#else
	return(NULL);
#endif
#endif
};


/*
 * wrapper: GeoIP_country_name_by_addr_v6
 * return: NULL: no result
 */
const char *libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6(GeoIP *gi, const char *addr) {
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char *result_GeoIP_country_name_by_addr_v6 = NULL;

	libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6();

	if (dl_status_GeoIP_country_name_by_addr_v6 != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call: dl_GeoIP_country_name_by_addr_v6.func");
	result_GeoIP_country_name_by_addr_v6 = (*dl_GeoIP_country_name_by_addr_v6.func)(gi, addr);

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_name_by_addr_v6);
#else
#ifdef SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6
	return(GeoIP_country_name_by_addr_v6(gi, addr));
#else
	return(NULL);
#endif
#endif
};


/*
 * wrapper: GeoIP_country_name_by_ipnum_v6
 */
const char *  libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum_v6 (GeoIP* gi, geoipv6_t ipnum) {
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);;

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_name_by_ipnum_v6 = NULL;

	libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_ipnum_v6();

	if (dl_status_GeoIP_country_name_by_ipnum_v6 != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	result_GeoIP_country_name_by_ipnum_v6 = (*dl_GeoIP_country_name_by_ipnum_v6.func)(gi, ipnum);

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
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);;

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_code_by_ipnum_v6 = NULL;

	libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6();

	if (dl_status_GeoIP_country_code_by_ipnum_v6 != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	result_GeoIP_country_code_by_ipnum_v6 = (*dl_GeoIP_country_code_by_ipnum_v6.func)(gi, ipnum);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_name_by_addr = NULL;
	const char *dl_symbol = "GeoIP_name_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_name_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_name_by_addr.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_name_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_name_by_addr = (*dl_GeoIP_name_by_addr.func)(gi, addr);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_geoip_info);
	};

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_name_by_addr_v6 = NULL;
	const char *dl_symbol = "GeoIP_name_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_name_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_name_by_addr_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_name_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

	result_GeoIP_name_by_addr_v6 = (*dl_GeoIP_name_by_addr_v6.func)(gi, addr);

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

/* libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6 */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_ipnum_v6(void) {
	const char *dl_symbol = "GeoIP_country_code_by_ipnum_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_code_by_ipnum_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_ipnum_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_ERROR;
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_symbol GeoIP_country_code_by_ipnum_v6 not found");
			// fprintf(stderr, "%s\n", error); // stay silent
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};

	} else if (dl_status_GeoIP_country_code_by_ipnum_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};


/* libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_ipnum_v6 */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_ipnum_v6(void) {
	const char *dl_symbol = "GeoIP_country_name_by_ipnum_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_name_by_ipnum_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_ipnum_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_ipnum_v6 = IPV6CALC_DL_STATUS_ERROR;
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_symbol GeoIP_country_name_by_ipnum_v6 not found");
			// fprintf(stderr, "%s\n", error); // stay silent
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_name_by_ipnum_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};

	} else if (dl_status_GeoIP_country_name_by_ipnum_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};


/* libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_addr_v6 */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_addr_v6(void) {
	const char *dl_symbol = "GeoIP_country_code_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_code_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_addr_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_symbol GeoIP_country_code_by_addr_v6 not found");
			// fprintf(stderr, "%s\n", error); // >= 1.4.8
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_code_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};


/* libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6 */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6(void) {
	const char *dl_symbol = "GeoIP_country_name_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_name_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_addr_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_symbol GeoIP_country_name_by_addr_v6 not found");
			// fprintf(stderr, "%s\n", error); // >= 1.4.8
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_country_name_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
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
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_lib_version == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_lib_version.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_ERROR;
			// fprintf(stderr, "%s\n", error); // be quiet, optional feature (>= 1.4.7)
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_GeoIP_lib_version == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
			fprintf(stderr, "%s/%s: Previous call of dlsym already failed: %s\n", __FILE__, __func__, dl_symbol);
		};
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper_GeoIP) != 0 ) {
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
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called with feature value to test: 0x%08x",features);

	if ((wrapper_features_GeoIP & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Return with result: %d", result);
	return(result);
};

/* country_code */
const char *libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	const char *GeoIP_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called with addr=%s proto=%d", addr, proto);

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
		if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CC_BY_ADDR) != 0) {
			GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_code_by_addr_v6(gi, addr);
		} else if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CC_BY_IPNUM) != 0) {
			/* backward compatibility */
			ipv6calc_ipv6addr ipv6addr;
			char tempstring[NI_MAXHOST] = "";
			int result = 0;
			result = addr_to_ipv6addrstruct(addr, tempstring, &ipv6addr);
			if (result != 0) {
				goto END_libipv6calc_db_wrapper;
			};
			GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_code_by_ipnum_v6(gi, ipv6addr.in6_addr);
		};
	};

	if (GeoIP_result_ptr == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	if (strlen(GeoIP_result_ptr) > 2) {
		GeoIP_result_ptr = NULL;
		goto END_libipv6calc_db_wrapper;
	};

	GEOIP_DB_USAGE_MAP_TAG(GeoIP_type);

END_libipv6calc_db_wrapper:
	libipv6calc_db_wrapper_GeoIP_delete(gi);

	return(GeoIP_result_ptr);
};


/* country_name */
const char *libipv6calc_db_wrapper_GeoIP_wrapper_country_name_by_addr(const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	const char *GeoIP_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called with addr=%s proto=%d", addr, proto);

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
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_name_by_addr(gi, addr);
	} else if (proto == 6) {
		if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CN_BY_ADDR) != 0) {
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6");
			GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6(gi, addr);
		} else if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CN_BY_IPNUM) != 0) {
			/* backward compatibility */
			ipv6calc_ipv6addr ipv6addr;
			char tempstring[NI_MAXHOST] = "";
			int result = 0;
			result = addr_to_ipv6addrstruct(addr, tempstring, &ipv6addr);
			if (result != 0) {
				goto END_libipv6calc_db_wrapper;
			};
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum");
			GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum_v6(gi, ipv6addr.in6_addr);
		};
	};

	GEOIP_DB_USAGE_MAP_TAG(GeoIP_type);

END_libipv6calc_db_wrapper:
	libipv6calc_db_wrapper_GeoIP_delete(gi);

	return(GeoIP_result_ptr);
};


/* asnum */
char *libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	char *GeoIP_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		if ((wrapper_features_GeoIP & IPV6CALC_DB_IPV4_TO_AS) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Database/Support not available: GEOIP_ASNUM_EDITION");
			return(NULL);
		};

		/* TODO: workaround in case of old headerfile is used */
		GeoIP_type = GEOIP_ASNUM_EDITION;

	} else if (proto == 6) {
		if ((wrapper_features_GeoIP & IPV6CALC_DB_IPV6_TO_AS) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Database/Support not available: GEOIP_ASNUM_EDITION_V6");
			return(NULL);
		};

		/* TODO: workaround in case of old headerfile is used */
		GeoIP_type = GEOIP_ASNUM_EDITION_V6;

	} else {
		return(NULL);
	};

	gi = libipv6calc_db_wrapper_GeoIP_open_type(GeoIP_type, 0);
	if (gi == NULL) {
		return(NULL);
	};

	if (proto == 4) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_name_by_addr(gi, addr);
	} else if (proto == 6) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_name_by_addr_v6(gi, addr);
	};

	if (GeoIP_result_ptr == NULL) {
		return(NULL);
	};

	GEOIP_DB_USAGE_MAP_TAG(GeoIP_type);

	libipv6calc_db_wrapper_GeoIP_delete(gi);

	return(GeoIP_result_ptr);
};


/* record: city */
GeoIPRecord *libipv6calc_db_wrapper_GeoIP_wrapper_record_city_by_addr(const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	GeoIPRecord *GeoIP_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper_GeoIP, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		GeoIP_type = GEOIP_CITY_EDITION_REV1;
	} else if (proto == 6) {
		if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_LIB_VERSION) != 0) {
			GeoIP_type = GEOIP_CITY_EDITION_REV1_V6;
		};
	} else {
		return (NULL);
	};

	gi = libipv6calc_db_wrapper_GeoIP_open_type(GeoIP_type, 0);
	if (gi == NULL) {
		return (NULL);
	};

	if (proto == 4) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_record_by_addr(gi, addr);
	} else if (proto == 6) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_record_by_addr_v6(gi, addr);
	};

	if (GeoIP_result_ptr == NULL) {
		return (NULL);
	};

	libipv6calc_db_wrapper_GeoIP_delete(gi);

	GEOIP_DB_USAGE_MAP_TAG(GeoIP_type);

	return(GeoIP_result_ptr);
};

#endif


