/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 * Version    : $Id: libipv6calc_db_wrapper_BuiltIn.c,v 1.15 2014/07/21 06:14:27 ds6peter Exp $
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc BuiltIn database wrapper
 *    - decoupling databases from main binary
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

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

#ifdef SUPPORT_DB_IPV4_REG
#include "../ipv4-assignment/dbipv4addr_assignment.h"
#endif

#ifdef SUPPORT_DB_IPV6_REG
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
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_BUILTIN
	wrapper_features_BuiltIn |= IPV6CALC_DB_AS_TO_REGISTRY;
	builtin_asn        = 1;

	wrapper_features_BuiltIn |= IPV6CALC_DB_CC_TO_REGISTRY;
	builtin_cc_reg     = 1;

#ifdef SUPPORT_DB_IPV4_REG
	wrapper_features_BuiltIn |= IPV6CALC_DB_IPV4_TO_REGISTRY;
	builtin_ipv4       = 1;
#endif

#ifdef SUPPORT_DB_IPV6_REG
	wrapper_features_BuiltIn |= IPV6CALC_DB_IPV6_TO_REGISTRY;
	builtin_ipv6       = 1;
#endif

#ifdef SUPPORT_DB_IEEE
	wrapper_features_BuiltIn |= IPV6CALC_DB_IEEE_TO_INFO;
	builtin_ieee       = 1;
#endif

	wrapper_features |= wrapper_features_BuiltIn;

#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
	return 0;
};


/*
 * function cleanup the BuiltIn wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_BuiltIn_wrapper_cleanup(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	// currently nothing to do

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
	return 0;
};


/*
 * function info of BuiltIn wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_BuiltIn_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_BUILTIN
	snprintf(string, size, "BuiltIn databases available: ASN_REG=%d IPV4_REG=%d IPV6_REG=%d IEEE=%d CC_REG=%d", builtin_asn, builtin_ipv4, builtin_ipv6, builtin_ieee, builtin_cc_reg);
#else
	snprintf(string, size, "No BuiltIn databases support compiled-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
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

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

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

#ifdef SUPPORT_DB_IPV4_REG
	if (wrapper_features_BuiltIn & IPV6CALC_DB_IPV4_TO_REGISTRY) {
		printf("%sBuiltIn: %-5s: %s\n", prefix, "IPv4", dbipv4addr_registry_status);
	};
#endif

#ifdef SUPPORT_DB_IPV6_REG
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

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished");
	return;
};


#ifdef SUPPORT_BUILTIN

/*******************************
 * Wrapper functions for BuiltIn
 *******************************/

// get registry number by AS number
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_as_num32(const uint32_t as_num32) {

	int max = MAXENTRIES_ARRAY(dbasn_assignment);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with as_num32=%d max=%d", as_num32, max);

	// binary search
	int i, r = -1;
	int i_min = 0;
	int i_max = max;
	int i_old = -1;

	i = max / 2;
	while (i_old != i) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Search for as_num32=%d max=%d i=%d start=%d stop=%d", as_num32, max, i, dbasn_assignment[i].asn_start, dbasn_assignment[i].asn_stop);

		if (as_num32 < dbasn_assignment[i].asn_start) {
			// to high in array, jump down
			i_max = i;
		} else if (as_num32 > dbasn_assignment[i].asn_stop) {
			// to low in array, jump up
			i_min = i;
		} else {
			// hit
			r = i;
			break;
		};

		i_old = i;
		i = (i_max - i_min) / 2 + i_min;
	};

	if (r != -1) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished with success result: %d", dbasn_assignment[r].registry);

		return(dbasn_assignment[r].registry);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished without success");

		return(REGISTRY_UNKNOWN);
	};
};


// get registry number by CC index
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_cc_index(const uint16_t cc_index) {
	int result = REGISTRY_UNKNOWN;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with cc_index=%d", cc_index);

	if (cc_index > COUNTRYCODE_INDEX_MAX) {
		goto END_libipv6calc_db_wrapper;
	} else if (cc_index > MAXENTRIES_ARRAY(cc_index_reg_assignment)) {
		goto END_libipv6calc_db_wrapper;
	} else if (cc_index == COUNTRYCODE_INDEX_UNKNOWN) {
		result = REGISTRY_IANA;
		goto END_libipv6calc_db_wrapper;
	} else if ((cc_index >= COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN) && (cc_index <= COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MAX)) {
		result = cc_index - COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN;
		goto END_libipv6calc_db_wrapper;
	};

	result = cc_index_reg_assignment[cc_index].registry;

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Return registry=%s (%d) (cc_index=%d)", libipv6calc_registry_string_by_num(result), result, cc_index);

	return(result);
};


/*
 * Get IEEE vendor string
 * in:  macaddrp
 * mod: resultstring
 * out: 0=found, 1=not found
 */
int libipv6calc_db_wrapper_BuiltIn_ieee_vendor_string_by_macaddr(char *resultstring, const size_t resultstring_length, const ipv6calc_macaddr *macaddrp) {
	int retval = 1;

#ifdef SUPPORT_DB_IEEE
	int i;
	uint32_t idval, subidval;
#endif

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	/* catch special ones */
	if ((macaddrp->addr[0] == 0xfc && macaddrp->addr[1] == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		snprintf(resultstring, resultstring_length, "Linux ISDN-NET/PLIP");
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
				snprintf(resultstring, resultstring_length, "%s", libieee_iab[i].string_owner);
				return (0);
			};
		};
	};

	/* run through OUI36 list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui36); i++) {
		if (libieee_oui36[i].id == idval) {
			/* major id match */
			if (libieee_oui36[i].subid_begin <= subidval && libieee_oui36[i].subid_end >= subidval) {
				snprintf(resultstring, resultstring_length, "%s", libieee_oui36[i].string_owner);
				return (0);
			};
		};
	};

	/* run through OUI list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			snprintf(resultstring, resultstring_length, "%s", libieee_oui[i].string_owner);
			return (0);
		};
	};
#else
	snprintf(resultstring, resultstring_length, "(IEEE databases not compiled in)");
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
int libipv6calc_db_wrapper_BuiltIn_ieee_vendor_string_short_by_macaddr(char *resultstring, const size_t resultstring_length, const ipv6calc_macaddr *macaddrp) {
	int retval = 1;

#ifdef SUPPORT_DB_IEEE
	int i;
	uint32_t idval, subidval;
#endif

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	/* catch special ones */
	if ((macaddrp->addr[0] == 0xfc && macaddrp->addr[1] == 0xfc)) {
		/* Linux special OUI for ISDN-NET or PLIP interfaces */
		snprintf(resultstring, resultstring_length, "Linux-ISDN-NET+PLIP");
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
				snprintf(resultstring, resultstring_length, "%s", libieee_iab[i].shortstring_owner);
				return (0);
			};
		};
	};

	/* run through OUI36 list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui36); i++) {
		if (libieee_oui36[i].id == idval) {
			/* major id match */
			if (libieee_oui36[i].subid_begin <= subidval && libieee_oui36[i].subid_end >= subidval) {
				snprintf(resultstring, resultstring_length, "%s", libieee_oui36[i].shortstring_owner);
				return (0);
			};
		};
	};

	/* run through OUI list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			snprintf(resultstring, resultstring_length, "%s", libieee_oui[i].shortstring_owner);
			return (0);
		};
	};
#else
	snprintf(resultstring, resultstring_length, "(IEEE databases not compiled in)");
	return (0);
#endif

	/* not found */
   	retval = 1;	
	return (retval);
};


/*
 * Get reserved IPv4 address information as string
 * ret: NULL: not reserved, !=NULL: pointer to string
 */
static const char *libipv6calc_db_wrapper_BuiltIn_reserved_string_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp) {
	const char *info = NULL;

	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address: %08x", (unsigned int) ipv4);

	if ((ipv4 & 0xff000000u) == 0x00000000u) {
		// 0.0.0.0/8 (RFC 1122)
		info = "reserved(RFC1122#3.2.1.3)";
	} else if ((ipv4 & 0xff000000u) == 0x0a000000u) {
		// 10.0.0.0/8 (RFC 1918)
		info = "reserved(RFC1918#3)";
	} else if ((ipv4 & 0xffc00000u) == 0x64400000u) {
		// 100.64.0.0/10 (RFC 6598)
		info = "reserved(RFC6598)";
	} else if ((ipv4 & 0xff000000u) == 0x7f000000u) {
		// 127.0.0.0/8 (RFC 1122)
		info = "reserved(RFC1122#3.2.1.3)";
	} else if ((ipv4 & 0xffff0000u) == 0xa9fe0000u) {
		// 169.254.0.0/16 (RFC 1918)
		info = "reserved(RFC3927#1)";
	} else if ((ipv4 & 0xfff00000u) == 0xac100000u) {
		// 172.16.0.0/12 (RFC 1918)
		info = "reserved(RFC1918#3)";
	} else if ((ipv4 & 0xffff0000u) == 0xc0a80000u) {
		// 192.168.0.0/16 (RFC 1918)
		info = "reserved(RFC1918#3)";
	} else if ((ipv4 & 0xffffff00u) == 0xc0000000u) {
		// 192.0.0.0/24 (RFC 5736)
		info = "reserved(RFC5736#1)";
	} else if ((ipv4 & 0xffffff00u) == 0xc0000200u) {
		// 192.0.2.0/24 (RFC 3330)
		info = "reserved(RFC5737#1)";
	} else if ((ipv4 & 0xffffff00u) == 0xc0586300u) {
		// 192.88.99.0/24 (RFC 3068)
		info = "reserved(RFC3068#2.3)";
	} else if ((ipv4 & 0xfffe0000u) == 0xc6120000u) {
		// 198.18.0.0/15 (RFC 2544)
		info = "reserved(RFC2544#C.2.2)";
	} else if ((ipv4 & 0xffffff00u) == 0xc6336400u) {
		// 198.51.100.0/24 (RFC 5737)
		info = "reserved(RFC5737#3)";
	} else if ((ipv4 & 0xffffff00u) == 0xcb007100u) {
		// 203.0.113.0/24 (RFC 5737)
		info = "reserved(RFC5737#3)";
	} else if ((ipv4 & 0xf0000000u) == 0xe0000000u) {
		// 224.0.0.0/4 (RFC 3171)
		info = "reserved(RFC3171#2)";
	} else if ((ipv4 & 0xffffffffu) == 0xffffffffu) {
		// 255.255.255.255/32
		info = "reserved(RFC919#7)";
	} else if ((ipv4 & 0xf0000000u) == 0xf0000000u) {
		// 240.0.0.0/4 (RFC 1112)
		info = "reserved(RFC1112#4)";
	}; 

	if (info == NULL) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address is not reserved: %08x", (unsigned int) ipv4);
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address is reserved: %08x (%s)", (unsigned int) ipv4, info);
	};

	return(info);
};


/*
 * get registry string of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = Registry string
 * ret: 0: ok, 1: unknown, 2: reserved
 */
int libipv6calc_db_wrapper_BuiltIn_registry_string_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length) {
	const char *info = libipv6calc_db_wrapper_BuiltIn_reserved_string_by_ipv4addr(ipv4addrp);

	if (info != NULL) {
		// is reserved
		snprintf(resultstring, resultstring_length, "%s", info);
		return (2);
	};
	
#ifdef SUPPORT_DB_IPV4_REG
	int registry = libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv4addr(ipv4addrp);

	snprintf(resultstring, resultstring_length, "%s", libipv6calc_registry_string_by_num(registry));
	return(0);
#else
	snprintf(resultstring, resultstring_length, "%s", "(IPv4 database not compiled in)");
	return(1);
#endif
};


/*
 * get registry number of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp) {
	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address: %08x", (unsigned int) ipv4);

	int result = IPV4_ADDR_REGISTRY_UNKNOWN;

	const char *info = libipv6calc_db_wrapper_BuiltIn_reserved_string_by_ipv4addr(ipv4addrp);

	if (info != NULL) {
		return(IPV4_ADDR_REGISTRY_RESERVED);
	};

#ifdef SUPPORT_DB_IPV4_REG
	int i = -1;
	int match = -1;
	int i_min, i_max, i_old, max;

	max = MAXENTRIES_ARRAY(dbipv4addr_assignment);

	i_min = 0; i_max = max; i_old = -1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with ipv4=%08x max=%d", ipv4, max);

	// binary search in dbipv4addr_assignment
	i = max / 2;
	while (i_old != i) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Search in dbipv4addr_assignment for ipv4=%08x first=%08x last=%08x i=%d i_min=%d i_max=%d", ipv4, (unsigned int) dbipv4addr_assignment[i].first, (unsigned int) dbipv4addr_assignment[i].last, i, i_min, i_max);

		if (ipv4 < dbipv4addr_assignment[i].first) {
			// to high in array, jump down
			i_max = i;
		} else if (ipv4 > dbipv4addr_assignment[i].last) {
			// to low in array, jump up
			i_min = i;
		} else {
			// hit
			match = i;
			break;
		};

		i_old = i;
		i = (i_max - i_min) / 2 + i_min;
	};

	if (match != -1) {
		result = dbipv4addr_assignment[match].registry;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished with success result (dbipv4addr_assignment): match=%d reg=%d", match, result);
	};

	if (result == IPV4_ADDR_REGISTRY_UNKNOWN) {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Nothing found in dbipv4addr_assignment, fallback now to dbipv4addr_assignment_iana");

		max = MAXENTRIES_ARRAY(dbipv4addr_assignment_iana);

		i_min = 0; i_max = max; i_old = -1;

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with ipv4=%08x max=%d", ipv4, max);

		// binary search in dbipv4addr_assignment_iana (fallback)
		i = max / 2;
		while (i_old != i) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Search in dbipv4addr_assignment_iana for ipv4=%08x first=%08x last=%08x i=%d i_min=%d i_max=%d", ipv4, (unsigned int) dbipv4addr_assignment_iana[i].first, (unsigned int) dbipv4addr_assignment_iana[i].last, i, i_min, i_max);

			if (ipv4 < dbipv4addr_assignment_iana[i].first) {
				// to high in array, jump down
				i_max = i;
			} else if (ipv4 > dbipv4addr_assignment_iana[i].last) {
				// to low in array, jump up
				i_min = i;
			} else {
				// hit
				match = i;
				break;
			};

			i_old = i;
			i = (i_max - i_min) / 2 + i_min;
		};

		if (match != -1) {
			result = dbipv4addr_assignment_iana[match].registry;
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished with success result (dbipv4addr_assignment_iana): match=%d reg=%d", match, result);
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished without success");
		};
	};

#endif // SUPPORT_DB_IPV4_REG
	return(result);
};


/*
 * Get reserved IPv6 address information as string
 * ret: NULL: not reserved, !=NULL: pointer to string
 */
static const char *libipv6calc_db_wrapper_BuiltIn_reserved_string_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp) {
	const char *info = NULL;

	uint32_t ipv6_00_31 = ipv6addr_getdword(ipv6addrp, 0);
	uint32_t ipv6_32_63 = ipv6addr_getdword(ipv6addrp, 1);
	uint32_t ipv6_64_95 = ipv6addr_getdword(ipv6addrp, 2);
	uint32_t ipv6_96_127 = ipv6addr_getdword(ipv6addrp, 3);
	
	uint16_t ipv6_00_15 = ipv6addr_getword(ipv6addrp, 0);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given ipv6 prefix: %08x%08x", (unsigned int) ipv6_00_31, (unsigned int) ipv6_32_63);

	if ((ipv6_00_31 == 0) && (ipv6_32_63 == 0) && (ipv6_64_95 == 0) && (ipv6_96_127 == 0)) {
		// :: (RFC 4291)
		info = "reserved(RFC4291#2.5.2)";
	} else if ((ipv6_00_31 == 0) && (ipv6_32_63 == 0) && (ipv6_64_95 == 0) && (ipv6_96_127 == 1)) {
		// ::1 (RFC 4291)
		info = "reserved(RFC4291#2.5.3)";
	} else if ((ipv6_00_31 == 0) && (ipv6_32_63 == 0) && (ipv6_64_95 == 0)) {
		// ::x.x.x.x (RFC 4291)
		info = "reserved(RFC4291#2.5.5.1)";
	} else if ((ipv6_00_31 == 0) && (ipv6_32_63 == 0) && (ipv6_64_95 == 0x0000ffff)) {
		// ::ffff:x.x.x.x (RFC 4291)
		info = "reserved(RFC4291#2.5.5.2)";
	} else if (ipv6_00_31 == 0x20010000) {
		// 2001:0000::/32 (RFC 4380)
		info = "reserved(RFC4380#6)";
	} else if ((ipv6_00_31 & 0xfffffff0) == 0x20010010) {
		// 2001:0010::/28 (RFC 4843)
		info = "reserved(RFC4843#2)";
	} else if (ipv6_00_31 == 0x20010db8) {
		// 2001:0db8::/32 (RFC 3849)
		info = "reserved(RFC3849#4)";
	} else if ((ipv6_00_15 & 0xffff) == 0x2002) {
		// 2002::/16 (RFC 3056)
		info = "reserved(RFC3056#2)";
	} else if ((ipv6_00_15 & 0xfe00) == 0xfc00) {
		// fc00::/7 (RFC 4193)
		info = "reserved(RFC4193#3.1)";
	} else if ((ipv6_00_15 & 0xffe0) == 0xfe80) {
		// fe80::/10 (RFC 4291)
		info = "reserved(RFC4291#2.5.6)";
	} else if ((ipv6_00_15 & 0xffe0) == 0xfec0) {
		// fec0::/10 (RFC 4291)
		info = "reserved(RFC4291#2.5.7)";
	} else if ((ipv6_00_15 & 0xff00) == 0xff00) {
		// ffxx::/8 (RFC 4291)
		info = "reserved(RFC4291#2.7)";
	};

	if (info == NULL) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv6 address is not reserved: %08x%08x%08x%08x", (unsigned int) ipv6_00_31, (unsigned int) ipv6_32_63, (unsigned int) ipv6_64_95, (unsigned int) ipv6_96_127);
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv6 address is reserved: %08x%08x%08x%08x (%s)", (unsigned int) ipv6_00_31, (unsigned int) ipv6_32_63, (unsigned int) ipv6_64_95, (unsigned int) ipv6_96_127, info);
	};

	return(info);
};


/*
 * Get IPv6 address assignement information as string
 *
 * in : ipv6addrp = pointer to IPv6 address structure
 * mod: resultstring
 * ret: 0: ok, 1: unknown, 2: reserved
 */
int libipv6calc_db_wrapper_BuiltIn_registry_string_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length) {
	const char *info = libipv6calc_db_wrapper_BuiltIn_reserved_string_by_ipv6addr(ipv6addrp);

	if (info != NULL) {
		// is reserved
		snprintf(resultstring, resultstring_length, "%s", info);
		return (2);
	};

#ifdef SUPPORT_DB_IPV6_REG
	int registry = libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv6addr(ipv6addrp);

	snprintf(resultstring, resultstring_length, "%s", libipv6calc_registry_string_by_num(registry));
	return(0);
#else
	snprintf(resultstring, resultstring_length, "%s", "(IPv6 database not compiled in)");
	return(1);
#endif
};



/*
 * get registry number of an IPv6 address
 *
 * in:  ipv6addr = IPv6 address structure
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t ipv6_00_31 = ipv6addr_getdword(ipv6addrp, 0);
	uint32_t ipv6_32_63 = ipv6addr_getdword(ipv6addrp, 1);
	
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given ipv6 prefix: %08x%08x", (unsigned int) ipv6_00_31, (unsigned int) ipv6_32_63);

	if ((ipv6addrp->scope & IPV6_NEW_ADDR_6BONE) != 0) {
		return(IPV6_ADDR_REGISTRY_6BONE);
	};

	int result = IPV6_ADDR_REGISTRY_UNKNOWN;

	const char *info = libipv6calc_db_wrapper_BuiltIn_reserved_string_by_ipv6addr(ipv6addrp);

	if (info != NULL) {
		return(REGISTRY_RESERVED);
	};

#ifdef SUPPORT_DB_IPV6_REG
	int match = -1;
	int i;

	for (i = 0; i < MAXENTRIES_ARRAY(dbipv6addr_assignment); i++) {
		/* run through database array */
		if ( (ipv6_00_31 & dbipv6addr_assignment[i].ipv6mask_00_31) != dbipv6addr_assignment[i].ipv6addr_00_31 ) {
			/* MSB 00-31 do not match */
			continue;
		};

		if ( dbipv6addr_assignment[i].ipv6mask_32_63 != 0 ) {
			if ( (ipv6_32_63 & dbipv6addr_assignment[i].ipv6mask_32_63) != dbipv6addr_assignment[i].ipv6addr_32_63 ) {
				/* MSB 32-63 do not match */
				continue;
			};
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Found match: prefix=%08x%08x mask=%08x%08x  registry=%s (entry: %d)", \
			(unsigned int) dbipv6addr_assignment[i].ipv6addr_00_31, \
			(unsigned int) dbipv6addr_assignment[i].ipv6addr_32_63, \
			(unsigned int) dbipv6addr_assignment[i].ipv6mask_00_31, \
			(unsigned int) dbipv6addr_assignment[i].ipv6mask_32_63, \
			libipv6calc_registry_string_by_num(dbipv6addr_assignment[i].registry), i);
		match = i;
	};

	/* result */
	if ( match > -1 ) {
		result = dbipv6addr_assignment[match].registry;
	};
#endif
	return(result);
};



#endif		// SUPPORT_BUILTIN
