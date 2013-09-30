/*
 * Project    : ipv6calc/lib
 * File       : libipv6calcdebug.h
 * Version    : $Id: libipv6calcdebug.h,v 1.9 2013/09/30 21:26:41 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Debug information
 */ 


#ifndef _libipv6calcdebug_h_

#define _libipv6calcdebug_h_

#define DEBUG_ipv6calc_ALL				-1

//reserved for binaries					0x00000002l
//reserved for binaries					0x00000004l

#define DEBUG_ipv6calcoptions				0x00000008l

#define DEBUG_libipv6calc				0x00000010l
#define DEBUG_libipv6addr				0x00000020l
#define DEBUG_libipv4addr				0x00000040l
#define DEBUG_libipv6calctypes				0x00000080l

#define DEBUG_librfc3041				0x00000100l
#define DEBUG_librfc1884				0x00000200l
#define DEBUG_librfc2874				0x00000400l
#define DEBUG_librfc1886				0x00000800l
#define DEBUG_librfc3056				0x00001000l
#define DEBUG_libifinet6				0x00002000l

#define DEBUG_libipv6addr_iidrandomdetection		0x00010000l

#define DEBUG_libipv6addr_db_wrapper			0x00100000l
#define DEBUG_libipv6addr_db_wrapper_GeoIP		0x00200000l
#define DEBUG_libipv6addr_db_wrapper_IP2Location	0x00400000l
#define DEBUG_libipv6addr_db_wrapper_GeoIP_verbose	0x00800000l

/* print debug macros (old version) */
#define DEBUGPRINT(d, ...)	if ((ipv6calc_debug & d) != 0) { fprintf(stderr,  __VA_ARGS__); };

/* debug print with args */
#define DEBUGPRINT_WA(d, t, ...)	\
	if ((ipv6calc_debug & d) != 0) { \
		if (ipv6calc_debug == DEBUG_ipv6calc_ALL) { \
			fprintf(stderr, "%s/%s[0x%08lx]: " t "\n", __FILE__, __func__, d, __VA_ARGS__); \
		} else { \
			fprintf(stderr, "%s/%s: " t "\n", __FILE__, __func__, __VA_ARGS__); \
		}; \
	};

/* debug print no args */
#define DEBUGPRINT_NA(d, t)		\
	if ((ipv6calc_debug & d) != 0) { \
		if (ipv6calc_debug == DEBUG_ipv6calc_ALL) { \
			fprintf(stderr, "%s/%s[0x%08lx]: " t "\n", __FILE__, __func__, d); \
		} else { \
			fprintf(stderr, "%s/%s: " t "\n", __FILE__, __func__); \
		}; \
	};


#endif

/* global debug value */
extern long int ipv6calc_debug;
