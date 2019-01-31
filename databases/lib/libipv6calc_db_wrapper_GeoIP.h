/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_GeoIP.h
 * Version    : $Id$
 * Copyright  : 2013-2019 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_GeoIP.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_GeoIP_h

#define _libipv6calc_db_wrapper_GeoIP_h 1

#ifdef SUPPORT_GEOIP
#include "GeoIP.h"
#include "GeoIPCity.h"
#endif

#define GEOIP_LIB_FEATURE_IPv6			0x0001
#define GEOIP_LIB_FEATURE_LIB_VERSION		0x0002
#define GEOIP_LIB_FEATURE_IPV6_CC_BY_IPNUM	0x0100
#define GEOIP_LIB_FEATURE_IPV6_CN_BY_IPNUM	0x0200
#define GEOIP_LIB_FEATURE_IPV6_CC_BY_ADDR	0x0400
#define GEOIP_LIB_FEATURE_IPV6_CN_BY_ADDR	0x0800

// features
extern uint32_t wrapper_features_GeoIP;
extern uint32_t lib_features_GeoIP;

#endif

extern int          libipv6calc_db_wrapper_GeoIP_wrapper_init(void);
extern int          libipv6calc_db_wrapper_GeoIP_wrapper_cleanup(void);
extern const char  *libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(const char *addr, const int proto);
extern const char  *libipv6calc_db_wrapper_GeoIP_wrapper_country_name_by_addr(const char *addr, const int proto);
extern char        *libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(const char *addr, const int proto);
extern uint32_t     libipv6calc_db_wrapper_GeoIP_wrapper_asn_by_addr(const ipv6calc_ipaddr *ipaddrp);
extern void         libipv6calc_db_wrapper_GeoIP_wrapper_info(char* string, const size_t size);
extern void         libipv6calc_db_wrapper_GeoIP_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern char        *libipv6calc_db_wrapper_GeoIP_wrapper_db_info_used(void);

extern int          libipv6calc_db_wrapper_GeoIP_has_features(uint32_t features);

extern char geoip_lib_file[NI_MAXHOST];
extern char geoip_db_dir[NI_MAXHOST];


#ifdef SUPPORT_GEOIP
extern GeoIP	    *libipv6calc_db_wrapper_GeoIP_open(const char * filename, int flags);
extern GeoIP	    *libipv6calc_db_wrapper_GeoIP_open_type(int type, int flags);
extern int           libipv6calc_db_wrapper_GeoIP_db_avail(int type);
extern int           libipv6calc_db_wrapper_GeoIP_cleanup(void);
extern unsigned char libipv6calc_db_wrapper_GeoIP_database_edition(GeoIP *gi);
extern char         *libipv6calc_db_wrapper_GeoIP_database_info(GeoIP *gi);
extern void          libipv6calc_db_wrapper_GeoIP_delete(GeoIP *gi);
extern const char   *libipv6calc_db_wrapper_GeoIP_country_code_by_addr(GeoIP *gi, const char *addr);
extern const char   *libipv6calc_db_wrapper_GeoIP_country_name_by_addr(GeoIP *gi, const char *addr);
extern const char   *libipv6calc_db_wrapper_GeoIP_country_name_by_addr(GeoIP *gi, const char *addr);
extern GeoIPRecord  *libipv6calc_db_wrapper_GeoIP_record_by_addr(GeoIP *gi, const char *addr);
extern void          libipv6calc_db_wrapper_GeoIPRecord_delete(GeoIPRecord *gir);
extern const char   *libipv6calc_db_wrapper_GeoIP_lib_version(void);
extern const char   *libipv6calc_db_wrapper_GeoIP_country_code_by_addr_v6(GeoIP *gi, const char *addr);
extern const char   *libipv6calc_db_wrapper_GeoIP_country_name_by_addr_v6(GeoIP *gi, const char *addr);

#ifdef SUPPORT_GEOIP_V6
#if HAVE_DECL_GEOIP_COUNTRY_CODE_BY_IPNUM_V6 == 1
extern const char   *libipv6calc_db_wrapper_GeoIP_country_code_by_ipnum_v6(GeoIP *gi, geoipv6_t ipnum);
extern const char   *libipv6calc_db_wrapper_GeoIP_country_name_by_ipnum_v6(GeoIP *gi, geoipv6_t ipnum);
#endif
#endif

extern void          libipv6calc_db_wrapper_GeoIP_setup_custom_directory(char *dir);
extern const char  **libipv6calc_db_wrapper_GeoIPDBDescription;
extern char       ***libipv6calc_db_wrapper_GeoIPDBFileName_ptr;

extern GeoIPRecord *libipv6calc_db_wrapper_GeoIP_wrapper_record_city_by_addr(const char *addr, const int proto);
#endif
