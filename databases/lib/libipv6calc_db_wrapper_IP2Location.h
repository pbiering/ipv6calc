/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_IP2Location.h
 * Version    : $Id: libipv6calc_db_wrapper_IP2Location.h,v 1.4 2014/07/20 10:28:40 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_IP2Location.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_IP2Location_h

#define _libipv6calc_db_wrapper_IP2Location_h 1

#ifdef SUPPORT_IP2LOCATION
#include "IP2Location.h"
#endif

#define IP2LOCATION_SUPPORT_UNKNOWN		0
#define IP2LOCATION_SUPPORT_COMPAT		1
#define IP2LOCATION_SUPPORT_FULL		2
#define IP2LOCATION_SUPPORT_NOTEXISTS		3

#define IP2LOCATION_IPV6_SUPPORT_UNKNOWN	0
#define IP2LOCATION_IPV6_SUPPORT_COMPAT		1
#define IP2LOCATION_IPV6_SUPPORT_FULL		2
#define IP2LOCATION_IPV6_SUPPORT_NOTEXISTS	3

// database types
#define IP2LOCATION_DB_IP_COUNTRY				1
#define IP2LOCATION_DB_IPV6_COUNTRY				12

#define IP2LOCATION_DB_IP_COUNTRY_ISP				257
#define IP2LOCATION_DB_IP_COUNTRY_REGION_CITY			258
#define IP2LOCATION_DB_IP_COUNTRY_REGION_CITY_ISP		259
#define IP2LOCATION_DB_IP_COUNTRY_REGION_CITY_ISP_DOMAIN	260

// database names and descriptions
typedef struct {
	const unsigned int number;
	const char        *filename;
	const char        *description;
	const uint32_t     feature;
} db_file_desc;

/*@unused@*/ static const db_file_desc libipv6calc_db_wrapper_IP2Location_db_file_desc[] = {
	{ IP2LOCATION_DB_IP_COUNTRY                , "IP-COUNTRY.BIN"                , "IPv4 Country"                , IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IP2LOCATION_IPV4 },
	{ IP2LOCATION_DB_IPV6_COUNTRY              , "IPV6-COUNTRY.BIN"              , "IPv6 Country"                , IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_IP2LOCATION_IPV6 },
};

// features
extern uint32_t wrapper_features_IP2Location;

/* text representations */
/*@unused@*/ static const s_type libipv6calc_db_wrapper_IP2Location_support[] = {
	{ IP2LOCATION_SUPPORT_UNKNOWN,		"unknown" },
	{ IP2LOCATION_SUPPORT_COMPAT,		"compat" },
	{ IP2LOCATION_SUPPORT_FULL,		"full" },
	{ IP2LOCATION_SUPPORT_NOTEXISTS,	"not-exists" },
};

/* text representations */
/*@unused@*/ static const s_type libipv6calc_db_wrapper_IP2Location_IPv6_support[] = {
	{ IP2LOCATION_IPV6_SUPPORT_UNKNOWN,	"unknown" },
	{ IP2LOCATION_IPV6_SUPPORT_COMPAT,	"compat" },
	{ IP2LOCATION_IPV6_SUPPORT_FULL,	"full" },
	{ IP2LOCATION_IPV6_SUPPORT_NOTEXISTS,	"not-exists" },
};


#endif

extern int         libipv6calc_db_wrapper_IP2Location_wrapper_init(void);
extern int         libipv6calc_db_wrapper_IP2Location_wrapper_cleanup(void);
extern const char *libipv6calc_db_wrapper_IP2Location_wrapper_asnum_by_addr (const char *addr, const int proto);
extern void        libipv6calc_db_wrapper_IP2Location_wrapper_info(char* string, const size_t size);
extern void        libipv6calc_db_wrapper_IP2Location_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern char       *libipv6calc_db_wrapper_IP2Location_wrapper_db_info_used(void);

extern char       *libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr(char *addr, const int proto);
extern char       *libipv6calc_db_wrapper_IP2Location_wrapper_country_name_by_addr(char *addr, const int proto);

extern int         libipv6calc_db_wrapper_IP2Location_has_features(uint32_t features);


#ifdef SUPPORT_IP2LOCATION
extern char ip2location_lib_file[NI_MAXHOST];
extern char ip2location_db_dir[NI_MAXHOST];

extern int                libipv6calc_db_wrapper_IP2Location_db_avail(int type);
extern const char        *libipv6calc_db_wrapper_IP2Location_lib_version(void);
extern IP2Location       *libipv6calc_db_wrapper_IP2Location_open(char *db);
extern IP2Location       *libipv6calc_db_wrapper_IP2Location_open_type(int type);
extern uint32_t           libipv6calc_db_wrapper_IP2Location_close(IP2Location *loc);
extern IP2LocationRecord *libipv6calc_db_wrapper_IP2Location_get_all(IP2Location *loc, char *ip);
void                      libipv6calc_db_wrapper_IP2Location_free_record(IP2LocationRecord *record);

extern const char  *libipv6calc_db_wrapper_IP2Location_dbdescription(int type);
extern char        *libipv6calc_db_wrapper_IP2Location_database_info(IP2Location *loc);

#endif
