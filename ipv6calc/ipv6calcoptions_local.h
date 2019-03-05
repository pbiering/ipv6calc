/*
 * Project    : ipv6calc/ipv6calc
 * File       : ipv6calc/ipv6calcoptions.h
 * Version    : $Id$
 * Copyright  : 2002-2019 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options for ipv6calc
 */ 

#include <getopt.h> 
#include <stdio.h>

#include "ipv6calctypes.h"
#include "ipv6calccommands.h"
#include "ipv6calcoptions.h"


/* Options */

/* define short options */
/*
 *  l = lower case
 *  u = upper case
 *  r = reverse nibble (ip6.int)
 *  a = reverse nibble (ip6.arpa)
 *  b = bitstring label
 *  m = output machine readable
 *  i = showinfo
 *  f = flush (each line in pipe mode)
 *  I <input type>
 *  O <output type>
 *  A <action type>
 *  E expression for action=filter
 */
static char *ipv6calc_shortopts = "vh?rmabfiulUFCI:O:A:E:";

/* define long options */
static struct option ipv6calc_longopts[] = {
	{"version", 0, NULL, (int) 'v' },

	/* additional help options */
	{"help"           , 0, NULL, (int) 'h'},
	{"examples"       , 0, NULL, CMD_printexamples   },
	{"printexamples"  , 0, NULL, CMD_printexamples   },

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

	{ "addr2cc"                 , 0, NULL, CMD_addr_to_countrycode },
	
	{ "showinfo"                , 0, NULL, CMD_showinfo },
	{ "show_types"              , 0, NULL, CMD_showinfotypes },
	{ "show-tokens"             , 0, NULL, CMD_showinfotypes },

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
	// { "quiet"                , 0, NULL, FORMATOPTION_NUM_quiet + FORMATOPTION_NUM_HEAD }, strange duplicate
	/* machine readable format/filter */
	{ "mrqva"                , 0, NULL, FORMATOPTION_NUM_mr_quote_always + FORMATOPTION_NUM_HEAD },
	{ "mrqvn"                , 0, NULL, FORMATOPTION_NUM_mr_quote_never  + FORMATOPTION_NUM_HEAD },
	{ "mrtvo"                , 1, NULL, FORMATOPTION_NUM_mr_value_only   + FORMATOPTION_NUM_HEAD },
	{ "mrst"                 , 1, NULL, FORMATOPTION_NUM_mr_select_token + FORMATOPTION_NUM_HEAD },
	{ "mrstpa"               , 1, NULL, FORMATOPTION_NUM_mr_select_token_pa + FORMATOPTION_NUM_HEAD },
	{ "mrmt"                 , 1, NULL, FORMATOPTION_NUM_mr_match_token  + FORMATOPTION_NUM_HEAD },
	{ "mrmts"                , 1, NULL, FORMATOPTION_NUM_mr_match_token_suffix  + FORMATOPTION_NUM_HEAD },
	
	{ "printmirrored"        , 0, NULL, FORMATOPTION_NUM_printmirrored + FORMATOPTION_NUM_HEAD },

	{ "print-iid-var"        , 0, NULL, FORMATOPTION_NUM_print_iid_var + FORMATOPTION_NUM_HEAD },

	/* new options */
	{ "in"        , 1, NULL, CMD_inputtype  },
	{ "out"       , 1, NULL, CMD_outputtype },
	{ "action"    , 1, NULL, CMD_actiontype },

	/* 6rd_prefix options for action */
	{ "6rd_relay_prefix"	, 1, NULL, CMD_6rd_relay_prefix },
	{ "6rd_prefix"		, 1, NULL, CMD_6rd_prefix },

	/* 6rd_prefix options for showinfo/action */
	{ "6rd_prefixlength"	, 1, NULL, CMD_6rd_prefixlength },

	/* test_* options action */
	{ "test_prefix"		, 1, NULL, CMD_test_prefix },
	{ "test_ge"		, 1, NULL, CMD_test_ge },
	{ "test_gt"		, 1, NULL, CMD_test_gt },
	{ "test_le"		, 1, NULL, CMD_test_le },
	{ "test_lt"		, 1, NULL, CMD_test_lt },

}; 


/* map between short and long options */
static s_ipv6calc_longopts_shortopts_map ipv6calc_longopts_shortopts_map[] = {
	{ 'r',	CMD_addr_to_ip6int					, "-O revnibbles.int"		},
	{ 'a',	CMD_addr_to_ip6arpa					, "-O revnibbles.arpa"		},
	{ 'b',	CMD_addr_to_bitstring					, "-O bitstring"		},
	{ 'i',	CMD_showinfo						, NULL				},
	{ 'l',	FORMATOPTION_NUM_printlowercase + FORMATOPTION_NUM_HEAD , "--printlowercase"		},
	{ 'u',	FORMATOPTION_NUM_printuppercase + FORMATOPTION_NUM_HEAD	, "--printuppercase"		},
	{ 'C',	FORMATOPTION_NUM_printcompressed + FORMATOPTION_NUM_HEAD, "--printcompressed"	},
	{ 'F',	FORMATOPTION_NUM_printfulluncompressed + FORMATOPTION_NUM_HEAD, "--printfulluncompressed"	},
	{ 'I',	CMD_inputtype						, NULL				},
	{ 'O',	CMD_outputtype						, NULL				},
	{ 'A',	CMD_actiontype						, NULL				},
	{ 'm',	FORMATOPTION_NUM_machinereadable + FORMATOPTION_NUM_HEAD, NULL				},
	{ '\0',	CMD_addr_to_compressed					, "--printcompressed"		},
	{ '\0',	CMD_addr_to_uncompressed				, "--printuncompressed"		},
	{ '\0',	CMD_addr_to_fulluncompressed				, "--printfulluncompressed"	},
	{ '\0',	CMD_addr_to_base85					, "-I ipv6addr -O base85"	},
	{ '\0',	CMD_base85_to_addr					, "-I base85 -O ipv6addr"	},
	{ '\0',	CMD_mac_to_eui64					, "-I mac -O eui64"	},
	{ '\0',	CMD_addr_to_ifinet6					, "-I ipv6addr -O ifinet6"	},
	{ '\0',	CMD_ifinet6_to_compressed				, "-I ifinet6 -O ipv6addr --printcompressed"},
	{ '\0',	CMD_ipv4_to_6to4addr					, "-I ipv4 -O ipv6addr -A conv6to4"},
	{ '\0',	CMD_eui64_to_privacy					, "-I iid_token -O iid_token -A genprivacyiid"},
	{ '\0',	0, NULL } // end marker
};
