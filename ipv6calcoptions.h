/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Version    : $Id: ipv6calcoptions.h,v 1.3 2002/03/17 10:13:51 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options for ipv6calc.c
 */ 

#include <getopt.h> 

#include "ipv6calctypes.h"


#ifndef _ipv6calcoptions_h_

#define _ipv6calcoptions_h_

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
static char *ipv6calc_shortopts = "vh?rmabd:iulq";

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
