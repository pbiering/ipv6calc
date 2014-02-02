/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 * Version    : $Id: libipv6calc_db_wrapper_BuiltIn.c,v 1.6 2014/02/02 09:20:49 ds6peter Exp $
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
static int builtin_cc_reg     = 0;
static int builtin_ipv4       = 0;
static int builtin_ipv6       = 0;
static int builtin_ieee       = 0;

#ifdef SUPPORT_BUILTIN
// load all built-in databases
#include "../as-assignment/dbasn_assignment.h"
#include "../cc-assignment/db_cc_reg_assignment.h"

#ifdef SUPPORT_DB_IPV4
#include "../ipv4-assignment/dbipv4addr_assignment.h"
#endif

#ifdef SUPPORT_DB_IPV6
#include "../ipv6-assignment/dbipv6addr_assignment.h"
#endif

#ifdef SUPPORT_DB_IEEE
#include "../ieee-iab/dbieee_iab.h"
#include "../ieee-oui/dbieee_oui.h"
#include "../ieee-oui36/dbieee_oui36.h"
#endif

#endif


/*
 * function initialise the BuiltIn wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_BuiltIn_wrapper_init(void) {
	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Called");

#ifdef SUPPORT_BUILTIN
	wrapper_features_BuiltIn |= IPV6CALC_DB_AS_TO_REGISTRY;
	builtin_asn        = 1;

	wrapper_features_BuiltIn |= IPV6CALC_DB_CC_TO_REGISTRY;
	builtin_cc_reg     = 1;

#ifdef SUPPORT_DB_IPV4
	wrapper_features_BuiltIn |= IPV6CALC_DB_IPV4_TO_REGISTRY;
	builtin_ipv4       = 1;
#endif

#ifdef SUPPORT_DB_IPV6
	wrapper_features_BuiltIn |= IPV6CALC_DB_IPV6_TO_REGISTRY;
	builtin_ipv6       = 1;
#endif

#ifdef SUPPORT_DB_IEEE
	wrapper_features_BuiltIn |= IPV6CALC_DB_IEEE_TO_INFO;
	builtin_ieee       = 1;
#endif

	wrapper_features |= wrapper_features_BuiltIn;

#endif

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Finished");
	return 0;
};


/*
 * function cleanup the BuiltIn wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_BuiltIn_wrapper_cleanup(void) {
	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Called");

	// currently nothing to do

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Finished");
	return 0;
};


/*
 * function info of BuiltIn wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_BuiltIn_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Called");

#ifdef SUPPORT_BUILTIN
	snprintf(string, size, "BuiltIn databases available: ASN=%d IPv4=%d IPv6=%d IEEE=%d CC_REG=%d", builtin_asn, builtin_ipv4, builtin_ipv6, builtin_ieee, builtin_cc_reg);
#else
	snprintf(string, size, "No BuiltIn databases support compiled-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Finished");
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

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Called");

	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	printf("%sBuiltIn: features: 0x%08x\n", prefix, wrapper_features_BuiltIn);

#ifdef SUPPORT_BUILTIN
	printf("%sBuiltIn: info of available databases\n", prefix);

	if (wrapper_features_BuiltIn & IPV6CALC_DB_AS_TO_REGISTRY) {
		printf("%sBuiltIn: %-5s: %s\n", prefix, "ASN", dbasn_registry_status);
	};

	if (wrapper_features_BuiltIn & IPV6CALC_DB_CC_TO_REGISTRY) {
		printf("%sBuiltIn: %-5s: %s\n", prefix, "CC", db_cc_registry_status);
	};

#ifdef SUPPORT_DB_IPV4
	if (wrapper_features_BuiltIn & IPV6CALC_DB_IPV4_TO_REGISTRY) {
		printf("%sBuiltIn: %-5s: %s\n", prefix, "IPv4", dbipv4addr_registry_status);
	};
#endif

#ifdef SUPPORT_DB_IPV6
	if (wrapper_features_BuiltIn & IPV6CALC_DB_IPV6_TO_REGISTRY) {
		printf("%sBuiltIn: %-5s: %s\n", prefix, "IPv6", dbipv6addr_registry_status);
	};
#endif

#ifdef SUPPORT_DB_IEEE
	if (wrapper_features_BuiltIn & IPV6CALC_DB_IEEE_TO_INFO) {
		printf("%sBuiltIn: %-5s: %s %s %s\n", prefix, "IEEE", libieee_iab_status, libieee_oui_status, libieee_oui36_status);
	};
#endif

#else
	snprintf(string, size, "%sNo BuiltIn support compiled-in", prefix);
#endif

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Finished");
	return;
};


#ifdef SUPPORT_BUILTIN

/*******************************
 * Wrapper functions for BuiltIn
 *******************************/

// get registry number by AS number
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_as_num32(const uint32_t as_num32) {
	int i = -1, i_new, i_old, r = -1;

	int max = MAXENTRIES_ARRAY(dbasn_assignment);

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "Called with as_num32=%d max=%d", as_num32, max);

	// binary search
	i_new = max / 2;
	i_old = 0;

	while (i != i_new) {
		i_old = i;
		i = i_new;

		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "Search for as_num32=%d max=%d i=%d start=%d stop=%d", as_num32, max, i, dbasn_assignment[i].asn_start, dbasn_assignment[i].asn_stop);

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
		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "Finished with success result: %d", dbasn_assignment[r].registry);

		return(dbasn_assignment[r].registry);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Finished without success");

		return(REGISTRY_UNKNOWN);
	};
};


// get registry number by CC index
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_cc_index(const uint16_t cc_index) {
	int result = REGISTRY_UNKNOWN;

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "Called with cc_index=%d", cc_index);

	if (cc_index > COUNTRYCODE_INDEX_MAX) {
		goto END_libipv6calc_db_wrapper;
	};

	if (cc_index > MAXENTRIES_ARRAY(cc_index_reg_assignment)) {
		goto END_libipv6calc_db_wrapper;
	};

	if (cc_index == COUNTRYCODE_INDEX_UNKNOWN) {
		result = REGISTRY_IANA;
		goto END_libipv6calc_db_wrapper;
	};

	result = cc_index_reg_assignment[cc_index].registry;

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "Return registry=%s (%d) (cc_index=%d)", libipv6calc_registry_string_by_num(result), result, cc_index);

	return(result);
};


/*
 * Get IEEE vendor string
 * in:  macaddrp
 * mod: resultstring
 * out: 0=found, 1=not found
 */
int libipv6calc_db_wrapper_BuiltIn_ieee_vendor_string_by_macaddr(char *resultstring, const ipv6calc_macaddr *macaddrp) {
	int retval = 1;

#ifdef SUPPORT_DB_IEEE
	int i;
	uint32_t idval, subidval;
#endif

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	/* catch special ones */
	if ((macaddrp->addr[0] == 0xfc && macaddrp->addr[1] == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		snprintf(resultstring, NI_MAXHOST - 1, "Linux ISDN-NET/PLIP");
		return (0);
	};

	if ( (macaddrp->addr[0] & 0x01) != 0 ) {
		/* Multicast */
		return (1);
	};

#ifdef SUPPORT_DB_IEEE
	idval = (macaddrp->addr[0] << 16) | (macaddrp->addr[1] << 8) | macaddrp->addr[2];
	subidval = (macaddrp->addr[3] << 16) | (macaddrp->addr[4] << 8) | macaddrp->addr[5];

	/* run through IAB list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_iab); i++) {
		if (libieee_iab[i].id == idval) {
			/* major id match */
			if (libieee_iab[i].subid_begin <= subidval && libieee_iab[i].subid_end >= subidval) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_iab[i].string_owner);
				return (0);
			};
		};
	};

	/* run through OUI36 list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui36); i++) {
		if (libieee_oui36[i].id == idval) {
			/* major id match */
			if (libieee_oui36[i].subid_begin <= subidval && libieee_oui36[i].subid_end >= subidval) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_oui36[i].string_owner);
				return (0);
			};
		};
	};

	/* run through OUI list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_oui[i].string_owner);
			return (0);
		};
	};
#else
	snprintf(resultstring, NI_MAXHOST - 1, "(IEEE databases not compiled in)");
	return (0);
#endif

	/* not found */
   	retval = 1;	
	return (retval);
};


/*
 * Get short IEEE vendor string
 * in:  macaddrp
 * mod: resultstring
 * out: 0=found, 1=not found
 */
int libipv6calc_db_wrapper_BuiltIn_ieee_vendor_string_short_by_macaddr(char *resultstring, const ipv6calc_macaddr *macaddrp) {
	int retval = 1;

#ifdef SUPPORT_DB_IEEE
	int i;
	uint32_t idval, subidval;
#endif

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	/* catch special ones */
	if ((macaddrp->addr[0] == 0xfc && macaddrp->addr[1] == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		snprintf(resultstring, NI_MAXHOST - 1, "Linux-ISDN-NET+PLIP");
		return (0);
	};
	
	if ( (macaddrp->addr[0] & 0x01) != 0 ) {
		/* Multicast */
		return (1);
	};

#ifdef SUPPORT_DB_IEEE
	idval = (macaddrp->addr[0] << 16) | (macaddrp->addr[1] << 8) | macaddrp->addr[2];
	subidval = (macaddrp->addr[3] << 16) | (macaddrp->addr[4] << 8) | macaddrp->addr[5];

	/* run through IAB list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_iab); i++) {
		if (libieee_iab[i].id == idval) {
			/* major id match */
			if (libieee_iab[i].subid_begin <= subidval && libieee_iab[i].subid_end >= subidval) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_iab[i].shortstring_owner);
				return (0);
			};
		};
	};

	/* run through OUI36 list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui36); i++) {
		if (libieee_oui36[i].id == idval) {
			/* major id match */
			if (libieee_oui36[i].subid_begin <= subidval && libieee_oui36[i].subid_end >= subidval) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_oui36[i].shortstring_owner);
				return (0);
			};
		};
	};

	/* run through OUI list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			snprintf(resultstring, NI_MAXHOST - 1, "%s", libieee_oui[i].shortstring_owner);
			return (0);
		};
	};
#else
	snprintf(resultstring, NI_MAXHOST - 1, "(IEEE databases not compiled in)");
	return (0);
#endif

	/* not found */
   	retval = 1;	
	return (retval);
};

#endif
