/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_DBIP.c
 * Version    : $Id$
 * Copyright  : 2013-2017 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc DB-IP.com database wrapper
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


#ifdef SUPPORT_DBIP

#include <db.h>

#include "libipv6calc_db_wrapper_DBIP.h"

char dbip_db_dir[NI_MAXHOST] = DBIP_DB;

static const char* wrapper_dbip_info = "DBIP";


// DB used for resolution after automatic election, which is currently done by checking for DB files and the sequence in the header file
static int dbip_db_country_v4 = 0;
static int dbip_db_country_v6 = 0;
static int dbip_db_region_city_v4 = 0;
static int dbip_db_region_city_v6 = 0;

typedef struct {
	unsigned int num;
	int dbtype;
	int dbym;
} s_ipv6calc_dbip_db;

#define DBIP_FREE	0
#define DBIP_COMM	1
#define	DBIP_MAX	2

static s_ipv6calc_dbip_db dbip_db_country_v4_best[DBIP_MAX];
static s_ipv6calc_dbip_db dbip_db_country_v6_best[DBIP_MAX];
static s_ipv6calc_dbip_db dbip_db_region_city_v4_best[DBIP_MAX];
static s_ipv6calc_dbip_db dbip_db_region_city_v6_best[DBIP_MAX];

// select FREE database if COMM is older than given months
int dbip_db_comm_to_free_switch_min_delta_months = 12;

// select better database of same product (COMM/FREE) only if not older than given months
int dbip_db_better_max_delta_months = 1;

// select only a specific DBIP db type
int dbip_db_only_type = 0;


#define DBIP_UNPACK_YM(dbym) ((dbym > 0) ? ((dbym % 12) + 1 + ((dbym / 12) + 2000) * 100) : 0)


/* database usage map */
#define DBIP_DB_MAX_BLOCKS_32	2	// 0-63
static uint32_t dbip_db_usage_map[DBIP_DB_MAX_BLOCKS_32];

#define DBIP_DB_USAGE_MAP_TAG(db)	if (db < (32 * DBIP_DB_MAX_BLOCKS_32)) { \
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Tag usage for db: %d", db); \
							dbip_db_usage_map[db / 32] |= 1 << (db % 32); \
						} else { \
							fprintf(stderr, "FIXME: unsupported db value (exceed limit): %d (%d)\n", db, 32 * DBIP_DB_MAX_BLOCKS_32 - 1); \
							exit(1); \
						};

char dbip_db_usage_string[NI_MAXHOST] = "";

// local cache
static DB*      db_ptr_cache[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc)];
static db_recno_t db_recno_max_cache[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc)];

// creation time of databases
time_t wrapper_db_unixtime_DBIP[MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc)];

// local prototyping
static char     *libipv6calc_db_wrapper_DBIP_dbfilename(const unsigned int type); 
static int libipv6calc_db_wrapper_DBIP_close(DB *dbp);


/*
 * function initialise the DBIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_DBIP_wrapper_init(void) {
	int i, dbym, dbtype, ret, product;
	DB *dbp;
	char datastring[NI_MAXHOST];
	char resultstring[NI_MAXHOST];

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called");

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Check for standard DBIP databases");

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "custom directory: %s", dbip_db_dir);

	/* check available databases for resolution */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {

		// add features to implemented
		wrapper_features_by_source_implemented[IPV6CALC_DB_SOURCE_DBIP] |= libipv6calc_db_wrapper_DBIP_db_file_desc[i].features;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "DBIP database test for availability: %s", libipv6calc_db_wrapper_DBIP_db_file_desc[i].filename);
		if (libipv6calc_db_wrapper_DBIP_db_avail(libipv6calc_db_wrapper_DBIP_db_file_desc[i].number) == 1) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "DBIP database available: %s", libipv6calc_db_wrapper_DBIP_db_file_desc[i].description);
			wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP] |= libipv6calc_db_wrapper_DBIP_db_file_desc[i].features;
		} else {
			// db not available
			continue;
		};

		// clean local cache
		db_ptr_cache[i] = NULL;
		db_recno_max_cache[i] = -1;

		dbp = libipv6calc_db_wrapper_DBIP_open_type(libipv6calc_db_wrapper_DBIP_db_file_desc[i].number | 0x10000, NULL);
		if (dbp == NULL) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Error opening DBIP by type");
			continue;
		};

		// get dbcreated_unixtime
		ret = libipv6calc_db_wrapper_bdb_get_data_by_key(dbp, "dbcreated_unixtime", datastring, sizeof(datastring));
		if (ret != 0) {
			snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve 'dbcreated_unixtime', unsupported db file");
			continue;
		};

		dbym = atoi(datastring) / (60*60*24*30);

		if (dbym == 0) {
			snprintf(resultstring, sizeof(resultstring), "%s", "'dbcreated_unixtime' is not proper, unsupported db file");
			continue;
		};

		// get dbtype
		ret = libipv6calc_db_wrapper_bdb_get_data_by_key(dbp, "dbtype", datastring, sizeof(datastring));
		if (ret != 0) {
			snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve 'dbtype', unsupported db file");
			continue;
		};

		dbtype = atoi(datastring);

		if ((dbip_db_only_type > 0) && (dbip_db_only_type != dbtype)) {
			// not selected
			continue;
		};

		if ((libipv6calc_db_wrapper_DBIP_db_file_desc[i].internal & IPV6CALC_DB_DBIP_INTERNAL_FREE) != 0) {
			product = DBIP_FREE;
		} else {
			product = DBIP_COMM;
		};

#define DBIP_DB_SELECT_BETTER(best) \
			if ( \
			  (best.num == 0) \
			  || ( \
				(best.dbym > 0) \
				  && ((best.dbym - dbym) <= dbip_db_better_max_delta_months) \
			     ) \
			) { \
				best.num = libipv6calc_db_wrapper_DBIP_db_file_desc[i].number; \
				best.dbym = dbym; \
				best.dbtype = dbtype; \
			};


		// note: databases are listed in sequence "less data" before "more data"
		if ((libipv6calc_db_wrapper_DBIP_db_file_desc[i].features & IPV6CALC_DB_IPV4_TO_CC) != 0) {
			DBIP_DB_SELECT_BETTER(dbip_db_country_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP_db_file_desc[i].features & IPV6CALC_DB_IPV6_TO_CC) != 0) {
			DBIP_DB_SELECT_BETTER(dbip_db_country_v6_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP_db_file_desc[i].features & (IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV4_TO_CITY)) != 0) {
			DBIP_DB_SELECT_BETTER(dbip_db_region_city_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP_db_file_desc[i].features & (IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV6_TO_CITY)) != 0) {
			DBIP_DB_SELECT_BETTER(dbip_db_region_city_v6_best[product])
		};

		wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP] |= libipv6calc_db_wrapper_DBIP_db_file_desc[i].features;
	};

	// select free instead of comm, if comm is outdated and free available
	if (dbip_db_comm_to_free_switch_min_delta_months > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "DBIP database priority check (comm->free) after months: %d", dbip_db_comm_to_free_switch_min_delta_months);
#define SELECT_FREE_INSTEAD_OF_COMM(free, comm, final) \
	if ((free.num > 0) && (comm.num > 0)) { \
		if (free.dbym - comm.dbym > dbip_db_comm_to_free_switch_min_delta_months) { \
			final = free.num; \
		}; \
	};
		SELECT_FREE_INSTEAD_OF_COMM(dbip_db_country_v4_best[DBIP_FREE], dbip_db_country_v4_best[DBIP_COMM], dbip_db_country_v4)
		SELECT_FREE_INSTEAD_OF_COMM(dbip_db_country_v6_best[DBIP_FREE], dbip_db_country_v6_best[DBIP_COMM], dbip_db_country_v6)
		SELECT_FREE_INSTEAD_OF_COMM(dbip_db_region_city_v4_best[DBIP_FREE], dbip_db_region_city_v4_best[DBIP_COMM], dbip_db_region_city_v4)
		SELECT_FREE_INSTEAD_OF_COMM(dbip_db_region_city_v6_best[DBIP_FREE], dbip_db_region_city_v6_best[DBIP_COMM], dbip_db_region_city_v6)
	};

#define FILL_EMPTY(product, final) \
	if ((product.num > 0) && (final == 0)) { \
		final = product.num; \
	};

	// fill empty ones with comm
	FILL_EMPTY(dbip_db_country_v4_best[DBIP_COMM], dbip_db_country_v4)
	FILL_EMPTY(dbip_db_country_v6_best[DBIP_COMM], dbip_db_country_v6)
	FILL_EMPTY(dbip_db_region_city_v4_best[DBIP_COMM], dbip_db_region_city_v4)
	FILL_EMPTY(dbip_db_region_city_v6_best[DBIP_COMM], dbip_db_region_city_v6)

	// fill empty ones with free
	FILL_EMPTY(dbip_db_country_v4_best[DBIP_FREE], dbip_db_country_v4)
	FILL_EMPTY(dbip_db_country_v6_best[DBIP_FREE], dbip_db_country_v6)
	FILL_EMPTY(dbip_db_region_city_v4_best[DBIP_FREE], dbip_db_region_city_v4)
	FILL_EMPTY(dbip_db_region_city_v6_best[DBIP_FREE], dbip_db_region_city_v6)


	/* close handles which are not necessary further on */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		if (db_ptr_cache[i] != NULL) {
			if (
				(libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == dbip_db_country_v4_best[DBIP_FREE].num)
			    ||	(libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == dbip_db_country_v4_best[DBIP_COMM].num)
			    ||	(libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == dbip_db_country_v6_best[DBIP_FREE].num)
			    ||	(libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == dbip_db_country_v6_best[DBIP_COMM].num)
			) {
				// database is in use
				continue;
			};

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Close further unused DBIP: type=%d desc='%s'", libipv6calc_db_wrapper_DBIP_db_file_desc[i].number, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description);
			libipv6calc_db_wrapper_DBIP_close(db_ptr_cache[i]);
		};
	};

	wrapper_features |= wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP];

	return 0;
};


/*
 * wrapper: DBIP_close
 */
static int libipv6calc_db_wrapper_DBIP_close(DB *dbp) {
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called");

	if (dbp != NULL) {
		/* cleanup cache entry */
		for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
			if (db_ptr_cache[i] == dbp) {
				db_ptr_cache[i] = NULL;
				db_recno_max_cache[i] = -1;
			};
		};

		dbp->close(dbp, 0);
	};

	return(0);
};


/*
 * function cleanup the DBIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_DBIP_wrapper_cleanup(void) {
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called");

#ifdef SUPPORT_DBIP
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		if (db_ptr_cache[i] != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Close DBIP: type=%d desc='%s'", libipv6calc_db_wrapper_DBIP_db_file_desc[i].number, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description);
			libipv6calc_db_wrapper_DBIP_close(db_ptr_cache[i]);
		};
	};
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Finished");
	return 0;
};


/*
 * function info of DBIP wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_DBIP_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called");

#ifdef SUPPORT_DBIP
	snprintf(string, size, "DBIP available databases: Country4=%d Country6=%d City4=%d City6=%d", (wrapper_features & IPV6CALC_DB_IPV4_TO_CC) ? 1 : 0, (wrapper_features & IPV6CALC_DB_IPV6_TO_CC) ? 1 : 0, (wrapper_features & IPV6CALC_DB_IPV4_TO_CITY) ? 1 : 0, (wrapper_features & IPV6CALC_DB_IPV6_TO_CITY) ? 1 : 0);
#else
	snprintf(string, size, "No DBIP support built-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Finished");
	return;
};

/*
 * function print database info of DBIP wrapper
 *
 * in : (void)
 * out: (void)
 */
void libipv6calc_db_wrapper_DBIP_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	DB *dbp;
	int i, type, count = 0;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called");

	IPV6CALC_DB_FEATURE_INFO(prefix, IPV6CALC_DB_SOURCE_DBIP)

#ifdef SUPPORT_DBIP
	fprintf(stderr, "%sDBIP: info of available databases in directory: %s\n", prefix, dbip_db_dir);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		type = libipv6calc_db_wrapper_DBIP_db_file_desc[i].number;

		if (libipv6calc_db_wrapper_DBIP_db_avail(type)) {
			// DBIP returned that database is available
			dbp = libipv6calc_db_wrapper_DBIP_open_type(type | 0x10000, NULL);
			if (dbp == NULL) {
				fprintf(stderr, "%sDBIP: %-30s: %-30s (CAN'T OPEN database information)\n", prefix, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description, libipv6calc_db_wrapper_DBIP_dbfilename(type));
			} else {
				fprintf(stderr, "%sDBIP: %-30s: %-30s (%s)\n", prefix, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description, libipv6calc_db_wrapper_DBIP_db_file_desc[i].filename, libipv6calc_db_wrapper_DBIP_database_info(type));
				libipv6calc_db_wrapper_DBIP_close(dbp);
				count++;
			};
		} else {
			if (level_verbose == LEVEL_VERBOSE2) {
				fprintf(stderr, "%sDBIP: %-30s: %-30s (%s)\n", prefix, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description, libipv6calc_db_wrapper_DBIP_dbfilename(type), strerror(errno));
			};
			continue;
		};
	};

	if (count == 0) {
		fprintf(stderr, "%sDBIP: NO available databases found in directory: %s\n", prefix, dbip_db_dir);
	} else {
		if (level_verbose >= LEVEL_VERBOSE2) {
			fprintf(stderr, "%sDBIP: detected best databases FREE    Country4=%-3d DB%-2d %6d  Country6=%-3d DB%-2d %6d  City4=%-3d DB%-2d %6d  City6=%-3d DB%-2d %6d\n"
				, prefix
				, dbip_db_country_v4_best[DBIP_FREE].num
				, dbip_db_country_v4_best[DBIP_FREE].dbtype
				, DBIP_UNPACK_YM(dbip_db_country_v4_best[DBIP_FREE].dbym)
				, dbip_db_country_v6_best[DBIP_FREE].num
				, dbip_db_country_v6_best[DBIP_FREE].dbtype
				, DBIP_UNPACK_YM(dbip_db_country_v6_best[DBIP_FREE].dbym)
				, dbip_db_region_city_v4_best[DBIP_FREE].num
				, dbip_db_region_city_v4_best[DBIP_FREE].dbtype
				, DBIP_UNPACK_YM(dbip_db_region_city_v4_best[DBIP_FREE].dbym)
				, dbip_db_region_city_v6_best[DBIP_FREE].num
				, dbip_db_region_city_v6_best[DBIP_FREE].dbtype
				, DBIP_UNPACK_YM(dbip_db_region_city_v6_best[DBIP_FREE].dbym)
			);

			fprintf(stderr, "%sDBIP: detected best databases COMM    Country4=%-3d DB%-2d %6d  Country6=%-3d DB%-2d %6d  City4=%-3d DB%-2d %6d  City6=%-3d DB%-2d %6d\n"
				, prefix
				, dbip_db_country_v4_best[DBIP_COMM].num
				, dbip_db_country_v4_best[DBIP_COMM].dbtype
				, DBIP_UNPACK_YM(dbip_db_country_v4_best[DBIP_COMM].dbym)
				, dbip_db_country_v6_best[DBIP_COMM].num
				, dbip_db_country_v6_best[DBIP_COMM].dbtype
				, DBIP_UNPACK_YM(dbip_db_country_v6_best[DBIP_COMM].dbym)
				, dbip_db_region_city_v4_best[DBIP_COMM].num
				, dbip_db_region_city_v4_best[DBIP_COMM].dbtype
				, DBIP_UNPACK_YM(dbip_db_region_city_v4_best[DBIP_COMM].dbym)
				, dbip_db_region_city_v6_best[DBIP_COMM].num
				, dbip_db_region_city_v6_best[DBIP_COMM].dbtype
				, DBIP_UNPACK_YM(dbip_db_region_city_v6_best[DBIP_COMM].dbym)
			);
		} else if (level_verbose >= LEVEL_VERBOSE) {
			fprintf(stderr, "%sDBIP: detected best databases FREE    Country4=%-3d  Country6=%-3d  City4=%-3d  City6=%-3d\n"
				, prefix
				, dbip_db_country_v4_best[DBIP_FREE].num
				, dbip_db_country_v6_best[DBIP_FREE].num
				, dbip_db_region_city_v4_best[DBIP_FREE].num
				, dbip_db_region_city_v6_best[DBIP_FREE].num
			);

			fprintf(stderr, "%sDBIP: detected best databases COMM    Country4=%-3d  Country6=%-3d  City4=%-3d  City6=%-3d\n"
				, prefix
				, dbip_db_country_v4_best[DBIP_COMM].num
				, dbip_db_country_v6_best[DBIP_COMM].num
				, dbip_db_region_city_v4_best[DBIP_COMM].num
				, dbip_db_region_city_v6_best[DBIP_COMM].num
			);

		};

		if (level_verbose >= LEVEL_VERBOSE) {
			fprintf(stderr, "%sDBIP: selected best databases normal  Country4=%-3d%s  Country6=%-3d%s  City4=%-3d%s  City6=%-3d\n"
				, prefix
				, dbip_db_country_v4
				, (level_verbose >= LEVEL_VERBOSE2) ? "            " : ""
				, dbip_db_country_v6
				, (level_verbose >= LEVEL_VERBOSE2) ? "            " : ""
				, dbip_db_region_city_v4
				, (level_verbose >= LEVEL_VERBOSE2) ? "            " : ""
				, dbip_db_region_city_v6
			);

			if (dbip_db_comm_to_free_switch_min_delta_months > 0) {
				fprintf(stderr, "%sDBIP: selected best databases method: COMM older than %d months are deselected in case of FREE is available\n"
					, prefix
					, dbip_db_comm_to_free_switch_min_delta_months
				);
			};

			if (dbip_db_better_max_delta_months > 0) {
				fprintf(stderr, "%sDBIP: selected best databases method: COMM/FREE with more features are only selected in case not older than %d months of already found COMM/FREE\n"
					, prefix
					, dbip_db_better_max_delta_months
				);
			};

			if (dbip_db_only_type > 0) {
				fprintf(stderr, "%sDBIP: selected best databases method: by applying given DB type filter: %d\n"
					, prefix
					, dbip_db_only_type
				);
			};
		};
	};
#else // SUPPORT_DBIP
	snfprintf(stderr, string, size, "%sNo DBIP support built-in", prefix);
#endif // SUPPORT_DBIP

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Finished");
	return;
};


/*
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_DBIP_wrapper_db_info_used(void) {
	int type, i;
	char tempstring[NI_MAXHOST];
	char *info;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called");

	for (i = 0; i < DBIP_DB_MAX_BLOCKS_32; i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "dbip_db_usage_map[%d]=%08x", i, (unsigned int) dbip_db_usage_map[i]);
	};

	for (type = 0; type < 32 * DBIP_DB_MAX_BLOCKS_32; type++) {
		if ((dbip_db_usage_map[type / 32] & (1 << (type % 32))) != 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "DB type used: %d", type);

			info = libipv6calc_db_wrapper_DBIP_database_info(type);

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "type=%d info=%s", type, info);

			if (strlen(dbip_db_usage_string) > 0) {
				if (strstr(dbip_db_usage_string, info) != NULL) {
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "type=%d info=%s (skip, already displayed)", type, info);
					continue;
				}; // string already included

				snprintf(tempstring, sizeof(tempstring), "%s / %s", dbip_db_usage_string, info);
			} else {
				snprintf(tempstring, sizeof(tempstring), "%s", info);
			};

			snprintf(dbip_db_usage_string, sizeof(dbip_db_usage_string), "%s", tempstring);
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "type=%d dbip_db_usage_string=%s", type, dbip_db_usage_string);
		};
	};

	return(dbip_db_usage_string);
};



#ifdef SUPPORT_DBIP

/*******************************
 * Wrapper extension functions for DBIP
 *******************************/

/*
 * wrapper extension: DBIP_dbfilename
 */
static char *libipv6calc_db_wrapper_DBIP_dbfilename(const unsigned int type) {
	static char tempstring[NI_MAXHOST];
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called: %s type=%d", wrapper_dbip_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	snprintf(tempstring, sizeof(tempstring), "%s/%s", dbip_db_dir, libipv6calc_db_wrapper_DBIP_db_file_desc[i].filename);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Finished: %s type=%d has filename=%s", wrapper_dbip_info, type, tempstring);

	return(tempstring);
};


/*
 * wrapper extension: DBIP_dbdescription
 */
const char *libipv6calc_db_wrapper_DBIP_dbdescription(const unsigned int type) {
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called: %s type=%d", wrapper_dbip_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return("unknown");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Finished: %s type=%d has description=%s", wrapper_dbip_info, type, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description);

	return(libipv6calc_db_wrapper_DBIP_db_file_desc[i].description);
};


/*
 * wrapper extension: DBIP_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_DBIP_db_avail(const unsigned int type) {
	char *filename;
	int r = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called: %s type=%d", wrapper_dbip_info, type);

	filename = libipv6calc_db_wrapper_DBIP_dbfilename(type);

	if (filename == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	r = (access(filename, R_OK) == 0) ? 1:0;

	if (r == 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Finished: %s type=%d (still unknown) (r=%d: %s)", wrapper_dbip_info, type, r, strerror(errno));
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Finished: %s type=%d (%s) (r=%d)", wrapper_dbip_info, type, filename, r);
	};

END_libipv6calc_db_wrapper:
	return(r);
};


/*
 * wrapper extension: DBIP_open_type
 * input:
 * 	type (mandatory)
 * 		if | 0x10000 -> info is opened and ptr is not cached
 * 	db_recno_max_ptr (set if not NULL)
 */
DB *libipv6calc_db_wrapper_DBIP_open_type(const unsigned int type_flag, long int *db_recno_max_ptr) {
	DB *dbp = NULL;
	DBC *dbcp;
	DBT key, data;

	int type = (type_flag & 0xffff);
	int info_selector = ((type_flag & 0x10000) != 0) ? 1 : 0;
	char *filename;
	int  entry = -1, i;
	int ret;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called: %s type=%d (%s)", wrapper_dbip_info, type, (info_selector == 0) ? "data" : "info");

	// check for valid type
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == (type & 0xffff)) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	if ((info_selector == 0) && (db_ptr_cache[entry] != NULL)) {
		// already open
		dbp = db_ptr_cache[entry];

		if (db_recno_max_ptr != NULL) {
			*db_recno_max_ptr = db_recno_max_cache[entry];
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Database already opened (cached) dbp=%p type=%d recno_max: %u", dbp, type, db_recno_max_cache[entry]);
		goto END_libipv6calc_db_wrapper;
	};

	// retrieve filename
	filename = libipv6calc_db_wrapper_DBIP_dbfilename(type);

	if (filename == NULL) {
		return(NULL);
	};

	if ((ret = db_create(&dbp, NULL, 0)) != 0) {
		if (ipv6calc_quiet == 0) {
			fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		};
		return(NULL);
	};

	if ((ret = dbp->open(dbp, NULL, filename, (info_selector == 0) ? "data" : "info", (info_selector == 0) ? DB_RECNO : DB_BTREE, DB_RDONLY, 0444)) != 0) {
		if (ipv6calc_quiet == 0) {
			fprintf(stderr, "db->open failed: %s (%s)\n", db_strerror(ret), filename);
		};
		return(NULL);
	};

	if (info_selector == 0) {
		// cache entry
		db_ptr_cache[entry] = dbp;

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
			// dbp->err(dbp, ret, "DB->cursor/DB_LAST"); // do not print error message if db is empty
			goto END_libipv6calc_db_wrapper_close_error;
		};

		/* Close the cursor. */
		if ((ret = dbcp->c_close(dbcp)) != 0) {
			dbp->err(dbp, ret, "DBcursor->close");
			goto END_libipv6calc_db_wrapper_close_error;
		};

		db_recno_max_cache[entry] = *(db_recno_t *)key.data;

		if (db_recno_max_cache[entry] < 2) {
			goto END_libipv6calc_db_wrapper_close_error;
		};

		if (db_recno_max_ptr != NULL) {
			*db_recno_max_ptr = db_recno_max_cache[entry];
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Database successfully opened (fill-cache), dbp=%p type=%d recno_max=%u", dbp, type, db_recno_max_cache[entry]);
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Database successfully opened, dbp=%p type=%d (info)", dbp, type);
	};

	// jump to "good end"
	goto END_libipv6calc_db_wrapper;

END_libipv6calc_db_wrapper_close_error:
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "error opening database, close now");
	libipv6calc_db_wrapper_DBIP_close(dbp);
	dbp = NULL;

END_libipv6calc_db_wrapper:
	return(dbp);
};


/*******************************
 * Wrapper functions for DBIP
 *******************************/


/*
 * wrapper: DBIP_database_info
 */
char *libipv6calc_db_wrapper_DBIP_database_info(const unsigned int type) {
	static char resultstring[NI_MAXHOST];
	char datastring[NI_MAXHOST];
	char tempstring[NI_MAXHOST];

	DB *dbp;
	int ret, i, entry = -1;


	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called: %s", wrapper_dbip_info);

	// check for valid type
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == (type & 0xffff)) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		ERRORPRINT_WA("Invalid type (FIX CODE): %d", type);
		goto END_libipv6calc_db_wrapper;
	};


	dbp = libipv6calc_db_wrapper_DBIP_open_type(type | 0x10000, NULL);

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

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Database dbusage string: %s", datastring);

	// get dbdate
	ret = libipv6calc_db_wrapper_bdb_get_data_by_key(dbp, "dbdate", datastring, sizeof(datastring));
	if (ret != 0) {
		snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve 'dbdate', unsupported db file");
		goto END_libipv6calc_db_wrapper_close;
	};

	char year[5];
	strncpy(year, datastring, 4);
	year[4] = '\0';
	snprintf(resultstring, sizeof(resultstring), "DBIP-%d/%s %s Copyright (c) %s db-ip.com All Rights Reserved"
		, type
		, datastring
		, (type <= 2) ? "FREE" : "COMM"
		, year
	);

	// get dbcreated_unixtime
	ret = libipv6calc_db_wrapper_bdb_get_data_by_key(dbp, "dbcreated_unixtime", datastring, sizeof(datastring));
	if (ret != 0) {
		snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve 'dbcreated_unixtime', unsupported db file");
		goto END_libipv6calc_db_wrapper_close;
	};

	wrapper_db_unixtime_DBIP[entry] = atoi(datastring);

	if (wrapper_db_unixtime_DBIP[entry] == 0) {
		snprintf(resultstring, sizeof(resultstring), "%s", "'dbcreated_unixtime' is not proper, unsupported db file");
		goto END_libipv6calc_db_wrapper_close;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "wrapper_db_unixtime_DBIP=%ld", (long int) wrapper_db_unixtime_DBIP[entry]);

	strftime(datastring, sizeof(datastring), "%Y%m%d-%H%M%S UTC", gmtime(&wrapper_db_unixtime_DBIP[entry]));
	snprintf(tempstring, sizeof(tempstring), "%s, created: %s", resultstring, datastring);
	snprintf(resultstring, sizeof(resultstring), "%s", tempstring);

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_DBIP_close(dbp);

END_libipv6calc_db_wrapper:
	return(resultstring);
};


/*
 * wrapper: DBIP_get_country_short
 */
char *libipv6calc_db_wrapper_DBIP_get_country_short(DB *dbp, char *ip) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called: %s ip=%s", wrapper_dbip_info, ip);

	// not implemented so far
	if (dbp == NULL) { }; // make compiler happy (avoid unused "...")
	//return(DBIP_get_country_short(loc, ip));
	return(NULL);
};


/*********************************************
 * Abstract functions
 * *******************************************/

/* function query for feature set
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_DBIP_has_features(uint32_t features) {
	int result = -1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called with feature value to test: 0x%08x", features);

	if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP] & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Return with result: %d", result);
	return(result);
};


/* country code */
int libipv6calc_db_wrapper_DBIP_wrapper_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len) {
	int result = -1;
	DB *dbp;

	static char resultstring[NI_MAXHOST];

	int DBIP_type = 0;

	long int recno_max;

	//DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called with addr=%s proto=%d", addr, proto);

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		DBIP_type = dbip_db_country_v4;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP] & IPV6CALC_DB_IPV4_TO_CC) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "No DBIP database supporting IPv4 country available");
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		DBIP_type = dbip_db_country_v6;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP] & IPV6CALC_DB_IPV6_TO_CC) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "No DBIP database supporting IPv6 country available");
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	dbp = libipv6calc_db_wrapper_DBIP_open_type(DBIP_type, &recno_max);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Error opening DBIP by type");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "database opened type=%d recno_max=%ld", DBIP_type, recno_max);

	result = libipv6calc_db_wrapper_get_entry_generic(
		(void *) dbp,						// pointer to database
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) ? IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2 : IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4,	// key format
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) ? 32 : 64,	// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		recno_max,						// number of rows
		ipaddrp->addr[0],					// lookup key MSB
		ipaddrp->addr[1],					// lookup key LSB
		resultstring,						// data ptr
		NULL							// function pointer
	);

	if (result < 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "resultstring=%s", resultstring);

	char *token, *ptrptr;

	int token_count = 0;

	// split result string
	ptrptr = resultstring;
	token = strsep(&ptrptr, ";");

	while (token != NULL) {
		token_count++;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Database entry found %d: %s", token_count, token);

		if (token_count == 1) {
			/* country (is always 1st token) */
			if (strlen(token) > 0) {
				snprintf(country, country_len, "%s", token);
			} else {
				snprintf(country, country_len, "%s", "-");
			};
			break;
		};

		/* get next token */
		token = strsep(&ptrptr, ";");
	};

	if (token_count != 1) {
		ERRORPRINT_WA("data has more entries than expected, corrupt database: %d", token_count);
		goto END_libipv6calc_db_wrapper_close;
	};

	if (strlen(country) == 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "did not return a record for 'country'");
	};

	result = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "result entry=%d: country=%s", result, country);

	DBIP_DB_USAGE_MAP_TAG(DBIP_type);

	goto END_libipv6calc_db_wrapper; // keep db open

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_DBIP_close(dbp);

END_libipv6calc_db_wrapper:
	return(result);
};


/* city (& region) */
int libipv6calc_db_wrapper_DBIP_wrapper_city_by_addr(const ipv6calc_ipaddr *ipaddrp, char *city, const size_t city_len, char *region, const size_t region_len) {
	int result = -1;
	DB *dbp;

	static char resultstring[NI_MAXHOST];

	int DBIP_type = 0;

	long int recno_max;

	int region_token = 2; // default
	int city_token = 3; // default

	//DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Called with addr=%s proto=%d", addr, proto);

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		DBIP_type = dbip_db_region_city_v4;
		if ((DBIP_type == DBIP_DB_IPV4_LOCATION) || (DBIP_type == DBIP_DB_IPV4_FULL)) {
			city_token++; // location/full have district inbetween;
		};

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP] & IPV6CALC_DB_IPV4_TO_CITY) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "No DBIP database supporting IPv4 city/region available");
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		DBIP_type = dbip_db_region_city_v6;
		if ((DBIP_type == DBIP_DB_IPV6_LOCATION) || (DBIP_type == DBIP_DB_IPV6_FULL)) {
			city_token++; // location/full have district inbetween;
		};

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP] & IPV6CALC_DB_IPV6_TO_CITY) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "No DBIP database supporting IPv6 city/region available");
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	dbp = libipv6calc_db_wrapper_DBIP_open_type(DBIP_type, &recno_max);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Error opening DBIP by type");
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_get_entry_generic(
		(void *) dbp,						// pointer to database
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) ? IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2 : IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4,	// key format
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) ? 32 : 64,	// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		recno_max,						// number of rows
		ipaddrp->addr[0],					// lookup key MSB
		ipaddrp->addr[1],					// lookup key LSB
		resultstring,						// data ptr
		NULL							// function pointer
	);

	if (result < 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "resultstring=%s", resultstring);

	char *token, *ptrptr;

	int token_count = 0;

	// split result string
	ptrptr = resultstring;
	token = strsep(&ptrptr, ";");

	while (token != NULL) {
		token_count++;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Database entry found %d: %s", token_count, token);

		if (token_count == region_token) {
			/* region */
			if (strlen(token) > 0) {
				snprintf(region, region_len, "%s", token);
			} else {
				snprintf(region, region_len, "%s", "-");
			};
		} else if (token_count == city_token) {
			/* city */
			if (strlen(token) > 0) {
				snprintf(city, city_len, "%s", token);
			} else {
				snprintf(city, city_len, "%s", "-");
			};
			break;
		};

		/* get next token */
		token = strsep(&ptrptr, ";");
	};

	if (token_count != city_token) {
		ERRORPRINT_WA("data has more entries than expected, corrupt database: %d", token_count);
		goto END_libipv6calc_db_wrapper_close;
	};

	if (strlen(region) == 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "did not return a record for 'region'");
	};

	if (strlen(city) == 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "did not return a record for 'city'");
	};

	result = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "result entry=%d: city=%s region=%s", result, city, region);

	DBIP_DB_USAGE_MAP_TAG(DBIP_type);

	goto END_libipv6calc_db_wrapper; // keep db open

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_DBIP_close(dbp);

END_libipv6calc_db_wrapper:
	return(result);
};


/* all information */
int libipv6calc_db_wrapper_DBIP_all_by_addr(const ipv6calc_ipaddr *ipaddrp, DBIP_Record *recordp) {
	int result = -1;

	DB *dbp;

	static char resultstring[NI_MAXHOST];

	int DBIP_type = 0;

	long int recno_max;

	// clear structure
	snprintf(recordp->country  , DBIP_SIZE_COUNTRY  , "%s", "");
	snprintf(recordp->stateprov, DBIP_SIZE_STATEPROV, "%s", "");
	snprintf(recordp->district , DBIP_SIZE_DISTRICT , "%s", "");
	snprintf(recordp->city     , DBIP_SIZE_CITY     , "%s", "");
	snprintf(recordp->zipcode  , DBIP_SIZE_ZIPCODE  , "%s", "");
	recordp->latitude = 0;
	recordp->longitude = 0;
	recordp->geoname_id = 0;
	recordp->timezone_offset = 0;
	snprintf(recordp->timezone_name    , DBIP_SIZE_TIMEZONE_NAME, "%s", "");
	snprintf(recordp->isp_name         , DBIP_SIZE_ISP_NAME     , "%s", "");
	snprintf(recordp->connection_type  , DBIP_SIZE_CONN_TYPE    , "%s", "");
	snprintf(recordp->organization_name, DBIP_SIZE_ORG_NAME     , "%s", "");

	int t_country = 1, t_stateprov = 0, t_district = 0, t_city = 0, t_zipcode = 0, t_latitude = 0 , t_longitude = 0, t_geoname_id = 0, \
		t_timezone_offset = 0, t_timezone_name = 0, t_isp_name = 0, t_connection_type = 0, t_organization_name = 0;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		DBIP_type = dbip_db_region_city_v4;

		if (DBIP_type == 0) {
			// fallback
			DBIP_type = dbip_db_country_v4;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		DBIP_type = dbip_db_region_city_v6;

		if (DBIP_type == 0) {
			// fallback
			DBIP_type = dbip_db_country_v6;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	// set token numbers
	// country : country
	// city    : country,stateprov,city
	// location: country,stateprov,district,city,zipcode,latitude,longitude,geoname_id,timezone_offset,timezone_name
	// isp     : country,isp_name,connection_type,organization_name
	// full    : country,stateprov,district,city,zipcode,latitude,longitude,geoname_id,timezone_offset,timezone_name,isp_name,connection_type,organization_name

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "DBIP_type: %d", DBIP_type);
	if ((DBIP_type == DBIP_DB_IPV4_CITY) || (DBIP_type == DBIP_DB_IPV6_CITY)) {
		t_stateprov = 2;
		t_city = 3;
	} else if ((DBIP_type == DBIP_DB_IPV4_LOCATION) || (DBIP_type == DBIP_DB_IPV4_FULL) \
                || (DBIP_type == DBIP_DB_IPV6_LOCATION) || (DBIP_type == DBIP_DB_IPV6_FULL)
	) {
		t_stateprov = 2;
		t_district = 3;
		t_city = 4;
		t_zipcode = 5;
		t_latitude = 6;
		t_longitude = 7;
		t_geoname_id = 8;
		t_timezone_offset = 9;
		t_timezone_name = 10;

		if ((DBIP_type == DBIP_DB_IPV4_FULL) || (DBIP_type == DBIP_DB_IPV6_FULL)) {
			t_isp_name = 11;
			t_connection_type = 12;
			t_organization_name = 13;
		};
	} else if ((DBIP_type == DBIP_DB_IPV4_ISP) || (DBIP_type == DBIP_DB_IPV6_ISP)) {
		t_isp_name = 2;
		t_connection_type = 3;
		t_organization_name = 4;
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Unsupported DBIP_type: %d", DBIP_type);
		goto END_libipv6calc_db_wrapper;
	};

	dbp = libipv6calc_db_wrapper_DBIP_open_type(DBIP_type, &recno_max);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "Error opening DBIP by type");
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_get_entry_generic(
		(void *) dbp,						// pointer to database
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) ? IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2 : IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4,	// key format
		(ipaddrp->proto == IPV6CALC_PROTO_IPV4) ? 32 : 64,	// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		recno_max,						// number of rows
		ipaddrp->addr[0],					// lookup key MSB
		ipaddrp->addr[1],					// lookup key LSB
		resultstring,						// data ptr
		NULL							// function pointer
	);

	if (result < 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "resultstring=%s", resultstring);

	char *token, *ptrptr;

	int token_count = 0;

	// split result string
	ptrptr = resultstring;
	token = strsep(&ptrptr, ";");

	while (token != NULL) {
		token_count++;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "Database entry found %d: %s", token_count, token);

		if (token_count == t_country) {
			// country
			snprintf(recordp->country  , DBIP_SIZE_COUNTRY  , "%s", token);
		} else if (token_count == t_stateprov) {
			snprintf(recordp->stateprov, DBIP_SIZE_STATEPROV, "%s", token);
		} else if (token_count == t_district) {
			snprintf(recordp->district , DBIP_SIZE_DISTRICT , "%s", token);
		} else if (token_count == t_city) {
			snprintf(recordp->city     , DBIP_SIZE_CITY     , "%s", token);
		} else if (token_count == t_zipcode) {
			snprintf(recordp->zipcode  , DBIP_SIZE_ZIPCODE  , "%s", token);
		} else if (token_count == t_latitude) {
			recordp->latitude = atof(token);
		} else if (token_count == t_longitude) {
			recordp->longitude = atof(token);
		} else if (token_count == t_geoname_id) {
			recordp->geoname_id = atol(token);
		} else if (token_count == t_timezone_offset) {
			recordp->timezone_offset = atof(token);
		} else if (token_count == t_timezone_name) {
			snprintf(recordp->timezone_name    , DBIP_SIZE_TIMEZONE_NAME, "%s", token);
		} else if (token_count == t_isp_name) {
			snprintf(recordp->isp_name         , DBIP_SIZE_ISP_NAME     , "%s", token);
		} else if (token_count == t_connection_type) {
			snprintf(recordp->connection_type  , DBIP_SIZE_CONN_TYPE    , "%s", token);
		} else if (token_count == t_organization_name) {
			snprintf(recordp->organization_name, DBIP_SIZE_ORG_NAME     , "%s", token);
		};

		/* get next token */
		token = strsep(&ptrptr, ";");
	};

	result = 0;

	DBIP_DB_USAGE_MAP_TAG(DBIP_type);

	goto END_libipv6calc_db_wrapper; // keep db open

END_libipv6calc_db_wrapper:

	return(result);
};


#endif
#endif
