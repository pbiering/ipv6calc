/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.h
 * Version    : $Id: ipv6calctypes.h,v 1.3 2002/03/02 17:27:27 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv6calctypes.h
 */ 

#include "ipv6calc.h"

/* prototypes */
extern int ipv6calctypes_checktype(char *string);

/* defines */
#ifndef _ipv6calctypes_h_

#define _ipv6calctypes_h_

/*
 * Format handling matrix for new address option style
 */


/* Format number definitions, each possible format has one number */
#define FORMAT_auto		0x0000
#define FORMAT_revnibbles_int	0x0001		
#define FORMAT_revnibbles_arpa	0x0002		
#define FORMAT_bitstring	0x0004
#define FORMAT_ipv6addr		0x0008
#define FORMAT_ipv4addr		0x0010
#define FORMAT_mac		0x0020
#define FORMAT_eui64		0x0040
#define FORMAT_base85		0x0080
#define FORMAT_ifinet6		0x0100

/* Primary label of format number, keeping also an explanation */
typedef struct {
	long int number;
	char *token;
	char *explanation;
	char *aliases;
} s_format;

static const s_format ipv6calc_formatstrings[] = {
	{ FORMAT_auto           , "auto"           , "automatic detection", "" },
	{ FORMAT_revnibbles_int , "revnibbles.int" , "dotseparated nibbles reverse, ending with ip6.int. (for DNS/PTR)", "" },
	{ FORMAT_revnibbles_arpa, "revnibbles.arpa", "dotseparated nibbles reverse, ending with ip6.arpa. (for DNS/PTR)", "" },
	{ FORMAT_bitstring      , "bitstring"      , "bitstring labes, ending with ip6.arpa. (for DNS/PTR)", "" },
	{ FORMAT_ipv6addr       , "ipv6addr"       , "IPv6 address", "ipv6" },
	{ FORMAT_ipv4addr       , "ipv4addr"       , "IPv4 address", "ipv4" },
	{ FORMAT_mac            , "mac"            , "MAC address (48 bits)", "eui48" },
	{ FORMAT_eui64          , "eui64"          , "EUI-64 identifier (64 bits)", "" },
	{ FORMAT_base85         , "base85"         , "Base-85 string", "" },
	{ FORMAT_ifinet6        , "ifinet6"        , "Like line in /proc/net/if_inet6", "" },
};

/* Format conversion matrix */
static const long int ipv6calc_formatmatrix[][2] = {
	{ FORMAT_auto           , 0xffff },
	{ FORMAT_revnibbles_int , 0 }, /* input currently not supported */
	{ FORMAT_revnibbles_arpa, 0 }, /* input currently not supported */
	{ FORMAT_bitstring      , 0 }, /* input currently not supported */
	{ FORMAT_ipv6addr       , FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ipv6addr | FORMAT_base85 | FORMAT_ifinet6 },
	{ FORMAT_ipv4addr       , FORMAT_ipv6addr },
	{ FORMAT_mac            , FORMAT_eui64 },
	{ FORMAT_eui64          , 0 }, /* input currently not supported */
	{ FORMAT_base85         , FORMAT_base85 | FORMAT_ipv6addr | FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ifinet6 },
	{ FORMAT_ifinet6        , FORMAT_base85 | FORMAT_ipv6addr | FORMAT_revnibbles_int | FORMAT_revnibbles_arpa | FORMAT_bitstring | FORMAT_ifinet6 }
};


/* Format options */
#define FORMATOPTION_HEAD			0xf000
#define FORMATOPTION_printlowercase		0x0001
#define FORMATOPTION_printuppercase		0x0002
#define FORMATOPTION_printprefix		0x0004
#define FORMATOPTION_printsuffix		0x0008
#define FORMATOPTION_maskprefix			0x0010
#define FORMATOPTION_masksuffix			0x0020
#define FORMATOPTION_printstart			0x0040
#define FORMATOPTION_printend			0x0080
#define FORMATOPTION_printcompressed		0x0100
#define FORMATOPTION_printuncompressed		0x0200
#define FORMATOPTION_printfulluncompressed	0x0400

/* Possible format option map */
static const int ipv6calc_outputformatoptionmap[][2]  = {
	{ FORMAT_revnibbles_int , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend },
	{ FORMAT_revnibbles_arpa, FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend },
	{ FORMAT_bitstring      , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend },
	{ FORMAT_ipv6addr       , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase | FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_maskprefix | FORMATOPTION_masksuffix | FORMATOPTION_printstart | FORMATOPTION_printend | FORMATOPTION_printcompressed | FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed },
	{ FORMAT_mac            , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_eui64          , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_base85         , 0 },
	{ FORMAT_ifinet6        , FORMATOPTION_printlowercase | FORMATOPTION_printuppercase },
	{ FORMAT_ipv4addr       , 0 }
};

/*

};

*/

#endif
