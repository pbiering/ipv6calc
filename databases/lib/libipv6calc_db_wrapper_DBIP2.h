/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_DBIP2.h
 * Copyright  : 2019-2025 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_DBIP2.c
 */

#include "ipv6calctypes.h"
#include "libipaddr.h"

#ifndef _libipv6calc_db_wrapper_DBIP2_h

#define _libipv6calc_db_wrapper_DBIP2_h 1

// database types
#define DBIP2_DB_COUNTRY_LITE				1
#define DBIP2_DB_CITY_LITE				2
#define DBIP2_DB_ASN_LITE				3
#define DBIP2_DB_COUNTRY				4	// commercial
#define DBIP2_DB_LOCATION				5	// commercial
#define DBIP2_DB_ISP					6	// commercial
#define DBIP2_DB_LOCATION_ISP				7	// commercial
#define DBIP2_DB_MAX					DBIP2_DB_LOCATION_ISP

#define IPV6CALC_DB_DBIP2_INTERNAL_LITE		0x00000001
#define IPV6CALC_DB_DBIP2_INTERNAL_COMM		0x00000002

#endif


/* prototypes */

extern int         libipv6calc_db_wrapper_DBIP2_wrapper_init(void);
extern int         libipv6calc_db_wrapper_DBIP2_wrapper_cleanup(void);
extern void        libipv6calc_db_wrapper_DBIP2_wrapper_info(char* string, const size_t size);
extern void        libipv6calc_db_wrapper_DBIP2_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern char       *libipv6calc_db_wrapper_DBIP2_wrapper_db_info_used(void);
extern int         libipv6calc_db_wrapper_DBIP2_has_features(const uint32_t features);

#ifdef SUPPORT_DBIP2
#include "libipv6calc_db_wrapper_MMDB.h"

extern int         libipv6calc_db_wrapper_DBIP2_wrapper_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len);
extern uint32_t    libipv6calc_db_wrapper_DBIP2_wrapper_asn_by_addr(const ipv6calc_ipaddr *ipaddrp, char *as_orgname, const size_t as_orgname_length);
extern uint32_t    libipv6calc_db_wrapper_DBIP2_wrapper_GeonameID_by_addr(const ipv6calc_ipaddr *ipaddrp, int *source_ptr);
extern int         libipv6calc_db_wrapper_DBIP2_all_by_addr(const ipv6calc_ipaddr *ipaddrp, libipv6calc_db_wrapper_geolocation_record *recordp);

extern char dbip2_db_dir[PATH_MAX];
extern int  dbip2_db_comm_to_free_switch_min_delta_months;
extern int  dbip2_db_only_type;

extern int         libipv6calc_db_wrapper_DBIP2_db_avail(const unsigned int type);
extern int         libipv6calc_db_wrapper_DBIP2_open_type(const unsigned int type);

extern const char *libipv6calc_db_wrapper_DBIP2_dbdescription(const unsigned int type);
extern char       *libipv6calc_db_wrapper_DBIP2_database_info(const unsigned int type);
#endif
