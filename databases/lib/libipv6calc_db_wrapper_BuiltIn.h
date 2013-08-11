/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_BuiltIn.h
 * Version    : $Id: libipv6calc_db_wrapper_BuiltIn.h,v 1.1 2013/08/11 16:42:11 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_BuiltIn.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_BuiltIn_h

#define _libipv6calc_db_wrapper_BuiltIn_h 1

#define SUPPORT_BUILTIN  // default


/* ASN assignment structure */
typedef struct {
	const uint32_t asn_start;
	const uint32_t asn_stop;
	const uint8_t  registry;
} s_asn_assignment;

extern int  libipv6calc_db_wrapper_BuiltIn_wrapper_init(void);
extern int  libipv6calc_db_wrapper_BuiltIn_wrapper_cleanup(void);
extern void libipv6calc_db_wrapper_BuiltIn_wrapper_info(char* string, const size_t size);
extern void libipv6calc_db_wrapper_BuiltIn_wrapper_print_db_info(const int level_verbose, const char *prefix_string);

extern int libipv6calc_db_wrapper_BuiltIn_registry_num_by_as_num32(const uint32_t as_num32);
#endif
