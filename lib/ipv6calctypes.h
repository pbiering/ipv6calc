/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.h
 * Version    : $Id: ipv6calctypes.h,v 1.2 2002/03/24 16:58:21 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv6calctypes.c
 */ 

#include <getopt.h> 

/* prototypes */
extern int ipv6calctypes_checktype(const char *string);
extern int ipv6calctypes_checkaction(const char *string);

/* defines */
#ifndef _ipv6calctypes_h_

#define _ipv6calctypes_h_

/*
 * Format handling matrix for new address option style
 */

/* Format number definitions, each possible format has one number */
#define FORMAT_auto		0x00000
#define FORMAT_revnibbles_int	0x00001
#define FORMAT_revnibbles_arpa	0x00002
#define FORMAT_bitstring	0x00004
#define FORMAT_ipv6addr		0x00008
#define FORMAT_ipv4addr		0x00010
#define FORMAT_mac		0x00020
#define FORMAT_eui64		0x00040
#define FORMAT_base85		0x00080
#define FORMAT_ifinet6		0x00100
#define FORMAT_iid		0x00200
#define FORMAT_iid_token	0x00400
#define FORMAT_addrtype 	0x00800
#define FORMAT_ouitype	 	0x01000
#define FORMAT_ipv6addrtype 	0x02000
#define FORMAT_ipv6logconv 	0x04000
#define FORMAT_any	 	0x08000
#define FORMAT_prefix_mac	0x10000

/* Primary label of format number, keeping also an explanation */
typedef struct {
	const int number;
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
	{ FORMAT_prefix_mac     , "prefix+mac"     , "IPv6 prefix and a MAC address", "" },
};

/* Format conversion matrix */
/*@unused@*/ static const int ipv6calc_formatmatrix[13][2] = {
	{ FORMAT_auto           , 0x5ff },
	{ FORMAT_revnibbles_int , 0x5ff },
	{ FORMAT_revnibbles_arpa, 0x5ff },
	{ FORMAT_bitstring      , 0x5ff },
	{ FORMAT_ipv6addr       , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 },
	{ FORMAT_ipv4addr       , FORMAT_ipv6addr },
	{ FORMAT_mac            , FORMAT_eui64 },
	{ FORMAT_eui64          , 0 },
	{ FORMAT_base85         , FORMAT_base85 | FORMAT_ipv6addr | FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ifinet6 },
	{ FORMAT_ifinet6        , FORMAT_base85 | FORMAT_ipv6addr | FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ifinet6 },
	{ FORMAT_iid_token      , FORMAT_iid_token },
	{ FORMAT_ipv6logconv    , FORMAT_ipv6addrtype | FORMAT_addrtype | FORMAT_ouitype | FORMAT_any },
	{ FORMAT_prefix_mac     , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 }
};


/* Format options */
#define FORMATOPTION_HEAD			0xf0000
#define FORMATOPTION_printlowercase		0x00001
#define FORMATOPTION_printuppercase		0x00002
#define FORMATOPTION_printprefix		0x00004
#define FORMATOPTION_printsuffix		0x00008
#define FORMATOPTION_maskprefix			0x00010
#define FORMATOPTION_masksuffix			0x00020
#define FORMATOPTION_printstart			0x00040
#define FORMATOPTION_printend			0x00080
#define FORMATOPTION_printcompressed		0x00100
#define FORMATOPTION_printuncompressed		0x00200
#define FORMATOPTION_printfulluncompressed	0x00400
#define FORMATOPTION_machinereadable		0x00800
#define FORMATOPTION_quiet			0x01000

typedef struct {
	int number;
	char *token;
	char *explanation;
} s_formatoption;

/*@unused@*/ static const s_formatoption ipv6calc_formatoptionstrings[] = {
	{ FORMATOPTION_printlowercase   , "--printlowercase"   , "Print output in lower case" },
	{ FORMATOPTION_printuppercase   , "--printuppercase"   , "Print output in upper case" },
	{ FORMATOPTION_printprefix      , "--printprefix"      , "Print only prefix" },
	{ FORMATOPTION_printsuffix      , "--printsuffix"      , "Print only suffix" },
	{ FORMATOPTION_maskprefix       , "--maskprefix"       , "Mask prefix bits (suffix set to 0)" },
	{ FORMATOPTION_masksuffix       , "--masksuffix"       , "Mask suffix bits (prefix set to 0)" },
	{ FORMATOPTION_printstart       , "--printstart <num>" , "Printing starts at bit <num>" },
	{ FORMATOPTION_printend         , "--printend <num>"   , "Printing ends at bit <num>" },
	{ FORMATOPTION_printcompressed  , "--printcompressed"  , "Print in compressed format" },
	{ FORMATOPTION_printuncompressed  , "--printuncompressed"  , "Print in uncompressed format" },
	{ FORMATOPTION_printfulluncompressed  , "--printfulluncompressed"  , "Print in full uncompressed format" },
	{ FORMATOPTION_machinereadable  , "--machinereadable|-m" , "Print output machine readable" },
	{ FORMATOPTION_quiet            , "--quiet|-q"         , "Be more quiet" },
};

/* Possible format option map */
/*@unused@*/ static const int ipv6calc_outputformatoptionmap[11][2]  = {
	{ FORMAT_revnibbles_int , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend },
	{ FORMAT_revnibbles_arpa, FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend },
	{ FORMAT_bitstring      , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend },
	{ FORMAT_ipv6addr       , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend | FORMATOPTION_printcompressed | FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed },
	{ FORMAT_mac            , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_eui64          , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_base85         , 0 },
	{ FORMAT_ifinet6        , 0 },
	{ FORMAT_ipv4addr       , FORMATOPTION_machinereadable },
	{ FORMAT_iid_token      , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_prefix_mac     , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase }
};


/* Actions */
#define ACTION_auto			0x0000000
#define ACTION_mac_to_eui64		0x0000001
#define ACTION_ipv4_to_6to4addr		0x0000002
#define ACTION_iid_token_to_privacy	0x0000004
#define ACTION_prefix_mac_to_ipv6	0x0000008

typedef struct {
	int number;
	char *token;
	char *explanation;
	char *aliases;
} s_action;

/*@unused@*/ static const s_action ipv6calc_actionstrings[] = {
	{ ACTION_auto                 , "auto"           , "Automatic selection of action (default)", "" },
	{ ACTION_mac_to_eui64         , "geneui64"       , "Converts a MAC address to an EUI-64 address", "" },
	{ ACTION_ipv4_to_6to4addr     , "conv6to4"       , "Converts IPv4 address <-> 6to4 IPv6 address (prefix)", "" },
	{ ACTION_iid_token_to_privacy , "genprivacyiid"  , "Generates a privacy interface ID out of a given one and a token", "" },
	{ ACTION_prefix_mac_to_ipv6   , "prefixmac2ipv6" , "Generates an IPv6 address out of a prefix and a MAC address", "" }
};


#endif
