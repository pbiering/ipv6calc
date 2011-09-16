/*
 * Project    : ipv6calc
 * File       : librfc5569.c
 * Version    : $Id: librfc5569.c,v 1.1 2011/09/16 18:05:13 peter Exp $
 * Copyright  : 2011 by Raphaël Assénat <raph (at) raphnet.net>
 *
 * Information:
 *  Function library for conversions defined in RFC 5569
 */
#include <stdio.h>
#include "libipv4addr.h"
#include "libipv6addr.h"
#include "librfc5569.h"

static void ipv6addr_to_bytes(const ipv6calc_ipv6addr *ipv6addrp, uint8_t bytes[16])
{
	int i;
	
	for (i=0; i<16; i++) {
		bytes[i] = ipv6addr_getoctet(ipv6addrp, i);
	}
}

static void ipv6addr_from_bytes(ipv6calc_ipv6addr *ipv6addrp, uint8_t bytes[16])
{
	int i;

	for (i=0; i<16; i++) {
		ipv6addr_setoctet(ipv6addrp, i, bytes[i]);
	}
}


int librfc5569_calc_6rd_local_prefix(ipv6calc_ipv6addr *sixrd_prefix, const ipv6calc_ipv4addr *sixrd_relay_prefix, const ipv6calc_ipv4addr *local_ip, char *resultstring)
{
	uint32_t local_ip_bits, j;
	uint8_t tmpaddr[16];
	int i;
	int local_prefix_len;

	local_prefix_len = sixrd_prefix->prefixlength + 32 - sixrd_relay_prefix->prefixlength;
	if (local_prefix_len > 64) {
		snprintf(resultstring, NI_MAXHOST - 1, "Length of local prefix > 64: %d", local_prefix_len);
		return (1);
	}

	local_ip_bits = local_ip->in_addr.s_addr;
	local_ip_bits <<= sixrd_relay_prefix->prefixlength;
	
	ipv6addr_to_bytes(sixrd_prefix, tmpaddr);

	for (j=0x80000000,i=sixrd_prefix->prefixlength; i<local_prefix_len; i++,j>>=1)
	{
		if (local_ip_bits & j)
			tmpaddr[i>>3] |= 0x80 >> (i&0x7);
	}

	ipv6addr_from_bytes(sixrd_prefix, tmpaddr);
	
	sixrd_prefix->prefixlength = local_prefix_len;

	return (0);
}

