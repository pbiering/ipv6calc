/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 * Version    : $Id: libipv6calc_db_wrapper_BuiltIn.c,v 1.2 2013/08/15 16:54:36 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc BuiltIn database wrapper
 *    - decoupling databases from main binary
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "config.h"

#include "libipv6calcdebug.h"

#include "libipv6calc_db_wrapper.h"

#include "libipv6calc_db_wrapper_BuiltIn.h"

#include "libieee.h"

uint32_t wrapper_features_BuiltIn = 0;

static int builtin_asn        = 0;
static int builtin_ipv4       = 0;
static int builtin_ipv6       = 0;
static int builtin_ieee       = 0;

#ifdef SUPPORT_BUILTIN
// load all built-in databases
#include "../as-assignment/dbasn_assignment.h"
#include "../ipv4-assignment/dbipv4addr_assignment.h"
#include "../ipv6-assignment/dbipv6addr_assignment.h"
#include "../ieee-iab/dbieee_iab.h"
#include "../ieee-oui/dbieee_oui.h"
#include "../ieee-oui36/dbieee_oui36.h"
#endif


/*
 * function initialise the BuiltIn wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_BuiltIn_wrapper_init(void) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_BUILTIN
	wrapper_features_BuiltIn |= IPV6CALC_DB_AS_TO_REGISTRY;
	wrapper_features_BuiltIn |= IPV6CALC_DB_IPV4_TO_REGISTRY;
	wrapper_features_BuiltIn |= IPV6CALC_DB_IPV6_TO_REGISTRY;
	wrapper_features_BuiltIn |= IPV6CALC_DB_IEEE_TO_INFO;

	wrapper_features |= wrapper_features_BuiltIn;

	builtin_asn        = 1;
	builtin_ipv4       = 1;
	builtin_ipv6       = 1;
	builtin_ieee       = 1;
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return 0;
};


/*
 * function cleanup the BuiltIn wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_BuiltIn_wrapper_cleanup(void) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return 0;
};


/*
 * function info of BuiltIn wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_BuiltIn_wrapper_info(char* string, const size_t size) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_BUILTIN
	snprintf(string, size, "BuiltIn databases available: ASN=%d IPv4=%d IPv6=%d IEEE=%d", builtin_asn, builtin_ipv4, builtin_ipv6, builtin_ieee);
#else
	snprintf(string, size, "No BuiltIn databases support compiled-in");
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return;
};


/*
 * function print database info of BuiltIn wrapper
 *
 * in : (void)
 * out: (void)
 */
void libipv6calc_db_wrapper_BuiltIn_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	const char *prefix = "\0";
	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	printf("%sBuiltIn: features: 0x%08x\n", prefix, wrapper_features_BuiltIn);

#ifdef SUPPORT_BUILTIN
	printf("%sBuiltIn: info of available databases\n", prefix);

	if (wrapper_features_BuiltIn & IPV6CALC_DB_AS_TO_REGISTRY) {
		printf("%sBuiltIn: %-5s: %s\n", prefix, "ASN", dbasn_registry_status);
	};

	if (wrapper_features_BuiltIn & IPV6CALC_DB_IPV4_TO_REGISTRY) {
		printf("%sBuiltIn: %-5s: %s\n", prefix, "IPv4", dbipv4addr_registry_status);
	};

	if (wrapper_features_BuiltIn & IPV6CALC_DB_IPV6_TO_REGISTRY) {
		printf("%sBuiltIn: %-5s: %s\n", prefix, "IPv6", dbipv6addr_registry_status);
	};

	if (wrapper_features_BuiltIn & IPV6CALC_DB_IEEE_TO_INFO) {
		printf("%sBuiltIn: %-5s: %s %s %s\n", prefix, "IEEE", libieee_iab_status, libieee_oui_status, libieee_oui36_status);
	};
#else
	snprintf(string, size, "%sNo BuiltIn support compiled-in", prefix);
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished\n", __FILE__, __func__);
	};
	return;
};


#ifdef SUPPORT_BUILTIN

/*******************************
 * Wrapper functions for BuiltIn
 *******************************/

// get registry number by AS number
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_as_num32(const uint32_t as_num32) {
	int i = -1, i_new, i_old, r = -1;
	int max = sizeof(dbasn_assignment) / sizeof(dbasn_assignment[0]);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called with as_num32=%d max=%d\n", __FILE__, __func__, as_num32, max);
	};

	// binary search
	i_new = max / 2;
	i_old = 0;

	while (i != i_new) {
		i_old = i;
		i = i_new;

		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Search for as_num32=%d max=%d i=%d start=%d stop=%d\n", __FILE__, __func__, as_num32, max, i, dbasn_assignment[i].asn_start, dbasn_assignment[i].asn_stop);
		};

		if (as_num32 < dbasn_assignment[i].asn_start) {
			// to high, jump down
			i_new = i - abs(i_old - i) / 2;
		} else if (as_num32 > dbasn_assignment[i].asn_stop) {
			// to low, jump up
			i_new = i + abs(i_old - i) / 2;
		} else {
			// hit
			r = i;
			break;
		};
	};

	if (r != -1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Finished with success result: %d\n", __FILE__, __func__, dbasn_assignment[r].registry);
		};

		return(dbasn_assignment[r].registry);
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Finished without success\n", __FILE__, __func__);
		};

		return(REGISTRY_UNKNOWN);
	};
};

#endif
