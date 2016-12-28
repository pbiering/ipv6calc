/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_GeoIP.c
 * Version    : $Id$
 * Copyright  : 2013-2016 by Peter Bieringer <pb (at) bieringer.de>
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

#ifdef SUPPORT_GEOIP_DYN
#define GEOIP_WORKAROUND_NUM_DB_TYPES_MAX 32
#else
#define GEOIP_WORKAROUND_NUM_DB_TYPES_MAX NUM_DB_TYPES
#endif

uint32_t lib_features_GeoIP = 0;

char geoip_db_dir[NI_MAXHOST] = GEOIP_DB;

#ifdef SUPPORT_GEOIP_DYN
char geoip_lib_file[NI_MAXHOST] = GEOIP_DYN_LIB;
static const char* wrapper_geoip_info = "dyn-load";

/* define status and dynamic load functions */
static int dl_status_GeoIP_open = IPV6CALC_DL_STATUS_UNKNOWN;
typedef GeoIP *(*dl_GeoIP_open_t)(const char * filename, int flags);
static union { dl_GeoIP_open_t func; void * obj; } dl_GeoIP_open;

static int dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_UNKNOWN;
typedef GeoIP *(*dl_GeoIP_open_type_t)(int type, int flags);
static union { dl_GeoIP_open_type_t func; void * obj; } dl_GeoIP_open_type;

static int dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_UNKNOWN;
typedef int (*dl_GeoIP_db_avail_t)(int type);
static union { dl_GeoIP_db_avail_t func; void * obj; } dl_GeoIP_db_avail;

static int dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_UNKNOWN;
typedef unsigned char (*dl_GeoIP_database_edition_t)(GeoIP* gi);
static union { dl_GeoIP_database_edition_t func; void * obj; } dl_GeoIP_database_edition;

static int dl_status_GeoIP_delete = IPV6CALC_DL_STATUS_UNKNOWN;
typedef void (*dl_GeoIP_delete_t)(GeoIP* gi);
static union { dl_GeoIP_delete_t func; void * obj; } dl_GeoIP_delete;

static int dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_GeoIP_database_info_t)(GeoIP* gi);
static union { dl_GeoIP_database_info_t func; void * obj; } dl_GeoIP_database_info;

static int dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_GeoIP_country_code_by_addr_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_country_code_by_addr_t func; void * obj; } dl_GeoIP_country_code_by_addr;

static int dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_GeoIP_country_name_by_addr_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_country_name_by_addr_t func; void * obj; } dl_GeoIP_country_name_by_addr;

static int dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_GeoIP_name_by_addr_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_name_by_addr_t func; void * obj; } dl_GeoIP_name_by_addr;

static int dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_UNKNOWN;
typedef GeoIPRecord *(*dl_GeoIP_record_by_addr_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_record_by_addr_t func; void * obj; } dl_GeoIP_record_by_addr;

static int dl_status_GeoIP_record_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
typedef GeoIPRecord *(*dl_GeoIP_record_by_addr_v6_t)(GeoIP *gi, const char *addr);
static union { dl_GeoIP_record_by_addr_v6_t func; void * obj; } dl_GeoIP_record_by_addr_v6;

static int dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_UNKNOWN;
typedef void (*dl_GeoIPRecord_delete_t)(GeoIPRecord *gir);
static union { dl_GeoIPRecord_delete_t func; void * obj; } dl_GeoIPRecord_delete;

static int dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_GeoIP_country_code_by_addr_v6_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_country_code_by_addr_v6_t func; void * obj; } dl_GeoIP_country_code_by_addr_v6;

static int dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_GeoIP_country_name_by_addr_v6_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_country_name_by_addr_v6_t func; void * obj; } dl_GeoIP_country_name_by_addr_v6;

static int dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_GeoIP_name_by_addr_v6_t)(GeoIP* gi, const char *addr);
static union { dl_GeoIP_name_by_addr_v6_t func; void * obj; } dl_GeoIP_name_by_addr_v6;

static int dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_UNKNOWN;
typedef char *(*dl_GeoIP_lib_version_t)(void);
static union { dl_GeoIP_lib_version_t func; void * obj; } dl_GeoIP_lib_version;

static int dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_UNKNOWN;
typedef int (*dl_GeoIP_cleanup_t)(void);
static union { dl_GeoIP_cleanup_t func; void * obj; } dl_GeoIP_cleanup;

static int dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_UNKNOWN;
typedef void (*dl_GeoIP_setup_custom_director_t)(char *dir);
static union { dl_GeoIP_setup_custom_director_t func; void * obj; } dl_GeoIP_setup_custom_directory;

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
typedef char *(*dl_GeoIP_country_name_by_ipnum_v6_t)(GeoIP* gi, geoipv6_t ipnum);
static union { dl_GeoIP_country_name_by_ipnum_v6_t func; void * obj; } dl_GeoIP_country_name_by_ipnum_v6;

static int dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_UNKNOWN;
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
						DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Tag usage for db: %d", db); \
						geoip_db_usage_map[db / 32] |= 1 << (db % 32); \
					} else { \
						fprintf(stderr, "FIXME: unsupported db value (exceed limit): %d (%d)\n", db, 32 * GEOIP_DB_MAX_BLOCKS_32 - 1); \
						exit(1); \
					};

char geoip_db_usage_string[NI_MAXHOST] = "";

// local cache
static GeoIP *db_ptr_cache[GEOIP_WORKAROUND_NUM_DB_TYPES_MAX];

/*
 * function initialise the GeoIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_GeoIP_wrapper_init(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called");

#ifdef SUPPORT_GEOIP_DYN
	char *error;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Load library: %s", geoip_lib_file);

	dlerror();    /* Clear any existing error */

	dl_GeoIP_handle = dlopen(geoip_lib_file, RTLD_NOW | RTLD_LOCAL);

	if (dl_GeoIP_handle == NULL) {
		if ((strcmp(geoip_lib_file, GEOIP_DYN_LIB) != 0) || (ipv6calc_verbose > 0)) {
			NONQUIETPRINT_WA("GeoIP dynamic library load failed (disable support): %s", dlerror())
		};
		return(1);
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Loaded library successful: %s", geoip_lib_file);

	libipv6calc_db_wrapper_GeoIP_cleanup();

	/* GeoIPDBDescription */
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", "GeoIPDBDescription");
	dlerror();    /* Clear any existing error */
	*(void **) (&dl_GeoIPDBDescription.obj) = dlsym(dl_GeoIP_handle, "GeoIPDBDescription");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		return(1);
	};
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", "GeoIPDBDescription");
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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", "GeoIPDBFileName");
	dlerror();    /* Clear any existing error */
	dl_GeoIPDBFileName_ptr = dlsym(dl_GeoIP_handle, "GeoIPDBFileName");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		return(1);
	};
	libipv6calc_db_wrapper_GeoIPDBFileName_ptr = dl_GeoIPDBFileName_ptr;
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Assigned dlsym: %s", "GeoIPDBFileName");

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

	libipv6calc_db_wrapper_GeoIPDBDescription = GeoIPDBDescription;
	libipv6calc_db_wrapper_GeoIPDBFileName_ptr = &GeoIPDBFileName;
	geoip_num_db_types = NUM_DB_TYPES;
#endif // SUPPORT_GEOIP_DYN

#ifdef GEOIP_WORKAROUND_NUM_DB_TYPES
	// workaround to determine NUM_DB_TYPES until GeoIP API provides a function
	
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Try to estimate minimum geoip_num_db_types (to avoid segfaults)");

	if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_LIB_VERSION) != 0) {
		geoip_num_db_types = 31 + 1; // >= 1.4.7
	} else if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CC_BY_IPNUM) != 0) {
		geoip_num_db_types = 12 + 1; // >= 1.4.5
	} else {
		geoip_num_db_types = 11 + 1; // < 1.4.5
	};

	if (geoip_num_db_types > GEOIP_WORKAROUND_NUM_DB_TYPES_MAX) {
		geoip_num_db_types = GEOIP_WORKAROUND_NUM_DB_TYPES_MAX;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Estimation of minimum geoip_num_db_types: %d", geoip_num_db_types - 1);
#else
	geoip_num_db_types = 0; // TODO: call function, once provided via GeoIP API
#endif


	DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper_GeoIP)
		unsigned int i;
		if (geoip_num_db_types > 0) {
			for (i = 0; i < geoip_num_db_types; i++) {
				DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper_GeoIP_verbose)
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP_verbose, "GeoIP(verbose): GeoIPDBDescription Entry #%d: %s", i, libipv6calc_db_wrapper_GeoIPDBDescription[i]);
				DEBUGSECTION_ELSE
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIPDBDescription Entry #%d: %s", i, libipv6calc_db_wrapper_GeoIPDBDescription[i]);
				DEBUGSECTION_END
			};
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIPDBDescription Entries can't be displayed, number of entries can't be retrieved (missing support)");
		};
	DEBUGSECTION_END

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_setup_custom_directory");

	libipv6calc_db_wrapper_GeoIP_setup_custom_directory(geoip_db_dir);

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Returned from libipv6calc_db_wrapper_GeoIP_setup_custom_directory");

#ifdef SUPPORT_GEOIP_DYN
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION)");

	int r = libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION); // dummy call to trigger _GeoIP_setup_dbfilename

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Result of dummy GeoIP_db_avail call: %d", r);

#else // SUPPORT_GEOIP_DYN

#endif // SUPPORT_GEOIP_DYN

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP library features: 0x%04x", lib_features_GeoIP);

	if (libipv6calc_db_wrapper_GeoIPDBFileName_ptr == NULL) {
		fprintf(stderr, "%s/%s: libipv6calc_db_wrapper_GeoIPDBFileName_ptr == NULL (unexpected)\n", __FILE__, __func__);
		exit(1);
	};

	// add features to implemented
	wrapper_features_by_source_implemented[IPV6CALC_DB_SOURCE_GEOIP] |= IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_GEOIP_IPV4 | IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_GEOIP_IPV6 | IPV6CALC_DB_IPV4_TO_AS | IPV6CALC_DB_IPV6_TO_AS | IPV6CALC_DB_IPV4_TO_CITY | IPV6CALC_DB_IPV6_TO_CITY | IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV4_TO_COUNTRY | IPV6CALC_DB_IPV6_TO_COUNTRY;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Check for standard GeoIP databases");

	/* check required databases for resolution */
	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION) == 1) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP database GEOIP_COUNTRY_EDITION available");
		geoip_country_v4 = 1;
		wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] |= IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_GEOIP_IPV4 | IPV6CALC_DB_IPV4_TO_COUNTRY;
	};

#ifdef SUPPORT_GEOIP_V6
#if HAVE_DECL_GEOIP_COUNTRY_EDITION_V6 == 1
	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION_V6) == 1) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP database GEOIP_COUNTRY_EDITION_V6 available");
		geoip_country_v6 = 1;
		wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] |= IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_GEOIP_IPV6 | IPV6CALC_DB_IPV6_TO_COUNTRY;
	};
#endif
#endif

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_ASNUM_EDITION) == 1) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP database GEOIP_ASNUM_EDITION available");
		geoip_asnum_v4 = 1;
		wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] |= IPV6CALC_DB_IPV4_TO_AS | IPV6CALC_DB_GEOIP_IPV4;
	};

	if ((lib_features_GeoIP & (GEOIP_LIB_FEATURE_IPV6_CC_BY_ADDR | GEOIP_LIB_FEATURE_IPV6_CN_BY_ADDR)) != 0) {
#if HAVE_DECL_GEOIP_ASNUM_EDITION_V6 == 1
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP check for GEOIP_ASNUM_EDITION_V6 & GEOIP_CITY_EDITION_REV1_V6");
		if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_ASNUM_EDITION_V6) == 1) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP database GEOIP_ASNUM_EDITION_V6 available");
			geoip_asnum_v6 = 1;
			wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] |= IPV6CALC_DB_IPV6_TO_AS | IPV6CALC_DB_GEOIP_IPV6;
		};

		if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_CITY_EDITION_REV1_V6) == 1) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP database GEOIP_CITY_EDITION_REV1_V6 available");
			geoip_city_v6 = 1;
			wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] |= IPV6CALC_DB_GEOIP_IPV6 | IPV6CALC_DB_IPV6_TO_CITY | IPV6CALC_DB_IPV6_TO_REGION;
		};
#else
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP check skipped for GEOIP_ASNUM_EDITION_V6 & GEOIP_CITY_EDITION_REV1_V6");
#endif
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP library looks like missing features for GEOIP_ASNUM_EDITION_V6 & GEOIP_CITY_EDITION_REV1_V6");
	};

	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_CITY_EDITION_REV1) == 1) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP database GEOIP_CITY_EDITION_REV1 available");
		geoip_city_v4 = 1;
		wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] |= IPV6CALC_DB_GEOIP_IPV4 | IPV6CALC_DB_IPV4_TO_CITY | IPV6CALC_DB_IPV4_TO_REGION;
	};

#ifdef SUPPORT_GEOIP_V6
#ifdef GEOIP_CITY_EDITION_REV1_V6
	if (libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_CITY_EDITION_REV1_V6) == 1) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIP database GEOIP_CITY_EDITION_REV1_V6 available");
		geoip_city_v4 = 1;
		wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] |= IPV6CALC_DB_GEOIP_IPV6 | IPV6CALC_DB_IPV6_TO_CITY | IPV6CALC_DB_IPV6_TO_REGION;
	};
#endif // GEOIP_CITY_EDITION_REV1_V6
#endif // SUPPORT_GEOIP_V6

	wrapper_features |= wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP];

	return 0;
};


#ifdef SUPPORT_GEOIP
/*
 * function GeoIP_close
 */
static int libipv6calc_db_wrapper_GeoIP_close(GeoIP *gi) {
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called");

	if (gi != NULL) {
		/* cleanup cache entry */
		for (i = 0; i < GEOIP_WORKAROUND_NUM_DB_TYPES_MAX; i++) {
			if (db_ptr_cache[i] == gi) {
				db_ptr_cache[i] = NULL;
			};
		};

		libipv6calc_db_wrapper_GeoIP_delete(gi);
	};

	return(0);
};
#endif // SUPPORT_GEOIP


/*
 * function cleanup the GeoIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_GeoIP_wrapper_cleanup(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called");

#ifdef SUPPORT_GEOIP
	int i;

	for (i = 0; i < GEOIP_WORKAROUND_NUM_DB_TYPES_MAX; i++) {
		if (db_ptr_cache[i] != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Close GeoIP: type=%d", i);
			libipv6calc_db_wrapper_GeoIP_close(db_ptr_cache[i]);
		};
	};

	libipv6calc_db_wrapper_GeoIP_cleanup();
#endif

	dl_GeoIP_handle = NULL; // disable handle

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Finished");
	return 0;
};


/*
 * function info of GeoIP wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_GeoIP_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called");

#ifdef SUPPORT_GEOIP
	snprintf(string, size, "GeoIP available databases: Country4=%d Country6=%d ASN4=%d ASN6=%d City4=%d City6=%d", geoip_country_v4, geoip_country_v6, geoip_asnum_v4, geoip_asnum_v6, geoip_city_v4, geoip_city_v6);
#else
	snprintf(string, size, "No GeoIP support built-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Finished");
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
	int count = 0;
	unsigned int i;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called");

	IPV6CALC_DB_FEATURE_INFO(prefix, IPV6CALC_DB_SOURCE_GEOIP)

#ifdef SUPPORT_GEOIP

#ifdef SUPPORT_GEOIP_DYN
	if (dl_GeoIP_handle == NULL) {
		fprintf(stderr, "%sGeoIP: info of available databases in directory: %s LIBRARY-NOT-LOADED\n", prefix, geoip_db_dir);
	} else {
#endif // SUPPORT_GEOIP_DYN

	fprintf(stderr, "%sGeoIP: info of available databases in directory: %s (max: %d)\n", prefix, geoip_db_dir, geoip_num_db_types - 1);

	for (i = 0; i < geoip_num_db_types; i++) {
		if (libipv6calc_db_wrapper_GeoIP_db_avail(i)) {
			// GeoIP returned that database is available
			gi = libipv6calc_db_wrapper_GeoIP_open_type(i, 0);
			if (gi == NULL) {
				if (i == GEOIP_CITY_EDITION_REV0) { continue; };

#if HAVE_DECL_GEOIP_LARGE_COUNTRY_EDITION == 1
				if (i == GEOIP_LARGE_COUNTRY_EDITION) { continue; };
#endif
#if HAVE_DECL_GEOIP_CITY_EDITION_REV0_V6 == 1
				if (i == GEOIP_CITY_EDITION_REV0_V6) { continue; };
#endif
#if HAVE_DECL_GEOIP_LARGE_COUNTRY_EDITION_V6 == 1
				if (i == GEOIP_LARGE_COUNTRY_EDITION_V6) { continue; };
#endif

				if (level_verbose == LEVEL_VERBOSE2) {
					fprintf(stderr, "%sGeoIP: %-33s:[%2d] %-20s (CAN'T OPEN)\n", prefix, libipv6calc_db_wrapper_GeoIPDBDescription[i], i, rindex((*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i], '/') + 1);
				};
			} else {
				count++;
				fprintf(stderr, "%sGeoIP: %-33s:[%2d] %-20s (%s)\n", prefix, libipv6calc_db_wrapper_GeoIPDBDescription[i], i, rindex((*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i], '/') + 1, libipv6calc_db_wrapper_GeoIP_database_info(gi));
			};
		} else {
			if (geoip_num_db_types > 0) {
				if ((*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i] == NULL) {
					continue;
				};

				if (level_verbose == LEVEL_VERBOSE2) {
					// fprintf(stderr, "%sGeoIP: %-33s: %-40s (MISSING FILE)\n", prefix, libipv6calc_db_wrapper_GeoIPDBDescription[i], (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
				} else {
					continue;
				};
			 };
		};
	};
	if ((geoip_num_db_types == 0) && (level_verbose == LEVEL_VERBOSE2)) {
		fprintf(stderr, "%sGeoIP: other possible databases can't be displayed, number of entries can't be retrieved (missing support)\n", prefix);
	};
#ifdef SUPPORT_GEOIP_DYN
	};
#endif

	if (count == 0) {
		fprintf(stderr, "%sGeoIP: No available databases found in directory: %s\n", prefix, geoip_db_dir);
	};

#else
	snfprintf(stderr, string, size, "%sNo GeoIP support built-in", prefix);
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Finished");
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
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called");

	int r = 1;

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_cleanup";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_cleanup == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_cleanup.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_ERROR;
			// fprintf(stderr, "%s\n", error); // >= 1.4.8
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_cleanup = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_cleanup == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
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

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Finished");
	return (r);
};



/*
 * wrapper: GeoIP_lib_version
 */
const char *libipv6calc_db_wrapper_GeoIP_lib_version(void) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

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

		dlerror();    /* Clear any existing error */

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

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called");

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_setup_custom_directory";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_setup_custom_directory == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_setup_custom_directory.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s (unsupported too old library)\n", error);
			exit(1); // must have
		};

		dl_status_GeoIP_setup_custom_directory = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_setup_custom_directory == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_cleanup");

	libipv6calc_db_wrapper_GeoIP_cleanup(); // free old stuff

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Returned from libipv6calc_db_wrapper_GeoIP_cleanup");

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dl_GeoIP_setup_custom_directory: %s", dir);

	dlerror();    /* Clear any existing error */

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

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_db_avail (to trigger _GeoIP_setup_dbfilename) for db type: %d", GEOIP_COUNTRY_EDITION);

	r = libipv6calc_db_wrapper_GeoIP_db_avail(GEOIP_COUNTRY_EDITION); // dummy call to trigger _GeoIP_setup_dbfilename

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Returned from libipv6calc_db_wrapper_GeoIP_db_avail with result: %d", r);

	DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper_GeoIP)
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "List now available GeoIPDBFilename Entries (max: %d)", geoip_num_db_types - 1);

		if (geoip_num_db_types > 0) {
			unsigned int i;
			for (i = 0; i < geoip_num_db_types; i++) {
				DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper_GeoIP_verbose)
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP_verbose, "GeoIP(verbose): GeoIPDBFileName Entry #%d: %s", i, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
				DEBUGSECTION_ELSE
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIPDBFileName Entry #%d: %s", i, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[i]);
				DEBUGSECTION_END
			};
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "GeoIPDBDescription Entries can't be displayed, number of entries can't be retrieved (missing support)");
		};
	DEBUGSECTION_END

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Finished: %s with dir: %s", wrapper_geoip_info, dir);
	return;
};


/*
 * wrapper: GeoIP_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_GeoIP_db_avail(int type) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s type=%d (desc: %s)", wrapper_geoip_info, type, libipv6calc_db_wrapper_GeoIPDBDescription[type]);

#ifdef SUPPORT_GEOIP_DYN
	int result_GeoIP_db_avail = 0;
	const char *dl_symbol = "GeoIP_db_avail";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_db_avail == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_db_avail.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_db_avail = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_db_avail == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	result_GeoIP_db_avail = (*dl_GeoIP_db_avail.func)(type);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call to dl_GeoIP_db_avail results in: %d", result_GeoIP_db_avail);

	if (result_GeoIP_db_avail == 0) {
		goto END_libipv6calc_db_wrapper;
	};

	dlerror();    /* Clear any existing error */

END_libipv6calc_db_wrapper:
	return(result_GeoIP_db_avail);
#else
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call: GeoIP_db_avail type=%d", type);
	int r = GeoIP_db_avail(type);
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Returned from GeoIP_db_avail result=%d", r);

	DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper_GeoIP)
		if ((*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[type] == NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Finished: %s type=%d (still unknown) (r=%d)", wrapper_geoip_info, type, r);
		} else {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Finished: %s type=%d (%s) (r=%d)", wrapper_geoip_info, type, (*libipv6calc_db_wrapper_GeoIPDBFileName_ptr)[type], r);
		};
	DEBUGSECTION_END

	return(r);
#endif
};


/*
 * wrapper: GeoIP_open_type
 */
GeoIP *libipv6calc_db_wrapper_GeoIP_open_type(int type, int flags) {
	GeoIP *gi = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s type=%d (%s)", wrapper_geoip_info, type, libipv6calc_db_wrapper_GeoIPDBDescription[type]);

	if (libipv6calc_db_wrapper_GeoIP_db_avail(type) != 1) {
		return(NULL);
	};

	if (db_ptr_cache[type] != NULL) {
		// already open
		gi = db_ptr_cache[type];

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Database already opened (cached) gi=%p type=%d", gi, type);
		goto END_libipv6calc_db_wrapper;
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_open_type";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_open_type == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_open_type.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_ERROR;
			NONQUIETPRINT_WA("%s", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_open_type = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_open_type == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	gi = (*dl_GeoIP_open_type.func)(type, flags);

	if ((error = dlerror()) != NULL)  {
		NONQUIETPRINT_WA("%s", error);
		goto END_libipv6calc_db_wrapper;
	};
#else
	if (flags == 0) { };  // make compiler happy (avoid unused "...")
	gi = GeoIP_open_type(type, GEOIP_STANDARD);
#endif

	if (gi == NULL) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Database can't be openend type=%d", type);
		goto END_libipv6calc_db_wrapper;
	};

	// fill cache
	db_ptr_cache[type] = gi;
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Database successfully opened (fill-cache), gi=%p type=%d", gi, type)

END_libipv6calc_db_wrapper:
	return(gi);
};


/*
 * wrapper: GeoIP_open
 */
GeoIP* libipv6calc_db_wrapper_GeoIP_open(const char * filename, int flags) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s filename=%s", wrapper_geoip_info, filename);

#ifdef SUPPORT_GEOIP_DYN
	GeoIP *gi = NULL;
	const char *dl_symbol = "GeoIP_open";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_open == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_open.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_open = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_open = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_open == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

	gi = (*dl_GeoIP_open.func)(filename, flags);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(gi);
#else
	if (flags == 0) { };  // make compiler happy (avoid unused "...")
	return(GeoIP_open(filename, GEOIP_STANDARD));
#endif
};


/*
 * wrapper: GeoIP_database_edition
 */
unsigned char libipv6calc_db_wrapper_GeoIP_database_edition (GeoIP* gi) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	unsigned char result_GeoIP_database_edition = '\0';
	const char *dl_symbol = "GeoIP_database_edition";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_database_edition == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_database_edition.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_database_edition = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_database_edition == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_database_info = NULL;
	const char *dl_symbol = "GeoIP_database_info";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_database_info == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_database_info.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_database_info = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_database_info == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "DB used: %d", db);

			gi = libipv6calc_db_wrapper_GeoIP_open_type(db, 0);
			info = libipv6calc_db_wrapper_GeoIP_database_info(gi);

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "DB info: %s", info);

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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

	if (gi == NULL) {
		return;
	};

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIP_delete";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_delete == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_delete) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_delete = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_delete = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_delete == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_code_by_addr = NULL;
	const char *dl_symbol = "GeoIP_country_code_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_country_code_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_addr.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_country_code_by_addr = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_country_code_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_country_name_by_addr = NULL;
	const char *dl_symbol = "GeoIP_country_name_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_country_name_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_addr.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_country_name_by_addr = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_country_name_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	GeoIPRecord* result_GeoIP_record_by_addr = NULL;
	const char *dl_symbol = "GeoIP_record_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_record_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_record_by_addr.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_record_by_addr = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_record_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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

#ifdef SUPPORT_GEOIP_V6
/*
 * wrapper: GeoIP_record_by_addr_v6
 */
GeoIPRecord *libipv6calc_db_wrapper_GeoIP_record_by_addr_v6(GeoIP *gi, const char *addr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	GeoIPRecord* result_GeoIP_record_by_addr_v6 = NULL;
	const char *dl_symbol = "GeoIP_record_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_record_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_record_by_addr_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_record_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_record_by_addr_v6 = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_record_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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
#endif


/*
 * wrapper: GeoIPRecord_delete
 */
void libipv6calc_db_wrapper_GeoIPRecord_delete (GeoIPRecord *gir) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	const char *dl_symbol = "GeoIPRecord_delete";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIPRecord_delete == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIPRecord_delete.obj) = dlsym(dl_GeoIP_handle, "GeoIPRecord_delete");

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIPRecord_delete = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIPRecord_delete == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char *result_GeoIP_country_code_by_addr_v6 = NULL;

	libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_addr_v6();

	if (dl_status_GeoIP_country_code_by_addr_v6 != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dl_GeoIP_country_code_by_addr_v6.func");

	result_GeoIP_country_code_by_addr_v6 = (*dl_GeoIP_country_code_by_addr_v6.func)(gi, addr);

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Returned from dl_GeoIP_country_code_by_addr_v6.func");

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_code_by_addr_v6);
#else
#ifdef SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6
	return(GeoIP_country_code_by_addr_v6(gi, addr));
#else
	if (strlen(addr) == 0) { }; // make compiler happy (avoid unused "...")
	if (gi == NULL) { }; // make compiler happy (avoid unused "...")
	return(NULL);
#endif
#endif
};


/*
 * wrapper: GeoIP_country_name_by_addr_v6
 * return: NULL: no result
 */
const char *libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6(GeoIP *gi, const char *addr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char *result_GeoIP_country_name_by_addr_v6 = NULL;

	libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6();

	if (dl_status_GeoIP_country_name_by_addr_v6 != IPV6CALC_DL_STATUS_OK) {
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call: dl_GeoIP_country_name_by_addr_v6.func");
	result_GeoIP_country_name_by_addr_v6 = (*dl_GeoIP_country_name_by_addr_v6.func)(gi, addr);

END_libipv6calc_db_wrapper:
	return(result_GeoIP_country_name_by_addr_v6);
#else
#ifdef SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6
	return(GeoIP_country_name_by_addr_v6(gi, addr));
#else
	if (strlen(addr) == 0) { }; // make compiler happy (avoid unused "...")
	if (gi == NULL) { }; // make compiler happy (avoid unused "...")
	return(NULL);
#endif
#endif
};


#ifdef SUPPORT_GEOIP_V6

/*
 * wrapper: GeoIP_country_name_by_ipnum_v6
 */
const char *  libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum_v6 (GeoIP* gi, geoipv6_t ipnum) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);;

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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);;

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

#endif // SUPPORT_GEOIP_V6

/*
 * wrapper: GeoIP_name_by_addr
 */
char* libipv6calc_db_wrapper_GeoIP_name_by_addr (GeoIP* gi, const char *addr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_name_by_addr = NULL;
	const char *dl_symbol = "GeoIP_name_by_addr";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_name_by_addr == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_name_by_addr.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_name_by_addr = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_name_by_addr == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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


#ifdef SUPPORT_GEOIP_V6
/*
 * wrapper: GeoIP_name_by_addr_v6
 */
char* libipv6calc_db_wrapper_GeoIP_name_by_addr_v6 (GeoIP* gi, const char *addr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called: %s", wrapper_geoip_info);

#ifdef SUPPORT_GEOIP_DYN
	char* result_GeoIP_name_by_addr_v6 = NULL;
	const char *dl_symbol = "GeoIP_name_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_GeoIP_name_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_name_by_addr_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_GeoIP_name_by_addr_v6 = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_name_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	dlerror();    /* Clear any existing error */

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
#endif // SUPPORT_GEOIP_V6


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
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_code_by_ipnum_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_ipnum_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_ERROR;
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_symbol GeoIP_country_code_by_ipnum_v6 not found");
			// fprintf(stderr, "%s\n", error); // stay silent
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_code_by_ipnum_v6 = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_country_code_by_ipnum_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};


/* libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_ipnum_v6 */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_ipnum_v6(void) {
	const char *dl_symbol = "GeoIP_country_name_by_ipnum_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_name_by_ipnum_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_ipnum_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_ipnum_v6 = IPV6CALC_DL_STATUS_ERROR;
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_symbol GeoIP_country_name_by_ipnum_v6 not found");
			// fprintf(stderr, "%s\n", error); // stay silent
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_name_by_ipnum_v6 = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_country_name_by_ipnum_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};


/* libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_addr_v6 */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_code_by_addr_v6(void) {
	const char *dl_symbol = "GeoIP_country_code_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_code_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_code_by_addr_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_symbol GeoIP_country_code_by_addr_v6 not found");
			// fprintf(stderr, "%s\n", error); // >= 1.4.8
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_code_by_addr_v6 = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_country_code_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};


/* libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6 */
static void libipv6calc_db_wrapper_dl_load_GeoIP_country_name_by_addr_v6(void) {
	const char *dl_symbol = "GeoIP_country_name_by_addr_v6";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_country_name_by_addr_v6 == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_country_name_by_addr_v6.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_ERROR;
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_symbol GeoIP_country_name_by_addr_v6 not found");
			// fprintf(stderr, "%s\n", error); // >= 1.4.8
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_country_name_by_addr_v6 = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_country_name_by_addr_v6 == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
	};

END_libipv6calc_db_wrapper_dl_load:
	return;
};

static void libipv6calc_db_wrapper_dl_load_GeoIP_lib_version (void) {
	const char *dl_symbol = "GeoIP_lib_version";
	char *error;

	if (dl_GeoIP_handle == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "dl_GeoIP_handle not defined");
		goto END_libipv6calc_db_wrapper_dl_load;
	};

	if (dl_status_GeoIP_lib_version == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_GeoIP_lib_version.obj) = dlsym(dl_GeoIP_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_ERROR;
			// fprintf(stderr, "%s\n", error); // be quiet, optional feature (>= 1.4.7)
			goto END_libipv6calc_db_wrapper_dl_load;
		};

		dl_status_GeoIP_lib_version = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_GeoIP_lib_version == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper_dl_load;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Previous call of dlsym already successful: %s", dl_symbol);
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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called with feature value to test: 0x%08x",features);

	if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Return with result: %d", result);
	return(result);
};

/* country_code */
const char *libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	const char *GeoIP_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		GeoIP_type = GEOIP_COUNTRY_EDITION;
#ifdef SUPPORT_GEOIP_V6
#if HAVE_DECL_GEOIP_COUNTRY_EDITION_V6 == 1
	} else if (proto == 6) {
		GeoIP_type = GEOIP_COUNTRY_EDITION_V6;
#endif
#endif // SUPPORT_GEOIP_V6
	} else {
		goto END_libipv6calc_db_wrapper;
	};

	gi = libipv6calc_db_wrapper_GeoIP_open_type(GeoIP_type, 0);

	if (gi == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	if (proto == 4) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_code_by_addr(gi, addr);
#ifdef SUPPORT_GEOIP_V6
	} else if (proto == 6) {
		if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CC_BY_ADDR) != 0) {
			GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_code_by_addr_v6(gi, addr);
		} else if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CC_BY_IPNUM) != 0) {
			/* backward compatibility */
			ipv6calc_ipv6addr ipv6addr;
			char tempstring[NI_MAXHOST] = "";
			int result = 0;
			result = addr_to_ipv6addrstruct(addr, tempstring, sizeof(tempstring), &ipv6addr);
			if (result != 0) {
				goto END_libipv6calc_db_wrapper;
			};
			GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_code_by_ipnum_v6(gi, ipv6addr.in6_addr);
		};
#endif // SUPPORT_GEOIP_V6
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
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Result for addr=%s proto=%d: %s", addr, proto, GeoIP_result_ptr);
	return(GeoIP_result_ptr);
};


/* country_name */
const char *libipv6calc_db_wrapper_GeoIP_wrapper_country_name_by_addr(const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	const char *GeoIP_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		GeoIP_type = GEOIP_COUNTRY_EDITION;
#ifdef SUPPORT_GEOIP_V6
#if HAVE_DECL_GEOIP_COUNTRY_EDITION_V6 == 1
	} else if (proto == 6) {
		GeoIP_type = GEOIP_COUNTRY_EDITION_V6;
#endif
#endif // SUPPORT_GEOIP_V6
	} else {
		return (NULL);
	};

	gi = libipv6calc_db_wrapper_GeoIP_open_type(GeoIP_type, 0);
	if (gi == NULL) {
		return (NULL);
	};

	if (proto == 4) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_name_by_addr(gi, addr);
#ifdef SUPPORT_GEOIP_V6
	} else if (proto == 6) {
		if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CN_BY_ADDR) != 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6");
			GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6(gi, addr);
		} else if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_IPV6_CN_BY_IPNUM) != 0) {
			/* backward compatibility */
			ipv6calc_ipv6addr ipv6addr;
			char tempstring[NI_MAXHOST] = "";
			int result = 0;
			result = addr_to_ipv6addrstruct(addr, tempstring, sizeof(tempstring), &ipv6addr);
			if (result != 0) {
				goto END_libipv6calc_db_wrapper;
			};
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Call libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum");
			GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum_v6(gi, ipv6addr.in6_addr);
		};
#endif // SUPPORT_GEOIP_V6
	};

	if (GeoIP_result_ptr == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	if (strlen(GeoIP_result_ptr) == 0) {
		GeoIP_result_ptr = NULL;
		goto END_libipv6calc_db_wrapper;
	};

	GEOIP_DB_USAGE_MAP_TAG(GeoIP_type);

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Result for addr=%s proto=%d: %s", addr, proto, GeoIP_result_ptr);
	return(GeoIP_result_ptr);
};


/* asnum */
char *libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	char *GeoIP_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] & IPV6CALC_DB_IPV4_TO_AS) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Database/Support not available: GEOIP_ASNUM_EDITION");
			goto END_libipv6calc_db_wrapper;
		};

#if HAVE_DECL_GEOIP_ASNUM_EDITION == 1
		GeoIP_type = GEOIP_ASNUM_EDITION;
#else
		goto END_libipv6calc_db_wrapper;
#endif

#ifdef SUPPORT_GEOIP_V6
	} else if (proto == 6) {
		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] & IPV6CALC_DB_IPV6_TO_AS) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Database/Support not available: GEOIP_ASNUM_EDITION_V6");
			goto END_libipv6calc_db_wrapper;
		};

#if HAVE_DECL_GEOIP_ASNUM_EDITION_V6 == 1
		GeoIP_type = GEOIP_ASNUM_EDITION_V6;
#else
		goto END_libipv6calc_db_wrapper;
#endif

#endif // SUPPORT_GEOIP_V6
	} else {
		goto END_libipv6calc_db_wrapper;
	};

	gi = libipv6calc_db_wrapper_GeoIP_open_type(GeoIP_type, 0);

	if (gi == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	if (proto == 4) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_name_by_addr(gi, addr);
#ifdef SUPPORT_GEOIP_V6
	} else if (proto == 6) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_name_by_addr_v6(gi, addr);
#endif // SUPPORT_GEOIP_V6
	};

	if (GeoIP_result_ptr == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	GEOIP_DB_USAGE_MAP_TAG(GeoIP_type);

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Result for addr=%s proto=%d: %s", addr, proto, GeoIP_result_ptr);
	return(GeoIP_result_ptr);
};


/* record: city */
GeoIPRecord *libipv6calc_db_wrapper_GeoIP_wrapper_record_city_by_addr(const char *addr, const int proto) {
	GeoIP *gi;
	int GeoIP_type = 0;
	GeoIPRecord *GeoIP_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_GeoIP, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		GeoIP_type = GEOIP_CITY_EDITION_REV1;
#ifdef SUPPORT_GEOIP_V6
	} else if (proto == 6) {
		if ((lib_features_GeoIP & GEOIP_LIB_FEATURE_LIB_VERSION) != 0) {
#if HAVE_DECL_GEOIP_CITY_EDITION_REV1_V6 == 1
			GeoIP_type = GEOIP_CITY_EDITION_REV1_V6;
#else
			return (NULL);
#endif
		};
#endif // SUPPORT_GEOIP_V6
	} else {
		return (NULL);
	};

	gi = libipv6calc_db_wrapper_GeoIP_open_type(GeoIP_type, 0);
	if (gi == NULL) {
		return (NULL);
	};

	if (proto == 4) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_record_by_addr(gi, addr);
#ifdef SUPPORT_GEOIP_V6
	} else if (proto == 6) {
		GeoIP_result_ptr = libipv6calc_db_wrapper_GeoIP_record_by_addr_v6(gi, addr);
#endif // SUPPORT_GEOIP_V6
	};

	if (GeoIP_result_ptr == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	GEOIP_DB_USAGE_MAP_TAG(GeoIP_type);

END_libipv6calc_db_wrapper:
	return(GeoIP_result_ptr);
};

#endif


