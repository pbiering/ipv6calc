/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.h
 * Version    : $Id: ipv6calctypes.h,v 1.2 2002/03/02 10:46:03 peter Exp $
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

#define FORMAT_MAXNUM		10

/* Format number definitions, each possible format has one number */
#define FORMAT_auto		0
#define FORMAT_revnibbles_int	1		
#define FORMAT_bitstring	2
#define FORMAT_ipv6addr		3
#define FORMAT_mac		4
#define FORMAT_eui64		5
#define FORMAT_base85		6
#define FORMAT_ifinet6		7
#define FORMAT_ipv4addr		8
#define FORMAT_revnibbles_arpa	9		

#define FORMAT_STRING_MAXNUM	12

/* Format string definitions */
static const char* ipv6calc_formatstrings[FORMAT_STRING_MAXNUM] = {
	"auto"            /*  0 */,
	"revnibbles.int"   /*  1 */,
	"revnibbles.arpa"  /*  2 */,
	"bitstring"  /*  3 */,
	"ipv6"       /*  4 */,
	"ipv6addr"   /*  5 */,
	"mac"        /*  6 */,
	"eui48"      /*  7 */,
	"eui64"      /*  8 */,
	"base85"     /*  9 */,
	"ifinet6"    /* 10 */,
	"ipv4"       /* 11 */,
};

/* Format string alias map */
static const int ipv6calc_formatstringaliasmap[FORMAT_STRING_MAXNUM][2]  = {
	{  0, FORMAT_auto      },
	{  1, FORMAT_revnibbles_int  },
	{  2, FORMAT_revnibbles_arpa },
	{  3, FORMAT_bitstring },
	{  4, FORMAT_ipv6addr  },
	{  5, FORMAT_ipv6addr  },
	{  6, FORMAT_mac       },
	{  7, FORMAT_mac       },
	{  8, FORMAT_eui64     },
	{  9, FORMAT_base85    },
	{ 10, FORMAT_ifinet6   },
	{ 11, FORMAT_ipv4addr  }
};

/* Format options */
#define FORMATOPTION_printlowercase		0x001
#define FORMATOPTION_printuppercase		0x002
#define FORMATOPTION_printprefix		0x004
#define FORMATOPTION_printsuffix		0x008
#define FORMATOPTION_maskprefix			0x010
#define FORMATOPTION_masksuffix			0x020
#define FORMATOPTION_printstart			0x040
#define FORMATOPTION_printend			0x080
#define FORMATOPTION_printcompressed		0x100
#define FORMATOPTION_printuncompressed		0x200
#define FORMATOPTION_printfulluncompressed	0x400

/* Possible format option map */
static const int ipv6calc_outputformatoptionmap[FORMAT_MAXNUM][2]  = {
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
const int ipv6calc_formatmatrix[FORMAT_MAXNUM][FORMAT_MAXNUM];

ipv6calc_formatmatrix[FORMAT_auto]      = { 0 };
ipv6calc_formatmatrix[FORMAT_revnibble] = { 0 };
 = {
	unsigned int [
	{ 	
#define FORMAT_auto		0x00
#define FORMAT_revnibble	0x01		
#define FORMAT_bitstring	0x02
#define FORMAT_ipv6addr		0x03
#define FORMAT_mac		0x04
#define FORMAT_eui64		0x05
#define FORMAT_base85		0x06
#define FORMAT_ifinet6		0x07
#define FORMAT_ipv4addr		0x08
 "nibbles", "bitstring", "ipv6", "mac", "eui64", "base85", "ifinet6", "ipv4"

};

*/

#endif
