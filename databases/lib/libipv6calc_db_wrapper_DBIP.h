/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_DBIP.h
 * Version    : $Id: libipv6calc_db_wrapper_DBIP.h,v 1.1 2014/08/27 06:48:55 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_DBIP.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_DBIP_h

#define _libipv6calc_db_wrapper_DBIP_h 1

#define DBIP_SUPPORT_UNKNOWN		0
#define DBIP_SUPPORT_COMPAT		1
#define DBIP_SUPPORT_FULL		2
#define DBIP_SUPPORT_NOTEXISTS		3

#define DBIP_IPV6_SUPPORT_UNKNOWN	0
#define DBIP_IPV6_SUPPORT_COMPAT	1
#define DBIP_IPV6_SUPPORT_FULL		2
#define DBIP_IPV6_SUPPORT_NOTEXISTS	3

// database types
#define DBIP_DB_IPV4_COUNTRY				1
#define DBIP_DB_IPV6_COUNTRY				2
#define DBIP_DB_IPV4_CITY				3
#define DBIP_DB_IPV6_CITY				4


/*@unused@*/ static const db_file_desc libipv6calc_db_wrapper_DBIP_db_file_desc[] = {
	{ DBIP_DB_IPV4_COUNTRY    , "ipv6calc-dbip-country-ipv4.db"      , "IPv4 Country" , IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_DBIP_IPV4 },
	{ DBIP_DB_IPV6_COUNTRY    , "ipv6calc-dbip-country-ipv6.db"      , "IPv6 Country" , IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_DBIP_IPV6 },
	{ DBIP_DB_IPV4_CITY       , "ipv6calc-dbip-city-ipv4.db"         , "IPv4 City"    , IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_DBIP_IPV4 },
	{ DBIP_DB_IPV6_CITY       , "ipv6calc-dbip-city-ipv6.db"         , "IPv6 City"    , IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_DBIP_IPV6 },
};

// features
extern uint32_t wrapper_features_DBIP;

/* text representations */
/*@unused@*/ static const s_type libipv6calc_db_wrapper_DBIP_support[] = {
	{ DBIP_SUPPORT_UNKNOWN,		"unknown" },
	{ DBIP_SUPPORT_COMPAT,		"compat" },
	{ DBIP_SUPPORT_FULL,		"full" },
	{ DBIP_SUPPORT_NOTEXISTS,	"not-exists" },
};

/* text representations */
/*@unused@*/ static const s_type libipv6calc_db_wrapper_DBIP_IPv6_support[] = {
	{ DBIP_IPV6_SUPPORT_UNKNOWN,	"unknown" },
	{ DBIP_IPV6_SUPPORT_COMPAT,	"compat" },
	{ DBIP_IPV6_SUPPORT_FULL,	"full" },
	{ DBIP_IPV6_SUPPORT_NOTEXISTS,	"not-exists" },
};

typedef struct
{
        FILE *filehandle;
        uint8_t databasetype;
        uint8_t databasecolumn;
        uint8_t databaseday;
        uint8_t databasemonth;
        uint8_t databaseyear;
        uint32_t databasecount;
        uint32_t databaseaddr;
        uint32_t ipversion;
} DBIP;


#endif

extern int         libipv6calc_db_wrapper_DBIP_wrapper_init(void);
extern int         libipv6calc_db_wrapper_DBIP_wrapper_cleanup(void);
extern const char *libipv6calc_db_wrapper_DBIP_wrapper_asnum_by_addr (const char *addr, const int proto);
extern void        libipv6calc_db_wrapper_DBIP_wrapper_info(char* string, const size_t size);
extern void        libipv6calc_db_wrapper_DBIP_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern char       *libipv6calc_db_wrapper_DBIP_wrapper_db_info_used(void);

extern char       *libipv6calc_db_wrapper_DBIP_wrapper_country_code_by_addr(char *addr, const int proto);
extern char       *libipv6calc_db_wrapper_DBIP_wrapper_country_name_by_addr(char *addr, const int proto);

extern int         libipv6calc_db_wrapper_DBIP_has_features(uint32_t features);


#ifdef SUPPORT_DBIP

#include <db.h>

extern char dbip_lib_file[NI_MAXHOST];
extern char dbip_db_dir[NI_MAXHOST];

extern int          libipv6calc_db_wrapper_DBIP_db_avail(int type);
extern const char  *libipv6calc_db_wrapper_DBIP_lib_version(void);
extern DB          *libipv6calc_db_wrapper_DBIP_open(char *db);
extern DB          *libipv6calc_db_wrapper_DBIP_open_type(int type);
extern uint32_t     libipv6calc_db_wrapper_DBIP_close(DB *dbp);
extern char        *libipv6calc_db_wrapper_DBIP_get_all(DB *dbp, char *ip);

extern const char  *libipv6calc_db_wrapper_DBIP_dbdescription(int type);
extern char        *libipv6calc_db_wrapper_DBIP_database_info(DB *dbp);

#endif
