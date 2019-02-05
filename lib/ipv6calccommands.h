/*
 * Project    : ipv6calc
 * File       : ipv6calccommands.h
 * Version    : $Id$
 * Copyright  : 2002-2019 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GPLv2
 *
 * Information:
 *  Header file containing commands for ipv6calc
 */ 

#ifndef _ipv6calccommands_h_

#define _ipv6calccommands_h_

/* verbose levels */
#define LEVEL_VERBOSE			0x1
#define LEVEL_VERBOSE2			0x2

/* command values */
#define CMD_printhelp			0x0000001
#define CMD_printversion		0x0000002
#define CMD_printversion_verbose	0x0000020
#define CMD_printversion_verbose2	0x0000040

#define CMD_printexamples		0x0000004
#define CMD_printoldoptions		0x0000008	// EOL 2014-04-02

/* new style options */
#define CMD_inputtype			0x0200000
#define CMD_outputtype			0x0400000
#define CMD_actiontype			0x0800000


/* shortcut commands */
#define CMD_shortcut_start		0x0001010
#define CMD_addr_to_ip6int		0x0001010
#define CMD_addr_to_compressed		0x0001020
#define CMD_addr_to_uncompressed	0x0001030
#define CMD_addr_to_fulluncompressed	0x0001040
#define CMD_addr_to_ifinet6		0x0001050
#define CMD_addr_to_base85		0x0001060
#define CMD_addr_to_ip6arpa		0x0001070
#define CMD_addr_to_bitstring		0x0001080
#define CMD_ipv4_to_6to4addr		0x0001090
#define CMD_base85_to_addr		0x00010a0
#define CMD_mac_to_eui64		0x00010b0
#define CMD_eui64_to_privacy		0x00010c0
#define CMD_ifinet6_to_compressed	0x00010d0
#define CMD_shortcut_end		0x00010d0

#define CMD_showinfo			0x0010010
#define CMD_showinfotypes		0x0010020

/* database options (old) */
#define DB_ip2location_ipv4		0x0020010
#define DB_ip2location_ipv6		0x0020020
#define DB_ip2location_ipv4_default	0x0020030
#define DB_ip2location_ipv6_default	0x0020040
#define DB_geoip_ipv4			0x0021010
#define DB_geoip_ipv6			0x0021020
#define DB_geoip_ipv4_default		0x0002103
#define DB_geoip_ipv6_default		0x0002104

/* database options (new) */
#define DB_ip2location_disable		0x0020000
#define DB_ip2location_dir		0x0020050
#define DB_ip2location_lib		0x0020060
#define DB_ip2location_lite_to_sample_autoswitch_max_delta_months	0x0020100
#define DB_ip2location_comm_to_lite_switch_min_delta_months		0x0020110
#define DB_ip2location_only_type	0x0020120
#define DB_ip2location_allow_softlinks	0x0020130

#define DB_geoip_disable		0x0021000
#define DB_geoip_dir			0x0021050
#define DB_geoip_lib			0x0021060

#define DB_dbip_disable			0x0022000
#define DB_dbip_dir			0x0022050
#define DB_dbip_comm_to_free_switch_min_delta_months		0x0022110
#define DB_dbip_only_type		0x0022120

#define DB_external_disable		0x0023000
#define DB_external_dir			0x0023050

#define DB_builtin_disable		0x0024000

#define DB_mmdb_disable			0x0025000
#define DB_mmdb_lib			0x0025060

#define DB_geoip2_disable		0x0026000
#define DB_geoip2_dir			0x0026050

#define DB_dbip2_disable		0x0027000
#define DB_dbip2_dir			0x0027050
#define DB_dbip2_comm_to_free_switch_min_delta_months		0x0027110
#define DB_dbip2_only_type		0x0027120

#define DB_common_priorization		0x002fff0


/* address anonymizer options */
#define CMD_ANON_PRESET_STANDARD	0x0030010
#define CMD_ANON_PRESET_CAREFUL		0x0030020
#define CMD_ANON_PRESET_PARANOID	0x0030030
#define CMD_ANON_METHOD_OPTION		0x00300e0
#define CMD_ANON_PRESET_OPTION		0x00300f0
#define CMD_ANON_MASK_IPV4		0x0030100
#define CMD_ANON_MASK_IPV6		0x0030110
#define CMD_ANON_MASK_EUI64		0x0030120
#define CMD_ANON_MASK_MAC		0x0030130
#define CMD_ANON_MASK_AUTOADJUST	0x0030180
#define CMD_ANON_MASK_IID		0x0030190	// deprecated

/* 6rd options */
#define CMD_6rd_prefix			0x0040010
#define CMD_6rd_relay_prefix		0x0040020
#define CMD_6rd_prefixlength		0x0040030

/* address test options */
#define CMD_test_prefix			0x0050010	// prefix
#define CMD_test_gt			0x0050020	// greater than
#define CMD_test_ge			0x0050030	// greater equal
#define CMD_test_lt			0x0050040	// less than
#define CMD_test_le			0x0050050	// less equal

/* no operations (dummy) */
#define OPTION_NOOP			0xfffffff

#endif
