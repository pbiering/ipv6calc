/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_IP2Location.c
 * Version    : $Id: libipv6calc_db_wrapper_IP2Location.c,v 1.2 2013/09/21 17:25:56 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc IP2Location database wrapper
 *    - decoupling databases from main binary
 *    - optional support of dynamic library loading (based on config.h)
 */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/stat.h>

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
static IP2Location* (*dl_IP2Location_open)(char *db) = NULL;

static int dl_status_IP2Location_close = IPV6CALC_DL_STATUS_UNKNOWN;
static uint32_t (*dl_IP2Location_close)(IP2Location *loc) = NULL;

static int dl_status_IP2Location_get_country_short = IPV6CALC_DL_STATUS_UNKNOWN;
static IP2LocationRecord *(*dl_IP2Location_get_country_short)(IP2Location *loc, char *ip) = NULL;

static int dl_status_IP2Location_get_country_long = IPV6CALC_DL_STATUS_UNKNOWN;
static IP2LocationRecord *(*dl_IP2Location_get_country_long)(IP2Location *loc, char *ip) = NULL;

static int dl_status_IP2Location_get_all = IPV6CALC_DL_STATUS_UNKNOWN;
static IP2LocationRecord *(*dl_IP2Location_get_all)(IP2Location *loc, char *ip) = NULL;

static int dl_status_IP2Location_free_record = IPV6CALC_DL_STATUS_UNKNOWN;
static IP2LocationRecord* (*dl_IP2Location_free_record)(IP2LocationRecord *record) = NULL;

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

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_IP2LOCATION_DYN
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Load library: %s\n", __FILE__, __func__, ip2location_lib_file);
	};

	dl_IP2Location_handle = dlopen(ip2location_lib_file, RTLD_NOW | RTLD_LOCAL);

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "%s/%s: Loading of library failed: %s\n", __FILE__, __func__, ip2location_lib_file);
		return(1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Loaded library successful: %s\n", __FILE__, __func__, ip2location_lib_file);
	};
#else
	// nothing to set for the moment
#endif

#ifdef SUPPORT_IP2LOCATION_DYN
	// nothing to set for the moment
#else
	// nothing to set for the moment
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Check for standard IP2Location databases\n", __FILE__, __func__);
	};

	/* check available databases for resolution */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_IP2Location_db_file_desc); i++) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: IP2Location database test for availability: %s\n", __FILE__, __func__, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].filename);
		};
		if (libipv6calc_db_wrapper_IP2Location_db_avail(libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number) == 1) {
			if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
				fprintf(stderr, "%s/%s: IP2Location database available: %s\n", __FILE__, __func__, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description);
			};
			wrapper_features_IP2Location |= libipv6calc_db_wrapper_IP2Location_db_file_desc[i].feature;
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Version of linked library: %s / IPv6 support: %s / custom directory: %s\n", __FILE__, __func__, libipv6calc_db_wrapper_IP2Location_lib_version(), libipv6calc_db_wrapper_IP2Location_IPv6_support[wrapper_ip2location_ipv6_support].token, ip2location_db_dir);
	};

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_IP2LOCATION
	// no general cleanup provided
#endif

	dl_IP2Location_handle = NULL; // disable handle

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return 0;
};


/*
 * function info of IP2Location wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_IP2Location_wrapper_info(char* string, const size_t size) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_IP2LOCATION
	snprintf(string, size, "IP2Location available databases: Country4=%d Country6=%d ASN4=%d ASN6=%d City4=%d City6=%d", ip2location_country_v4, ip2location_country_v6, ip2location_asnum_v4, ip2location_asnum_v6, ip2location_city_v4, ip2location_city_v6);
#else
	snprintf(string, size, "No IP2Location support built-in");
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
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
	int i, type;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	printf("%sIP2Location: features: 0x%08x\n", prefix, wrapper_features_IP2Location);

#ifdef SUPPORT_IP2LOCATION
	printf("%sIP2Location: info of available databases in directory: %s\n", prefix, ip2location_db_dir);
	for (i = 0; i < sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc) / sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc[0]); i++) {
		type = libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number;

		if (libipv6calc_db_wrapper_IP2Location_db_avail(type)) {
			// IP2Location returned that database is available
			loc = libipv6calc_db_wrapper_IP2Location_open_type(type);
			if (loc == NULL) {
				printf("%sIP2Location: %-27s: %-40s (CAN'T OPEN)\n", prefix, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description, libipv6calc_db_wrapper_IP2Location_dbfilename(type));
			} else {
				printf("%sIP2Location: %-27s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description, libipv6calc_db_wrapper_IP2Location_dbfilename(type), libipv6calc_db_wrapper_IP2Location_database_info(loc));
				libipv6calc_db_wrapper_IP2Location_close(loc);
			};
		} else {
			continue;
		};
	};
#else
	snprintf(string, size, "%sNo IP2Location support built-in", prefix);
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return;
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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_ip2location_info);
	};

#ifdef SUPPORT_IP2LOCATION_DYN
	char *result_IP2Location_lib_version = "unsupported";

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

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s type=%d\n", __FILE__, __func__, wrapper_ip2location_info, type);
	};

	for (i = 0; i < sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc) / sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc[0]); i++) {
		if (libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	snprintf(tempstring, sizeof(tempstring), "%s/%s", ip2location_db_dir, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].filename);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished: %s type=%d has filename=%s\n", __FILE__, __func__, wrapper_ip2location_info, type, tempstring);
	};

	return(tempstring);
};


/*
 * wrapper extension: IP2Location_dbdescription
 */
const char *libipv6calc_db_wrapper_IP2Location_dbdescription(int type) {
	int  entry = -1, i;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s type=%d\n", __FILE__, __func__, wrapper_ip2location_info, type);
	};

	for (i = 0; i < sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc) / sizeof(libipv6calc_db_wrapper_IP2Location_db_file_desc[0]); i++) {
		if (libipv6calc_db_wrapper_IP2Location_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return("unknown");
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished: %s type=%d has description=%s\n", __FILE__, __func__, wrapper_ip2location_info, type, libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description);
	};

	return(libipv6calc_db_wrapper_IP2Location_db_file_desc[i].description);
};


/*
 * wrapper extension: IP2Location_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_IP2Location_db_avail(int type) {
	char *filename;
	struct stat file_stat;
	int r = 0;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s type=%d\n", __FILE__, __func__, wrapper_ip2location_info, type);
	};

	filename = libipv6calc_db_wrapper_IP2Location_dbfilename(type);

	if (filename == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	r = (stat(filename, &file_stat) == 0) ? 1:0;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		if (r == 0) {
			fprintf(stderr, "%s/%s: Finished: %s type=%d (still unknown) (r=%d)\n", __FILE__, __func__, wrapper_ip2location_info, type, r);
		} else {
			fprintf(stderr, "%s/%s: Finished: %s type=%d (%s) (r=%d)\n", __FILE__, __func__, wrapper_ip2location_info, type, filename, r);
		};
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

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s type=%d\n", __FILE__, __func__, wrapper_ip2location_info, type);
	};

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s filename=%s\n", __FILE__, __func__, wrapper_ip2location_info, db);
	};

#ifdef SUPPORT_IP2LOCATION_DYN
	IP2Location *loc = NULL;
	const char *dl_symbol = "IP2Location_open";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_open == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_open) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_open = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_open = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_IP2Location_open == IPV6CALC_DL_STATUS_ERROR) {
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

	loc = (*dl_IP2Location_open)(db);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_ip2location_info);
	};

#ifdef SUPPORT_IP2LOCATION_DYN
	uint32_t result = 0;
	const char *dl_symbol = "IP2Location_close";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_close == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_close) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_close = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_close = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_IP2Location_close == IPV6CALC_DL_STATUS_ERROR) {
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

	result = (*dl_IP2Location_close)(loc);

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

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_ip2location_info);
	};

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_ip2location_info);
	};

#ifdef SUPPORT_IP2LOCATION_DYN
	const char *dl_symbol = "IP2Location_free_record";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_free_record == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_free_record) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_free_record = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_free_record = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_IP2Location_free_record == IPV6CALC_DL_STATUS_ERROR) {
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

	(*dl_IP2Location_free_record)(record);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_ip2location_info);
	};

#ifdef SUPPORT_IP2LOCATION_DYN
	IP2LocationRecord *result_IP2Location_get_country_short = NULL;
	const char *dl_symbol = "IP2Location_get_country_short";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_get_country_long == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_get_country_short) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_get_country_short = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_get_country_short = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_IP2Location_get_country_long == IPV6CALC_DL_STATUS_ERROR) {
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

	result_IP2Location_get_country_short = (*dl_IP2Location_get_country_short)(loc, ip);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_ip2location_info);
	};

#ifdef SUPPORT_IP2LOCATION_DYN
	IP2LocationRecord *result_IP2Location_get_country_long = NULL;
	const char *dl_symbol = "IP2Location_get_country_long";
	char *error;

	if (dl_IP2Location_handle == NULL) {
		fprintf(stderr, "dl_IP2Location handle not defined\n");
		goto END_libipv6calc_db_wrapper;
	};

	if (dl_status_IP2Location_get_country_long == IPV6CALC_DL_STATUS_UNKNOWN) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_get_country_long) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_get_country_long = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_get_country_long = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_IP2Location_get_country_long == IPV6CALC_DL_STATUS_ERROR) {
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

	result_IP2Location_get_country_long = (*dl_IP2Location_get_country_long)(loc, ip);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: %s\n", __FILE__, __func__, wrapper_ip2location_info);
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
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call dlsym: %s\n", __FILE__, __func__, dl_symbol);
		};

		dlerror();    /* Clear any existing error */

		*(void **) (&dl_IP2Location_get_all) = dlsym(dl_IP2Location_handle, dl_symbol);

		if ((error = dlerror()) != NULL)  {
			dl_status_IP2Location_get_all = IPV6CALC_DL_STATUS_ERROR;
			fprintf(stderr, "%s\n", error);
			goto END_libipv6calc_db_wrapper;
		};

		dl_status_IP2Location_get_all = IPV6CALC_DL_STATUS_OK;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Called dlsym successful: %s\n", __FILE__, __func__, dl_symbol);
		};
	} else if (dl_status_IP2Location_get_all == IPV6CALC_DL_STATUS_ERROR) {
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

	result_IP2Location_get_all = (*dl_IP2Location_get_all)(loc, ip);

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called with feature value to test: 0x%08x\n", __FILE__, __func__, features);
	};

	if ((wrapper_features_IP2Location & features) == features) {
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
char *libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr(char *addr, const int proto) {
	IP2Location *loc;
	IP2LocationRecord *record;

	int IP2Location_type = 0;
	char *IP2Location_result_ptr;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called with addr=%s proto=%d\n", __FILE__, __func__, addr, proto);
	};

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

	record = libipv6calc_db_wrapper_IP2Location_get_country_short(loc, addr);

	if (record == NULL) {
		return (NULL);
	};

	IP2Location_result_ptr = record->country_short;

	if (IP2Location_result_ptr == NULL) {
		return (NULL);
	};

	if (strlen(IP2Location_result_ptr) > 2) {
		return (NULL);
	};

	libipv6calc_db_wrapper_IP2Location_close(loc);

	return(IP2Location_result_ptr);
};

/* country_name */
char *libipv6calc_db_wrapper_IP2Location_wrapper_country_name_by_addr(char *addr, const int proto) {
	IP2Location *loc;
	IP2LocationRecord *record;

	int IP2Location_type = 0;
	char *IP2Location_result_ptr;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called with addr=%s proto=%d\n", __FILE__, __func__, addr, proto);
	};

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

	libipv6calc_db_wrapper_IP2Location_close(loc);

	return(IP2Location_result_ptr);
};

#endif
#endif