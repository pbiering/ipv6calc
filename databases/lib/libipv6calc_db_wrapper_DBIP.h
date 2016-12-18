/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_DBIP.h
 * Version    : $Id$
 * Copyright  : 2014-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_DBIP.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_DBIP_h

#define _libipv6calc_db_wrapper_DBIP_h 1

// database types
#define DBIP_DB_IPV4_COUNTRY				1
#define DBIP_DB_IPV6_COUNTRY				2
#define DBIP_DB_IPV4_CITY				3
#define DBIP_DB_IPV6_CITY				4
#define DBIP_DB_MAX					(DBIP_DB_IPV6_CITY + 1)


/*@unused@*/ static const db_file_desc libipv6calc_db_wrapper_DBIP_db_file_desc[] = {
	{ DBIP_DB_IPV4_COUNTRY    , "ipv6calc-dbip-ipv4-country.db"      , "IPv4 CountryCode" , IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_DBIP_IPV4 },
	{ DBIP_DB_IPV6_COUNTRY    , "ipv6calc-dbip-ipv6-country.db"      , "IPv6 CountryCode" , IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_DBIP_IPV6 },
	{ DBIP_DB_IPV4_CITY       , "ipv6calc-dbip-ipv4-city.db"         , "IPv4 City/Region/CountryCode", IPV6CALC_DB_IPV4_TO_CITY | IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_DBIP_IPV4 },
	{ DBIP_DB_IPV6_CITY       , "ipv6calc-dbip-ipv6-city.db"         , "IPv6 City/Region/CountryCode", IPV6CALC_DB_IPV6_TO_CITY | IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_DBIP_IPV6 },
};

// features
extern uint32_t wrapper_features_DBIP;

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
extern void        libipv6calc_db_wrapper_DBIP_wrapper_info(char* string, const size_t size);
extern void        libipv6calc_db_wrapper_DBIP_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern char       *libipv6calc_db_wrapper_DBIP_wrapper_db_info_used(void);

extern int         libipv6calc_db_wrapper_DBIP_wrapper_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len);
extern int	   libipv6calc_db_wrapper_DBIP_wrapper_city_by_addr(const ipv6calc_ipaddr *ipaddrp, char *city, const size_t city_len, char *region, const size_t region_len); 

extern int         libipv6calc_db_wrapper_DBIP_has_features(const uint32_t features);


#ifdef SUPPORT_DBIP

#include <db.h>

extern char dbip_db_dir[NI_MAXHOST];

extern int          libipv6calc_db_wrapper_DBIP_db_avail(const int type);
extern DB          *libipv6calc_db_wrapper_DBIP_open_type(const int type_flag, long int *db_recno_max_ptr);

extern const char  *libipv6calc_db_wrapper_DBIP_dbdescription(const int type);
extern char        *libipv6calc_db_wrapper_DBIP_database_info(const int type);

#endif
