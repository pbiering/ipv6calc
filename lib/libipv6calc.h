/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id$
 * Copyright  : 2001-2021 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#ifndef _libipv6calc_h

#define _libipv6calc_h 1


#include "ipv6calctypes.h"


/* typedefs */

/* Registries (main registries must below 8 for anonymization mapping) */
#define REGISTRY_APNIC        0x02
#define REGISTRY_RIPENCC      0x03
#define REGISTRY_LACNIC       0x04
#define REGISTRY_AFRINIC      0x05
#define REGISTRY_ARIN         0x06

#define REGISTRY_6BONE        0x08
#define REGISTRY_IANA         0x09
#define REGISTRY_6TO4         0x0c
#define REGISTRY_RESERVED     0x0e
#define REGISTRY_UNKNOWN      0x0f

/* some generic defines */
#define IPV6CALC_PROTO_IPV4		4
#define IPV6CALC_PROTO_IPV6		6

/* some generic macros */

// pack value using xor/mask/shift
#define PACK_XMS(v, x, m, s) 	((((v) ^ (x)) & (m)) << (s))

// unpack value using xor/mask/shift
#define UNPACK_XMS(p, x, m, s)	((((p) >> (s)) ^ (x)) & (m))

// return 16-bit MSB from 32-bit
#define U32_MSB16(v)	((v >> 16) & 0xffff)

// return 16-bit LSB from 32-bit
#define U32_LSB16(v)	(v & 0xffff)

// max entries of a const array
#define MAXENTRIES_ARRAY(a)	(signed long) (sizeof(a) / sizeof(a[0]))

/* non-quiet print with args */
#define NONQUIETPRINT_WA(t, ...)        if (ipv6calc_quiet == 0) { fprintf(stderr, t "\n", __VA_ARGS__); };

/* non-quiet print no args */
#define NONQUIETPRINT_NA(t)             if (ipv6calc_quiet == 0) { fprintf(stderr, t "\n"); };


#define IPV6CALC_LIB_VERSION_CHECK_EXIT(version_numeric, version_string) \
	if (version_numeric != libipv6calc_lib_version_numeric()) { \
		ERRORPRINT_WA("Library version is not matching: has:%s required:%s", libipv6calc_lib_version_string(), version_string); \
		exit(1); \
	};

#endif // _libipv6calc_h


/* references */
extern const s_ipv6calc_anon_set ipv6calc_anon_set_list[];
extern const int                 ipv6calc_anon_set_list_entries;

extern const s_ipv6calc_anon_methods ipv6calc_anon_methods[];
extern const int                     ipv6calc_anon_methods_entries;

extern const s_type2 ipv6calc_registries[];
extern const int     ipv6calc_registries_entries;


/* prototypes */
extern int ipv6calc_quiet;
extern int ipv6calc_verbose;

extern void string_to_upcase(char *string);
extern void string_to_lowcase(char *string);

extern void string_to_reverse(char *string);
extern void string_to_reverse_dotted(char *string, const size_t string_length);

extern uint32_t libipv6calc_autodetectinput(const char *string);

extern int   libipv6calc_anon_set_by_name(s_ipv6calc_anon_set *ipv6calc_anon_set, const char* name);
extern void  libipv6calc_anon_infostring(char* string, const int stringlength, const s_ipv6calc_anon_set *ipv6calc_anon_set);
extern const char *libipv6calc_anon_method_name(const s_ipv6calc_anon_set *ipv6calc_anon_set);
extern int   libipv6calc_anon_supported(const s_ipv6calc_anon_set *ipv6calc_anon_set);

extern const char *libipv6calc_registry_string_by_num(const int registry);

extern int libipv6calc_bitcount_uint32_t(const uint32_t value);

extern       uint32_t libipv6calc_lib_version_numeric(void);
extern const char    *libipv6calc_lib_version_string(void);
extern       uint32_t libipv6calc_api_version_numeric(void);
extern const char    *libipv6calc_api_version_string(void);
