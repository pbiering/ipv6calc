/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_MMDB.h
 * Version    : $Id$
 * Copyright  : 2019-2019 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_MMDB.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_MMDB_h

#define _libipv6calc_db_wrapper_MMDB_h 1

#ifdef SUPPORT_MMDB
#include "maxminddb.h"
#endif

#endif // _libipv6calc_db_wrapper_MMDB_h

extern int          libipv6calc_db_wrapper_MMDB_wrapper_init(void);
extern void         libipv6calc_db_wrapper_MMDB_wrapper_cleanup(void);
extern void         libipv6calc_db_wrapper_MMDB_wrapper_info(char* string, const size_t size);

#ifdef SUPPORT_MMDB

#ifdef SUPPORT_MMDB_DYN
extern char mmdb_lib_file[NI_MAXHOST];
#endif

extern int          libipv6calc_db_wrapper_MMDB_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len, MMDB_s *const mmdb);
extern int          libipv6calc_db_wrapper_MMDB_all_by_addr(const ipv6calc_ipaddr *ipaddrp, libipv6calc_db_wrapper_geolocation_record *recordp, MMDB_s *const mmdb);
//MMDB_lookup_result_s libipv6calc_db_wrapper_MMDB_wrapper_lookup_addr (const ipv6calc_ipaddr *ipaddrp, int *const mmdb_error, MMDB_s *const mmdb);
extern uint32_t     libipv6calc_db_wrapper_MMDB_asn_by_addr(const ipv6calc_ipaddr *ipaddrp, MMDB_s *const mmdb);

extern int	    libipv6calc_db_wrapper_MMDB_open(const char *const filename, uint32_t flags, MMDB_s *const mmdb);
extern void         libipv6calc_db_wrapper_MMDB_close(MMDB_s *const mmdb);
extern const char  *libipv6calc_db_wrapper_MMDB_lib_version(void);
extern const char  *libipv6calc_db_wrapper_MMDB_strerror(int error_code);

extern int          libipv6calc_db_wrapper_MMDB_get_value(MMDB_entry_s *const start, MMDB_entry_data_s *const entry_data, const char *const *const path);
extern int          libipv6calc_db_wrapper_MMDB_get_entry_data_list(MMDB_entry_s *start, MMDB_entry_data_list_s **const entry_data_list);
extern void         libipv6calc_db_wrapper_MMDB_free_entry_data_list(MMDB_entry_data_list_s *const entry_data_list);
extern int          libipv6calc_db_wrapper_MMDB_dump_entry_data_list(FILE *const stream, MMDB_entry_data_list_s *const entry_data_list, int indent);

extern MMDB_lookup_result_s libipv6calc_db_wrapper_MMDB_lookup_sockaddr(MMDB_s *const mmdb, const struct sockaddr *const sockaddr, int *const mmdb_error);
#endif
