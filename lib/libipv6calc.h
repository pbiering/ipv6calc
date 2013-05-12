/*
 * Project    : ipv6calc
 * File       : libipv6calc.h
 * Version    : $Id: libipv6calc.h,v 1.12 2013/05/12 07:34:04 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc.c
 */ 

#include "ipv6calc_inttypes.h"
#include "ipv6calctypes.h"
#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"


/* typedefs */

#ifndef _libipv6calc_h

#define _libipv6calc_h 1

/* master filter structure */
typedef struct {
	s_ipv6calc_filter_ipv4addr filter_ipv4addr;
	s_ipv6calc_filter_ipv6addr filter_ipv6addr;
	s_ipv6calc_filter_macaddr  filter_macaddr;
} s_ipv6calc_filter_master;


/*@unused@*/ static const s_ipv6calc_anon_set ipv6calc_anon_set_list[] = {
	// name                 short ip4 ip6 iid mac method
	{ "anonymize-standard", "as", 24, 56, 40, 24, 1 },
	{ "anonymize-careful" , "ac", 20, 48, 24, 24, 1 },
	{ "anonymize-paranoid", "ap", 16, 40,  0, 24, 1 },
	{ "zeroize-standard"  , "zs", 24, 56, 40, 24, 2 },
	{ "zeroize-careful"   , "zc", 20, 48, 24, 24, 2 },
	{ "zeroize-paranoid"  , "zp", 16, 40,  0, 24, 2 }
};

/*@unused@*/ static const s_ipv6calc_anon_methods ipv6calc_anon_methods[] = {
	{ "anonymize", 1, "reliable anonymization, keep as much type information as possible" },
	{ "zeroise"  , 2, "simple zero'ising according to given masks, probably loose type information" },
};

#endif


/* prototypes */
extern void string_to_upcase(char *string);
extern void string_to_lowcase(char *string);

extern void string_to_reverse(char *string);
extern void string_to_reverse_dotted(char *string);

extern uint32_t libipv6calc_autodetectinput(const char *string);

int  libipv6calc_filter_parse(const char *expression, s_ipv6calc_filter_master *filter_master);
void libipv6calc_filter_clear(s_ipv6calc_filter_master *filter_master);

int  libipv6calc_anon_set_by_name(s_ipv6calc_anon_set *ipv6calc_anon_set, const char* name);
void libipv6calc_anon_infostring(char* string, const int stringlength, const s_ipv6calc_anon_set *ipv6calc_anon_set);
