/*
 * Project    : ipv6calc
 * File       : ipv6calccommands.h
 * Version    : $Id: ipv6calccommands.h,v 1.1 2002/04/21 11:31:25 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing commands for ipv6calc
 */ 

#ifndef _ipv6calccommands_h_

#define _ipv6calccommands_h_

/* command values */
#define CMD_printhelp			0x00001
#define CMD_printversion		0x00002

#define CMD_printexamples		0x00004
#define CMD_printoldoptions		0x00008

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

#endif
