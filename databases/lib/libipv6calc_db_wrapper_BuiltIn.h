/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_BuiltIn.h
 * Version    : $Id: libipv6calc_db_wrapper_BuiltIn.h,v 1.7 2014/05/11 09:49:38 ds6peter Exp $
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_BuiltIn.c
 */

#include "ipv6calctypes.h"
#include "libmac.h"
#include "libipv4addr.h"
#include "libipv6addr.h"

#ifndef _libipv6calc_db_wrapper_BuiltIn_h

#define _libipv6calc_db_wrapper_BuiltIn_h 1

#define SUPPORT_BUILTIN  // default

// features
extern uint32_t wrapper_features_BuiltIn;

/* ASN->Registry assignment structure */
typedef struct {
	const uint32_t asn_start;
	const uint32_t asn_stop;
	const uint8_t  registry;
} s_asn_assignment;


/* CC->Registry assignment structure */
typedef struct {
	const char    cc[3];
	const uint8_t registry;
} s_cc_reg_assignment;

typedef struct {
	const uint8_t  registry;
} s_cc_index_reg_assignment;


/* IPv4 address assignment structure */
typedef struct {
	const uint32_t first;
	const uint32_t last;
	const uint8_t  registry;
} s_ipv4addr_assignment;


/* IPv6 address assignment structure */
typedef struct {
	const uint32_t ipv6addr_00_31;
	const uint32_t ipv6addr_32_63;
	const uint32_t ipv6mask_00_31;
	const uint32_t ipv6mask_32_63;
	const uint8_t  prefixlength;	/* prefix length (0-128) 8 bit*/
	const uint8_t  registry;
} s_ipv6addr_assignment;

#endif


extern int  libipv6calc_db_wrapper_BuiltIn_wrapper_init(void);
extern int  libipv6calc_db_wrapper_BuiltIn_wrapper_cleanup(void);
extern void libipv6calc_db_wrapper_BuiltIn_wrapper_info(char *string, const size_t size);
extern void libipv6calc_db_wrapper_BuiltIn_wrapper_print_db_info(const int level_verbose, const char *prefix_string);

extern int libipv6calc_db_wrapper_BuiltIn_registry_num_by_as_num32(const uint32_t as_num32);
extern int libipv6calc_db_wrapper_BuiltIn_registry_num_by_cc_index(const uint16_t cc_index);

// IEEE
extern int libipv6calc_db_wrapper_BuiltIn_ieee_vendor_string_by_macaddr(char *resultstring, const size_t resultstring_length, const ipv6calc_macaddr *macaddrp);
extern int libipv6calc_db_wrapper_BuiltIn_ieee_vendor_string_short_by_macaddr(char *resultstring, const size_t resultstring_length, const ipv6calc_macaddr *macaddrp); 

// IPv4 Registry
extern int libipv6calc_db_wrapper_BuiltIn_registry_string_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length);
extern int libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp);

// IPv6 Registry
extern int libipv6calc_db_wrapper_BuiltIn_registry_string_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length);
extern int libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp);
