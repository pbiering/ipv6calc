/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_IP2Location.c
 * Version    : $Id: libipv6calc_db_wrapper_IP2Location.c,v 1.16 2014/05/16 20:34:02 ds6peter Exp $
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc IP2Location database wrapper
 *    - decoupling databases from main binary
 *    - optional support of dynamic library loading (based on config.h)
 */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <errno.h>

#include "config.h"

#include "libipv6calcdebug.h"
#include "libipv6calc.h"

#include "libipv6calc_db_wrapper.h"

#ifdef SUPPORT_IP2LOCATION

#include "libipv6calc_db_wrapper_IP2Location.h"

char ip2location_db_dir[NI_MAXHOST] = IP2LOCATION_DB;

/* 
 * API_VERSION is defined as a bareword in IP2Location.h, 
 *  we need this trick to stringify it. Blah.
 */
#define makestr(x) #x
#define xmakestr(x) makestr(x)


uint32_t wrapper_features_IP2Location = 0;

#ifdef SUPPORT_IP2LOCATION_DYN
char ip2location_lib_file[NI_MAXHOST] = IP2LOCATION_DYN_LIB;

static const char* wrapper_ip2location_info = "dyn-load";
static int wrapper_ip2location_ipv6_support = IP2LOCATION_IPV6_SUPPORT_UNKNOWN;
// static int wrapper_ip2location_support      = IP2LOCATION_SUPPORT_UNKNOWN; currently not used

/* define status and dynamic load functions */
static int dl_status_IP2Location_open = IPV6CALC_DL_STATUS_UNKNOWN;
//static IP2Location* (*dl_IP2Location_open)(char *db) = NULL;
typedef IP2Location *(*dl_IP2Location_open_t)(char *db);
static union { dl_IP2Location_open_t func; void * obj; } dl_IP2Location_open;

static int dl_status_IP2Location_close = IPV6CALC_DL_STATUS_UNKNOWN;
//static uint32_t (*dl_IP2Location_close)(IP2Location *loc) = NULL;
typedef uint32_t (*dl_IP2Location_close_t)(IP2Location *loc);
static union { dl_IP2Location_close_t func; void * obj; } dl_IP2Location_close;

static int dl_status_IP2Location_get_country_short = IPV6CALC_DL_STATUS_UNKNOWN;
//static IP2LocationRecord *(*dl_IP2Location_get_country_short)(IP2Location *loc, char *ip) = NULL;
typedef IP2LocationRecord *(*dl_IP2Location_get_country_short_t)(IP2Location *loc, char *ip);
static union { dl_IP2Location_get_country_short_t func; void * obj; } dl_IP2Location_get_country_short;

static int dl_status_IP2Location_get_country_long = IPV6CALC_DL_STATUS_UNKNOWN;
//static IP2LocationRecord *(*dl_IP2Location_get_country_long)(IP2Location *loc, char *ip) = NULL;
typedef IP2LocationRecord *(*dl_IP2Location_get_country_long_t)(IP2Location *loc, char *ip);
static union { dl_IP2Location_get_country_long_t func; void * obj; } dl_IP2Location_get_country_long;

static int dl_status_IP2Location_get_all = IPV6CALC_DL_STATUS_UNKNOWN;
//static IP2LocationRecord *(*dl_IP2Location_get_all)(IP2Location *loc, char *ip) = NULL;
typedef IP2LocationRecord *(*dl_IP2Location_get_all_t)(IP2Location *loc, char *ip);
static union { dl_IP2Location_get_all_t func; void * obj; } dl_IP2Location_get_all;

static int dl_status_IP2Location_free_record = IPV6CALC_DL_STATUS_UNKNOWN;
//static IP2LocationRecord* (*dl_IP2Location_free_record)(IP2LocationRecord *record) = NULL;
typedef IP2LocationRecord *(*dl_IP2Location_free_record_t)(IP2LocationRecord *record);
static union { dl_IP2Location_free_record_t func; void * obj; } dl_IP2Location_free_record;

#else // SUPPORT_IP2LOCATION_DYN
static const char* wrapper_ip2location_info = "built-in";
#ifdef SUPPORT_IP2LOCATION_V6
static int wrapper_ip2location_ipv6_support = IP2LOCATION_IPV6_SUPPORT_FULL;
#else
static int wrapper_ip2location_ipv6_support = IP2LOCATION_IPV6_SUPPORT_UNKNOWN;
#endif
#endif // SUPPORT_IP2LOCATION_DYN

static int ip2location_country_v4 = 0;
static int ip2location_country_v6 = 0;
static int ip2location_asnum_v4 = 0;
static int ip2location_asnum_v6 = 0;
static int ip2location_city_v4 = 0;
static int ip2location_city_v6 = 0;

static void *dl_IP2Location_handle = NULL;

char ***libipv6calc_db_wrapper_IP2LocationDBFileName_ptr = NULL;
const char **libipv6calc_db_wrapper_IP2LocationDBDescription = NULL;

/* database usage map */
#define IP2LOCATION_DB_MAX_BLOCKS_32	2	// 0-63
static uint32_t ip2location_db_usage_map[IP2LOCATION_DB_MAX_BLOCKS_32];

#define IP2LOCATION_DB_USAGE_MAP_TAG(db)	if (db < (32 * IP2LOCATION_DB_MAX_BLOCKS_32)) { \
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location, "Tag usage for db: %d", db); \
							ip2location_db_usage_map[db / 32] |= 1 << (db % 32); \
						} else { \
							fprintf(stderr, "FIXME: unsupported db value (exceed limit): %d (%d)\n", db, 32 * IP2LOCATION_DB_MAX_BLOCKS_32 - 1); \
							exit(1); \
						};

char ip2location_db_usage_string[NI_MAXHOST] = "";


// local prototyping
static char     *libipv6calc_db_wrapper_IP2Location_dbfilename(int type); 


/*
 * function initialise the IP2Location wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_IP2Location_wrapper_init(void) {
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_IP2LOCATION_DYN
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Load library: %s", ip2location_lib_file);

	dl_IP2Location_handle = dlopen(ip2location_lib_file, RTLD_NOW | RTLD_LOCAL);

	if (dl_IP2Location_handle == NULL) {
		NONQUIETPRINT_WA("IP2Location dynamic library load failed: %s", dlerror());
		return(1);
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Loaded library successful: %s", ip2location_lib_file);
#else
	// nothing to set for the moment
#endif

#ifdef SUPPORT_IP2LOCATION_DYN
	// nothing to set for the moment
#else
	// nothing to set for the moment
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Check for standard IP2Location databases");

	/* check available databases for resolution */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location_db_file_desc); i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "IP2Location database test for availability: %s", libipv6calc_db_wrapper_IP2Location_db_file_desc[i].filename);
		if (libipv6calc_db_wrapper_IP2Location_db_avail(libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number) == 1) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "IP2Location database available: %s", libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description);
			wrapper_features_IP2Location |= libipv6calc_db_wrapper_IP2Location_db_file_desc[i].feature;
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Version of linked library: %s / IPv6 support: %s / custom directory: %s", libipv6calc_db_wrapper_IP2Location_lib_version(), libipv6calc_db_wrapper_IP2Location_IPv6_support[wrapper_ip2location_ipv6_support].token, ip2location_db_dir);

	wrapper_features |= wrapper_features_IP2Location;

	return 0;
};


/*
 * function cleanup the IP2Location wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_IP2Location_wrapper_cleanup(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_IP2LOCATION
	// no general cleanup provided
#endif

	dl_IP2Location_handle = NULL; // disable handle

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
	return 0;
};


/*
 * function info of IP2Location wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_IP2Location_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_IP2LOCATION
	snprintf(string, size, "IP2Location available databases: Country4=%d Country6=%d ASN4=%d ASN6=%d City4=%d City6=%d", ip2location_country_v4, ip2location_country_v6, ip2location_asnum_v4, ip2location_asnum_v6, ip2location_city_v4, ip2location_city_v6);
#else
	snprintf(string, size, "No IP2Location support built-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
	return;
};

/*
 * function print database info of IP2Location wrapper
 *
 * in : (void)
 * out: (void)
 */
void libipv6calc_db_wrapper_IP2Location_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	IP2Location *loc;
	int i, type, count = 0;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	printf("%sIP2Location: features: 0x%08x\n", prefix, wrapper_features_IP2Location);

#ifdef SUPPORT_IP2LOCATION
	printf("%sIP2Location: info of available databases in directory: %s\n", prefix, ip2location_db_dir);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location_db_file_desc); i++) {
		type = libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number;

#ifdef SUPPORT_IP2LOCATION_DYN
		if (dl_IP2Location_handle == NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Check whether db file exists: %s", libipv6calc_db_wrapper_IP2Location_dbfilename(type));
			if (access(libipv6calc_db_wrapper_IP2Location_dbfilename(type), R_OK) == 0) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "DB file exists: %s", libipv6calc_db_wrapper_IP2Location_dbfilename(type));
				printf("%sIP2Location: %-27s: %-40s (LIBRARY-NOT-LOADED)\n", prefix, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description, libipv6calc_db_wrapper_IP2Location_dbfilename(type));
			} else {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "DB file doesn't exist or can't open: %s (%s)", libipv6calc_db_wrapper_IP2Location_dbfilename(type), strerror(errno));
				if (level_verbose == LEVEL_VERBOSE2) {
					printf("%sIP2Location: %-27s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description, libipv6calc_db_wrapper_IP2Location_dbfilename(type), strerror(errno));
				};
			};
		} else {
#endif // SUPPORT_IP2LOCATION_DYN
		if (libipv6calc_db_wrapper_IP2Location_db_avail(type)) {
			// IP2Location returned that database is available
			loc = libipv6calc_db_wrapper_IP2Location_open_type(type);
			if (loc == NULL) {
				printf("%sIP2Location: %-27s: %-40s (CAN'T OPEN)\n", prefix, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description, libipv6calc_db_wrapper_IP2Location_dbfilename(type));
			} else {
				printf("%sIP2Location: %-27s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description, libipv6calc_db_wrapper_IP2Location_dbfilename(type), libipv6calc_db_wrapper_IP2Location_database_info(loc));
				libipv6calc_db_wrapper_IP2Location_close(loc);
				count++;
			};
		} else {
			if (level_verbose == LEVEL_VERBOSE2) {
				printf("%sIP2Location: %-27s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description, libipv6calc_db_wrapper_IP2Location_dbfilename(type), strerror(errno));
			};
			continue;
		};
#ifdef SUPPORT_IP2LOCATION_DYN
		};
#endif // SUPPORT_IP2LOCATION_DYN
	};

	if (count == 0) {
		printf("%sIP2Location: NO available databases found in directory: %s\n", prefix, ip2location_db_dir);
	};
#else // SUPPORT_IP2LOCATION
	snprintf(string, size, "%sNo IP2Location support built-in", prefix);
#endif // SUPPORT_IP2LOCATION

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
	return;
};


/*
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_IP2Location_wrapper_db_info_used(void) {
	int db;
	IP2Location *loc;
	char tempstring[NI_MAXHOST];
	char *info;

	for (db = 0; db < 32 * IP2LOCATION_DB_MAX_BLOCKS_32; db++) {
		if ((ip2location_db_usage_map[db / 32] & (1 << (db % 32))) != 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_IP2Location, "DB used: %d", db);

			loc = libipv6calc_db_wrapper_IP2Location_open_type(db);
			info = libipv6calc_db_wrapper_IP2Location_database_info(loc);

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			if (strlen(ip2location_db_usage_string) > 0) {
				if (strstr(ip2location_db_usage_string, info) != NULL) { continue; }; // string already included

				snprintf(tempstring, sizeof(tempstring), "%s / %s", ip2location_db_usage_string, info);
			} else {
				snprintf(tempstring, sizeof(tempstring), "%s", info);
			};

			snprintf(ip2location_db_usage_string, sizeof(ip2location_db_usage_string), "%s", tempstring);
		};
	};

	return(ip2location_db_usage_string);
};



#ifdef SUPPORT_IP2LOCATION

/*******************************
 * Wrapper extensenion functions for IP2Location
 * because of missing support in library itself
 *******************************/

/*
 * wrapper extension: IP2Location_lib_version
 */
const char * libipv6calc_db_wrapper_IP2Location_lib_version(void) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s", wrapper_ip2location_info);

#ifdef SUPPORT_IP2LOCATION_DYN
	const char *result_IP2Location_lib_version;
	if (dl_IP2Location_handle == NULL) {
		result_IP2Location_lib_version = "LIBARY-NOT-LOADED";
	} else {
		result_IP2Location_lib_version = "version-not-provided";
	};

	return(result_IP2Location_lib_version);
#else
	return("API version: " xmakestr(API_VERSION));
#endif
};


/*
 * wrapper extension: IP2Location_dbfilename
 */
static char *libipv6calc_db_wrapper_IP2Location_dbfilename(int type) {
	static char tempstring[NI_MAXHOST];
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s type=%d", wrapper_ip2location_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	snprintf(tempstring, sizeof(tempstring), "%s/%s", ip2location_db_dir, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].filename);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished: %s type=%d has filename=%s", wrapper_ip2location_info, type, tempstring);

	return(tempstring);
};


/*
 * wrapper extension: IP2Location_dbdescription
 */
const char *libipv6calc_db_wrapper_IP2Location_dbdescription(int type) {
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s type=%d", wrapper_ip2location_info, type);

	for (i = 0; i < sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc) / sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc[0]); i++) {
		if (libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return("unknown");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished: %s type=%d has description=%s", wrapper_ip2location_info, type, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description);

	return(libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description);
};


/*
 * wrapper extension: IP2Location_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_IP2Location_db_avail(int type) {
	char *filename;
	int r = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s type=%d", wrapper_ip2location_info, type);

	filename = libipv6calc_db_wrapper_IP2Location_dbfilename(type);

	if (filename == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	r = (access(filename, R_OK) == 0) ? 1:0;

	if (r == 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished: %s type=%d (still unknown) (r=%d: %s)", wrapper_ip2location_info, type, r, strerror(errno));
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished: %s type=%d (%s) (r=%d)", wrapper_ip2location_info, type, filename, r);
	};

END_libipv6calc_db_wrapper:
	return(r);
};


/*
 * wrapper extension: IP2Location_open_type
 */
IP2Location *libipv6calc_db_wrapper_IP2Location_open_type(int type) {
	char *filename;
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s type=%d", wrapper_ip2location_info, type);

	for (i = 0; i < sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc) / sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc[0]); i++) {
		if (libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	filename = libipv6calc_db_wrapper_IP2Location_dbfilename(type);

	if (filename == NULL) {
		return(NULL);
	};

	return(libipv6calc_db_wrapper_IP2Location_open(filename));
};


/*******************************
 * Wrapper functions for IP2Location
 *******************************/



/*
 * wrapper: IP2Location_open
 */
IP2Location *libipv6calc_db_wrapper_IP2Location_open(char *db) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s filename=%s", wrapper_ip2location_info, db);

#ifdef SUPPORT_IP2LOCATION_DYN
	IP2Location *loc = NULL;
	const char *dl_symbol = "IP2Location_open";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_open == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_open.obj) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_open = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_open = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_IP2Location_open == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	loc = (*dl_IP2Location_open.func)(db);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(loc);
#else
	return(IP2Location_open(db));
#endif
};


/*
 * wrapper: IP2Location_close
 */
uint32_t libipv6calc_db_wrapper_IP2Location_close(IP2Location *loc) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s", wrapper_ip2location_info);

#ifdef SUPPORT_IP2LOCATION_DYN
	uint32_t result = 0;
	const char *dl_symbol = "IP2Location_close";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_close == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_close.obj) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_close = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_close = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_IP2Location_close == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	result = (*dl_IP2Location_close.func)(loc);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result);
#else
	return(IP2Location_close(loc));
#endif
};


/*
 * wrapper: IP2Location_database_info
 */
char *libipv6calc_db_wrapper_IP2Location_database_info(IP2Location *loc) {
	static char resultstring[NI_MAXHOST];

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s", wrapper_ip2location_info);

	if (loc == NULL) {
		snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve database information");
	} else {
		snprintf(resultstring, sizeof(resultstring), "IP2L-DB%d %04d%02d%02d Copyright (c) %04d IP2Location All Rights Reserved", 
                                loc->databasetype,
				loc->databaseyear + 2000,
                                loc->databasemonth,
                                loc->databaseday,
				loc->databaseyear + 2000
		);
	};

	return(resultstring);
};


/*
 * wrapper: IP2Location_free_record
 */ 
void libipv6calc_db_wrapper_IP2Location_free_record(IP2LocationRecord *record) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s", wrapper_ip2location_info);

#ifdef SUPPORT_IP2LOCATION_DYN
	const char *dl_symbol = "IP2Location_free_record";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_free_record == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_free_record) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_free_record = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_free_record = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_IP2Location_free_record == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	(*dl_IP2Location_free_record.func)(record);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
#else
	IP2Location_free_record(record);
#endif
	return;
};


/*
 * wrapper: IP2Location_get_country_short
 */
IP2LocationRecord *libipv6calc_db_wrapper_IP2Location_get_country_short(IP2Location *loc, char *ip) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s ip=%s", wrapper_ip2location_info, ip);

#ifdef SUPPORT_IP2LOCATION_DYN
	IP2LocationRecord *result_IP2Location_get_country_short = NULL;
	const char *dl_symbol = "IP2Location_get_country_short";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_get_country_long == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_get_country_short.obj) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_get_country_short = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_get_country_short = IPV6CALC_DL_STATUS_OK;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_IP2Location_get_country_long == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	result_IP2Location_get_country_short = (*dl_IP2Location_get_country_short.func)(loc, ip);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_IP2Location_get_country_short);
#else
	return(IP2Location_get_country_short(loc, ip));
#endif
};


/*
 * wrapper: IP2Location_get_country_long
 */
IP2LocationRecord *libipv6calc_db_wrapper_IP2Location_get_country_long(IP2Location *loc, char *ip) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s ip=%s", wrapper_ip2location_info, ip);

#ifdef SUPPORT_IP2LOCATION_DYN
	IP2LocationRecord *result_IP2Location_get_country_long = NULL;
	const char *dl_symbol = "IP2Location_get_country_long";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_get_country_long == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_get_country_long.obj) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_get_country_long = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_get_country_long = IPV6CALC_DL_STATUS_OK;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_IP2Location_get_country_long == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	result_IP2Location_get_country_long = (*dl_IP2Location_get_country_long.func)(loc, ip);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_IP2Location_get_country_long);
#else
	return(IP2Location_get_country_long(loc, ip));
#endif
};


/*
 * wrapper: IP2Location_get_all
 */
IP2LocationRecord *libipv6calc_db_wrapper_IP2Location_get_all(IP2Location *loc, char *ip) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s", wrapper_ip2location_info);

	if (loc == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "loc==NULL, return NULL");
		return(NULL);
	};

#ifdef SUPPORT_IP2LOCATION_DYN
	IP2LocationRecord *result_IP2Location_get_all = NULL;
	const char *dl_symbol = "IP2Location_get_all";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_get_all == IPV6CALC_DL_STATUS_UNKNOWN) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call dlsym: %s", dl_symbol);

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_get_all.obj) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_get_all = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_get_all = IPV6CALC_DL_STATUS_OK;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called dlsym successful: %s", dl_symbol);
	} else if (dl_status_IP2Location_get_all == IPV6CALC_DL_STATUS_ERROR) {
		/* already known issue */
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already failed: %s", dl_symbol);
		goto END_libipv6calc_db_wrapper;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Previous call of dlsym already successful: %s", dl_symbol);
	};

	result_IP2Location_get_all = (*dl_IP2Location_get_all.func)(loc, ip);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto END_libipv6calc_db_wrapper;
	};

END_libipv6calc_db_wrapper:
	return(result_IP2Location_get_all);
#else
	return(IP2Location_get_all(loc, ip));
#endif
};


/*********************************************
 * Abstract functions
 * *******************************************/

/* function query for feature set
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_IP2Location_has_features(uint32_t features) {
	int result = -1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with feature value to test: 0x%08x", features);

	if ((wrapper_features_IP2Location & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Return with result: %d", result);
	return(result);
};

/* country_code */
char *libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr(char *addr, const int proto) {
	IP2Location *loc;
	IP2LocationRecord *record;

	int IP2Location_type = 0;
	char *IP2Location_result_ptr = NULL;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		IP2Location_type = IP2LOCATION_DB_IP_COUNTRY;
	} else if (proto == 6) {
		IP2Location_type = IP2LOCATION_DB_IPV6_COUNTRY;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Unsupported proto: %d", proto);
		goto END_libipv6calc_db_wrapper;
	};

	loc = libipv6calc_db_wrapper_IP2Location_open_type(IP2Location_type);

	if (loc == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Error opening IP2Location by type");
		goto END_libipv6calc_db_wrapper;
	};

	record = libipv6calc_db_wrapper_IP2Location_get_country_short(loc, addr);

	if (record == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "libipv6calc_db_wrapper_IP2Location_get_country_short did not return a record");
		goto END_libipv6calc_db_wrapper_close;
	};

	IP2Location_result_ptr = record->country_short;

	if (IP2Location_result_ptr == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "libipv6calc_db_wrapper_IP2Location_get_country_short did not contain a country_short code");
		goto END_libipv6calc_db_wrapper_close;
	};

	if (strlen(IP2Location_result_ptr) > 2) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "libipv6calc_db_wrapper_IP2Location_get_country_short did not return a proper country_short code (length > 2): %s", IP2Location_result_ptr);
		goto END_libipv6calc_db_wrapper_close;
	};

	IP2LOCATION_DB_USAGE_MAP_TAG(IP2Location_type);

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_IP2Location_close(loc);

END_libipv6calc_db_wrapper:
	return(IP2Location_result_ptr);
};

/* country_name */
char *libipv6calc_db_wrapper_IP2Location_wrapper_country_name_by_addr(char *addr, const int proto) {
	IP2Location *loc;
	IP2LocationRecord *record;

	int IP2Location_type = 0;
	char *IP2Location_result_ptr;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		IP2Location_type = IP2LOCATION_DB_IP_COUNTRY;
	} else if (proto == 6) {
		IP2Location_type = IP2LOCATION_DB_IPV6_COUNTRY;
	} else {
		return (NULL);
	};

	loc = libipv6calc_db_wrapper_IP2Location_open_type(IP2Location_type);

	if (loc == NULL) {
		return (NULL);
	};

	record = libipv6calc_db_wrapper_IP2Location_get_country_long(loc, addr);

	if (record == NULL) {
		return (NULL);
	};

	IP2Location_result_ptr = record->country_long;

	if (IP2Location_result_ptr == NULL) {
		return (NULL);
	};

	IP2LOCATION_DB_USAGE_MAP_TAG(IP2Location_type);

	libipv6calc_db_wrapper_IP2Location_close(loc);

	return(IP2Location_result_ptr);
};

#endif
#endif
