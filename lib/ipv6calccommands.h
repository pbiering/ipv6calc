/*
 * Project    : ipv6calc
 * File       : ipv6calccommands.h
 * Version    : $Id: ipv6calccommands.h,v 1.9 2011/10/08 11:50:13 peter Exp $
 * Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing commands for ipv6calc
 */ 

#ifndef _ipv6calccommands_h_

#define _ipv6calccommands_h_

/* command values */
#define CMD_printhelp			0x0000001
#define CMD_printversion		0x0000002
#define CMD_printversion_verbose	0x0000020

#define CMD_printexamples		0x0000004
#define CMD_printoldoptions		0x0000008

/* new style options */
#define CMD_inputtype			0x0200000
#define CMD_outputtype			0x0400000
#define CMD_actiontype			0x0800000


/* shortcut commands */
#define CMD_shortcut_start		0x0000101
#define CMD_addr_to_ip6int		0x0000101
#define CMD_addr_to_compressed		0x0000102
#define CMD_addr_to_uncompressed	0x0000103
#define CMD_addr_to_fulluncompressed	0x0000104
#define CMD_addr_to_ifinet6		0x0000105
#define CMD_addr_to_base85		0x0000106
#define CMD_addr_to_ip6arpa		0x0000107
#define CMD_addr_to_bitstring		0x0000108
#define CMD_ipv4_to_6to4addr		0x0000109
#define CMD_base85_to_addr		0x000010a
#define CMD_mac_to_eui64		0x000010b
#define CMD_eui64_to_privacy		0x000010c
#define CMD_ifinet6_to_compressed	0x000010d
#define CMD_shortcut_end		0x000010d

#define CMD_showinfo			0x0001001
#define CMD_showinfotypes		0x0001002

/* database options */
#define DB_ip2location_ipv4		0x0002001
#define DB_ip2location_ipv6		0x0002002
#define DB_ip2location_ipv4_default	0x0002003
#define DB_ip2location_ipv6_default	0x0002004
#define DB_geoip_ipv4			0x0002011
#define DB_geoip_ipv6			0x0002012
#define DB_geoip_ipv4_default		0x0002013
#define DB_geoip_ipv6_default		0x0002014

/* address anonymizer options */
#define CMD_ANON_PRESET_STANDARD	0x0003001
#define CMD_ANON_PRESET_CAREFUL		0x0003002
#define CMD_ANON_PRESET_PARANOID	0x0003003
#define CMD_ANON_MASK_IPV4		0x0003010
#define CMD_ANON_MASK_IPV6		0x0003011
#define CMD_ANON_NO_MASK_IID		0x0003012

/* 6rd options */
#define CMD_6rd_prefix			0x0004001
#define CMD_6rd_relay_prefix		0x0004002

#endif
