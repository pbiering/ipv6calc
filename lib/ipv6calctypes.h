/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.h
 * Version    : $Id: ipv6calctypes.h,v 1.53 2015/08/23 09:53:27 ds6peter Exp $
 * Copyright  : 2002-2015 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv6calctypes.c
 */ 

#include "ipv6calc_inttypes.h"
#include "ipv6calccommands.h"
#include <getopt.h>


/* defines */
#ifndef _ipv6calctypes_h_

#define _ipv6calctypes_h_

/* text representations for defines */
typedef struct {
	const int number;
	const char *token;
} s_type;

typedef struct {
        const int number;
        const char *token;
        const char *tokensimple;
} s_type2;


/*
 * Format handling matrix for new address option style
 */

/* Format number definitions, each possible format has one number */
#define FORMAT_NUM_HEAD			0x200

#define FORMAT_NUM_auto			0
#define FORMAT_NUM_revnibbles_int	1
#define FORMAT_NUM_revnibbles_arpa	2
#define FORMAT_NUM_bitstring		3
#define FORMAT_NUM_ipv6addr		4
#define FORMAT_NUM_ipv4addr		5
#define FORMAT_NUM_mac			6
#define FORMAT_NUM_eui64		7
#define FORMAT_NUM_base85		8
#define FORMAT_NUM_ifinet6		9
#define FORMAT_NUM_iid			10
#define FORMAT_NUM_iid_token		11
#define FORMAT_NUM_addrtype 		12
#define FORMAT_NUM_ouitype	 	13
#define FORMAT_NUM_ipv6addrtype 	14
#define FORMAT_NUM_ipv6logconv 		15
#define FORMAT_NUM_any	 		16
#define FORMAT_NUM_prefix_mac		17
#define FORMAT_NUM_revipv4		18
#define FORMAT_NUM_ipv4hex		19
#define FORMAT_NUM_ipv4revhex		20
#define FORMAT_NUM_octal		21
#define FORMAT_NUM_hex			22
#define FORMAT_NUM_ipv6literal		23
#define FORMAT_NUM_ipv6rd		24
#define FORMAT_NUM_asn			29
#define FORMAT_NUM_auto_noresult	30
#define FORMAT_NUM_undefined		31

#define FORMAT_auto		(uint32_t) 0x00000u
#define FORMAT_revnibbles_int	(uint32_t) (1 << FORMAT_NUM_revnibbles_int)
#define FORMAT_revnibbles_arpa	(uint32_t) (1 << FORMAT_NUM_revnibbles_arpa)
#define FORMAT_bitstring	(uint32_t) (1 << FORMAT_NUM_bitstring)
#define FORMAT_ipv6addr		(uint32_t) (1 << FORMAT_NUM_ipv6addr)
#define FORMAT_ipv4addr		(uint32_t) (1 << FORMAT_NUM_ipv4addr)
#define FORMAT_mac		(uint32_t) (1 << FORMAT_NUM_mac)
#define FORMAT_macaddr		(uint32_t) (1 << FORMAT_NUM_mac)	// compatible syntax
#define FORMAT_eui64		(uint32_t) (1 << FORMAT_NUM_eui64)
#define FORMAT_base85		(uint32_t) (1 << FORMAT_NUM_base85)
#define FORMAT_ifinet6		(uint32_t) (1 << FORMAT_NUM_ifinet6)
#define FORMAT_iid		(uint32_t) (1 << FORMAT_NUM_iid)
#define FORMAT_iid_token	(uint32_t) (1 << FORMAT_NUM_iid_token)
#define FORMAT_addrtype 	(uint32_t) (1 << FORMAT_NUM_addrtype)
#define FORMAT_ouitype	 	(uint32_t) (1 << FORMAT_NUM_ouitype)
#define FORMAT_ipv6addrtype 	(uint32_t) (1 << FORMAT_NUM_ipv6addrtype)
#define FORMAT_ipv6logconv 	(uint32_t) (1 << FORMAT_NUM_ipv6logconv)
#define FORMAT_any	 	(uint32_t) (1 << FORMAT_NUM_any)
#define FORMAT_prefix_mac	(uint32_t) (1 << FORMAT_NUM_prefix_mac)
#define FORMAT_revipv4		(uint32_t) (1 << FORMAT_NUM_revipv4)
#define FORMAT_ipv4hex		(uint32_t) (1 << FORMAT_NUM_ipv4hex)
#define FORMAT_ipv4revhex	(uint32_t) (1 << FORMAT_NUM_ipv4revhex)
#define FORMAT_octal		(uint32_t) (1 << FORMAT_NUM_octal)
#define FORMAT_hex		(uint32_t) (1 << FORMAT_NUM_hex)
#define FORMAT_ipv6literal	(uint32_t) (1 << FORMAT_NUM_ipv6literal)
#define FORMAT_asn		(uint32_t) (1 << FORMAT_NUM_asn)
#define FORMAT_auto_noresult	(uint32_t) (1 << FORMAT_NUM_auto_noresult)
#define FORMAT_undefined	(uint32_t) (1 << FORMAT_NUM_undefined)

/* Primary label of format number, keeping also an explanation */
typedef struct {
	const uint32_t number;
	const char *token;
	const char *explanation;
	const char *aliases;
} s_format;

/*@unused@*/ static const s_format ipv6calc_formatstrings[] = {
	{ FORMAT_auto           , "auto"           , "automatic detection", "" },
	{ FORMAT_revnibbles_int , "revnibbles.int" , "dot separated nibbles reverse, ending with ip6.int.", "" },
	{ FORMAT_revnibbles_arpa, "revnibbles.arpa", "dot separated nibbles reverse, ending with ip6.arpa.", "" },
	{ FORMAT_bitstring      , "bitstring"      , "bitstring labes, ending with ip6.arpa.", "" },
	{ FORMAT_ipv6addr       , "ipv6addr"       , "IPv6 address", "ipv6" },
	{ FORMAT_ipv4addr       , "ipv4addr"       , "IPv4 address", "ipv4" },
	{ FORMAT_mac            , "mac"            , "MAC address (48 bits)", "eui48" },
	{ FORMAT_eui64          , "eui64"          , "EUI-64 identifier (64 bits)", "" },
	{ FORMAT_base85         , "base85"         , "Base-85 string", "" },
	{ FORMAT_ifinet6        , "ifinet6"        , "Like line in /proc/net/if_inet6", "" },
	{ FORMAT_iid            , "iid"            , "Interface identifier", "" },
	{ FORMAT_iid_token      , "iid+token"      , "Interface identifier and token", "" },
	{ FORMAT_addrtype       , "addrtype"       , "Address type", "" },
	{ FORMAT_ouitype        , "ouitype"        , "OUI (IEEE) type", "" },
	{ FORMAT_ipv6addrtype   , "ipv6addrtype"   , "IPv6 address type", "" },
	{ FORMAT_ipv6logconv    , "ipv6logconv"    , "ipv6logconv (currently not supported)", "" },
	{ FORMAT_any            , "any"            , "any type (currently not supported)", "" },
	{ FORMAT_revipv4	, "revipv4"        , "reverse IPv4, ending with in-addr.arpa", "" },
	{ FORMAT_ipv4hex	, "ipv4hex" 	   , "IPv4 in hexdecimal format", "" },
	{ FORMAT_ipv4revhex	, "ipv4revhex"     , "IPv4 in byte-reversed hexdecimal format", "" },
	{ FORMAT_octal		, "octal"	   , "IP address in escaped octal format", "" },
	{ FORMAT_hex		, "hex"	 	   , "IP address in hexadecimal format", "" },
	{ FORMAT_ipv6literal	, "ipv6literal"	   , "IPv6 address in literal", "" },
	{ FORMAT_prefix_mac     , "prefix+mac"     , "IPv6 prefix and a MAC address", "" },
	{ FORMAT_asn            , "asn"            , "Autonomous System Number", "" },
};

/* Format conversion matrix */
/*@unused@*/ static const uint32_t ipv6calc_formatmatrix[][2] = {
	{ FORMAT_auto           , 0x5ff },
	{ FORMAT_revnibbles_int , 0x5ff },
	{ FORMAT_revnibbles_arpa, 0x5ff },
	{ FORMAT_bitstring      , 0x5ff },
	{ FORMAT_ipv6addr       , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 | FORMAT_octal | FORMAT_ipv6literal | FORMAT_hex },
	{ FORMAT_ipv6literal    , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 | FORMAT_octal | FORMAT_ipv6literal },
	{ FORMAT_ipv4addr       , FORMAT_ipv4addr | FORMAT_ipv6addr | FORMAT_revipv4 | FORMAT_ipv4hex | FORMAT_hex },
	{ FORMAT_ipv4hex        , FORMAT_ipv4addr | FORMAT_ipv6addr | FORMAT_revipv4 | FORMAT_ipv4hex | FORMAT_hex },
	{ FORMAT_ipv4revhex     , FORMAT_ipv4addr | FORMAT_ipv6addr | FORMAT_revipv4 | FORMAT_ipv4hex },
	{ FORMAT_mac            , FORMAT_eui64 },
	{ FORMAT_eui64          , FORMAT_eui64 },
	{ FORMAT_base85         , FORMAT_base85 | FORMAT_ipv6addr | FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ifinet6 },
	{ FORMAT_ifinet6        , FORMAT_base85 | FORMAT_ipv6addr | FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ifinet6 },
	{ FORMAT_iid_token      , FORMAT_iid_token },
	{ FORMAT_ipv6logconv    , FORMAT_ipv6addrtype | FORMAT_addrtype | FORMAT_ouitype | FORMAT_any },
	{ FORMAT_prefix_mac     , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 },
	{ FORMAT_asn            , FORMAT_auto_noresult },
};


/* Format options */
#define FORMATOPTION_NUM_HEAD			0x300	// offset

#define FORMATOPTION_NUM_printlowercase		1	// 00000002
#define FORMATOPTION_NUM_printuppercase		2	// 00000004
#define FORMATOPTION_NUM_printprefix		3	// 00000008
#define FORMATOPTION_NUM_printsuffix		4	// 00000010
#define FORMATOPTION_NUM_maskprefix		5	// 00000020
#define FORMATOPTION_NUM_masksuffix		6	// 00000040
#define FORMATOPTION_NUM_printstart		7	// 00000080
#define FORMATOPTION_NUM_printend		8	// 00000100
#define FORMATOPTION_NUM_printcompressed	9	// 00000200
#define FORMATOPTION_NUM_printuncompressed	10	// 00000400
#define FORMATOPTION_NUM_printfulluncompressed	11	// 00000800
#define FORMATOPTION_NUM_machinereadable	12	// 00001000
#define FORMATOPTION_NUM_quiet			13	// 00002000
#define FORMATOPTION_NUM_printmirrored		14	// 00004000
#define FORMATOPTION_NUM_forceprefix		15	// 00008000
#define FORMATOPTION_NUM_literal		16	// 00010000
#define FORMATOPTION_NUM_print_iid_var		17	// 00020000
#define FORMATOPTION_NUM_no_prefixlength	18	// 00040000
#define FORMATOPTION_NUM_mr_quote_always	19	// 00080000
#define FORMATOPTION_NUM_mr_quote_never		20	// 00100000
#define FORMATOPTION_NUM_mr_value_only		21	// 00200000
#define FORMATOPTION_NUM_mr_select_token	22	// 00400000
#define FORMATOPTION_NUM_mr_select_token_pa	23	// 00800000
#define FORMATOPTION_NUM_mr_match_token		24	// 01000000
#define FORMATOPTION_NUM_mr_quote_default	25	// 02000000 flag for controlling old quote behavior < 0.99.0

#define FORMATOPTION_NUM_printembedded		31	// special

#define FORMATOPTION_printlowercase		(uint32_t) (1 << FORMATOPTION_NUM_printlowercase)
#define FORMATOPTION_printuppercase		(uint32_t) (1 << FORMATOPTION_NUM_printuppercase)
#define FORMATOPTION_printprefix		(uint32_t) (1 << FORMATOPTION_NUM_printprefix)
#define FORMATOPTION_printsuffix		(uint32_t) (1 << FORMATOPTION_NUM_printsuffix)
#define FORMATOPTION_maskprefix			(uint32_t) (1 << FORMATOPTION_NUM_maskprefix)
#define FORMATOPTION_masksuffix			(uint32_t) (1 << FORMATOPTION_NUM_masksuffix)
#define FORMATOPTION_printstart			(uint32_t) (1 << FORMATOPTION_NUM_printstart)
#define FORMATOPTION_printend			(uint32_t) (1 << FORMATOPTION_NUM_printend)
#define FORMATOPTION_printcompressed		(uint32_t) (1 << FORMATOPTION_NUM_printcompressed)
#define FORMATOPTION_printuncompressed		(uint32_t) (1 << FORMATOPTION_NUM_printuncompressed)
#define FORMATOPTION_printfulluncompressed	(uint32_t) (1 << FORMATOPTION_NUM_printfulluncompressed)
#define FORMATOPTION_machinereadable		(uint32_t) (1 << FORMATOPTION_NUM_machinereadable)
#define FORMATOPTION_quiet			(uint32_t) (1 << FORMATOPTION_NUM_quiet)
#define FORMATOPTION_printmirrored		(uint32_t) (1 << FORMATOPTION_NUM_printmirrored)
#define FORMATOPTION_forceprefix		(uint32_t) (1 << FORMATOPTION_NUM_forceprefix)
#define FORMATOPTION_literal			(uint32_t) (1 << FORMATOPTION_NUM_literal)
#define FORMATOPTION_print_iid_var		(uint32_t) (1 << FORMATOPTION_NUM_print_iid_var)
#define FORMATOPTION_no_prefixlength		(uint32_t) (1 << FORMATOPTION_NUM_no_prefixlength)
#define FORMATOPTION_mr_quote_always		(uint32_t) (1 << FORMATOPTION_NUM_mr_quote_always)
#define FORMATOPTION_mr_quote_never		(uint32_t) (1 << FORMATOPTION_NUM_mr_quote_never)
#define FORMATOPTION_mr_value_only		(uint32_t) (1 << FORMATOPTION_NUM_mr_value_only)
#define FORMATOPTION_mr_select_token		(uint32_t) (1 << FORMATOPTION_NUM_mr_select_token)
#define FORMATOPTION_mr_select_token_pa		(uint32_t) (1 << FORMATOPTION_NUM_mr_select_token_pa)
#define FORMATOPTION_mr_match_token		(uint32_t) (1 << FORMATOPTION_NUM_mr_match_token)
#define FORMATOPTION_mr_quote_default		(uint32_t) (1 << FORMATOPTION_NUM_mr_quote_default) // flag for controlling old quote behavior < 0.99.0

#define FORMATOPTION_printembedded		(uint32_t) (1 << FORMATOPTION_NUM_printembedded)

typedef struct {
	const uint32_t number;
	const char *token;
	const char *explanation;
} s_formatoption;

/*@unused@*/ static const s_formatoption ipv6calc_formatoptionstrings[] = {
	{ FORMATOPTION_printlowercase   , "--lowercase|-l"     , "Print output in lower case" },
	{ FORMATOPTION_printuppercase   , "--uppercase|-u"     , "Print output in upper case" },
	{ FORMATOPTION_printprefix      , "--printprefix"      , "Print only prefix" },
	{ FORMATOPTION_printsuffix      , "--printsuffix"      , "Print only suffix" },
	{ FORMATOPTION_maskprefix       , "--maskprefix"       , "Mask prefix bits (suffix set to 0)" },
	{ FORMATOPTION_masksuffix       , "--masksuffix"       , "Mask suffix bits (prefix set to 0)" },
	{ FORMATOPTION_printstart       , "--printstart <num>" , "Printing starts at bit <num>" },
	{ FORMATOPTION_printend         , "--printend <num>"   , "Printing ends at bit <num>" },
	{ FORMATOPTION_printcompressed  , "--printcompressed|-C"  , "Print in compressed format" },
	{ FORMATOPTION_printuncompressed  , "--printuncompressed|-U"  , "Print in uncompressed format" },
	{ FORMATOPTION_printfulluncompressed  , "--printfulluncompressed|-F"  , "Print in full uncompressed format" },
	{ FORMATOPTION_machinereadable  , "--machinereadable|-m" , "Print output machine readable" },
	{ FORMATOPTION_quiet            , "--quiet|-q"         , "Be more quiet" },
	{ FORMATOPTION_printmirrored	, "--printmirrored"    , "Print output mirrored" },
	{ FORMATOPTION_forceprefix	, "--forceprefix <num>", "Force prefix to <num>" },
	{ FORMATOPTION_print_iid_var	, "--print-iid-var"	, "IPv6 address IID variance"},
	{ FORMATOPTION_no_prefixlength	, "--no-prefixlength"	, "Don't print prefix length if given on input"},
};

/* Possible format option map */
/*@unused@*/ static const uint32_t ipv6calc_outputformatoptionmap[][2]  = {
	{ FORMAT_revnibbles_int , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend | FORMATOPTION_printmirrored },
	{ FORMAT_revnibbles_arpa, FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend | FORMATOPTION_printmirrored },
	{ FORMAT_revipv4, FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printmirrored },
	{ FORMAT_bitstring      , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend },
	{ FORMAT_ipv6addr       , FORMATOPTION_machinereadable | FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printcompressed | FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed | FORMATOPTION_forceprefix | FORMATOPTION_print_iid_var | FORMATOPTION_no_prefixlength},
	{ FORMAT_mac            , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_eui64          , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_base85         , 0 },
	{ FORMAT_ifinet6        , 0 },
	{ FORMAT_ipv4addr       , FORMATOPTION_machinereadable | FORMATOPTION_no_prefixlength },
	{ FORMAT_iid_token      , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_octal          , FORMATOPTION_printfulluncompressed },
	{ FORMAT_ipv6literal    , FORMATOPTION_machinereadable | FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printcompressed | FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed },
	{ FORMAT_hex            , FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_forceprefix },
	{ FORMAT_prefix_mac     , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase }
};


/* Actions */
#define ACTION_NUM_auto			0
#define ACTION_NUM_mac_to_eui64		1
#define ACTION_NUM_ipv4_to_6to4addr	2
#define ACTION_NUM_iid_token_to_privacy	3
#define ACTION_NUM_prefix_mac_to_ipv6	4
#define ACTION_NUM_anonymize		5
#define ACTION_NUM_6rd_local_prefix	6
#define ACTION_NUM_6rd_extract_ipv4	7
#define ACTION_NUM_ipv4_to_nat64	8
#define ACTION_NUM_filter		15
#define ACTION_NUM_undefined		31

#define ACTION_auto			(uint32_t) 0x0
#define ACTION_mac_to_eui64		(uint32_t) (1 << ACTION_NUM_mac_to_eui64)
#define ACTION_ipv4_to_6to4addr		(uint32_t) (1 << ACTION_NUM_ipv4_to_6to4addr)
#define ACTION_iid_token_to_privacy	(uint32_t) (1 << ACTION_NUM_iid_token_to_privacy)
#define ACTION_prefix_mac_to_ipv6	(uint32_t) (1 << ACTION_NUM_prefix_mac_to_ipv6)
#define ACTION_anonymize		(uint32_t) (1 << ACTION_NUM_anonymize)
#define ACTION_6rd_local_prefix		(uint32_t) (1 << ACTION_NUM_6rd_local_prefix)
#define ACTION_6rd_extract_ipv4		(uint32_t) (1 << ACTION_NUM_6rd_extract_ipv4)
#define ACTION_ipv4_to_nat64		(uint32_t) (1 << ACTION_NUM_ipv4_to_nat64)
#define ACTION_filter			(uint32_t) (1 << ACTION_NUM_filter)
#define ACTION_undefined		(uint32_t) (1 << ACTION_NUM_undefined)

#define ANON_METHOD_ANONYMIZE		1
#define ANON_METHOD_ZEROIZE		2
#define ANON_METHOD_KEEPTYPEASNCC	3

typedef struct {
	const uint32_t number;
	const char *token;
	const char *explanation;
	const char *aliases;
} s_action;

/*@unused@*/ static const s_action ipv6calc_actionstrings[] = {
	{ ACTION_auto                 , "auto"            , "Automatic selection of action (default)", "" },
	{ ACTION_mac_to_eui64         , "geneui64"        , "Converts a MAC address to an EUI-64 address", "" },
	{ ACTION_ipv4_to_6to4addr     , "conv6to4"        , "Converts IPv4 address <-> 6to4 IPv6 address (prefix)", "" },
	{ ACTION_ipv4_to_nat64        , "convnat64"       , "Converts IPv4 address <-> NAT64 IPv6 address", "" },
	{ ACTION_iid_token_to_privacy , "genprivacyiid"   , "Generates a privacy interface ID out of a given one (arg1) and a token (arg2)", "" },
	{ ACTION_prefix_mac_to_ipv6   , "prefixmac2ipv6"  , "Generates an IPv6 address out of a prefix and a MAC address", "" },
	{ ACTION_anonymize            , "anonymize"       , "Anonymize IPv4/IPv6 address without loosing much information", "" },
	{ ACTION_6rd_local_prefix     , "6rd_local_prefix", "Calculate the 6rd prefix from given IPv6 prefix (& relay prefix) and IPv4", "" },
	{ ACTION_6rd_extract_ipv4     , "6rd_extract_ipv4", "Extract from 6rd address the include IPv4 address", "" },
	{ ACTION_filter	              , "filter"          , "Filter addresses related to filter options", "" },
};

/* Possible action option map (required) */
/*@unused@*/ static const uint32_t ipv6calc_actionoptionmap[][3]  = {
	{ ACTION_auto			, 0 , 0},
	{ ACTION_mac_to_eui64		, 0 , 0},
	{ ACTION_ipv4_to_6to4addr	, 0 , 0},
	{ ACTION_ipv4_to_nat64		, 0 , 0},
	{ ACTION_iid_token_to_privacy	, 0 , 0},
	{ ACTION_prefix_mac_to_ipv6	, 0 , 0},
	{ ACTION_anonymize		, 0 , 0},
	{ ACTION_6rd_local_prefix	, CMD_6rd_prefix, 0},
	{ ACTION_6rd_local_prefix	, CMD_6rd_relay_prefix, 1 },
	{ ACTION_6rd_extract_ipv4	, CMD_6rd_prefixlength, 0},
};

/* anonymization set */
typedef struct {
	char name[32];
	char name_short[3];
	int mask_ipv4;
	int mask_ipv6;
	int mask_eui64;
	int mask_mac;
	int mask_autoadjust;
	int method;	// 1=anonymize, 2=zeroize, 3=keep type asn countrycode
} s_ipv6calc_anon_set;

/* anonymization methods */
typedef struct {
	const char name[32];
	const int method;
	const char description[128];
} s_ipv6calc_anon_methods;


/**** filter structures ****/
#define IPV6CALC_FILTER_DB_CC_MAX	16
#define IPV6CALC_FILTER_DB_ASN_MAX	16
#define IPV6CALC_FILTER_DB_REGISTRY_MAX	8
#define IPV6CALC_FILTER_IPV4ADDR	16
#define IPV6CALC_FILTER_IPV6ADDR	16

/* DB CC (CountryCode) filter structure */
typedef struct {
	int active;
	int cc_must_have_max;
	int cc_may_not_have_max;
	uint16_t cc_must_have[IPV6CALC_FILTER_DB_CC_MAX];
	uint16_t cc_may_not_have[IPV6CALC_FILTER_DB_CC_MAX];
} s_ipv6calc_filter_db_cc;


/* DB ASN (Autonomous System Number) filter structure */
typedef struct {
	int active;
	int asn_must_have_max;
	int asn_may_not_have_max;
	uint32_t asn_must_have[IPV6CALC_FILTER_DB_ASN_MAX];
	uint32_t asn_may_not_have[IPV6CALC_FILTER_DB_ASN_MAX];
} s_ipv6calc_filter_db_asn;


/* DB Registry filter structure */
typedef struct {
	int active;
	int registry_must_have_max;
	int registry_may_not_have_max;
	uint32_t registry_must_have[IPV6CALC_FILTER_DB_REGISTRY_MAX];
	uint32_t registry_may_not_have[IPV6CALC_FILTER_DB_REGISTRY_MAX];
} s_ipv6calc_filter_db_registry;

/* typeinfo filter structure */
typedef struct {
	int active;
	uint32_t typeinfo_must_have;
	uint32_t typeinfo_may_not_have;
} s_ipv6calc_filter_typeinfo;


#endif // _ipv6calctypes_h_


/* prototypes */
extern uint32_t ipv6calctypes_checktype(const char *string);
extern uint32_t ipv6calctypes_checkaction(const char *string);

extern const char *ipv6calctypes_format_string_by_type(const uint32_t format_type);
