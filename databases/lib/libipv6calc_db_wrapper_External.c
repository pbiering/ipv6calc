/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_External.c
 * Version    : $Id: libipv6calc_db_wrapper_External.c,v 1.3 2014/10/11 11:33:41 ds6peter Exp $
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc External (superseeding BuiltIn) database wrapper
 */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "config.h"

#include "libipv6calcdebug.h"
#include "libipv6calc.h"

#include "libipv6calc_db_wrapper.h"


#ifdef SUPPORT_EXTERNAL

#include <db.h>

#include "libipv6calc_db_wrapper_External.h"

char external_db_dir[NI_MAXHOST] = EXTERNAL_DB;

static const char* wrapper_external_info = "External";


/* database usage map */
#define EXTERNAL_DB_MAX_BLOCKS_32	2	// 0-63
static uint32_t external_db_usage_map[EXTERNAL_DB_MAX_BLOCKS_32];

#define EXTERNAL_DB_USAGE_MAP_TAG(db)	if (db < (32 * EXTERNAL_DB_MAX_BLOCKS_32)) { \
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Tag usage for db: %d", db); \
							external_db_usage_map[db / 32] |= 1 << (db % 32); \
						} else { \
							fprintf(stderr, "FIXME: unsupported db value (exceed limit): %d (%d)\n", db, 32 * EXTERNAL_DB_MAX_BLOCKS_32 - 1); \
							exit(1); \
						};

char external_db_usage_string[NI_MAXHOST] = "";

// local cache
#define IPV6CALC_DBD_SUBDB_MAX 2
static DB *db_ptr_cache[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc)][IPV6CALC_DBD_SUBDB_MAX];
static db_recno_t db_recno_max_cache[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc)][IPV6CALC_DBD_SUBDB_MAX];

// creation time of databases
time_t wrapper_db_unixtime_External[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc)];


// local prototyping
static char     *libipv6calc_db_wrapper_External_dbfilename(int type); 
static char     *libipv6calc_db_wrapper_External_database_info(int type);


/*
 * function initialise the External wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_External_wrapper_init(void) {
	int i, j;
	char *result;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Called");

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Check for External databases in directory: %s", external_db_dir);

	/* check available databases for resolution */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		// clean local cache
		for (j = 0; j < IPV6CALC_DBD_SUBDB_MAX; j++) {
			db_ptr_cache[i][j] = NULL;
			db_recno_max_cache[i][j] = -1;
		};
		wrapper_db_unixtime_External[i] = 0;

		// add features to implemented
		wrapper_features_by_source_implemented[IPV6CALC_DB_SOURCE_EXTERNAL] |= libipv6calc_db_wrapper_External_db_file_desc[i].features;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "External database test for availability: %s", libipv6calc_db_wrapper_External_db_file_desc[i].filename);

		if (libipv6calc_db_wrapper_External_db_avail(libipv6calc_db_wrapper_External_db_file_desc[i].number) != 1) {
			// no file found
			continue;
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "External database available: %s type=%d", libipv6calc_db_wrapper_External_db_file_desc[i].description, libipv6calc_db_wrapper_External_db_file_desc[i].number);

		result = libipv6calc_db_wrapper_External_database_info(libipv6calc_db_wrapper_External_db_file_desc[i].number);

		if (strlen(result) == 0) {
			// no proper database
			continue;
		};

		if (wrapper_db_unixtime_External[i] == 0) {
			// no proper database
			continue;
		};

		// finally mark database features as available
		wrapper_features_by_source[IPV6CALC_DB_SOURCE_EXTERNAL] |= libipv6calc_db_wrapper_External_db_file_desc[i].features;
	};

	wrapper_features |= wrapper_features_by_source[IPV6CALC_DB_SOURCE_EXTERNAL];

	return 0;
};


/*
 * wrapper: External_close
 */
static int libipv6calc_db_wrapper_External_close(DB *dbp) {
	int i, j;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Called");

	if (dbp != NULL) {
		/* cleanup cache entry */
		for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
			for (j = 0; j < IPV6CALC_DBD_SUBDB_MAX; j++) {
				if (db_ptr_cache[i][j] == dbp) {
					db_ptr_cache[i][j] = NULL;
					db_recno_max_cache[i][j] = -1;
				};
			};
		};

		dbp->close(dbp, 0);
	};

	return(0);
};


/*
 * function cleanup the External wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_External_wrapper_cleanup(void) {
	int i, j;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Called");

#ifdef SUPPORT_EXTERNAL
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		if (db_ptr_cache[i] != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Close External: type=%d desc='%s'", libipv6calc_db_wrapper_External_db_file_desc[i].number, libipv6calc_db_wrapper_External_db_file_desc[i].description);
			for (j = 0; j < IPV6CALC_DBD_SUBDB_MAX; j++) {
				libipv6calc_db_wrapper_External_close(db_ptr_cache[i][j]);
			};
		};
	};
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Finished");
	return 0;
};


/*
 * function info of External wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_External_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Called");

#ifdef SUPPORT_EXTERNAL
	//snprintf(string, size, "External available databases: Country4=%d Country6=%d ASN4=%d ASN6=%d City4=%d City6=%d", (wrapper_features & IPV6CALC_DB_IPV4_TO_CC) ? 1 : 0, (wrapper_features & IPV6CALC_DB_IPV6_TO_CC) ? 1 : 0, external_asnum_v4, external_asnum_v6, external_city_v4, external_city_v6);
	snprintf(string, size, "External available databases: TODO");
#else
	snprintf(string, size, "No External support built-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Finished");
	return;
};

/*
 * function print database info of External wrapper
 *
 * in : (void)
 * out: (void)
 */
void libipv6calc_db_wrapper_External_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	int i, type, count = 0;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Called");

	IPV6CALC_DB_FEATURE_INFO(prefix, IPV6CALC_DB_SOURCE_EXTERNAL)

#ifdef SUPPORT_EXTERNAL
	printf("%sExternal: info of available databases in directory: %s\n", prefix, external_db_dir);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		type = libipv6calc_db_wrapper_External_db_file_desc[i].number;

		if (libipv6calc_db_wrapper_External_db_avail(type)) {
			printf("%sExternal: %-27s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_External_db_file_desc[i].description, libipv6calc_db_wrapper_External_dbfilename(type), libipv6calc_db_wrapper_External_database_info(type));
			count++;
		} else {
			if (level_verbose == LEVEL_VERBOSE2) {
				printf("%sExternal: %-27s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_External_db_file_desc[i].description, libipv6calc_db_wrapper_External_dbfilename(type), strerror(errno));
			};
			continue;
		};
	};

	if (count == 0) {
		printf("%sExternal: NO available databases found in directory: %s\n", prefix, external_db_dir);
	};
#else // SUPPORT_EXTERNAL
	snprintf(string, size, "%sNo External support built-in", prefix);
#endif // SUPPORT_EXTERNAL

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Finished");
	return;
};


/*
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_External_wrapper_db_info_used(void) {
	int type, i;
	char tempstring[NI_MAXHOST];
	char *info;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Called");

	for (i = 0; i < EXTERNAL_DB_MAX_BLOCKS_32; i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "external_db_usage_map[%d]=%08x", i, (unsigned int) external_db_usage_map[i]);
	};

	for (type = 0; type < 32 * EXTERNAL_DB_MAX_BLOCKS_32; type++) {
		if ((external_db_usage_map[type / 32] & (1 << (type % 32))) != 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "DB type used: %d", type);

			info = libipv6calc_db_wrapper_External_database_info(type);

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "type=%d info=%s", type, info);

			if (strlen(external_db_usage_string) > 0) {
				if (strstr(external_db_usage_string, info) != NULL) {
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "type=%d info=%s (skip, already displayed)", type, info);
					continue;
				}; // string already included

				snprintf(tempstring, sizeof(tempstring), "%s / %s", external_db_usage_string, info);
			} else {
				snprintf(tempstring, sizeof(tempstring), "%s", info);
			};

			snprintf(external_db_usage_string, sizeof(external_db_usage_string), "%s", tempstring);
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "type=%d external_db_usage_string=%s", type, external_db_usage_string);
		};
	};

	return(external_db_usage_string);
};



#ifdef SUPPORT_EXTERNAL

/*******************************
 * Wrapper extension functions for External
 *******************************/

/*
 * wrapper extension: External_dbfilename
 */
static char *libipv6calc_db_wrapper_External_dbfilename(int type) {
	static char tempstring[NI_MAXHOST];
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called: %s type=%d", wrapper_external_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_External_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	snprintf(tempstring, sizeof(tempstring), "%s/%s", external_db_dir, libipv6calc_db_wrapper_External_db_file_desc[i].filename);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Finished: %s type=%d has filename=%s", wrapper_external_info, type, tempstring);

	return(tempstring);
};


/*
 * wrapper extension: External_dbdescription
 */
const char *libipv6calc_db_wrapper_External_dbdescription(int type) {
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called: %s type=%d", wrapper_external_info, type);

	for (i = 0; i < sizeof(libipv6calc_db_wrapper_External_db_file_desc) / sizeof(libipv6calc_db_wrapper_External_db_file_desc[0]); i++) {
		if (libipv6calc_db_wrapper_External_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return("unknown");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Finished: %s type=%d has description=%s", wrapper_external_info, type, libipv6calc_db_wrapper_External_db_file_desc[i].description);

	return(libipv6calc_db_wrapper_External_db_file_desc[i].description);
};


/*
 * wrapper extension: External_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_External_db_avail(int type) {
	char *filename;
	int r = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called: %s type=%d", wrapper_external_info, type);

	filename = libipv6calc_db_wrapper_External_dbfilename(type);

	if (filename == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	r = (access(filename, R_OK) == 0) ? 1:0;

	if (r == 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Finished: %s type=%d (still unknown) (r=%d: %s)", wrapper_external_info, type, r, strerror(errno));
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Finished: %s type=%d (%s) (r=%d)", wrapper_external_info, type, filename, r);
	};

END_libipv6calc_db_wrapper:
	return(r);
};


/*
 * wrapper extension: External_open_type
 * input:
 * 	type (mandatory)
 * 		if | 0x10000 -> info is opened and ptr is not cached
 * 		if | 0x20000 -> data-iana is opened and ptr is not cached (only IPv4)
 * 	db_recno_max_ptr (set if not NULL)
 */
DB *libipv6calc_db_wrapper_External_open_type(const int type_flag, long int *db_recno_max_ptr) {
	DB *dbp = NULL;
	DBC *dbcp;
	DBT key, data;

	int type = (type_flag & 0xffff);
	int info_selector = ((type_flag & 0x10000) != 0) ? 1 : 0;
	int data_iana_selector = ((type_flag & 0x20000) != 0) ? 1 : 0;
	int subdb = (data_iana_selector == 0) ? 0 : 1;

	char *filename;
	int entry = -1, i;
	int ret;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called: %s type=%d (%s)", wrapper_external_info, type, (info_selector == 0) ? ((data_iana_selector == 0) ? "data" : "data-iana") : "info");

	// check for valid type
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_External_db_file_desc[i].number == (type & 0xffff)) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	if ((info_selector == 0) && (db_ptr_cache[entry][subdb] != NULL)) {
		// already open
		dbp = db_ptr_cache[entry][subdb];

		if (db_recno_max_ptr != NULL) {
			*db_recno_max_ptr = db_recno_max_cache[entry][subdb];
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database already opened (cached) dbp=%p type=%d subdb=%d recno_max: %u", dbp, type, subdb, db_recno_max_cache[entry][subdb]);

		goto END_libipv6calc_db_wrapper;
	};

	// retrieve filename
	filename = libipv6calc_db_wrapper_External_dbfilename(type);

	if (filename == NULL) {
		return(NULL);
	};

	if ((ret = db_create(&dbp, NULL, 0)) != 0) {
		if (ipv6calc_quiet == 0) {
			fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		};
		return(NULL);
	};

	if ((ret = dbp->open(dbp, NULL, filename, (info_selector == 0) ? ((data_iana_selector == 0) ? "data" : "data-iana") : "info", (info_selector == 0) ? DB_RECNO : DB_HASH, DB_RDONLY, 0444)) != 0) {
		if (ipv6calc_quiet == 0) {
			dbp->err(dbp, ret, "%s", filename);
		};
		return(NULL);
	};

	if (info_selector == 0) {
		// cache entry
		db_ptr_cache[entry][subdb] = dbp;

		// get amount of entries in database
		memset(&key, 0, sizeof(key));
		memset(&data, 0, sizeof(data));

		/* Acquire a cursor for the database. */
		if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
			dbp->err(dbp, ret, "DB->cursor");
			goto END_libipv6calc_db_wrapper_close_error;
		};

		/* Walk through the database and print out the key/data pairs. */
		if ((ret = dbcp->c_get(dbcp, &key, &data, DB_LAST)) != 0) {
			dbp->err(dbp, ret, "DB->cursor/DB_LAST");
			goto END_libipv6calc_db_wrapper_close_error;
		};

		/* Close the cursor. */
		if ((ret = dbcp->c_close(dbcp)) != 0) {
			dbp->err(dbp, ret, "DBcursor->close");
			goto END_libipv6calc_db_wrapper_close_error;
		};

		db_recno_max_cache[entry][subdb] = *(db_recno_t *)key.data;

		if (db_recno_max_cache[entry][subdb] < 2) {
			goto END_libipv6calc_db_wrapper_close_error;
		};

		if (db_recno_max_ptr != NULL) {
			*db_recno_max_ptr = db_recno_max_cache[entry][subdb];
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database successfully opened (fill-cache), dbp=%p type=%d subdb=%d recno_max=%u", dbp, type, subdb, db_recno_max_cache[entry][subdb]);
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database successfully opened, dbp=%p type=%d (info)", dbp, type);
	};

	// jump to "good end"
	goto END_libipv6calc_db_wrapper;

END_libipv6calc_db_wrapper_close_error:
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "error opening database, close now");
	libipv6calc_db_wrapper_External_close(dbp);
	dbp = NULL;

END_libipv6calc_db_wrapper:
	return(dbp);
};


/*******************************
 * Wrapper functions for External
 *******************************/

/*
 * wrapper: External_database_info
 */
char *libipv6calc_db_wrapper_External_database_info(const int type) {
	static char resultstring[NI_MAXHOST] = "";
	char datastring[NI_MAXHOST];
	int ret, i, entry = -1;
	DB *dbp;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called: %s", wrapper_external_info);

	// check for valid type
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_External_db_file_desc[i].number == (type & 0xffff)) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		ERRORPRINT_WA("Invalid type (FIX CODE): %d", type);
		goto END_libipv6calc_db_wrapper;
	};

	dbp = libipv6calc_db_wrapper_External_open_type(type | 0x10000, NULL);

	if (dbp == NULL) {
		snprintf(resultstring, sizeof(resultstring), "%s", "(CAN'T OPEN)");
		goto END_libipv6calc_db_wrapper;
	};

	// get dbusage
	ret = libipv6calc_db_wrapper_bdb_get_data_by_key(dbp, "dbusage", datastring, sizeof(datastring));
	if (ret != 0) {
		snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve 'dbusage', unsupported db file");
		goto END_libipv6calc_db_wrapper_close;
	};
	if (strcmp(datastring, "ipv6calc") != 0) {
		snprintf(resultstring, sizeof(resultstring), "%s", "dbusage!=ipv6calc, unsupported db file");
		goto END_libipv6calc_db_wrapper_close;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database dbusage string: %s", datastring);

	// get dbdate
	ret = libipv6calc_db_wrapper_bdb_get_data_by_key(dbp, "dbdate", datastring, sizeof(datastring));
	if (ret != 0) {
		snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve 'dbdate', unsupported db file");
		goto END_libipv6calc_db_wrapper_close;
	};

	// get dbcreated_unixtime
	ret = libipv6calc_db_wrapper_bdb_get_data_by_key(dbp, "dbcreated_unixtime", datastring, sizeof(datastring));
	if (ret != 0) {
		snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve 'dbcreated_unixtime', unsupported db file");
		goto END_libipv6calc_db_wrapper_close;
	};

	wrapper_db_unixtime_External[entry] = atoi(datastring);

	if (wrapper_db_unixtime_External[entry] == 0) {
		snprintf(resultstring, sizeof(resultstring), "%s", "'dbcreated_unixtime' is not proper, unsupported db file");
		goto END_libipv6calc_db_wrapper_close;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "wrapper_db_unixtime_External=%ld", (long int) wrapper_db_unixtime_External[entry]);

	strftime(datastring, sizeof(datastring), "%Y%m%d-%H%M%S UTC", gmtime(&wrapper_db_unixtime_External[entry]));
	snprintf(resultstring, sizeof(resultstring), "created: %s", datastring);

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_External_close(dbp);

END_libipv6calc_db_wrapper:
	return(resultstring);
};


/*********************************************
 * Abstract functions
 * *******************************************/

/* query for available features
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_External_has_features(uint32_t features) {
	int result = -1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called with feature value to test: 0x%08x", features);

	if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_EXTERNAL] & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Return with result: %d", result);
	return(result);
};


/* query db_unixtime by feature
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
time_t libipv6calc_db_wrapper_External_db_unixtime_by_feature(uint32_t feature) {
	time_t result = 0;
	int i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called with feature value to get db_unixtime: 0x%08x", feature);

	// run through entries
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		if ((libipv6calc_db_wrapper_External_db_file_desc[i].features & feature) == feature) {
			// found
			if (wrapper_db_unixtime_External[i] > result) {
				result = wrapper_db_unixtime_External[i];
			};
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Return for feature=0x08x db_unixtime=%ld", (long int) result);
	return(result);
};


/* registry by address */
int libipv6calc_db_wrapper_External_wrapper_registry_by_addr(const char *addr, const int proto) {
	int result = REGISTRY_UNKNOWN;

	int i;
	DB *dbp;

	static char resultstring[NI_MAXHOST];

	char *data_ptr = NULL;

	int External_type = 0;

	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	uint32_t ipv4 = 0, ipv6_00_31 = 0, ipv6_32_63 = 0;

	long int recno_max;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		External_type = EXTERNAL_DB_IPV4_REGISTRY;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_EXTERNAL] & IPV6CALC_DB_IPV4_TO_REGISTRY) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "No external database supporting IPv4 registry available");
			goto END_libipv6calc_db_wrapper;
		};
		// convert char to structure
		result = addr_to_ipv4addrstruct(addr, resultstring, sizeof(resultstring), &ipv4addr);
	} else if (proto == 6) {
		External_type = EXTERNAL_DB_IPV6_REGISTRY;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_EXTERNAL] & IPV6CALC_DB_IPV6_TO_REGISTRY) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "No external database supporting IPv6 registry available");
			goto END_libipv6calc_db_wrapper;
		};

		// convert char to structure
		result = addr_to_ipv6addrstruct(addr, resultstring, sizeof(resultstring), &ipv6addr);
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Unsupported proto: %d", proto);
		goto END_libipv6calc_db_wrapper;
	};

	if (result != 0) {
		ERRORPRINT_WA("error converting address string for proto %d: %s", proto, addr);
		goto END_libipv6calc_db_wrapper;
	};

	dbp = libipv6calc_db_wrapper_External_open_type(External_type, &recno_max);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Error opening External by type");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "database opened type=%d recno_max=%ld", External_type, recno_max);

	if (proto == 4) {
		ipv4 = ipv4addr_getdword(&ipv4addr);
	} else if (proto == 6) {
		ipv6_00_31 = ipv6addr_getdword(&ipv6addr, 0);
		ipv6_32_63 = ipv6addr_getdword(&ipv6addr, 1);
	};

	result = libipv6calc_db_wrapper_get_entry_generic(
		(void *) dbp,						// pointer to database
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
		(proto == 4) ? IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x2 : IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x4,   // key format
		(proto == 4) ? 32 : 64,					// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		recno_max,						// number of rows
		(proto == 4) ? ipv4 : ipv6_00_31,			// lookup key MSB
		(proto == 4) ? 0    : ipv6_32_63,			// lookup key LSB
		resultstring,						// data ptr
		NULL							// function pointer
	);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "resultstring=%s", resultstring);

	char datastring[NI_MAXHOST];

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	int token_count = 0;

	snprintf(datastring, sizeof(datastring), "%s", data_ptr);

	// split result string
	token = strtok_r(resultstring, ";", ptrptr);
	while (token != NULL) {
		token_count++;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database entry found %d: %s", token_count, token);

		if (token_count == 1) {
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_registries); i++) {
				if (strcmp(token, ipv6calc_registries[i].tokensimple) == 0) {
					result = ipv6calc_registries[i].number;
					break;
				};
			};
		};

		/* get next token */
		token = strtok_r(NULL, ";", ptrptr);
	};

	if (token_count != 1) {
		ERRORPRINT_WA("data has more entries than expected, corrupt database: %d", token_count);
		goto END_libipv6calc_db_wrapper_close;
	};

	if (result == REGISTRY_UNKNOWN) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "did not return a record for 'registry'");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "result registry=%d", result);

	EXTERNAL_DB_USAGE_MAP_TAG(External_type);

	goto END_libipv6calc_db_wrapper; // keep db open

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_External_close(dbp);

END_libipv6calc_db_wrapper:
	return(result);
};


/*
 * get registry number of an IPv4/IPv6 address
 *
 * in:  ipaddr
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_External_registry_num_by_addr(const ipv6calc_ipaddr *ipaddrp) {
	DB *dbp, *dbp_iana;
	long int recno_max;
	static char resultstring[NI_MAXHOST];
	char *data_ptr = NULL;
	int i, result;
	int retval = REGISTRY_UNKNOWN;

	int External_type;

	switch (ipaddrp->proto) {
	    case IPV6CALC_PROTO_IPV4:
		External_type = EXTERNAL_DB_IPV4_REGISTRY;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Given IPv4 address: %08x", (unsigned int) ipaddrp->addr[0]);
		break;

	    case IPV6CALC_PROTO_IPV6:
		External_type = EXTERNAL_DB_IPV6_REGISTRY;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Given IPv6 address prefix (0-63): %08x%08x", (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
		break;

	    default:
		ERRORPRINT_WA("unsupported protocol: %d (FIX CODE)", ipaddrp->proto);
		exit(EXIT_FAILURE);
		break;
	};


	// data (standard)
	dbp = libipv6calc_db_wrapper_External_open_type(External_type, &recno_max);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "database opened type=%d recno_max=%ld dbp=%p", External_type, recno_max, dbp);

	result = libipv6calc_db_wrapper_get_entry_generic(
		(void *) dbp,							// pointer to database
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB,				// type of data_ptr
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST \
		  : IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK,			// key type
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2 \
		  : IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x4 ,	// key format
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? 32 \
		  : 64,								// key length
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY \
		  : IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST,		// search type
		recno_max,							// number of rows
		ipaddrp->addr[0],						// lookup key MSB
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? 0 \
		  : ipaddrp->addr[1],						// lookup key LSB
		resultstring,							// data ptr
		NULL								// function pointer
	);

	if (result >= 0 ) {
		goto END_libipv6calc_db_wrapper_match;
	};

	if (ipaddrp->proto != IPV6CALC_PROTO_IPV4) {
		goto END_libipv6calc_db_wrapper;
	};

	// data-iana (fallback for IPv4 only)
	dbp_iana = libipv6calc_db_wrapper_External_open_type(External_type | 0x20000, &recno_max);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "database opened type=%d (data-iana) recno_max=%ld", External_type, recno_max);

	result = libipv6calc_db_wrapper_get_entry_generic(
		(void *) dbp_iana,					// pointer to database
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
		IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2,   // key format
		32,							// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		recno_max,						// number of rows
		ipaddrp->addr[0],							// lookup key MSB
		0,							// lookup key LSB
		resultstring,						// data ptr
		NULL							// function pointer
	);

	libipv6calc_db_wrapper_External_close(dbp_iana);

	if (result >= 0 ) {
		goto END_libipv6calc_db_wrapper_match;
	};

	goto END_libipv6calc_db_wrapper;

END_libipv6calc_db_wrapper_match:

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "resultstring=%s", resultstring);

	char datastring[NI_MAXHOST];

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	int token_count = 0;

	snprintf(datastring, sizeof(datastring), "%s", data_ptr);

	// split result string
	token = strtok_r(resultstring, ";", ptrptr);
	while (token != NULL) {
		token_count++;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database entry found %d: %s", token_count, token);

		if (token_count == 1) {
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_registries); i++) {
				if (strcmp(token, ipv6calc_registries[i].tokensimple) == 0) {
					retval = ipv6calc_registries[i].number;
					break;
				};
			};
		};

		/* get next token */
		token = strtok_r(NULL, ";", ptrptr);
	};

	if (token_count != 1) {
		ERRORPRINT_WA("data has more entries than expected, corrupt database: %d", token_count);
		goto END_libipv6calc_db_wrapper_close;
	};

	if (retval == REGISTRY_UNKNOWN) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "did not return a record for 'registry'");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "result registry=%d", retval);

	EXTERNAL_DB_USAGE_MAP_TAG(External_type);

	goto END_libipv6calc_db_wrapper; // keep db open

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_External_close(dbp);

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "retval=%d", retval);
	return(retval);
};


/*
 * get country code of an IPv4/IPv6 address
 *
 * in:  ipaddr
 * mod: country code
 * out: status of retrievment (0=success, -1=problem)
 */
int libipv6calc_db_wrapper_External_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len) {
	DB *dbp;
	long int recno_max;
	static char resultstring[NI_MAXHOST];
	char *data_ptr = NULL;
	int result;
	int retval = -1;

	int External_type;

	switch (ipaddrp->proto) {
	    case IPV6CALC_PROTO_IPV4:
		External_type = EXTERNAL_DB_IPV4_COUNTRYCODE;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Given IPv4 address: %08x", (unsigned int) ipaddrp->addr[0]);
		break;

	    case IPV6CALC_PROTO_IPV6:
		External_type = EXTERNAL_DB_IPV6_COUNTRYCODE;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Given IPv6 address prefix (0-63): %08x%08x", (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
		break;

	    default:
		ERRORPRINT_WA("unsupported protocol: %d (FIX CODE)", ipaddrp->proto);
		exit(EXIT_FAILURE);
		break;
	};


	// data (standard)
	dbp = libipv6calc_db_wrapper_External_open_type(External_type, &recno_max);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "database opened type=%d recno_max=%ld dbp=%p", External_type, recno_max, dbp);

	result = libipv6calc_db_wrapper_get_entry_generic(
		(void *) dbp,							// pointer to database
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB,				// type of data_ptr
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST \
		  : IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK,			// key type
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2 \
		  : IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4 ,	// key format
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? 32 \
		  : 64,								// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,			// search type
		recno_max,							// number of rows
		ipaddrp->addr[0],						// lookup key MSB
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) \
		  ? 0 \
		  : ipaddrp->addr[1],						// lookup key LSB
		resultstring,							// data ptr
		NULL								// function pointer
	);

	if (result < 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "resultstring=%s", resultstring);

	char datastring[NI_MAXHOST];

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	int token_count = 0;

	snprintf(datastring, sizeof(datastring), "%s", data_ptr);

	// split result string
	token = strtok_r(resultstring, ";", ptrptr);
	while (token != NULL) {
		token_count++;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database entry found %d: %s", token_count, token);

		if (token_count == 1) {
			/* country */
			snprintf(country, country_len, "%s", token);
		};

		/* get next token */
		token = strtok_r(NULL, ";", ptrptr);
	};

	if (token_count != 1) {
		ERRORPRINT_WA("data has more entries than expected, corrupt database: %d", token_count);
		goto END_libipv6calc_db_wrapper_close;
	};

	if (strlen(country) != 2) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "did not return a record for 'CountryCode'");
		goto END_libipv6calc_db_wrapper;
	};

	retval = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "result CountryCode=%s", country);

	EXTERNAL_DB_USAGE_MAP_TAG(External_type);

	goto END_libipv6calc_db_wrapper; // keep db open

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_External_close(dbp);

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "retval=%d", retval);
	return(retval);
};

#endif //SUPPORT_EXTERNAL
#endif //SUPPORT_EXTERNAL
