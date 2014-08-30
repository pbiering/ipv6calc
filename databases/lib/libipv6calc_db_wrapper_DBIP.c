/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_DBIP.c
 * Version    : $Id: libipv6calc_db_wrapper_DBIP.c,v 1.6 2014/08/30 23:38:37 ds6peter Exp $
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
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

#include "config.h"

#include "libipv6calcdebug.h"
#include "libipv6calc.h"

#include "libipv6calc_db_wrapper.h"


#ifdef SUPPORT_DBIP

#include <db.h>

#include "libipv6calc_db_wrapper_DBIP.h"

char dbip_db_dir[NI_MAXHOST] = DBIP_DB;


uint32_t wrapper_features_DBIP = 0;

static const char* wrapper_dbip_info = "DBIP";

static int dbip_asnum_v4 = 0;
static int dbip_asnum_v6 = 0;
static int dbip_city_v4 = 0;
static int dbip_city_v6 = 0;

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


// local prototyping
static char     *libipv6calc_db_wrapper_DBIP_dbfilename(int type); 


/*
 * function initialise the DBIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_DBIP_wrapper_init(void) {
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Check for standard DBIP databases");

	/* check available databases for resolution */
	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "DBIP database test for availability: %s", libipv6calc_db_wrapper_DBIP_db_file_desc[i].filename);
		if (libipv6calc_db_wrapper_DBIP_db_avail(libipv6calc_db_wrapper_DBIP_db_file_desc[i].number) == 1) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "DBIP database available: %s", libipv6calc_db_wrapper_DBIP_db_file_desc[i].description);
			wrapper_features_DBIP |= libipv6calc_db_wrapper_DBIP_db_file_desc[i].feature;
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "custom directory: %s", dbip_db_dir);

	wrapper_features |= wrapper_features_DBIP;

	return 0;
};


/*
 * function cleanup the DBIP wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_DBIP_wrapper_cleanup(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_DBIP
	// no general cleanup provided
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
	return 0;
};


/*
 * function info of DBIP wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_DBIP_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_DBIP
	snprintf(string, size, "DBIP available databases: Country4=%d Country6=%d ASN4=%d ASN6=%d City4=%d City6=%d", (wrapper_features & IPV6CALC_DB_IPV4_TO_CC) ? 1 : 0, (wrapper_features & IPV6CALC_DB_IPV6_TO_CC) ? 1 : 0, dbip_asnum_v4, dbip_asnum_v6, dbip_city_v4, dbip_city_v6);
#else
	snprintf(string, size, "No DBIP support built-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
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

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	printf("%sDBIP: features: 0x%08x\n", prefix, wrapper_features_DBIP);

#ifdef SUPPORT_DBIP
	printf("%sDBIP: info of available databases in directory: %s\n", prefix, dbip_db_dir);

	for (i = 0; i < MAXENTRIES_ARRAY(libipv6calc_db_wrapper_DBIP_db_file_desc); i++) {
		type = libipv6calc_db_wrapper_DBIP_db_file_desc[i].number;

		if (libipv6calc_db_wrapper_DBIP_db_avail(type)) {
			// DBIP returned that database is available
			dbp = libipv6calc_db_wrapper_DBIP_open_type(type);
			if (dbp == NULL) {
				printf("%sDBIP: %-27s: %-40s (CAN'T OPEN)\n", prefix, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description, libipv6calc_db_wrapper_DBIP_dbfilename(type));
			} else {
				printf("%sDBIP: %-27s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description, libipv6calc_db_wrapper_DBIP_dbfilename(type), libipv6calc_db_wrapper_DBIP_database_info(dbp));
				libipv6calc_db_wrapper_DBIP_close(dbp);
				count++;
			};
		} else {
			if (level_verbose == LEVEL_VERBOSE2) {
				printf("%sDBIP: %-27s: %-40s (%s)\n", prefix, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description, libipv6calc_db_wrapper_DBIP_dbfilename(type), strerror(errno));
			};
			continue;
		};
	};

	if (count == 0) {
		printf("%sDBIP: NO available databases found in directory: %s\n", prefix, dbip_db_dir);
	};
#else // SUPPORT_DBIP
	snprintf(string, size, "%sNo DBIP support built-in", prefix);
#endif // SUPPORT_DBIP

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
	return;
};


/*
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_DBIP_wrapper_db_info_used(void) {
	int db;
	DB *dbp;
	char tempstring[NI_MAXHOST];
	char *info;

	for (db = 0; db < 32 * DBIP_DB_MAX_BLOCKS_32; db++) {
		if ((dbip_db_usage_map[db / 32] & (1 << (db % 32))) != 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_DBIP, "DB used: %d", db);

			dbp = libipv6calc_db_wrapper_DBIP_open_type(db);
			info = libipv6calc_db_wrapper_DBIP_database_info(dbp);

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			if (strlen(dbip_db_usage_string) > 0) {
				if (strstr(dbip_db_usage_string, info) != NULL) { continue; }; // string already included

				snprintf(tempstring, sizeof(tempstring), "%s / %s", dbip_db_usage_string, info);
			} else {
				snprintf(tempstring, sizeof(tempstring), "%s", info);
			};

			snprintf(dbip_db_usage_string, sizeof(dbip_db_usage_string), "%s", tempstring);
		};
	};

	return(dbip_db_usage_string);
};



#ifdef SUPPORT_DBIP

/*******************************
 * Wrapper extension functions for DBIP
 * because of missing support in library itself
 *******************************/

/*
 * wrapper extension: DBIP_dbfilename
 */
static char *libipv6calc_db_wrapper_DBIP_dbfilename(int type) {
	static char tempstring[NI_MAXHOST];
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s type=%d", wrapper_dbip_info, type);

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

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished: %s type=%d has filename=%s", wrapper_dbip_info, type, tempstring);

	return(tempstring);
};


/*
 * wrapper extension: DBIP_dbdescription
 */
const char *libipv6calc_db_wrapper_DBIP_dbdescription(int type) {
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s type=%d", wrapper_dbip_info, type);

	for (i = 0; i < sizeof(libipv6calc_db_wrapper_DBIP_db_file_desc) / sizeof(libipv6calc_db_wrapper_DBIP_db_file_desc[0]); i++) {
		if (libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return("unknown");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished: %s type=%d has description=%s", wrapper_dbip_info, type, libipv6calc_db_wrapper_DBIP_db_file_desc[i].description);

	return(libipv6calc_db_wrapper_DBIP_db_file_desc[i].description);
};


/*
 * wrapper extension: DBIP_db_avail
 * ret: 1=avail  0=not-avail
 */
int libipv6calc_db_wrapper_DBIP_db_avail(int type) {
	char *filename;
	int r = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s type=%d", wrapper_dbip_info, type);

	filename = libipv6calc_db_wrapper_DBIP_dbfilename(type);

	if (filename == NULL) {
		goto END_libipv6calc_db_wrapper;
	};

	r = (access(filename, R_OK) == 0) ? 1:0;

	if (r == 0) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished: %s type=%d (still unknown) (r=%d: %s)", wrapper_dbip_info, type, r, strerror(errno));
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished: %s type=%d (%s) (r=%d)", wrapper_dbip_info, type, filename, r);
	};

END_libipv6calc_db_wrapper:
	return(r);
};


/*
 * wrapper extension: DBIP_open_type
 */
DB *libipv6calc_db_wrapper_DBIP_open_type(int type) {
	char *filename;
	int  entry = -1, i;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s type=%d", wrapper_dbip_info, type);

	for (i = 0; i < sizeof(libipv6calc_db_wrapper_DBIP_db_file_desc) / sizeof(libipv6calc_db_wrapper_DBIP_db_file_desc[0]); i++) {
		if (libipv6calc_db_wrapper_DBIP_db_file_desc[i].number == type) {
			entry = i;
			break;
		};
	};

	if (entry < 0) {
		return(NULL);
	};

	filename = libipv6calc_db_wrapper_DBIP_dbfilename(type);

	if (filename == NULL) {
		return(NULL);
	};

	return(libipv6calc_db_wrapper_DBIP_open(filename));
};


/*******************************
 * Wrapper functions for DBIP
 *******************************/



/*
 * wrapper: DBIP_open
 */
DB *libipv6calc_db_wrapper_DBIP_open(char *db) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s filename=%s", wrapper_dbip_info, db);

	DB *dbp;
	int ret;

	if ((ret = db_create(&dbp, NULL, 0)) != 0) {
		fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		return(NULL);
	};
	if ((ret = dbp->open(dbp, NULL, db, NULL, DB_RECNO, DB_RDONLY, 0444)) != 0) {
		dbp->err(dbp, ret, "%s", db);
		return(NULL);
	};

	return(dbp);
};


/*
 * wrapper: DBIP_close
 */
uint32_t libipv6calc_db_wrapper_DBIP_close(DB *dbp) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s", wrapper_dbip_info);

	if (dbp != NULL) {
		dbp->close(dbp, 0);
	};

	return(0);
};


/*
 * wrapper: DBIP_database_info
 */
char *libipv6calc_db_wrapper_DBIP_database_info(DB *dbp) {
	static char resultstring[NI_MAXHOST];
	static char dbyear[5]; // YYYY + '\0'
	static char dbdate[9]; // YYYYMMDD + '\0'
	static char dbtype[5]; // DDDD + '\0'
	//char *dbusage, *dbformat, *dbdate, *dbtype, *dbproto, *dbcreated;

	DBT key, data;
	DBC *dbcp;
	int ret;

	char datastring[NI_MAXHOST];

	u_long recno, recno_max;

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s", wrapper_dbip_info);

	if (dbp == NULL) {
		snprintf(resultstring, sizeof(resultstring), "%s", "can't retrieve database information");
	} else {
		memset(&key, 0, sizeof(key));
		memset(&data, 0, sizeof(data));

		recno = 1; // info

		key.data = &recno;
		key.size = sizeof(recno);

		if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) != 0) {
			dbp->err(dbp, ret, "DB->get");
			snprintf(resultstring, sizeof(resultstring), "CORRUPT DB FILE"); 
			return(resultstring);
			//return(NULL);
		}

		snprintf(datastring, (data.size + 1) >= sizeof(datastring) ? sizeof(datastring) : data.size + 1, "%s", (char *) data.data);

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database info string: %s", datastring);

		// split info string
		token = strtok_r(datastring, ";", ptrptr);
		while (token != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database info string token/value pair found: %s", token);

			if (strncmp(token, "dbdate=", strlen("dbdate=")) == 0) {
				strncpy(dbdate, token + strlen("dbdate="), sizeof(dbdate) - 1);
				dbdate[sizeof(dbdate) - 1] = '\0';
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database info string token/value found: %s=%s", "dbdate", dbdate);

				strncpy(dbyear, token + strlen("dbdate="), sizeof(dbyear) - 1);
				dbyear[sizeof(dbyear) - 1] = '\0';
			};

			if (strncmp(token, "dbtype=", strlen("dbtype=")) == 0) {
				strncpy(dbtype, token + strlen("dbdate="), sizeof(dbtype) - 1);
				dbtype[sizeof(dbtype) - 1] = '\0';
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database info string token/value found: %s=%s", "dbtype", dbtype);
			};

			/* get next token */
			token = strtok_r(NULL, ";", ptrptr);
		};

		/* Re-initialize the key/data pair. */
		memset(&key, 0, sizeof(key));
		memset(&data, 0, sizeof(data));

		/* Acquire a cursor for the database. */
		if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
			dbp->err(dbp, ret, "DB->cursor");
			return (NULL);
		};

		/* Jump to last entry of the database */
		if ((ret = dbcp->c_get(dbcp, &key, &data, DB_LAST)) != 0) {
			dbp->err(dbp, ret, "DB->cursor/DB_LAST");
			return (NULL);
		};

		/* Close the cursor. */
		if ((ret = dbcp->c_close(dbcp)) != 0) {
			dbp->err(dbp, ret, "DBcursor->close");
			return (NULL);
		};

		recno_max = *(long unsigned int *)key.data;

		/* get last line */
		key.data = &recno_max;
		key.size = sizeof(recno_max);

		if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) != 0) {
			dbp->err(dbp, ret, "DB->get");
			return(NULL);
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database last entry key=%lu data=%.*s", *(u_long *)key.data, (int)data.size, (char *)data.data);

		snprintf(resultstring, sizeof(resultstring), "DBIP-DB%s %8s Copyright (c) %4s db-ip.com All Rights Reserved", 
                                dbtype,
				dbdate,
				dbyear
		);
	};

	return(resultstring);
};


/*
 * wrapper: DBIP_get_country_short
 */
char *libipv6calc_db_wrapper_DBIP_get_country_short(DB *dbp, char *ip) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: %s ip=%s", wrapper_dbip_info, ip);


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

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with feature value to test: 0x%08x", features);

	if ((wrapper_features_DBIP & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Return with result: %d", result);
	return(result);
};

/* country_code */
char *libipv6calc_db_wrapper_DBIP_wrapper_country_code_by_addr(const char *addr, const int proto) {
	DB *dbp;
	char *cc;

	static char resultstring[NI_MAXHOST];

	int DBIP_type = 0;
	char *DBIP_result_ptr = NULL;

	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	int result;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		DBIP_type = DBIP_DB_IPV4_COUNTRY;
		// convert char to structure
		result = addr_to_ipv4addrstruct(addr, resultstring, sizeof(resultstring), &ipv4addr);
	} else if (proto == 6) {
		DBIP_type = DBIP_DB_IPV6_COUNTRY;
		// convert char to structure
		result = addr_to_ipv6addrstruct(addr, resultstring, sizeof(resultstring), &ipv6addr);
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Unsupported proto: %d", proto);
		goto END_libipv6calc_db_wrapper;
	};

	if (result != 0) {
		ERRORPRINT_WA("error converting address string for proto %d: %s", proto, addr);
		goto END_libipv6calc_db_wrapper;
	};

	dbp = libipv6calc_db_wrapper_DBIP_open_type(DBIP_type);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Error opening DBIP by type");
		goto END_libipv6calc_db_wrapper;
	};

	if (proto == 4) {
		result = libipv6calc_db_wrapper_get_dbentry_by_ipv4addr(&ipv4addr, dbp, IPV6CALC_BDB_FORMAT_CHECK_FIRST_LAST | IPV6CALC_BDB_FORMAT_CHECK_32 | 0x01 | 0x10, resultstring, sizeof(resultstring));
	} else if (proto == 6) {
		result = libipv6calc_db_wrapper_get_dbentry_by_ipv6addr(&ipv6addr, dbp, IPV6CALC_BDB_FORMAT_CHECK_FIRST_LAST | IPV6CALC_BDB_FORMAT_CHECK_64 | 0x01 | 0x10, resultstring, sizeof(resultstring));
	};

	cc = resultstring;

	if (cc == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "did not return a record");
		goto END_libipv6calc_db_wrapper_close;
	};

	DBIP_result_ptr = cc;

	if (DBIP_result_ptr == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "did not contain a country_short code");
		goto END_libipv6calc_db_wrapper_close;
	};

	if (strlen(DBIP_result_ptr) > 2) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "did not return a proper country_short code (length > 2): %s", DBIP_result_ptr);
		goto END_libipv6calc_db_wrapper_close;
	};

	DBIP_DB_USAGE_MAP_TAG(DBIP_type);

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_DBIP_close(dbp);

END_libipv6calc_db_wrapper:
	return(DBIP_result_ptr);
};

/* country_code */
char *libipv6calc_db_wrapper_DBIP_wrapper_city_by_addr(const char *addr, const int proto) {
	DB *dbp;

	char *city;

	static char resultstring[NI_MAXHOST];

	int DBIP_type = 0;
	char *DBIP_result_ptr = NULL;

	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	int result;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with addr=%s proto=%d", addr, proto);

	if (proto == 4) {
		DBIP_type = DBIP_DB_IPV4_CITY;
		// convert char to structure
		result = addr_to_ipv4addrstruct(addr, resultstring, sizeof(resultstring), &ipv4addr);
	} else if (proto == 6) {
		DBIP_type = DBIP_DB_IPV6_CITY;
		// convert char to structure
		result = addr_to_ipv6addrstruct(addr, resultstring, sizeof(resultstring), &ipv6addr);
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Unsupported proto: %d", proto);
		goto END_libipv6calc_db_wrapper;
	};

	if (result != 0) {
		ERRORPRINT_WA("error converting address string for proto %d: %s", proto, addr);
		goto END_libipv6calc_db_wrapper;
	};

	dbp = libipv6calc_db_wrapper_DBIP_open_type(DBIP_type);

	if (dbp == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Error opening DBIP by type");
		goto END_libipv6calc_db_wrapper;
	};

	if (proto == 4) {
		result = libipv6calc_db_wrapper_get_dbentry_by_ipv4addr(&ipv4addr, dbp, IPV6CALC_BDB_FORMAT_CHECK_FIRST_LAST | IPV6CALC_BDB_FORMAT_CHECK_32 | 0x03 | 0x30, resultstring, sizeof(resultstring));
	} else if (proto == 6) {
		result = libipv6calc_db_wrapper_get_dbentry_by_ipv6addr(&ipv6addr, dbp, IPV6CALC_BDB_FORMAT_CHECK_FIRST_LAST | IPV6CALC_BDB_FORMAT_CHECK_64 | 0x03 | 0x30, resultstring, sizeof(resultstring));
	};

	city = resultstring;

	if (city == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "did not return a record");
		goto END_libipv6calc_db_wrapper_close;
	};

	DBIP_result_ptr = city;

	if (DBIP_result_ptr == NULL) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "did not contain a country_short code");
		goto END_libipv6calc_db_wrapper_close;
	};

	if (strlen(DBIP_result_ptr) > 2) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "did not return a proper country_short code (length > 2): %s", DBIP_result_ptr);
		goto END_libipv6calc_db_wrapper_close;
	};

	DBIP_DB_USAGE_MAP_TAG(DBIP_type);

END_libipv6calc_db_wrapper_close:
	libipv6calc_db_wrapper_DBIP_close(dbp);

END_libipv6calc_db_wrapper:
	return(DBIP_result_ptr);
};

#endif
#endif
