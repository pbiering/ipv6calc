/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstats.h
 * Version    : $Id: ipv6logstats.h,v 1.1 2003/06/15 12:17:49 peter Exp $
 * Copyright  : 2003 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Main header file
 */ 

/* global program related definitions */
#define PROGRAM_NAME "ipv6logstats"
#define PROGRAM_COPYRIGHT "(P) & (C) 2003 by Peter Bieringer <pb (at) bieringer.de>"

#define STATS_IPV4		0x00
#define STATS_IPV6		0x01

#define STATS_IPV4_IANA		0x12
#define STATS_IPV4_APNIC	0x13
#define STATS_IPV4_ARIN		0x14
#define STATS_IPV4_RIPE		0x15
#define STATS_IPV4_LACNIC	0x16
#define STATS_IPV4_RESERVED	0x1e
#define STATS_IPV4_UNKNOWN	0x1f

#define STATS_IPV6_6BONE	0x21
#define STATS_IPV6_IANA		0x22
#define STATS_IPV6_APNIC	0x23
#define STATS_IPV6_ARIN		0x24
#define STATS_IPV6_RIPE		0x25
#define STATS_IPV6_LACNIC	0x26
#define STATS_IPV6_RESERVED	0x2e
#define STATS_IPV6_UNKNOWN	0x2f

#define STATS_IPV6_6TO4_IANA		0x32
#define STATS_IPV6_6TO4_APNIC		0x33
#define STATS_IPV6_6TO4_ARIN		0x34
#define STATS_IPV6_6TO4_RIPE		0x35
#define STATS_IPV6_6TO4_LACNIC		0x36
#define STATS_IPV6_6TO4_RESERVED	0x3e
#define STATS_IPV6_6TO4_UNKNOWN		0x3f

/* labels statistic numbers */
typedef struct {
	const int	number;
	long unsigned int counter;
	const char *token;
} stat_entries;

/*@unused@*/ static stat_entries ipv6logstats_statentries[] = {
	{ STATS_IPV4		, 0, "IPv4" },
	{ STATS_IPV4_APNIC	, 0, "IPv4/APNIC" },
	{ STATS_IPV4_ARIN	, 0, "IPv4/ARIN" },
	{ STATS_IPV4_RIPE	, 0, "IPv4/RIPE" },
	{ STATS_IPV4_LACNIC	, 0, "IPv4/LACNIC" },
	{ STATS_IPV4_UNKNOWN	, 0, "IPv4/UNKNOWN" },
	{ STATS_IPV6		, 0, "IPv6" },
	{ STATS_IPV6_6BONE	, 0, "IPv6/6bone" },
	{ STATS_IPV6_IANA	, 0, "IPv6/IANA" },
	{ STATS_IPV6_APNIC	, 0, "IPv6/APNIC" },
	{ STATS_IPV6_ARIN	, 0, "IPv6/ARIN" },
	{ STATS_IPV6_RIPE	, 0, "IPv6/RIPE" },
	{ STATS_IPV6_LACNIC	, 0, "IPv6/LACNIC" },
	{ STATS_IPV6_RESERVED	, 0, "IPv6/RESERVED" },
	{ STATS_IPV6_UNKNOWN	, 0, "IPv6/UNKNOWN" },
	{ STATS_IPV6_6TO4_IANA	, 0, "IPv6/6to4/IANA" },
	{ STATS_IPV6_6TO4_APNIC	, 0, "IPv6/6to4/APNIC" },
	{ STATS_IPV6_6TO4_ARIN	, 0, "IPv6/6to4/ARIN" },
	{ STATS_IPV6_6TO4_RIPE	, 0, "IPv6/6to4/RIPE" },
	{ STATS_IPV6_6TO4_LACNIC, 0, "IPv6/6to4/LACNIC" },
	{ STATS_IPV6_6TO4_RESERVED, 0, "IPv6/6to4/RESERVED" },
	{ STATS_IPV6_6TO4_UNKNOWN, 0, "IPv6/6to4/UNKNOWN" }
};
