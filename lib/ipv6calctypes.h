/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.h
 * Version    : $Id$
 * Copyright  : 2002-2021 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GPLv2
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
#define FORMAT_NUM_ipv6to4		25

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
#define FORMAT_ipv6rd		(uint32_t) (1 << FORMAT_NUM_ipv6rd)
#define FORMAT_ipv6to4		(uint32_t) (1 << FORMAT_NUM_ipv6to4)

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
#define FORMATOPTION_NUM_mr_match_token_suffix	26	// 04000000

#define FORMATOPTION_NUM_print_octal		30	// octal with separator
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
#define FORMATOPTION_mr_match_token_suffix	(uint32_t) (1 << FORMATOPTION_NUM_mr_match_token_suffix)

#define FORMATOPTION_print_octal		(uint32_t) (1 << FORMATOPTION_NUM_print_octal)
#define FORMATOPTION_printembedded		(uint32_t) (1 << FORMATOPTION_NUM_printembedded)

typedef struct {
	const uint32_t number;
	const char *token;
	const char *explanation;
} s_formatoption;


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
#define ACTION_NUM_ipv6_to_mac		9
#define ACTION_NUM_ipv6_to_eui64	10
#define ACTION_NUM_filter		15
#define ACTION_NUM_test			16
#define ACTION_NUM_addr_to_countrycode	17
#define ACTION_NUM_db_dump		18
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
#define ACTION_ipv6_to_mac		(uint32_t) (1 << ACTION_NUM_ipv6_to_mac)
#define ACTION_ipv6_to_eui64		(uint32_t) (1 << ACTION_NUM_ipv6_to_eui64)
#define ACTION_filter			(uint32_t) (1 << ACTION_NUM_filter)
#define ACTION_test			(uint32_t) (1 << ACTION_NUM_test)
#define ACTION_addr_to_countrycode	(uint32_t) (1 << ACTION_NUM_addr_to_countrycode)
#define ACTION_db_dump			(uint32_t) (1 << ACTION_NUM_db_dump)
#define ACTION_undefined		(uint32_t) (1 << ACTION_NUM_undefined)

#define ANON_METHOD_ANONYMIZE		1
#define ANON_METHOD_ZEROIZE		2
#define ANON_METHOD_KEEPTYPEASNCC	3
#define ANON_METHOD_KEEPTYPEGEONAMEID	4

typedef struct {
	const uint32_t number;
	const char *token;
	const char *explanation;
	const char *aliases;
} s_action;


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


#define IPV6CALC_TEST_NONE	0
#define IPV6CALC_TEST_PREFIX	1
#define IPV6CALC_TEST_GT	2
#define IPV6CALC_TEST_GE	3
#define IPV6CALC_TEST_LT	4
#define IPV6CALC_TEST_LE	5

#define IPV6CALC_TEST_LIST_MIN	IPV6CALC_TEST_PREFIX
#define IPV6CALC_TEST_LIST_MAX	IPV6CALC_TEST_LE

#endif // _ipv6calctypes_h_


/* references */
extern const s_format ipv6calc_formatstrings[];
extern const int      ipv6calc_formatstrings_entries;

extern const s_formatoption ipv6calc_formatoptionstrings[];
extern const int            ipv6calc_formatoptionstrings_entries;

extern const s_action ipv6calc_actionstrings[];
extern const int      ipv6calc_actionstrings_entries;

extern const uint32_t ipv6calc_formatmatrix[][2];
extern const int      ipv6calc_formatmatrix_entries;

extern const uint32_t ipv6calc_outputformatoptionmap[][2];
extern const int      ipv6calc_outputformatoptionmap_entries;

extern const uint32_t ipv6calc_actionoptionmap[][3];
extern const int      ipv6calc_actionoptionmap_entries;


/* prototypes */
extern uint32_t ipv6calctypes_checktype(const char *string);
extern uint32_t ipv6calctypes_checkaction(const char *string);

extern const char *ipv6calctypes_format_string_by_type(const uint32_t format_type);
