/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_DBIP2.c
 * Copyright  : 2019-2025 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc DB-IP.com database wrapper for MaxMindDB databases
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


#ifdef SUPPORT_DBIP2

#include "libipv6calc_db_wrapper_DBIP2.h"

char dbip2_db_dir[PATH_MAX] = DBIP2_DB;

static const char* wrapper_dbip2_info = "DBIP2";


/* database files */
static const db_file_desc2 libipv6calc_db_wrapper_DBIP2_db_file_desc[] = {
	// country (lite)
	{ DBIP2_DB_COUNTRY_LITE
		, "dbip-country-lite.mmdb"
		, "IPv4/v6 CountryCode (LITE)"
		, IPV6CALC_DB_IP_TO_CC | IPV6CALC_DB_DBIP | IPV6CALC_DB_IP_TO_GEONAMEID | IPV6CALC_DB_IP_TO_COUNTRY
		, IPV6CALC_DB_DBIP2_INTERNAL_LITE
	},

	// city (lite)
	{ DBIP2_DB_CITY_LITE
		, "dbip-city-lite.mmdb"
		, "IPv4/v6 City (LITE)"
		, IPV6CALC_DB_IP_TO_CITY | IPV6CALC_DB_IP_TO_REGION | IPV6CALC_DB_IP_TO_CC | IPV6CALC_DB_DBIP | IPV6CALC_DB_IP_TO_GEONAMEID | IPV6CALC_DB_IP_TO_COUNTRY
		, IPV6CALC_DB_DBIP2_INTERNAL_LITE
	},

	// asn (lite)
	{ DBIP2_DB_ASN_LITE
		, "dbip-asn-lite.mmdb"
		, "IPv4/v6 ASN (LITE)"
		, IPV6CALC_DB_IP_TO_AS | IPV6CALC_DB_DBIP
		, IPV6CALC_DB_DBIP2_INTERNAL_LITE
	},

	// country
	{ DBIP2_DB_COUNTRY
		, "dbip-country.mmdb"
		, "IPv4/v6 CountryCode (COMM)"
		, IPV6CALC_DB_IP_TO_CC | IPV6CALC_DB_DBIP | IPV6CALC_DB_IP_TO_GEONAMEID | IPV6CALC_DB_IP_TO_COUNTRY
		, IPV6CALC_DB_DBIP2_INTERNAL_COMM
	},

	// location
	{ DBIP2_DB_LOCATION
		, "dbip-location.mmdb"
		, "IPv4/v6 Location (COMM)"
		, IPV6CALC_DB_IP_TO_CITY | IPV6CALC_DB_IP_TO_REGION | IPV6CALC_DB_IP_TO_CC | IPV6CALC_DB_DBIP | IPV6CALC_DB_IP_TO_COUNTRY | IPV6CALC_DB_IP_TO_GEONAMEID
		, IPV6CALC_DB_DBIP2_INTERNAL_COMM
	},

	// ISP
	{ DBIP2_DB_ISP
		, "dbip-isp.mmdb"
		, "IPv4/v6 ISP (COMM)", IPV6CALC_DB_IP_TO_CC | IPV6CALC_DB_DBIP | IPV6CALC_DB_IP_TO_AS
		, IPV6CALC_DB_DBIP2_INTERNAL_COMM
	},

	// full
	{ DBIP2_DB_LOCATION_ISP
		, "dbip-location-isp.mmdb"
		, "IPv4/v6 Location+ISP (COMM)"
		, IPV6CALC_DB_IP_TO_CITY | IPV6CALC_DB_IP_TO_REGION | IPV6CALC_DB_IP_TO_CC | IPV6CALC_DB_DBIP | IPV6CALC_DB_IP_TO_COUNTRY | IPV6CALC_DB_IP_TO_AS | IPV6CALC_DB_IP_TO_GEONAMEID
		, IPV6CALC_DB_DBIP2_INTERNAL_COMM
	},
};


// DB used for resolution after automatic election, which is currently done by checking for DB files and the sequence in the header file
static int dbip2_db_country_v4 = 0;
static int dbip2_db_country_v6 = 0;
static int dbip2_db_region_city_v4 = 0;
static int dbip2_db_region_city_v6 = 0;
static int dbip2_db_asn_v4 = 0;
static int dbip2_db_asn_v6 = 0;
static int dbip2_db_geonameid_v4 = 0;
static int dbip2_db_geonameid_v6 = 0;

#define DBIP2_LITE	0
#define DBIP2_COMM	1
#define	DBIP2_MAX	2

static s_libipv6calc_db_wrapper_db_info dbip2_db_country_v4_best[DBIP2_MAX];
static s_libipv6calc_db_wrapper_db_info dbip2_db_country_v6_best[DBIP2_MAX];
static s_libipv6calc_db_wrapper_db_info dbip2_db_region_city_v4_best[DBIP2_MAX];
static s_libipv6calc_db_wrapper_db_info dbip2_db_region_city_v6_best[DBIP2_MAX];
static s_libipv6calc_db_wrapper_db_info dbip2_db_asn_v4_best[DBIP2_MAX];
static s_libipv6calc_db_wrapper_db_info dbip2_db_asn_v6_best[DBIP2_MAX];
static s_libipv6calc_db_wrapper_db_info dbip2_db_geonameid_v4_best[DBIP2_MAX];
static s_libipv6calc_db_wrapper_db_info dbip2_db_geonameid_v6_best[DBIP2_MAX];

static int type2index[DBIP2_DB_MAX+1];

// select LITE database if COMM is older than given months
int dbip2_db_comm_to_free_switch_min_delta_months = 12;

// select better database of same product (COMM/LITE) only if not older than given months
int dbip2_db_better_max_delta_months = 1;

// select only a specific DBIP2 db type
int dbip2_db_only_type = 0;

/* database usage map */
#define DBIP2_DB_MAX_BLOCKS_32	2	// 0-63
static uint32_t dbip2_db_usage_map[DBIP2_DB_MAX_BLOCKS_32];

#define DBIP2_DB_USAGE_MAP_TAG(db)	if (db < (32 * DBIP2_DB_MAX_BLOCKS_32)) { \
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Tag usage for db: %d", db); \
							dbip2_db_usage_map[db / 32] |= 1 << (db % 32); \
						} else { \
							fprintf(stderr, "FIXME: unsupported db value (exceed limit): %d (%d)\n", db, 32 * DBIP2_DB_MAX_BLOCKS_32 - 1); \
							exit(1); \
						};

char dbip2_db_usage_string[IPV6CALC_STRING_MAX] = "";

// local cache
static MMDB_s mmdb_cache[DBIP2_DB_MAX+1];

// local prototyping
static char     *libipv6calc_db_wrapper_DBIP2_dbfilename(const unsigned int type); 
static void libipv6calc_db_wrapper_DBIP2_close(const int type);


/*
 * function initialise the DBIP2 wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_DBIP2_wrapper_init(void) {
	int i, dbym, dbtype, ret, product;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called");

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Check for standard DBIP2 databases");

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "custom directory: %s", dbip2_db_dir);

	/* check available databases for resolution */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP2_db_file_desc); i++) {
		// add entry to mapping
		type2index[libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number] = i;

		// clean local cache
		mmdb_cache[libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number].file_size = 0;
		mmdb_cache[libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number].flags = 0;

		// add features to implemented
		wrapper_features_by_source_implemented[IPV6CALC_DB_SOURCE_DBIP2] |= libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "DBIP2 database test for availability: %s", libipv6calc_db_wrapper_DBIP2_db_file_desc[i].filename);
		if (libipv6calc_db_wrapper_DBIP2_db_avail(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number) == 1) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "DBIP2 database available: %s", libipv6calc_db_wrapper_DBIP2_db_file_desc[i].description);
			wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] |= libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features;
		} else {
			// db not available
			continue;
		};

		ret = libipv6calc_db_wrapper_DBIP2_open_type(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number);
		if (ret != MMDB_SUCCESS) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Error opening DBIP2 by type");
			continue;
		};

		dbtype = libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number;

		if ((dbip2_db_only_type > 0) && (dbip2_db_only_type != dbtype)) {
			// not selected
			continue;
		};

		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].internal & IPV6CALC_DB_DBIP2_INTERNAL_LITE) != 0) {
			product = DBIP2_LITE;
		} else {
			product = DBIP2_COMM;
		};

		time_t db_time = mmdb_cache[dbtype].metadata.build_epoch;
		struct tm *db_gmtime = gmtime(&db_time);

		dbym = (db_gmtime->tm_year + 1900) * 12 + db_gmtime->tm_mon;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "DBIP2 type=%d dbym=%d Year/Month=%d unixtime=%lld", dbtype, dbym, UNPACK_YM(dbym), (long long unsigned int) mmdb_cache[dbtype].metadata.build_epoch);

#define DBIP2_DB_SELECT_BETTER(best) \
			if ( \
			  (best.num == 0) \
			  || ( \
				(best.dbym > 0) \
				  && ((best.dbym - dbym) <= dbip2_db_better_max_delta_months) \
			     ) \
			) { \
				best.num = libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number; \
				best.dbym = dbym; \
				best.dbtype = dbtype; \
			};


		// note: databases are listed in sequence "less data" before "more data"
		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features & IPV6CALC_DB_IPV4_TO_CC) != 0) {
			DBIP2_DB_SELECT_BETTER(dbip2_db_country_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features & IPV6CALC_DB_IPV6_TO_CC) != 0) {
			DBIP2_DB_SELECT_BETTER(dbip2_db_country_v6_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features & (IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV4_TO_CITY)) != 0) {
			DBIP2_DB_SELECT_BETTER(dbip2_db_region_city_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features & (IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV6_TO_CITY)) != 0) {
			DBIP2_DB_SELECT_BETTER(dbip2_db_region_city_v6_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features & IPV6CALC_DB_IPV4_TO_AS) != 0) {
			DBIP2_DB_SELECT_BETTER(dbip2_db_asn_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features & IPV6CALC_DB_IPV6_TO_AS) != 0) {
			DBIP2_DB_SELECT_BETTER(dbip2_db_asn_v6_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features & IPV6CALC_DB_IPV4_TO_GEONAMEID) != 0) {
			DBIP2_DB_SELECT_BETTER(dbip2_db_geonameid_v4_best[product])
		};

		if ((libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features & IPV6CALC_DB_IPV6_TO_GEONAMEID) != 0) {
			DBIP2_DB_SELECT_BETTER(dbip2_db_geonameid_v6_best[product])
		};

		wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] |= libipv6calc_db_wrapper_DBIP2_db_file_desc[i].features;
	};

	// select free instead of comm, if comm is outdated and free available
	if (dbip2_db_comm_to_free_switch_min_delta_months > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "DBIP2 database priority check (comm->free) after months: %d", dbip2_db_comm_to_free_switch_min_delta_months);
#define SELECT_LITE_INSTEAD_OF_COMM(free, comm, final) \
	if ((free.num > 0) && (comm.num > 0)) { \
		if (free.dbym - comm.dbym > dbip2_db_comm_to_free_switch_min_delta_months) { \
			final = free.num; \
		}; \
	};
		SELECT_LITE_INSTEAD_OF_COMM(dbip2_db_country_v4_best[DBIP2_LITE], dbip2_db_country_v4_best[DBIP2_COMM], dbip2_db_country_v4)
		SELECT_LITE_INSTEAD_OF_COMM(dbip2_db_country_v6_best[DBIP2_LITE], dbip2_db_country_v6_best[DBIP2_COMM], dbip2_db_country_v6)
		SELECT_LITE_INSTEAD_OF_COMM(dbip2_db_region_city_v4_best[DBIP2_LITE], dbip2_db_region_city_v4_best[DBIP2_COMM], dbip2_db_region_city_v4)
		SELECT_LITE_INSTEAD_OF_COMM(dbip2_db_region_city_v6_best[DBIP2_LITE], dbip2_db_region_city_v6_best[DBIP2_COMM], dbip2_db_region_city_v6)
		SELECT_LITE_INSTEAD_OF_COMM(dbip2_db_asn_v4_best[DBIP2_LITE], dbip2_db_asn_v4_best[DBIP2_COMM], dbip2_db_asn_v4)
		SELECT_LITE_INSTEAD_OF_COMM(dbip2_db_asn_v6_best[DBIP2_LITE], dbip2_db_asn_v6_best[DBIP2_COMM], dbip2_db_asn_v6)
		SELECT_LITE_INSTEAD_OF_COMM(dbip2_db_geonameid_v4_best[DBIP2_LITE], dbip2_db_geonameid_v4_best[DBIP2_COMM], dbip2_db_geonameid_v4)
		SELECT_LITE_INSTEAD_OF_COMM(dbip2_db_geonameid_v6_best[DBIP2_LITE], dbip2_db_geonameid_v6_best[DBIP2_COMM], dbip2_db_geonameid_v6)
	};

#define FILL_EMPTY(product, final) \
	if ((product.num > 0) && (final == 0)) { \
		final = product.num; \
	};

	// fill empty ones with comm
	FILL_EMPTY(dbip2_db_country_v4_best[DBIP2_COMM], dbip2_db_country_v4)
	FILL_EMPTY(dbip2_db_country_v6_best[DBIP2_COMM], dbip2_db_country_v6)
	FILL_EMPTY(dbip2_db_region_city_v4_best[DBIP2_COMM], dbip2_db_region_city_v4)
	FILL_EMPTY(dbip2_db_region_city_v6_best[DBIP2_COMM], dbip2_db_region_city_v6)
	FILL_EMPTY(dbip2_db_asn_v4_best[DBIP2_COMM], dbip2_db_asn_v4)
	FILL_EMPTY(dbip2_db_asn_v6_best[DBIP2_COMM], dbip2_db_asn_v6)
	FILL_EMPTY(dbip2_db_geonameid_v4_best[DBIP2_COMM], dbip2_db_geonameid_v4)
	FILL_EMPTY(dbip2_db_geonameid_v6_best[DBIP2_COMM], dbip2_db_geonameid_v6)

	// fill empty ones with free
	FILL_EMPTY(dbip2_db_country_v4_best[DBIP2_LITE], dbip2_db_country_v4)
	FILL_EMPTY(dbip2_db_country_v6_best[DBIP2_LITE], dbip2_db_country_v6)
	FILL_EMPTY(dbip2_db_region_city_v4_best[DBIP2_LITE], dbip2_db_region_city_v4)
	FILL_EMPTY(dbip2_db_region_city_v6_best[DBIP2_LITE], dbip2_db_region_city_v6)
	FILL_EMPTY(dbip2_db_asn_v4_best[DBIP2_LITE], dbip2_db_asn_v4)
	FILL_EMPTY(dbip2_db_asn_v6_best[DBIP2_LITE], dbip2_db_asn_v6)
	FILL_EMPTY(dbip2_db_geonameid_v4_best[DBIP2_LITE], dbip2_db_geonameid_v4)
	FILL_EMPTY(dbip2_db_geonameid_v6_best[DBIP2_LITE], dbip2_db_geonameid_v6)


	/* close handles which are not necessary further on */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP2_db_file_desc); i++) {
		if (mmdb_cache[libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number].file_size > 0) {
			if (
				(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_country_v4_best[DBIP2_LITE].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_country_v4_best[DBIP2_COMM].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_country_v6_best[DBIP2_LITE].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_country_v6_best[DBIP2_COMM].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_region_city_v4_best[DBIP2_LITE].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_region_city_v4_best[DBIP2_COMM].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_region_city_v6_best[DBIP2_LITE].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_region_city_v6_best[DBIP2_COMM].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_asn_v4_best[DBIP2_LITE].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_asn_v4_best[DBIP2_COMM].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_asn_v6_best[DBIP2_LITE].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_asn_v6_best[DBIP2_COMM].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_geonameid_v4_best[DBIP2_LITE].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_geonameid_v4_best[DBIP2_COMM].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_geonameid_v6_best[DBIP2_LITE].num)
			    ||	(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == dbip2_db_geonameid_v6_best[DBIP2_COMM].num)
			) {
				// database is in use
				continue;
			};

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Close further unused DBIP2: type=%d desc='%s'", libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].description);
			libipv6calc_db_wrapper_DBIP2_close(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number);
		};
	};

	wrapper_features |= wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2];

	return 0;
};


/*
 * wrapper: DBIP2_close
 */
static void libipv6calc_db_wrapper_DBIP2_close(const int type) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called type=%u", type);

	if (mmdb_cache[type].file_size > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Close DBIP2: type=%d desc='%s'", type, libipv6calc_db_wrapper_DBIP2_db_file_desc[type2index[type]].description);
		libipv6calc_db_wrapper_MMDB_close(&mmdb_cache[type]);
		/* cleanup cache entry */
		mmdb_cache[type].file_size = 0;
	} else if (mmdb_cache[type].flags > 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Already closed DBIP2: type=%d desc='%s'", type, libipv6calc_db_wrapper_DBIP2_db_file_desc[type2index[type]].description);
	};
};


/*
 * function cleanup the DBIP2 wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_DBIP2_wrapper_cleanup(void) {
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called");

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP2_db_file_desc); i++) {
		libipv6calc_db_wrapper_DBIP2_close(i);
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Finished");
	return 0;
};


/*
 * function info of DBIP2 wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_DBIP2_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called");

	snprintf(string, size, "db-ip.com(MMDB) database support: Country4=%d Country6=%d City4=%d City6=%d ASN4=%d ASN6=%d GeonameID4=%d GeonameID6=%d"
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV4_TO_CC) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV6_TO_CC) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV4_TO_CITY) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV6_TO_CITY) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV4_TO_AS) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV6_TO_AS) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV4_TO_GEONAMEID) ? 1 : 0
		, (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV6_TO_GEONAMEID) ? 1 : 0
	);

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Finished");
	return;
};

/*
 * function print database info of DBIP2 wrapper
 *
 * in : (void)
 * out: (void)
 */
void libipv6calc_db_wrapper_DBIP2_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	int ret, i, type, count = 0;
	s_libipv6calc_db_wrapper_db_info_all db_info_all;

	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called");

	IPV6CALC_DB_FEATURE_INFO(prefix, IPV6CALC_DB_SOURCE_DBIP2)

	fprintf(stderr, "%sdb-ip.com(MMDB): info of available databases in directory: %s\n", prefix, dbip2_db_dir);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP2_db_file_desc); i++) {
		type = libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number;

		if (libipv6calc_db_wrapper_DBIP2_db_avail(type)) {
			// DBIP2 returned that database is available
			ret = libipv6calc_db_wrapper_DBIP2_open_type(type);
			if (ret != MMDB_SUCCESS) {
				fprintf(stderr, "%sdb-ip.com(MMDB): %-30s: [%1d] %-30s (CAN'T OPEN database information)\n", prefix, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].description, type, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].filename);
			} else {
				fprintf(stderr, "%sdb-ip.com(MMDB): %-30s: [%1d] %-30s (%s)\n", prefix, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].description, type, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].filename, libipv6calc_db_wrapper_DBIP2_database_info(type));
				libipv6calc_db_wrapper_DBIP2_close(type);
				count++;
			};
		} else {
			if (level_verbose == LEVEL_VERBOSE2) {
				fprintf(stderr, "%sdb-ip.com(MMDB): %-30s: [%1d] %-30s (%s)\n", prefix, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].description, type, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].filename, strerror(errno));
			};
			continue;
		};
	};

	if (count == 0) {
		fprintf(stderr, "%sdb-ip.com(MMDB): NO available databases found in directory: %s\n", prefix, dbip2_db_dir);
	} else {
		if (level_verbose >= LEVEL_VERBOSE) {
			db_info_all.country4.num    = dbip2_db_country_v4_best[DBIP2_LITE].num;
			db_info_all.country4.dbtype = dbip2_db_country_v4_best[DBIP2_LITE].dbtype;
			db_info_all.country4.dbym   = dbip2_db_country_v4_best[DBIP2_LITE].dbym;
			db_info_all.country6.num    = dbip2_db_country_v6_best[DBIP2_LITE].num;
			db_info_all.country6.dbtype = dbip2_db_country_v6_best[DBIP2_LITE].dbtype;
			db_info_all.country6.dbym   = dbip2_db_country_v6_best[DBIP2_LITE].dbym;
			db_info_all.city4.num    = dbip2_db_region_city_v4_best[DBIP2_LITE].num;
			db_info_all.city4.dbtype = dbip2_db_region_city_v4_best[DBIP2_LITE].dbtype;
			db_info_all.city4.dbym   = dbip2_db_region_city_v4_best[DBIP2_LITE].dbym;
			db_info_all.city6.num    = dbip2_db_region_city_v6_best[DBIP2_LITE].num;
			db_info_all.city6.dbtype = dbip2_db_region_city_v6_best[DBIP2_LITE].dbtype;
			db_info_all.city6.dbym   = dbip2_db_region_city_v6_best[DBIP2_LITE].dbym;
			db_info_all.asn4.num    = dbip2_db_asn_v4_best[DBIP2_LITE].num;
			db_info_all.asn4.dbtype = dbip2_db_asn_v4_best[DBIP2_LITE].dbtype;
			db_info_all.asn4.dbym   = dbip2_db_asn_v4_best[DBIP2_LITE].dbym;
			db_info_all.asn6.num    = dbip2_db_asn_v6_best[DBIP2_LITE].num;
			db_info_all.asn6.dbtype = dbip2_db_asn_v6_best[DBIP2_LITE].dbtype;
			db_info_all.asn6.dbym   = dbip2_db_asn_v6_best[DBIP2_LITE].dbym;
			db_info_all.geonameid4.num    = dbip2_db_geonameid_v4_best[DBIP2_LITE].num;
			db_info_all.geonameid4.dbtype = dbip2_db_geonameid_v4_best[DBIP2_LITE].dbtype;
			db_info_all.geonameid4.dbym   = dbip2_db_geonameid_v4_best[DBIP2_LITE].dbym;
			db_info_all.geonameid6.num    = dbip2_db_geonameid_v6_best[DBIP2_LITE].num;
			db_info_all.geonameid6.dbtype = dbip2_db_geonameid_v6_best[DBIP2_LITE].dbtype;
			db_info_all.geonameid6.dbym   = dbip2_db_geonameid_v6_best[DBIP2_LITE].dbym;
			libipv6calc_db_wrapper_print_db_info_line(level_verbose, prefix_string, "db-ip.com(MMDB)", "detected best databases LITE", 1, db_info_all, 0);

			db_info_all.country4.num    = dbip2_db_country_v4_best[DBIP2_COMM].num;
			db_info_all.country4.dbtype = dbip2_db_country_v4_best[DBIP2_COMM].dbtype;
			db_info_all.country4.dbym   = dbip2_db_country_v4_best[DBIP2_COMM].dbym;
			db_info_all.country6.num    = dbip2_db_country_v6_best[DBIP2_COMM].num;
			db_info_all.country6.dbtype = dbip2_db_country_v6_best[DBIP2_COMM].dbtype;
			db_info_all.country6.dbym   = dbip2_db_country_v6_best[DBIP2_COMM].dbym;
			db_info_all.city4.num    = dbip2_db_region_city_v4_best[DBIP2_COMM].num;
			db_info_all.city4.dbtype = dbip2_db_region_city_v4_best[DBIP2_COMM].dbtype;
			db_info_all.city4.dbym   = dbip2_db_region_city_v4_best[DBIP2_COMM].dbym;
			db_info_all.city6.num    = dbip2_db_region_city_v6_best[DBIP2_COMM].num;
			db_info_all.city6.dbtype = dbip2_db_region_city_v6_best[DBIP2_COMM].dbtype;
			db_info_all.city6.dbym   = dbip2_db_region_city_v6_best[DBIP2_COMM].dbym;
			db_info_all.asn4.num    = dbip2_db_asn_v4_best[DBIP2_COMM].num;
			db_info_all.asn4.dbtype = dbip2_db_asn_v4_best[DBIP2_COMM].dbtype;
			db_info_all.asn4.dbym   = dbip2_db_asn_v4_best[DBIP2_COMM].dbym;
			db_info_all.asn6.num    = dbip2_db_asn_v6_best[DBIP2_COMM].num;
			db_info_all.asn6.dbtype = dbip2_db_asn_v6_best[DBIP2_COMM].dbtype;
			db_info_all.asn6.dbym   = dbip2_db_asn_v6_best[DBIP2_COMM].dbym;
			db_info_all.geonameid4.num    = dbip2_db_geonameid_v4_best[DBIP2_COMM].num;
			db_info_all.geonameid4.dbtype = dbip2_db_geonameid_v4_best[DBIP2_COMM].dbtype;
			db_info_all.geonameid4.dbym   = dbip2_db_geonameid_v4_best[DBIP2_COMM].dbym;
			db_info_all.geonameid6.num    = dbip2_db_geonameid_v6_best[DBIP2_COMM].num;
			db_info_all.geonameid6.dbtype = dbip2_db_geonameid_v6_best[DBIP2_COMM].dbtype;
			db_info_all.geonameid6.dbym   = dbip2_db_geonameid_v6_best[DBIP2_COMM].dbym;
			libipv6calc_db_wrapper_print_db_info_line(level_verbose, prefix_string, "db-ip.com(MMDB)", "detected best databases COMM", 1, db_info_all, 0);

			db_info_all.country4.num    = dbip2_db_country_v4;
			db_info_all.country6.num    = dbip2_db_country_v6;
			db_info_all.city4.num    = dbip2_db_region_city_v4;
			db_info_all.city6.num    = dbip2_db_region_city_v6;
			db_info_all.asn4.num    = dbip2_db_asn_v4;
			db_info_all.asn6.num    = dbip2_db_asn_v6;
			db_info_all.geonameid4.num    = dbip2_db_geonameid_v4;
			db_info_all.geonameid6.num    = dbip2_db_geonameid_v6;
			libipv6calc_db_wrapper_print_db_info_line(level_verbose, prefix_string, "db-ip.com(MMDB)", "selected best databases normal", 1, db_info_all, 1);

			if (dbip2_db_comm_to_free_switch_min_delta_months > 0) {
				fprintf(stderr, "%sdb-ip.com(MMDB): selected best databases method: COMM older than %d months are deselected in case of LITE is available\n"
					, prefix
					, dbip2_db_comm_to_free_switch_min_delta_months
				);
			};

			if (dbip2_db_better_max_delta_months > 0) {
				fprintf(stderr, "%sdb-ip.com(MMDB): selected best databases method: COMM/LITE with more features are only selected in case not older than %d months of already found COMM/LITE\n"
					, prefix
					, dbip2_db_better_max_delta_months
				);
			};

			if (dbip2_db_only_type > 0) {
				fprintf(stderr, "%sdb-ip.com(MMDB): selected best databases method: by applying given DB type filter: %d\n"
					, prefix
					, dbip2_db_only_type
				);
			};
		};
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Finished");
	return;
};


/*
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_DBIP2_wrapper_db_info_used(void) {
	int type, i;
	char tempstring[IPV6CALC_STRING_MAX];
	char *info;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called");

	for (i = 0; i < DBIP2_DB_MAX_BLOCKS_32; i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "dbip2_db_usage_map[%d]=%08x", i, (unsigned int) dbip2_db_usage_map[i]);
	};

	for (type = 0; type < 32 * DBIP2_DB_MAX_BLOCKS_32; type++) {
		if ((dbip2_db_usage_map[type / 32] & (1 << (type % 32))) != 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "DB type used: %d", type);

			info = libipv6calc_db_wrapper_DBIP2_database_info(type);

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "type=%d info=%s", type, info);

			STRCLR(tempstring);
			if (strlen(dbip2_db_usage_string) > 0) {
				if (strstr(dbip2_db_usage_string, info) != NULL) {
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "type=%d info=%s (skip, already displayed)", type, info);
					continue;
				}; // string already included

				STRCAT(tempstring, dbip2_db_usage_string);
				STRCAT(tempstring, " / ");
			};
			STRCAT(tempstring, info);

			snprintf(dbip2_db_usage_string, sizeof(dbip2_db_usage_string), "%s", tempstring);
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "type=%d dbip2_db_usage_string=%s", type, dbip2_db_usage_string);
		};
	};

	return(dbip2_db_usage_string);
};


/*******************************
 * Wrapper extension functions for DBIP2
 *******************************/

/*
 * wrapper extension: DBIP2_dbfilename
 */
static char *libipv6calc_db_wrapper_DBIP2_dbfilename(const unsigned int type) {
	static char tempstring[IPV6CALC_STRING_MAX];
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called: %s type=%d", wrapper_dbip2_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP2_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	STRCLR(tempstring);
	STRCAT(tempstring, dbip2_db_dir);
	STRCAT(tempstring, "/");
	STRCAT(tempstring, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].filename);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Finished: %s type=%d has filename=%s", wrapper_dbip2_info, type, tempstring);

	return(tempstring);
};


/*
 * wrapper extension: DBIP2_dbdescription
 */
const char *libipv6calc_db_wrapper_DBIP2_dbdescription(const unsigned int type) {
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called: %s type=%d", wrapper_dbip2_info, type);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP2_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return("unknown");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Finished: %s type=%d has description=%s", wrapper_dbip2_info, type, libipv6calc_db_wrapper_DBIP2_db_file_desc[i].description);

	return(libipv6calc_db_wrapper_DBIP2_db_file_desc[i].description);
};


/*
 * wrapper extension: DBIP2_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_DBIP2_db_avail(const unsigned int type) {
	char *filename;
	int r = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called: %s type=%d", wrapper_dbip2_info, type);

	filename = libipv6calc_db_wrapper_DBIP2_dbfilename(type);

	if (filename == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	r = (access(filename, R_OK) == 0) ? 1:0;

	if (r == 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Finished: %s type=%d (still unknown) (r=%d: %s)", wrapper_dbip2_info, type, r, strerror(errno));
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Finished: %s type=%d (%s) (r=%d)", wrapper_dbip2_info, type, filename, r);
	};

END_libipv6calc_db_wrapper:
	return(r);
};


/*
 * wrapper extension: DBIP2_open_type
 * input:
 * 	type (mandatory)
 * modified:
 * 	mmdb_cache
 * output:
 * 	result
 */
int libipv6calc_db_wrapper_DBIP2_open_type(const unsigned int type) {
	char *filename;
	int  entry = -1, i;
	int ret;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called: %s type=%d", wrapper_dbip2_info, type);

	// check for valid type
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP2_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		mmdb_cache[type].file_size = 0;
		mmdb_cache[type].flags = 0;
		ret = MMDB_FILE_OPEN_ERROR;
		goto END_libipv6calc_db_wrapper;
	};

	if (mmdb_cache[type].file_size > 0) {
		// already open
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Database already opened (cached) type=%d", type);
		ret = MMDB_SUCCESS;
		goto END_libipv6calc_db_wrapper;
	};

	// retrieve filename
	filename = libipv6calc_db_wrapper_DBIP2_dbfilename(type);

	if (filename == NULL) {
		mmdb_cache[type].file_size = 0;
		mmdb_cache[type].flags = 0;
		ret = MMDB_FILE_OPEN_ERROR;
		goto END_libipv6calc_db_wrapper;
	};

	ret = libipv6calc_db_wrapper_MMDB_open(filename, MMDB_MODE_MMAP, &mmdb_cache[type]);

	if (ret != MMDB_SUCCESS) {
		if (ipv6calc_quiet == 0) {
			fprintf(stderr, "MMDB_open failed: %s (%s)\n", libipv6calc_db_wrapper_MMDB_strerror(ret), filename);
			if (ret == MMDB_IO_ERROR) {
				fprintf(stderr, "MMDB_open IO error: %s (%s)\n", strerror(errno), filename);
			};
		};
		mmdb_cache[type].file_size = 0;
		mmdb_cache[type].flags = 0;
		ret = MMDB_FILE_OPEN_ERROR;
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Database successfully opened (fill-cache), type=%d", type);

END_libipv6calc_db_wrapper:
	return(ret);
};


/*******************************
 * Wrapper functions for DBIP2
 *******************************/


/*
 * wrapper: DBIP2_database_info
 */
char *libipv6calc_db_wrapper_DBIP2_database_info(const unsigned int type) {
	static char resultstring[IPV6CALC_STRING_MAX];
	char datastring[IPV6CALC_STRING_MAX];

	MMDB_s mmdb;
	int ret, i, entry = -1;


	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called: %s", wrapper_dbip2_info);

	// check for valid type
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP2_db_file_desc); i++) {
		if (libipv6calc_db_wrapper_DBIP2_db_file_desc[i].number == (type & 0xffff)) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		ERRORPRINT_WA("Invalid type (FIX CODE): %d", type);
		goto END_libipv6calc_db_wrapper;
	};


	ret = libipv6calc_db_wrapper_DBIP2_open_type(type);

	if (ret != MMDB_SUCCESS) {
		snprintf(resultstring, sizeof(resultstring), "%s", "(CAN'T OPEN database information)");
		goto END_libipv6calc_db_wrapper;
	};

	// get metadata
	//
/*
 const char *meta_dump = "\n"
                            "  Database metadata\n"
                            "    Node count:    %i\n"
                            "    Record size:   %i bits\n"
                            "    IP version:    IPv%i\n"
                            "    Binary format: %i.%i\n"
                            "    Build epoch:   %llu (%s)\n"
                            "    Type:          %s\n"
                            "    Languages:     ";

    fprintf(stdout, meta_dump,
*/

	mmdb = mmdb_cache[type];

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "meta: NodeCount=%i RecordSize=%i IpVersion=%i BinaryFormat=%i.%i BuildTime=%llu Type=%s", \
            mmdb.metadata.node_count,
            mmdb.metadata.record_size,
            mmdb.metadata.ip_version,
            mmdb.metadata.binary_format_major_version,
            mmdb.metadata.binary_format_minor_version,
            (long long unsigned int) mmdb.metadata.build_epoch,
            mmdb.metadata.database_type);

	const time_t epoch = (const time_t)mmdb.metadata.build_epoch;

	char year[5];
	strftime(year, sizeof(year), "%Y", gmtime(&epoch));
	snprintf(resultstring, sizeof(resultstring), "%s Copyright (c) %s db-ip.com All Rights Reserved"
		, mmdb.metadata.database_type
		, year
	);

	strftime(datastring, sizeof(datastring), "%Y%m%d-%H%M%S UTC", gmtime(&epoch));
	STRCAT(resultstring, ", created: ");
	STRCAT(resultstring, datastring);

END_libipv6calc_db_wrapper:
	return(resultstring);
};


/*********************************************
 * Abstract functions
 * *******************************************/

/* function query for feature set
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_DBIP2_has_features(uint32_t features) {
	int result = -1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Called with feature value to test: 0x%08x", features);

	if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Return with result: %d", result);
	return(result);
};


/* country code */
int libipv6calc_db_wrapper_DBIP2_wrapper_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len) {
	int result = MMDB_INVALID_DATA_ERROR;

	int DBIP2_type = 0;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		DBIP2_type = dbip2_db_country_v4;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV4_TO_CC) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "No DBIP2 database supporting IPv4 country available");
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		DBIP2_type = dbip2_db_country_v6;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV6_TO_CC) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "No DBIP2 database supporting IPv6 country available");
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_DBIP2_open_type(DBIP2_type);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Error opening DBIP2 by type");
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_MMDB_country_code_by_addr(ipaddrp, country, country_len, &mmdb_cache[DBIP2_type]);
 
	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "result country=%s", country);

	DBIP2_DB_USAGE_MAP_TAG(DBIP2_type);

	goto END_libipv6calc_db_wrapper; // keep db open

END_libipv6calc_db_wrapper:
	return(result);
};


/*
 * get AS 32-bit number and orgname (optional)
 * in : ipaddrp (mandatory)
 * in : data_source_ptr (optional, can be NULL)
 * out: as_orgname (optional if not NULL and as_orgname_length != 0)
 * in : as_orgname_length (optional required != 0)
 * out: AS 32-bit number
 */
uint32_t libipv6calc_db_wrapper_DBIP2_wrapper_asn_by_addr(const ipv6calc_ipaddr *ipaddrp, char *as_orgname, const size_t as_orgname_length) {
	uint32_t as_num = ASNUM_AS_UNKNOWN;
	int result;

	int DBIP2_type = 0;
	libipv6calc_db_wrapper_geolocation_record record_asn;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		DBIP2_type = dbip2_db_asn_v4;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV4_TO_AS) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "No DBIP2 database supporting IPv4 ASN");
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		DBIP2_type = dbip2_db_asn_v6;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV6_TO_AS) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "No DBIP2 database supporting IPv6 ASN");
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_DBIP2_open_type(DBIP2_type);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Error opening DBIP2 by type");
		goto END_libipv6calc_db_wrapper;
	};

	// AS Number
	as_num = libipv6calc_db_wrapper_MMDB_asn_by_addr(ipaddrp, &mmdb_cache[DBIP2_type]);
 
	if (as_num == ASNUM_AS_UNKNOWN) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "result ASN=%u", as_num);

	// AS Text (optional)
	if ((as_orgname != NULL) && (as_orgname_length > 0)) {
		result = libipv6calc_db_wrapper_MMDB_all_by_addr(ipaddrp, &record_asn, &mmdb_cache[DBIP2_type]);

		if (result != MMDB_SUCCESS) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "no match found");
			goto END_libipv6calc_db_wrapper;
		};

		// copy information
		snprintf(as_orgname, as_orgname_length, "%s", record_asn.organization_name);

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "result AS_ORGNAME=%s", as_orgname);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "lookup AS_ORGNAME skipped");
	};

	DBIP2_DB_USAGE_MAP_TAG(DBIP2_type);

	goto END_libipv6calc_db_wrapper; // keep db open

END_libipv6calc_db_wrapper:
	return(as_num);
};


/* GeonameID */
uint32_t libipv6calc_db_wrapper_DBIP2_wrapper_GeonameID_by_addr(const ipv6calc_ipaddr *ipaddrp, int *source_ptr) {
	uint32_t result = IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN;

	int DBIP2_type = 0;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		DBIP2_type = dbip2_db_geonameid_v4;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV4_TO_GEONAMEID) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "No DBIP2 database supporting IPv4 GeonameID");
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		DBIP2_type = dbip2_db_geonameid_v6;

		if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP2] & IPV6CALC_DB_IPV6_TO_GEONAMEID) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "No DBIP2 database supporting IPv6 GeonameID");
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_DBIP2_open_type(DBIP2_type);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Error opening DBIP2 by type");
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_MMDB_GeonameID_by_addr(ipaddrp, &mmdb_cache[DBIP2_type], source_ptr);

	if (result == IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "result GeonameID=%u", result);

	DBIP2_DB_USAGE_MAP_TAG(DBIP2_type);

	goto END_libipv6calc_db_wrapper;

END_libipv6calc_db_wrapper:
	return(result);
};


/* all information */
int libipv6calc_db_wrapper_DBIP2_all_by_addr(const ipv6calc_ipaddr *ipaddrp, libipv6calc_db_wrapper_geolocation_record *recordp) {
	int result = -1;

	int DBIP2_type = 0;
	int DBIP2_type_asn = 0;

	libipv6calc_db_wrapper_geolocation_record record_asn;

	libipv6calc_db_wrapper_geolocation_record_clear(recordp);

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		DBIP2_type = dbip2_db_region_city_v4;
		DBIP2_type_asn = dbip2_db_asn_v4;

		if (DBIP2_type == 0) {
			// fallback
			DBIP2_type = dbip2_db_country_v4;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		DBIP2_type = dbip2_db_region_city_v6;
		DBIP2_type_asn = dbip2_db_asn_v6;

		if (DBIP2_type == 0) {
			// fallback
			DBIP2_type = dbip2_db_country_v6;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Unsupported proto: %d", ipaddrp->proto);
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_DBIP2_open_type(DBIP2_type);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Error opening DBIP2 by type");
		goto END_libipv6calc_db_wrapper;
	};

	result = libipv6calc_db_wrapper_MMDB_all_by_addr(ipaddrp, recordp, &mmdb_cache[DBIP2_type]);

	if (result != MMDB_SUCCESS) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "no match found");
		goto END_libipv6calc_db_wrapper;
	};

	// ASN is stored in a different database
	if (DBIP2_type_asn > 0) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "fetch ASN information from dedicated database");
		result = libipv6calc_db_wrapper_DBIP2_open_type(DBIP2_type_asn);

		if (result != MMDB_SUCCESS) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "Error opening DBIP2 by type");
			goto END_libipv6calc_db_wrapper;
		};

		result = libipv6calc_db_wrapper_MMDB_all_by_addr(ipaddrp, &record_asn, &mmdb_cache[DBIP2_type_asn]);

		if (result != MMDB_SUCCESS) {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_DBIP2, "no match found");
			goto END_libipv6calc_db_wrapper;
		};

		// copy information
		recordp->asn = record_asn.asn;
                snprintf(recordp->organization_name, IPV6CALC_DB_SIZE_ORG_NAME, "%s", record_asn.organization_name);

		DBIP2_DB_USAGE_MAP_TAG(DBIP2_type_asn);
	};

	DBIP2_DB_USAGE_MAP_TAG(DBIP2_type);

END_libipv6calc_db_wrapper:
	return(result);
};

#endif
