/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_GeoIP.h
 * Version    : $Id: libipv6calc_db_wrapper_GeoIP.h,v 1.2 2013/06/23 13:43:17 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_GeoIP.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_GeoIP_h

#define _libipv6calc_db_wrapper_GeoIP_h 1


#define IPV6CALC_DB_GEOIP_LIB_NAME	"libGeoIP.so.1"

#define IPV6CALC_DB_GEOIP_CUSTOM_DIR	"/usr/share/GeoIP"

#ifdef SUPPORT_GEOIP
#include "GeoIP.h"
#include "GeoIPCity.h"
#endif

#define GEOIP_SUPPORT_UNKOWN		0
#define GEOIP_SUPPORT_COMPAT		1
#define GEOIP_SUPPORT_FULL		2
#define GEOIP_SUPPORT_NOTEXISTS		3

#define GEOIP_IPV6_SUPPORT_UNKOWN	0
#define GEOIP_IPV6_SUPPORT_COMPAT	1
#define GEOIP_IPV6_SUPPORT_FULL		2
#define GEOIP_IPV6_SUPPORT_NOTEXISTS	3

/* text representations */
/*@unused@*/ static const s_type libipv6calc_db_wrapper_GeoIP_support[] = {
	{ GEOIP_SUPPORT_UNKOWN,		"unknown" },
	{ GEOIP_SUPPORT_COMPAT,		"compat" },
	{ GEOIP_SUPPORT_FULL,		"full" },
	{ GEOIP_SUPPORT_NOTEXISTS,	"not-exists" },
};

/* text representations */
/*@unused@*/ static const s_type libipv6calc_db_wrapper_GeoIP_IPv6_support[] = {
	{ GEOIP_IPV6_SUPPORT_UNKOWN,	"unknown" },
	{ GEOIP_IPV6_SUPPORT_COMPAT,	"compat" },
	{ GEOIP_IPV6_SUPPORT_FULL,	"full" },
	{ GEOIP_IPV6_SUPPORT_NOTEXISTS,	"not-exists" },
};


#endif

extern int libipv6calc_db_wrapper_GeoIP_wrapper_init(void);
extern int libipv6calc_db_wrapper_GeoIP_wrapper_cleanup(void);

#ifdef SUPPORT_GEOIP
extern GeoIP*        libipv6calc_db_wrapper_GeoIP_open(const char * filename, int flags);
extern GeoIP*        libipv6calc_db_wrapper_GeoIP_open_type (int type, int flags);
extern int           libipv6calc_db_wrapper_GeoIP_db_avail(int type);
extern int           libipv6calc_db_wrapper_GeoIP_cleanup(void);
extern unsigned char libipv6calc_db_wrapper_GeoIP_database_edition (GeoIP* gi);
extern char*         libipv6calc_db_wrapper_GeoIP_database_info (GeoIP* gi);
extern void          libipv6calc_db_wrapper_GeoIP_delete(GeoIP* gi);
extern const char*   libipv6calc_db_wrapper_GeoIP_country_code_by_addr (GeoIP* gi, const char *addr);
extern const char*   libipv6calc_db_wrapper_GeoIP_country_name_by_addr (GeoIP* gi, const char *addr);
extern const char*   libipv6calc_db_wrapper_GeoIP_country_name_by_addr (GeoIP* gi, const char *addr);
extern GeoIPRecord*  libipv6calc_db_wrapper_GeoIP_record_by_addr (GeoIP* gi, const char *addr);
extern void          libipv6calc_db_wrapper_GeoIPRecord_delete (GeoIPRecord *gir);
extern const char *  libipv6calc_db_wrapper_GeoIP_lib_version(void);
extern const char *  libipv6calc_db_wrapper_GeoIP_country_code_by_addr_v6 (GeoIP* gi, const char *addr);
extern const char *  libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6 (GeoIP* gi, const char *addr);
extern const char *  libipv6calc_db_wrapper_GeoIP_country_code_by_ipnum_v6 (GeoIP* gi, geoipv6_t ipnum);
extern const char *  libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum_v6 (GeoIP* gi, geoipv6_t ipnum);
extern void          libipv6calc_db_wrapper_GeoIP_setup_custom_directory(char *dir);
extern const char ** libipv6calc_db_wrapper_GeoIPDBDescription;
extern char **       libipv6calc_db_wrapper_GeoIPDBFileName;
#endif
