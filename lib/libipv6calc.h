/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id: libipv6calc.h,v 1.22 2014/02/04 07:32:29 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#include "ipv6calc_inttypes.h"
#include "ipv6calctypes.h"
#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"


/* typedefs */

#ifndef _libipv6calc_h

#define _libipv6calc_h 1

/* master filter structure */
typedef struct {
	s_ipv6calc_filter_ipv4addr filter_ipv4addr;
	s_ipv6calc_filter_ipv6addr filter_ipv6addr;
	s_ipv6calc_filter_macaddr  filter_macaddr;
} s_ipv6calc_filter_master;


/*@unused@*/ static const s_ipv6calc_anon_set ipv6calc_anon_set_list[] = {
	// name                   short  ip4 ip6 iid mac  keep-oui method
	{ "anonymize-standard"  , "as"  , 24, 56, 40, 24, 1       , ANON_METHOD_ANONYMIZE     },
	{ "anonymize-careful"   , "ac"  , 20, 48, 24, 24, 1       , ANON_METHOD_ANONYMIZE     },
	{ "anonymize-paranoid"  , "ap"  , 16, 40,  0, 24, 0       , ANON_METHOD_ANONYMIZE     },
	{ "zeroize-standard"    , "zs"  , 24, 56, 40, 24, 1       , ANON_METHOD_ZEROIZE       },
	{ "zeroize-careful"     , "zc"  , 20, 48, 24, 24, 1       , ANON_METHOD_ZEROIZE       },
	{ "zeroize-paranoid"    , "zp"  , 16, 40,  0, 24, 0       , ANON_METHOD_ZEROIZE       },
	{ "keep-type-asn-cc"    , "kp"  , 24, 56, 40, 24, 1       , ANON_METHOD_KEEPTYPEASNCC }
};

/*@unused@*/ static const s_ipv6calc_anon_methods ipv6calc_anon_methods[] = {
	{ "anonymize"        , 1, "reliable anonymization, keep as much type information as possible" },
	{ "zeroize"          , 2, "simple zeroizing according to given masks, probably loose type information" },
	{ "keep-type-asn-cc" , 3, "special reliable anonymization, keep type & Autonomous System Number and CountryCode" }
};

/* Registries */
#define REGISTRY_APNIC        0x02
#define REGISTRY_RIPE         0x03
#define REGISTRY_RIPENCC      0x03
#define REGISTRY_LACNIC       0x04
#define REGISTRY_AFRINIC      0x05
#define REGISTRY_ARIN         0x06

#define REGISTRY_6BONE        0x08
#define REGISTRY_IANA         0x09
#define REGISTRY_6TO4         0x0c
#define REGISTRY_RESERVED     0x0e
#define REGISTRY_UNKNOWN      0x0f

// new array (try to remove old arrays)
/*@unused@*/ static const s_type ipv6calc_registries[] = {
        { REGISTRY_6BONE      , "6BONE"     },
        { REGISTRY_IANA       , "IANA"      },
        { REGISTRY_APNIC      , "APNIC"     },
        { REGISTRY_ARIN       , "ARIN"      },
        { REGISTRY_RIPE       , "RIPENCC"   },
        { REGISTRY_LACNIC     , "LACNIC"    },
        { REGISTRY_AFRINIC    , "AFRINIC"   },
        { REGISTRY_RESERVED   , "reserved"  },
        { REGISTRY_UNKNOWN    , "unknown"   }
};


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
#define MAXENTRIES_ARRAY(a)	(sizeof(a) / sizeof(a[0]))

/* non-quiet print with args */
#define NONQUIETPRINT_WA(t, ...)        if (ipv6calc_quiet == 0) { fprintf(stderr, t "\n", __VA_ARGS__); };

/* non-quiet print no args */
#define NONQUIETPRINT_NA(t)             if (ipv6calc_quiet == 0) { fprintf(stderr, t "\n"); };

#endif


/* prototypes */
extern int ipv6calc_quiet;

extern void string_to_upcase(char *string);
extern void string_to_lowcase(char *string);

extern void string_to_reverse(char *string);
extern void string_to_reverse_dotted(char *string);

extern uint32_t libipv6calc_autodetectinput(const char *string);

extern int  libipv6calc_filter_parse(const char *expression, s_ipv6calc_filter_master *filter_master);
extern void libipv6calc_filter_clear(s_ipv6calc_filter_master *filter_master);

extern int   libipv6calc_anon_set_by_name(s_ipv6calc_anon_set *ipv6calc_anon_set, const char* name);
extern void  libipv6calc_anon_infostring(char* string, const int stringlength, const s_ipv6calc_anon_set *ipv6calc_anon_set);
extern const char *libipv6calc_anon_method_name(const s_ipv6calc_anon_set *ipv6calc_anon_set);
extern int   libipv6calc_anon_supported(const s_ipv6calc_anon_set *ipv6calc_anon_set);

extern const char *libipv6calc_registry_string_by_num(const int registry);

