/*
 * Project    : ipv6calc
 * File       : ipv6calcoptions.h
 * Version    : $Id: ipv6calcoptions.h,v 1.24 2013/05/12 07:34:04 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options
 */ 

#include <getopt.h> 

#include "ipv6calctypes.h"
#include "ipv6calccommands.h"


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
 *  f = flush (each line in pipe mode)
 *  I <input type>
 *  O <output type>
 *  A <action type>
 *  E expression for action=filter
 */
/*@unused@*/ static char *ipv6calc_shortopts __attribute__ ((__unused__)) = "vh?rmabfd:iulqLGUFCI:O:A:E:";

/* define long options */
/*@null@*/ /*@unused@*/ static struct option ipv6calc_longopts[] = {
	{"version", 0, NULL, (int) 'v' },
	{"flush"  , 0, NULL, (int) 'f' },
	{"debug"  , 1, NULL, (int) 'd' },

	/* database options */
	{"db-ip2location-ipv4"         , 1, NULL, DB_ip2location_ipv4   },
	{"db-ip2location-ipv6"         , 1, NULL, DB_ip2location_ipv6   },
	{"db-ip2location-default"      , 0, NULL, (int) 'L'             },
	{"db-ip2location-ipv4-default" , 0, NULL, DB_geoip_ipv4_default },
	{"db-ip2location-ipv6-default" , 0, NULL, DB_geoip_ipv6_default },
	{"db-geoip"                    , 1, NULL, DB_geoip_ipv4         }, // backward compatibility
	{"db-geoip-ipv4"               , 1, NULL, DB_geoip_ipv4         },
	{"db-geoip-ipv6"               , 1, NULL, DB_geoip_ipv6         },
	{"db-geoip-default"            , 0, NULL, (int) 'G'             },
	{"db-geoip-ipv4-default"       , 0, NULL, DB_geoip_ipv4_default },
	{"db-geoip-ipv6-default"       , 0, NULL, DB_geoip_ipv6_default },

	/* help options */
	{"help"           , 0, NULL, (int) 'h'},
	{"examples"       , 0, NULL, CMD_printexamples   },
	{"printexamples"  , 0, NULL, CMD_printexamples   },
	{"printoldoptions", 0, NULL, CMD_printoldoptions },
	{"old"            , 0, NULL, CMD_printoldoptions },

	
	/* backward compatibility/shortcut commands */
	{"addr2ip6_int"             , 0, NULL, CMD_addr_to_ip6int },
	{"addr_to_ip6int"           , 0, NULL, CMD_addr_to_ip6int },

	{"addr2ip6_arpa"            , 0, NULL, CMD_addr_to_ip6arpa },
	{"addr_to_ip6arpa"          , 0, NULL, CMD_addr_to_ip6arpa },
	
	{"addr_to_bitstring"        , 0, NULL, CMD_addr_to_bitstring },

	{"addr2compaddr"            , 0, NULL, CMD_addr_to_compressed },
	{"addr_to_compressed"       , 0, NULL, CMD_addr_to_compressed },
	{"addr2uncompaddr"          , 0, NULL, CMD_addr_to_uncompressed },
	{"addr_to_uncompressed"     , 0, NULL, CMD_addr_to_uncompressed },
	
	{"addr_to_base85"           , 0, NULL, CMD_addr_to_base85 },
	{"base85_to_addr"           , 0, NULL, CMD_base85_to_addr },

	{"mac_to_eui64"             , 0, NULL, CMD_mac_to_eui64 },
	
	{ "addr2fulluncompaddr"     , 0, NULL, CMD_addr_to_fulluncompressed },
	{ "addr_to_fulluncompressed", 0, NULL, CMD_addr_to_fulluncompressed },
	{ "addr2if_inet6"           , 0, NULL, CMD_addr_to_ifinet6 },
	{ "addr_to_ifinet6"         , 0, NULL, CMD_addr_to_ifinet6 },
	{ "if_inet62addr"           , 0, NULL, CMD_ifinet6_to_compressed },
	{ "ifinet6_to_compressed"   , 0, NULL, CMD_ifinet6_to_compressed },

	{ "eui64_to_privacy"        , 0, NULL, CMD_eui64_to_privacy },
	
	{ "ipv4_to_6to4addr"        , 0, NULL, CMD_ipv4_to_6to4addr },
	
	{ "showinfo"                , 0, NULL, CMD_showinfo },
	{ "show_types"              , 0, NULL, CMD_showinfotypes },

	/* format options */
	{ "maskprefix"           , 0, NULL, FORMATOPTION_NUM_maskprefix + FORMATOPTION_NUM_HEAD },
	{ "masksuffix"           , 0, NULL, FORMATOPTION_NUM_masksuffix + FORMATOPTION_NUM_HEAD },
	
	{ "uppercase"            , 0, NULL, FORMATOPTION_NUM_printuppercase + FORMATOPTION_NUM_HEAD },
	{ "lowercase"            , 0, NULL, FORMATOPTION_NUM_printlowercase + FORMATOPTION_NUM_HEAD },

	{ "printstart"           , 1, NULL, FORMATOPTION_NUM_printstart  + FORMATOPTION_NUM_HEAD },
	{ "printend"             , 1, NULL, FORMATOPTION_NUM_printend    + FORMATOPTION_NUM_HEAD },
	{ "forceprefix"          , 1, NULL, FORMATOPTION_NUM_forceprefix + FORMATOPTION_NUM_HEAD },
	
	{ "printprefix"          , 0, NULL, FORMATOPTION_NUM_printprefix + FORMATOPTION_NUM_HEAD },
	{ "printsuffix"          , 0, NULL, FORMATOPTION_NUM_printsuffix + FORMATOPTION_NUM_HEAD },
	
	{ "printcompressed"      , 0, NULL, FORMATOPTION_NUM_printcompressed       + FORMATOPTION_NUM_HEAD },
	{ "printuncompressed"    , 0, NULL, FORMATOPTION_NUM_printuncompressed     + FORMATOPTION_NUM_HEAD },
	{ "printfulluncompressed", 0, NULL, FORMATOPTION_NUM_printfulluncompressed + FORMATOPTION_NUM_HEAD },
	
	{ "machine_readable"     , 0, NULL, FORMATOPTION_NUM_machinereadable + FORMATOPTION_NUM_HEAD },
	{ "quiet"                , 0, NULL, FORMATOPTION_NUM_quiet + FORMATOPTION_NUM_HEAD },
	
	{ "printmirrored"        , 0, NULL, FORMATOPTION_NUM_printmirrored + FORMATOPTION_NUM_HEAD },

	{ "print-iid-var"        , 0, NULL, FORMATOPTION_NUM_print_iid_var + FORMATOPTION_NUM_HEAD },

	/* address anonymizer options */
	{ "anonymize-standard" , 0, NULL, CMD_ANON_PRESET_STANDARD },
	{ "anonymize-careful"  , 0, NULL, CMD_ANON_PRESET_CAREFUL  },
	{ "anonymize-paranoid" , 0, NULL, CMD_ANON_PRESET_PARANOID },
	{ "anonymize-preset"   , 1, NULL, CMD_ANON_PRESET_OPTION   },
	{ "anonymize-method"   , 1, NULL, CMD_ANON_METHOD_OPTION   },
	{ "mask-ipv4" , 1, NULL, CMD_ANON_MASK_IPV4 },
	{ "mask-ipv6" , 1, NULL, CMD_ANON_MASK_IPV6 },
	{ "mask-iid"  , 1, NULL, CMD_ANON_MASK_IID },
	{ "mask-mac"  , 1, NULL, CMD_ANON_MASK_MAC },

	/* new options */
	{ "in"        , 1, NULL, CMD_inputtype  },
	{ "out"       , 1, NULL, CMD_outputtype },
	{ "action"    , 1, NULL, CMD_actiontype },

	/* 6rd_prefix options */
	{ "6rd_relay_prefix"	, 1, NULL, CMD_6rd_relay_prefix },
	{ "6rd_prefix"		, 1, NULL, CMD_6rd_prefix },

	{ NULL, 0, NULL, 0}
};                

#endif
