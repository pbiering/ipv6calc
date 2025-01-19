/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_GeoIP2.h
 * Copyright  : 2019-2024 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_GeoIP2.c
 */

#include "ipv6calctypes.h"
#include "libipaddr.h"

#ifndef _libipv6calc_db_wrapper_GeoIP2_h

#define _libipv6calc_db_wrapper_GeoIP2_h 1

// database types
#define GeoIP2_DB_COUNTRY_LITE				1
#define GeoIP2_DB_CITY_LITE				2
#define GeoIP2_DB_ASN					3
#define GeoIP2_DB_COUNTRY				4	// commercial
#define GeoIP2_DB_CITY					5	// commercial
#define GeoIP2_DB_ISP					6	// commercial
#define GeoIP2_DB_MAX					GeoIP2_DB_ISP

#define IPV6CALC_DB_GEOIP2_INTERNAL_FREE		0x00000001
#define IPV6CALC_DB_GEOIP2_INTERNAL_COMM		0x00000002

#endif


/* prototypes */

extern int         libipv6calc_db_wrapper_GeoIP2_wrapper_init(void);
extern int         libipv6calc_db_wrapper_GeoIP2_wrapper_cleanup(void);
extern void        libipv6calc_db_wrapper_GeoIP2_wrapper_info(char* string, const size_t size);
extern void        libipv6calc_db_wrapper_GeoIP2_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern char       *libipv6calc_db_wrapper_GeoIP2_wrapper_db_info_used(void);
extern int         libipv6calc_db_wrapper_GeoIP2_has_features(const uint32_t features);

#ifdef SUPPORT_GEOIP2
#include "libipv6calc_db_wrapper_MMDB.h"

extern int         libipv6calc_db_wrapper_GeoIP2_wrapper_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len);
extern uint32_t    libipv6calc_db_wrapper_GeoIP2_wrapper_asn_by_addr(const ipv6calc_ipaddr *ipaddrp, char *as_orgname, const size_t as_orgname_length);
extern uint32_t    libipv6calc_db_wrapper_GeoIP2_wrapper_GeonameID_by_addr(const ipv6calc_ipaddr *ipaddrp, int *source_ptr);
extern int         libipv6calc_db_wrapper_GeoIP2_all_by_addr(const ipv6calc_ipaddr *ipaddrp, libipv6calc_db_wrapper_geolocation_record *recordp);

extern char geoip2_db_dir[PATH_MAX];
extern int  geoip2_db_comm_to_free_switch_min_delta_months;
extern int  geoip2_db_only_type;

extern int         libipv6calc_db_wrapper_GeoIP2_db_avail(const unsigned int type);
extern int         libipv6calc_db_wrapper_GeoIP2_open_type(const unsigned int type_flag);

extern const char *libipv6calc_db_wrapper_GeoIP2_dbdescription(const unsigned int type);
extern char       *libipv6calc_db_wrapper_GeoIP2_database_info(const unsigned int type);
#endif
