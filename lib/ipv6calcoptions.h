/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Version    : $Id: ipv6calcoptions.h,v 1.1 2002/03/18 19:59:23 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options for ipv6calc.c
 */ 

#include <getopt.h> 

#include "ipv6calctypes.h"


#ifndef _ipv6calcoptions_h_

#define _ipv6calcoptions_h_

/* command values */
#define CMD_printhelp			0x00001
#define CMD_printversion		0x00002

#define CMD_printexamples		0x00004

/* new style options */
#define CMD_inputtype			0x0200000
#define CMD_outputtype			0x0400000
#define CMD_actiontype			0x0800000


/* shortcut commands */
#define CMD_shortcut_start		0x000101
#define CMD_addr_to_ip6int		0x000101
#define CMD_addr_to_compressed		0x000102
#define CMD_addr_to_uncompressed	0x000103
#define CMD_addr_to_fulluncompressed	0x000104
#define CMD_addr_to_ifinet6		0x000105
#define CMD_addr_to_base85		0x000106
#define CMD_addr_to_ip6arpa		0x000107
#define CMD_addr_to_bitstring		0x000108
#define CMD_ipv4_to_6to4addr		0x000109
#define CMD_base85_to_addr		0x00010a
#define CMD_mac_to_eui64		0x00010b
#define CMD_eui64_to_privacy		0x00010c
#define CMD_ifinet6_to_compressed	0x00010d
#define CMD_shortcut_end		0x00010d

#define CMD_showinfo			0x001001
#define CMD_showinfotypes		0x001002

/* Options */

/* define short options */
/*  v = version
 *  h|? = help
 *  d <debugvalue> = debug value
 *  l = lower case
 *  u = upper case
 *  r = reverse nibble (ip6.int)
 *  a = reverse nibble (ip6.arpa)
 *  b = bitstring label
 *  m = output machine readable
 *  i = showinfo
 *  q = be more quiet
 */
/*@unused@*/ static char *ipv6calc_shortopts = "vh?rmabd:iulq";

/* define long options */
static struct option ipv6calc_longopts[] = {
	{"version", 0, 0, (int) 'v'},
	{"debug", 1, 0, (int) 'd'},

	/* help options */
	{"help", 0, 0, (int) 'h'},
	{"examples"     , 0, 0, CMD_printexamples },
	{"printexamples", 0, 0, CMD_printexamples },

	
	/* backward compatibility/shortcut commands */
	{"addr2ip6_int", 0, 0, CMD_addr_to_ip6int },
	{"addr_to_ip6int", 0, 0, CMD_addr_to_ip6int },

	{"addr2ip6_arpa", 0, 0, CMD_addr_to_ip6arpa },
	{"addr_to_ip6arpa", 0, 0, CMD_addr_to_ip6arpa },
	
	{"addr_to_bitstring", 0, 0, CMD_addr_to_bitstring },

	{"addr2compaddr", 0, 0, CMD_addr_to_compressed },
	{"addr_to_compressed", 0, 0, CMD_addr_to_compressed },
	{"addr2uncompaddr", 0, 0, CMD_addr_to_uncompressed },
	{"addr_to_uncompressed", 0, 0, CMD_addr_to_uncompressed },
	
	{"addr_to_base85", 0, 0, CMD_addr_to_base85 },
	{"base85_to_addr", 0, 0, CMD_base85_to_addr },

	{"mac_to_eui64", 0, 0, CMD_mac_to_eui64 },
	
	{ "addr2fulluncompaddr", 0, 0, CMD_addr_to_fulluncompressed },
	{ "addr_to_fulluncompressed", 0, 0, CMD_addr_to_fulluncompressed },
	{ "addr2if_inet6", 0, 0, CMD_addr_to_ifinet6 },
	{ "addr_to_ifinet6", 0, 0, CMD_addr_to_ifinet6 },
	{ "if_inet62addr", 0, 0, CMD_ifinet6_to_compressed },
	{ "ifinet6_to_compressed", 0, 0, CMD_ifinet6_to_compressed },

	{ "eui64_to_privacy", 0, 0, CMD_eui64_to_privacy },
	
	{ "ipv4_to_6to4addr", 0, 0, CMD_ipv4_to_6to4addr },
	
	{ "showinfo", 0, 0, CMD_showinfo },
	{ "show_types", 0, 0, CMD_showinfotypes },

	/* format options */
	{ "maskprefix"           , 0, 0, FORMATOPTION_maskprefix + FORMATOPTION_HEAD },
	{ "masksuffix"           , 0, 0, FORMATOPTION_masksuffix + FORMATOPTION_HEAD },
	
	{ "uppercase"            , 0, 0, FORMATOPTION_printuppercase + FORMATOPTION_HEAD },
	{ "lowercase"            , 0, 0, FORMATOPTION_printlowercase + FORMATOPTION_HEAD },
	
	{ "printstart"           , 1, 0, FORMATOPTION_printstart + FORMATOPTION_HEAD },
	{ "printend"             , 1, 0, FORMATOPTION_printend + FORMATOPTION_HEAD },
	
	{ "printprefix"          , 0, 0, FORMATOPTION_printprefix + FORMATOPTION_HEAD },
	{ "printsuffix"          , 0, 0, FORMATOPTION_printsuffix + FORMATOPTION_HEAD },
	
	{ "printcompressed"      , 0, 0, FORMATOPTION_printcompressed       + FORMATOPTION_HEAD },
	{ "printuncompressed"    , 0, 0, FORMATOPTION_printuncompressed     + FORMATOPTION_HEAD },
	{ "printfulluncompressed", 0, 0, FORMATOPTION_printfulluncompressed + FORMATOPTION_HEAD },
	
	{ "machine_readable"     , 0, 0, FORMATOPTION_machinereadable + FORMATOPTION_HEAD },
	{ "quiet"                , 0, 0, FORMATOPTION_quiet + FORMATOPTION_HEAD },

	/* new options */
	{ "in"        , 1, 0, CMD_inputtype  },
	{ "out"       , 1, 0, CMD_outputtype },
	{ "action"    , 1, 0, CMD_actiontype },

	{NULL, 0, 0, 0}
};                

#endif
