/*
 * Project    : ipv6calc
 * File       : ipv6calctypes.h
 * Version    : $Id: ipv6calctypes.h,v 1.1 2002/03/01 23:27:25 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for ipv6calctypes.h
 */ 

extern int ipv6calctypes_checktype(char *string);

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
