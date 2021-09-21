/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_External.c
 * Version    : $Id$
 * Copyright  : 2013-2021 by Peter Bieringer <pb (at) bieringer.de>
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
#include <time.h>

#include "config.h"

#include "libipv6calcdebug.h"
#include "libipv6calc.h"

#include "libipv6calc_db_wrapper.h"


#ifdef SUPPORT_EXTERNAL

#include <db.h>

#include "libipv6calc_db_wrapper_External.h"

char external_db_dir[PATH_MAX] = EXTERNAL_DB;

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

char external_db_usage_string[IPV6CALC_STRING_MAX] = "";

// local cache
#define IPV6CALC_DBD_SUBDB_MAX 3
static DB *db_ptr_cache[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc)][IPV6CALC_DBD_SUBDB_MAX];
static db_recno_t db_recno_max_cache[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc)][IPV6CALC_DBD_SUBDB_MAX];

// creation time of databases
time_t wrapper_db_unixtime_External[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc)];


// local prototyping
static char     *libipv6calc_db_wrapper_External_dbfilename(unsigned int type); 
static char     *libipv6calc_db_wrapper_External_database_info(unsigned int type);


/*
 * function initialise the External wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_External_wrapper_init(void) {
	int i, j;
	char *result;
	DB *dbp;
	long int recno_max;

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

		// more sophisticated check for "data-info"
		if (libipv6calc_db_wrapper_External_db_file_desc[i].number == EXTERNAL_DB_IPV4_REGISTRY) {
			dbp = libipv6calc_db_wrapper_External_open_type(EXTERNAL_DB_IPV4_REGISTRY | 0x40000, &recno_max);
			if (dbp == NULL) {
				// disable feature
				wrapper_features_by_source[IPV6CALC_DB_SOURCE_EXTERNAL] &= ~IPV6CALC_DB_IPV4_TO_INFO;
			};
		} else if (libipv6calc_db_wrapper_External_db_file_desc[i].number == EXTERNAL_DB_IPV6_REGISTRY) {
			dbp = libipv6calc_db_wrapper_External_open_type(EXTERNAL_DB_IPV6_REGISTRY | 0x40000, &recno_max);
			if (dbp == NULL) {
				// disable feature
				wrapper_features_by_source[IPV6CALC_DB_SOURCE_EXTERNAL] &= ~IPV6CALC_DB_IPV6_TO_INFO;
			};
		};
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

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		if (db_ptr_cache[i] != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Close External: type=%d desc='%s'", libipv6calc_db_wrapper_External_db_file_desc[i].number, libipv6calc_db_wrapper_External_db_file_desc[i].description);
			for (j = 0; j < IPV6CALC_DBD_SUBDB_MAX; j++) {
				libipv6calc_db_wrapper_External_close(db_ptr_cache[i][j]);
			};
		};
	};

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

	snprintf(string, size, "External available databases: Country4=%d Country6=%d IPV4_REG=%d IPV6_REG=%d", \
		(wrapper_features & IPV6CALC_DB_IPV4_TO_CC) ? 1 : 0, \
		(wrapper_features & IPV6CALC_DB_IPV6_TO_CC) ? 1 : 0, \
		(wrapper_features & IPV6CALC_DB_IPV4_TO_REGISTRY) ? 1 : 0, \
		(wrapper_features & IPV6CALC_DB_IPV6_TO_REGISTRY) ? 1 : 0 \
	);

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

	fprintf(stderr, "%sExternal: info of available databases in directory: %s\n", prefix, external_db_dir);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_External_db_file_desc); i++) {
		type = libipv6calc_db_wrapper_External_db_file_desc[i].number;

		if (libipv6calc_db_wrapper_External_db_avail(type)) {
			fprintf(stderr, "%sExternal: %-20s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_External_db_file_desc[i].description, libipv6calc_db_wrapper_External_db_file_desc[i].filename, libipv6calc_db_wrapper_External_database_info(type));
			count++;
		} else {
			if (level_verbose == LEVEL_VERBOSE2) {
				fprintf(stderr, "%sExternal: %-20s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_External_db_file_desc[i].description, libipv6calc_db_wrapper_External_dbfilename(type), strerror(errno));
			};
			continue;
		};
	};

	if (count == 0) {
		fprintf(stderr, "%sExternal: NO available databases found in directory: %s\n", prefix, external_db_dir);
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Finished");
	return;
};


/*
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_External_wrapper_db_info_used(void) {
	int type, i;
	char tempstring[IPV6CALC_STRING_MAX];
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


/*******************************
 * Wrapper extension functions for External
 *******************************/

/*
 * wrapper extension: External_dbfilename
 */
static char *libipv6calc_db_wrapper_External_dbfilename(unsigned int type) {
	static char tempstring[IPV6CALC_STRING_MAX];
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
const char *libipv6calc_db_wrapper_External_dbdescription(const unsigned int type) {
	int  entry = -1;
	unsigned int i;

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
int libipv6calc_db_wrapper_External_db_avail(const unsigned int type) {
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
 * 		if | 0x20000 -> data-iana is opened
 * 		if | 0x40000 -> data-info is opened
 * 	db_recno_max_ptr (set if not NULL)
 */
DB *libipv6calc_db_wrapper_External_open_type(const unsigned int type_flag, long int *db_recno_max_ptr) {
	DB *dbp = NULL;
	DBC *dbcp;
	DBT key, data;

	int type = (type_flag & 0xffff);
	int info_selector = ((type_flag & 0x10000) != 0) ? 1 : 0;
	int data_iana_selector = ((type_flag & 0x20000) != 0) ? 1 : 0;
	int data_info_selector = ((type_flag & 0x40000) != 0) ? 1 : 0;
	int subdb = 0; // data

	char *filename;
	int entry = -1, i;
	int ret;

	const char *type_text;
	if (info_selector != 0) {
		type_text = "info";
	} else if (data_iana_selector != 0) {
		type_text = "data-iana";
		subdb = 1;
	} else if (data_info_selector != 0) {
		type_text = "data-info";
		subdb = 2;
	} else {
		type_text = "data";
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Called: %s type=%d (%s)", wrapper_external_info, type, type_text);

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

	if (libipv6calc_db_wrapper_External_db_avail(type) != 1) {
		return(NULL);

	};

	if ((ret = db_create(&dbp, NULL, 0)) != 0) {
		if (ipv6calc_quiet == 0) {
			fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		};
		return(NULL);
	};

	if ((ret = dbp->open(dbp, NULL, filename, type_text, (info_selector == 0) ? DB_RECNO : DB_BTREE, DB_RDONLY, 0444)) != 0) {
		if ((ipv6calc_quiet == 0) && (data_info_selector == 0)) {
			fprintf(stderr, "db->open failed: %s (%s) subdb=%s\n", db_strerror(ret), filename, type_text);
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
char *libipv6calc_db_wrapper_External_database_info(const unsigned int type) {
	static char resultstring[IPV6CALC_STRING_MAX] = ""; // has to be static because pointer is returned
	char datastring[IPV6CALC_STRING_MAX];
	char tempstring[IPV6CALC_STRING_MAX];
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
		snprintf(resultstring, sizeof(resultstring), "%s", "(CAN'T OPEN database information)");
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

	snprintf(resultstring, sizeof(resultstring), "EXTDB-%d/%s", type, datastring);

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
	snprintf(tempstring, sizeof(tempstring), "%s, created: %s", resultstring, datastring);
	snprintf(resultstring, sizeof(resultstring), "%s", tempstring);

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


/*
 * get registry number of an IPv4/IPv6 address
 *
 * in:  ipaddr
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_External_registry_num_by_addr(const ipv6calc_ipaddr *ipaddrp) {
	DB *dbp, *dbp_iana;
	long int recno_max;
	char resultstring[IPV6CALC_STRING_MAX];
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

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Error opening External by type");
		goto END_libipv6calc_db_wrapper;
	};

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
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "found match in database type=%d", External_type);
		goto END_libipv6calc_db_wrapper_match;
	};

	if (ipaddrp->proto != IPV6CALC_PROTO_IPV4) {
		goto END_libipv6calc_db_wrapper;
	};

	// data-iana (fallback for IPv4 only)
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "no found match in database type=%d, fallback to IANA data now for: %08x", External_type, ipaddrp->addr[0]);
	dbp_iana = libipv6calc_db_wrapper_External_open_type(External_type | 0x20000, &recno_max);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Error opening External by type");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "database opened type=%d (data-iana) recno_max=%ld", External_type, recno_max);

	result = libipv6calc_db_wrapper_get_entry_generic(
		(void *) dbp_iana,					// pointer to database
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
		IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2,   // key format
		32,							// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		recno_max,						// number of rows
		ipaddrp->addr[0],					// lookup key MSB
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

	char datastring[IPV6CALC_STRING_MAX];
	snprintf(datastring, sizeof(datastring), "%s", resultstring); // copy string for strtok

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	int token_count = 0;

	// split result string
	token = strtok_r(datastring, ";", ptrptr);
	while (token != NULL) {
		token_count++;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database entry found %d: %s", token_count, token);

		if (token_count == 1) {
			for (i = 0; i < ipv6calc_registries_entries; i++) {
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
		ERRORPRINT_WA("data has more entries than expected, corrupt database: %d (resultstring='%s' prefix=%08x%08x)", token_count, resultstring, (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
		goto END_libipv6calc_db_wrapper;
	};

	if (retval == REGISTRY_UNKNOWN) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "did not return a record for 'registry'");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "result registry=%d", retval);

	EXTERNAL_DB_USAGE_MAP_TAG(External_type);

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
	static char resultstring[IPV6CALC_STRING_MAX];
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

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Error opening External by type");
		goto END_libipv6calc_db_wrapper;
	};

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

	char datastring[IPV6CALC_STRING_MAX];
	snprintf(datastring, sizeof(datastring), "%s", resultstring); // copy string for strtok

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	int token_count = 0;

	// split result string
	token = strtok_r(datastring, ";", ptrptr);
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
		ERRORPRINT_WA("data has more entries than expected, corrupt database: %d (resultstring='%s' prefix=%08x%08x)", token_count, resultstring, (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
		goto END_libipv6calc_db_wrapper;
	};

	if (strlen(country) != 2) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "did not return a record for 'CountryCode'");
		goto END_libipv6calc_db_wrapper;
	};

	retval = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "result CountryCode=%s", country);

	EXTERNAL_DB_USAGE_MAP_TAG(External_type);

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "retval=%d", retval);
	return(retval);
};


/*
 * get info of an IPv4/IPv6 address
 *  stored in REGISTRY db in dedicated table
 *
 * in:  ipaddr
 * mod: string
 * out: 0=OK
 */
int libipv6calc_db_wrapper_External_info_by_ipaddr(const ipv6calc_ipaddr *ipaddrp, char *string, const size_t string_len) {
	DB *dbp;
	long int recno_max;
	char resultstring[IPV6CALC_STRING_MAX];
	int result;
	int retval = -1;

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


	// data-info
	dbp = libipv6calc_db_wrapper_External_open_type(External_type | 0x40000, &recno_max);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Error opening External by type");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "database opened type=%x recno_max=%ld dbp=%p", External_type | 0x40000, recno_max, dbp);

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

	char datastring[IPV6CALC_STRING_MAX];
	snprintf(datastring, sizeof(datastring), "%s", resultstring); // copy string for strtok

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	int token_count = 0;

	// split result string
	token = strtok_r(datastring, ";", ptrptr);
	while (token != NULL) {
		token_count++;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "Database entry found %d: %s", token_count, token);

		if (token_count == 1) {
			/* info */
			snprintf(string, string_len, "%s", token);
		};

		/* get next token */
		token = strtok_r(NULL, ";", ptrptr);
	};

	if (token_count != 1) {
		ERRORPRINT_WA("data has more entries than expected, corrupt database: %d (resultstring='%s' prefix=%08x%08x)", token_count, resultstring, (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
		goto END_libipv6calc_db_wrapper;
	};

	if (strlen(string) == 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "did not return a record for 'Info'");
		goto END_libipv6calc_db_wrapper;
	};

	retval = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "result Info=%s", string);

	EXTERNAL_DB_USAGE_MAP_TAG(External_type);

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "retval=%d", retval);
	return(retval);
};


/*
 * database dump
 *
 * in:  selector
 * in:  filter ('External' database only supports CountryCode)
 * in:  formatoptions
 * out: 0=OK
 */
int libipv6calc_db_wrapper_External_dump(const int selector, const s_ipv6calc_filter_master *filter_master, const uint32_t formatoptions) {
	DB *dbp;
	long int recno_max, recno, count = 0;
	char resultstring[IPV6CALC_STRING_MAX];
	char tempstring[IPV6CALC_STRING_MAX];
	char filterstring[IPV6CALC_STRING_MAX];
	int result;
	int retval = -1;
	char protocol;
	int i;

	uint32_t value_first_00_31, value_last_00_31;
        uint32_t value_first_32_63, value_last_32_63;

	const s_ipv6calc_filter_db_cc *filter;

	int External_type, key_format;

	char cc2[IPV6CALC_COUNTRYCODE_STRING_MAX];

	int prefixlength;
	uint32_t mask;
	uint32_t delta;

	ipv6calc_ipaddr ipaddr;

	switch (selector) {
	    case IPV6CALC_PROTO_IPV4:
		External_type = EXTERNAL_DB_IPV4_COUNTRYCODE;
		protocol = '4';
		key_format = IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2;

		if (filter_master->filter_ipv4addr.active == 0) {
			ERRORPRINT_NA("no IPv4 filter provided (ipv4.db.cc=<CountryCode>)");
			return (1);
		};

		if (filter_master->filter_ipv4addr.filter_db_cc.active == 0) {
			ERRORPRINT_NA("no IPv4 Database CountryCode filter provided (ipv4.db.cc=<CountryCode>)");
			return (1);
		};

		filter = &filter_master->filter_ipv4addr.filter_db_cc;

		break;

	    case IPV6CALC_PROTO_IPV6:
		External_type = EXTERNAL_DB_IPV6_COUNTRYCODE;
		protocol = '6';
		key_format = IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4;

		if (filter_master->filter_ipv6addr.active == 0) {
			ERRORPRINT_NA("no IPv6 filter provided (ipv6.db.cc=<CountryCode>)");
			return (1);
		};

		if (filter_master->filter_ipv6addr.filter_db_cc.active == 0) {
			ERRORPRINT_NA("no IPv6 Database CountryCode filter provided (ipv6.db.cc=<CountryCode>)");
			return (1);
		};

		filter = &filter_master->filter_ipv6addr.filter_db_cc;

		break;

	    default:
		ERRORPRINT_WA("unsupported selector: %d (FIX CODE)", selector);
		return (1);
		break;
	};

	// create filter info string
	snprintf(resultstring, sizeof(resultstring), "%s", ""); // clear string

	if (filter->cc_must_have_max > 0) {
		for (i = 0; i < filter->cc_must_have_max; i++) {
			libipv6calc_db_wrapper_country_code_by_cc_index(cc2, sizeof(cc2), filter->cc_must_have[i]);
			snprintf(tempstring, sizeof(tempstring), "%s %s", filterstring, cc2); 
			snprintf(filterstring, sizeof(filterstring), "%s", tempstring);
		};
	};

	if (filter->cc_may_not_have_max > 0) {
		for (i = 0; i < filter->cc_may_not_have_max; i++) {
			libipv6calc_db_wrapper_country_code_by_cc_index(cc2, sizeof(cc2), filter->cc_may_not_have[i]);
			snprintf(tempstring, sizeof(tempstring), "%s ^%s", filterstring, cc2); 
			snprintf(filterstring, sizeof(filterstring), "%s", tempstring);
		};
	};

	// data-info
	dbp = libipv6calc_db_wrapper_External_open_type(External_type, &recno_max);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_External, "Error opening External by type");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "database opened type=%x recno_max=%ld dbp=%p", External_type | 0x40000, recno_max, dbp);

	NONQUIETPRINT_WA("# 'External' database dump (with %lu entries) start with filter IPv%c && CountryCode(s):%s (suppress this line with option '-q')", recno_max, protocol, filterstring); // string has a trailing space

	for (recno = 1; recno <= recno_max; recno++) {
		result = libipv6calc_db_wrapper_bdb_fetch_row(
			dbp,			// pointer to database
			key_format,		// key format
			recno,			// row number
			&value_first_00_31,     // data 1 (MSB in case of 64 bits)
			&value_first_32_63,     // data 1 (LSB in case of 64 bits)
			&value_last_00_31,      // data 2 (MSB in case of 64 bits)
			&value_last_32_63,      // data 2 (LSB in case of 64 bits)
			resultstring		// data ptr
		);

		if (result < 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "no entry found for recno=%ld", recno);
			goto END_libipv6calc_db_wrapper;
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "resultstring=%s", resultstring);

		int16_t cc_index = libipv6calc_db_wrapper_cc_index_by_country_code(resultstring);

		switch (selector) {
		    case IPV6CALC_PROTO_IPV4:
			if (libipv6calc_db_cc_filter(cc_index, &filter_master->filter_ipv4addr.filter_db_cc) > 0) {
				continue;
			};

			count++;
			libipaddr_clearall(&ipaddr);
			ipaddr.proto = IPV6CALC_PROTO_IPV4;
			ipaddr.addr[0] = value_first_00_31;

			// convert start/end into prefix length
			delta = value_last_00_31 - value_first_00_31 + 1;

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "found delta (main): %u", delta);

			/* backfill with smaller segments if necessary */
			while (delta > 0) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "found delta: %u", delta);
				prefixlength = 32;
				mask = 0x0000001;
				while (prefixlength > 0) {
					if (delta < mask) {
						break;
					};

					mask <<= 1;
					prefixlength--;
				};

				mask >>= 1;
				prefixlength++;

				ipaddr.prefixlength = prefixlength;
				ipaddr.flag_valid = 1;
				ipaddr.flag_prefixuse = 1;

				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "# IPv4 start=%08x end=%08x prefixlength=%d CC: %s", value_first_00_31, value_last_00_31, prefixlength, resultstring);

				libipaddr_ipaddrstruct_to_string(&ipaddr, tempstring, sizeof(tempstring), formatoptions);
				fprintf(stdout, "%s\n", tempstring);

				delta -= mask;
				ipaddr.addr[0] += mask;
			};

			break;

		    case IPV6CALC_PROTO_IPV6:
			if (libipv6calc_db_cc_filter(cc_index, &filter_master->filter_ipv6addr.filter_db_cc) > 0) {
				continue;
			};

			count++;
			libipaddr_clearall(&ipaddr);
			ipaddr.proto = IPV6CALC_PROTO_IPV6;
			ipaddr.addr[0] = value_first_00_31;
			ipaddr.addr[1] = value_first_32_63;

			// convert mask into prefix length
			prefixlength = 0;
			mask = 0x8000000;
			while (mask > 0) {
				if ((value_last_00_31 & mask) == 0) {
					break;
				};

				prefixlength++;
				mask >>= 1;
			};

			if (prefixlength == 32) {
				// continue with 2nd block
				int mask = 0x8000000;
				while (mask > 0) {
					if ((value_last_32_63 & mask) == 0) {
						break;
					};

					prefixlength++;
					mask >>= 1;
				};
			};

			ipaddr.prefixlength = prefixlength;
			ipaddr.flag_valid = 1;
			ipaddr.flag_prefixuse = 1;
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "# IPv6 prefix=%08x%08x mask=%08x%08x prefixlength=%d CC: %s", value_first_00_31, value_first_32_63, value_last_00_31, value_last_32_63, prefixlength, resultstring);

			libipaddr_ipaddrstruct_to_string(&ipaddr, tempstring, sizeof(tempstring), formatoptions);
			fprintf(stdout, "%s\n", tempstring);
			break;
		};
	};


	EXTERNAL_DB_USAGE_MAP_TAG(External_type);
	retval = 0;

END_libipv6calc_db_wrapper:
	NONQUIETPRINT_WA("# 'External' database dump finished displaying %lu entries with filter IPv%c && CountryCode(s):%s (suppress this line with option '-q')", count, protocol, filterstring); // resultstring has a trailing space

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "retval=%d", retval);
	return(retval);
};

#endif //SUPPORT_EXTERNAL
