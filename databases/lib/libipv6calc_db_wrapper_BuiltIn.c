/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 * Version    : $Id$
 * Copyright  : 2013-2017 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc BuiltIn database wrapper
 *    - decoupling databases from main binary
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <time.h>

#include "config.h"

#include "libipv6calcdebug.h"

#include "libipv6calc_db_wrapper.h"

#include "libipv6calc_db_wrapper_BuiltIn.h"

#include "libieee.h"

#ifdef SUPPORT_BUILTIN
static int builtin_asn        = 0;
static int builtin_cc_reg     = 0;
static int builtin_ipv4       = 0;
static int builtin_ipv6       = 0;
static int builtin_ieee       = 0;

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

/* database usage map */
#define BUILTIN_DB_MAX_BLOCKS_32	2	// 0-63
static uint32_t builtin_db_usage_map[BUILTIN_DB_MAX_BLOCKS_32];

#define BUILTIN_DB_USAGE_MAP_TAG(db)	if (db < (32 * BUILTIN_DB_MAX_BLOCKS_32)) { \
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Tag usage for db: %d", db); \
							builtin_db_usage_map[db / 32] |= 1 << (db % 32); \
						} else { \
							fprintf(stderr, "FIXME: unsupported db value (exceed limit): %d (%d)\n", db, 32 * BUILTIN_DB_MAX_BLOCKS_32 - 1); \
							exit(1); \
						};

char builtin_db_usage_string[NI_MAXHOST] = "";


/*
 * function initialise the BuiltIn wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_BuiltIn_wrapper_init(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called");

	// add features to implemented
	wrapper_features_by_source_implemented[IPV6CALC_DB_SOURCE_BUILTIN] |= IPV6CALC_DB_AS_TO_REGISTRY | IPV6CALC_DB_CC_TO_REGISTRY | IPV6CALC_DB_IPV4_TO_REGISTRY | IPV6CALC_DB_IPV6_TO_REGISTRY | IPV6CALC_DB_IEEE_TO_INFO | IPV6CALC_DB_IPV4_TO_INFO | IPV6CALC_DB_IPV6_TO_INFO;

#ifdef SUPPORT_BUILTIN
#ifdef SUPPORT_DB_AS_REG
	wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] |= IPV6CALC_DB_AS_TO_REGISTRY;
	builtin_asn        = 1;
#endif

#ifdef SUPPORT_DB_CC_REG
	wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] |= IPV6CALC_DB_CC_TO_REGISTRY;
	builtin_cc_reg     = 1;
#endif

#ifdef SUPPORT_DB_IPV4_REG
	wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] |= IPV6CALC_DB_IPV4_TO_REGISTRY | IPV6CALC_DB_IPV4_TO_INFO;
	builtin_ipv4       = 1;
#endif

#ifdef SUPPORT_DB_IPV6_REG
	wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] |= IPV6CALC_DB_IPV6_TO_REGISTRY | IPV6CALC_DB_IPV6_TO_INFO;
	builtin_ipv6       = 1;
#endif

#ifdef SUPPORT_DB_IEEE
	wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] |= IPV6CALC_DB_IEEE_TO_INFO;
	builtin_ieee       = 1;
#endif

	wrapper_features |= wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN];

#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished");
	return 0;
};


/*
 * function cleanup the BuiltIn wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_BuiltIn_wrapper_cleanup(void) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called");

	// currently nothing to do

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished");
	return 0;
};


/*
 * function info of BuiltIn wrapper
 *
 * in : ptr and size of string to be filled
 * out: modified string;
 */
void libipv6calc_db_wrapper_BuiltIn_wrapper_info(char* string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called");

#ifdef SUPPORT_BUILTIN
	snprintf(string, size, "BuiltIn databases available: ASN_REG=%d IPV4_REG=%d IPV6_REG=%d IEEE=%d CC_REG=%d", builtin_asn, builtin_ipv4, builtin_ipv6, builtin_ieee, builtin_cc_reg);
#else
	snprintf(string, size, "No BuiltIn databases support compiled-in");
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished");
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

	if (level_verbose == 0) { }; // make compiler happy (avoid unused "...")

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called");

	if (prefix_string != NULL) {
		prefix = prefix_string;
	};

	IPV6CALC_DB_FEATURE_INFO(prefix, IPV6CALC_DB_SOURCE_BUILTIN)

#ifdef SUPPORT_BUILTIN
#if defined SUPPORT_DB_IPV4_REG || defined SUPPORT_DB_IPV6_REG
	char tempstring[NI_MAXHOST];
#endif

	fprintf(stderr, "%sBuiltIn: info of available databases\n", prefix);

#ifdef SUPPORT_DB_AS_REG
	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] & IPV6CALC_DB_AS_TO_REGISTRY) {
		fprintf(stderr, "%sBuiltIn: %-5s: %s\n", prefix, "ASN", dbasn_registry_status);
	};
#endif

#ifdef SUPPORT_DB_CC_REG
	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] & IPV6CALC_DB_CC_TO_REGISTRY) {
		fprintf(stderr, "%sBuiltIn: %-5s: %s\n", prefix, "CC", db_cc_registry_status);
	};
#endif

#ifdef SUPPORT_DB_IPV4_REG
	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] & IPV6CALC_DB_IPV4_TO_REGISTRY) {
		strftime(tempstring, sizeof(tempstring), "%Y%m%d-%H%M%S UTC", gmtime(&dbipv4addr_registry_unixtime));
		fprintf(stderr, "%sBuiltIn: %-5s: %s (created: %s)\n", prefix, "IPv4", dbipv4addr_registry_status, tempstring);
	};
#endif

#ifdef SUPPORT_DB_IPV6_REG
	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] & IPV6CALC_DB_IPV6_TO_REGISTRY) {
		strftime(tempstring, sizeof(tempstring), "%Y%m%d-%H%M%S UTC", gmtime(&dbipv6addr_registry_unixtime));
		fprintf(stderr, "%sBuiltIn: %-5s: %s (created: %s)\n", prefix, "IPv6", dbipv6addr_registry_status, tempstring);
	};
#endif

#ifdef SUPPORT_DB_IEEE
	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] & IPV6CALC_DB_IEEE_TO_INFO) {
		fprintf(stderr, "%sBuiltIn: %-5s: %s %s %s\n", prefix, "IEEE", libieee_iab_status, libieee_oui_status, libieee_oui36_status);
	};
#endif

#else
	fprintf(stderr, "%sNo BuiltIn support compiled-in\n", prefix);
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished");
	return;
};


/*
 * wrapper: string regarding used database infos
 */
char *libipv6calc_db_wrapper_BuiltIn_wrapper_db_info_used(void) {
	int type, i;
	char tempstring[NI_MAXHOST];
	char tempstring2[NI_MAXHOST];
	char *info;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called");

	for (i = 0; i < BUILTIN_DB_MAX_BLOCKS_32; i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "builtin_db_usage_map[%d]=%08x", i, (unsigned int) builtin_db_usage_map[i]);
	};

	for (type = 0; type < 32 * BUILTIN_DB_MAX_BLOCKS_32; type++) {
		if ((builtin_db_usage_map[type / 32] & (1 << (type % 32))) != 0) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "DB type used: %d", type);

			info = NULL;

			switch(type) {
#ifdef SUPPORT_DB_IPV4_REG
			    case BUILTIN_DB_IPV4_REGISTRY:
				snprintf(tempstring2, sizeof(tempstring2), "IPv4-REG:%s", dbipv4addr_registry_status);
				info = tempstring2;
				break;
#endif
#ifdef SUPPORT_DB_IPV6_REG
			    case BUILTIN_DB_IPV6_REGISTRY:
				snprintf(tempstring2, sizeof(tempstring2), "IPv6-REG:%s", dbipv6addr_registry_status);
				info = tempstring2;
				break;
#endif
#ifdef SUPPORT_DB_IEEE
			    case BUILTIN_DB_IAB:
				snprintf(tempstring2, sizeof(tempstring2), "IEEE:%s", libieee_iab_status);
				info = tempstring2;
				break;
			    case BUILTIN_DB_OUI:
				snprintf(tempstring2, sizeof(tempstring2), "IEEE:%s", libieee_oui_status);
				info = tempstring2;
				break;
			    case BUILTIN_DB_OUI36:
				snprintf(tempstring2, sizeof(tempstring2), "IEEE:%s", libieee_oui36_status);
				info = tempstring2;
				break;
#endif
#ifdef SUPPORT_DB_AS_REG
			    case BUILTIN_DB_AS_REG:
				snprintf(tempstring2, sizeof(tempstring2), "AS-REG:%s", dbasn_registry_status);
				info = tempstring2;
				break;
#endif
#ifdef SUPPORT_DB_CC_REG
			    case BUILTIN_DB_CC_REG:
				snprintf(tempstring2, sizeof(tempstring2), "CC-REG:%s", db_cc_registry_status);
				info = tempstring2;
				break;
#endif
			};

			if (info == NULL) { continue; }; // NULL pointer returned

			if (strlen(info) == 0) { continue; }; // empty string returned

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "type=%d info=%s", type, info);

			if (strlen(builtin_db_usage_string) > 0) {
				if (strstr(builtin_db_usage_string, info) != NULL) {
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "type=%d info=%s (skip, already displayed)", type, info);
					continue;
				}; // string already included

				snprintf(tempstring, sizeof(tempstring), "%s / %s", builtin_db_usage_string, info);
			} else {
				snprintf(tempstring, sizeof(tempstring), "%s", info);
			};

			snprintf(builtin_db_usage_string, sizeof(builtin_db_usage_string), "%s", tempstring);
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_External, "type=%d builtin_db_usage_string=%s", type, builtin_db_usage_string);
		};
	};

	return(builtin_db_usage_string);
};


/*********************************************
 * Abstract functions
 * *******************************************/

/* query for available features
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_BuiltIn_has_features(uint32_t features) {
	int result = -1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called with feature value to test: 0x%08x", features);

	if ((wrapper_features_by_source[IPV6CALC_DB_SOURCE_BUILTIN] & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Return with result: %d", result);
	return(result);
};


/* query db_unixtime by feature
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
time_t libipv6calc_db_wrapper_BuiltIn_db_unixtime_by_feature(uint32_t feature) {
	time_t result = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called with feature value to get db_unixtime: 0x%08x", feature);

	if ((IPV6CALC_DB_IPV4_TO_REGISTRY & feature) == feature) {
#ifdef SUPPORT_DB_IPV4_REG
		result = dbipv4addr_registry_unixtime;
#endif
	} else if ((IPV6CALC_DB_IPV6_TO_REGISTRY & feature) == feature) {
#ifdef SUPPORT_DB_IPV6_REG
		result = dbipv6addr_registry_unixtime;
#endif
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Return for feature=0x%08x db_unixtime=%ld", feature, (long int) result);
	return(result);
};


#ifdef SUPPORT_BUILTIN

/*******************************
 * Wrapper functions for BuiltIn
 *******************************/

// get registry number by AS number
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_as_num32(const uint32_t as_num32) {
	int result = REGISTRY_UNKNOWN;

	int max = MAXENTRIES_ARRAY(dbasn_assignment);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called with as_num32=%d max=%d", as_num32, max);

#ifdef SUPPORT_DB_AS_REG
	// binary search
	int i, r = -1;
	int i_min = 0;
	int i_max = max;
	int i_old = -1;

	i = max / 2;
	while (i_old != i) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Search for as_num32=%d max=%d i=%d start=%d stop=%d", as_num32, max, i, dbasn_assignment[i].asn_start, dbasn_assignment[i].asn_stop);

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
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished with success result: %d", dbasn_assignment[r].registry);

		result = dbasn_assignment[r].registry;
		BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_AS_REG);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished without success");

	};
#endif
	return(result);
};


// get registry number by CC index
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_cc_index(const uint16_t cc_index) {
	int result = REGISTRY_UNKNOWN;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called with cc_index=%d", cc_index);

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

#ifdef SUPPORT_DB_CC_REG
	result = cc_index_reg_assignment[cc_index].registry;
	BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_CC_REG);
#endif

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Return registry=%s (%d) (cc_index=%d)", libipv6calc_registry_string_by_num(result), result, cc_index);

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
				BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_IAB);
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
				BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_OUI36);
				return (0);
			};
		};
	};

	/* run through OUI list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			snprintf(resultstring, resultstring_length, "%s", libieee_oui[i].string_owner);
			BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_OUI);
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
				BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_IAB);
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
				BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_OUI36);
				return (0);
			};
		};
	};

	/* run through OUI list */
	for (i = 0; i < MAXENTRIES_ARRAY(libieee_oui); i++) {
		if (libieee_oui[i].id == idval) {
			/* match */
			snprintf(resultstring, resultstring_length, "%s", libieee_oui[i].shortstring_owner);
			BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_OUI);
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


#ifdef SUPPORT_DB_IPV4_REG
/*
 * dbipv4addr_assignment / get row (callback function for retrieving value from array)
 */
int libipv6calc_db_wrapper_BuiltIn_get_row_dbipv4addr_assignment(const uint32_t row, uint32_t *key_first_00_31_ptr, uint32_t *key_first_32_63_ptr, uint32_t *key_last_00_31_ptr, uint32_t *key_last_32_63_ptr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called fetching row: %lu", (unsigned long int) row);

	if (row >= MAXENTRIES_ARRAY(dbipv4addr_assignment)) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "row out of range: %lu (maximum: %lu)", (unsigned long int) row, (unsigned long int) MAXENTRIES_ARRAY(dbipv4addr_assignment) - 1);
		return(1);
	};

	*key_first_00_31_ptr = dbipv4addr_assignment[row].first;
	*key_last_00_31_ptr  = dbipv4addr_assignment[row].last;
	*key_first_32_63_ptr = 0;
	*key_last_32_63_ptr  = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Keys for row: %lu key_first_00_31_ptr=%08lx key_last_00_31_ptr=%08lxu",
		(unsigned long int) row,
		(unsigned long int) *key_first_00_31_ptr,
		(unsigned long int) *key_last_00_31_ptr
	);

	return(0);
};


/*
 * dbipv4addr_assignment_iana / get row (callback function for retrieving value from array)
 */
int libipv6calc_db_wrapper_BuiltIn_get_row_dbipv4addr_assignment_iana(const uint32_t row, uint32_t *key_first_00_31_ptr, uint32_t *key_first_32_63_ptr, uint32_t *key_last_00_31_ptr, uint32_t *key_last_32_63_ptr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called fetching row: %lu", (unsigned long int) row);

	if (row >= MAXENTRIES_ARRAY(dbipv4addr_assignment_iana)) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "row out of range: %lu (maximum: %lu)", (unsigned long int) row, (unsigned long int) MAXENTRIES_ARRAY(dbipv4addr_assignment_iana) - 1);
		return(1);
	};

	*key_first_00_31_ptr = dbipv4addr_assignment_iana[row].first;
	*key_last_00_31_ptr  = dbipv4addr_assignment_iana[row].last;
	*key_first_32_63_ptr = 0;
	*key_last_32_63_ptr  = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Keys for row: %lu key_first_00_31_ptr=%08lx key_last_00_31_ptr=%08lxu",
		(unsigned long int) row,
		(unsigned long int) *key_first_00_31_ptr,
		(unsigned long int) *key_last_00_31_ptr
	);

	return(0);
};


/*
 * dbipv4addr_info / get row (callback function for retrieving value from array)
 */
int libipv6calc_db_wrapper_BuiltIn_get_row_dbipv4addr_info(const uint32_t row, uint32_t *key_first_00_31_ptr, uint32_t *key_first_32_63_ptr, uint32_t *key_last_00_31_ptr, uint32_t *key_last_32_63_ptr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called fetching row: %lu", (unsigned long int) row);

	if (row >= MAXENTRIES_ARRAY(dbipv4addr_info)) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "row out of range: %lu (maximum: %lu)", (unsigned long int) row, (unsigned long int) MAXENTRIES_ARRAY(dbipv4addr_info) - 1);
		return(1);
	};

	*key_first_00_31_ptr = dbipv4addr_info[row].first;
	*key_last_00_31_ptr  = dbipv4addr_info[row].last;
	*key_first_32_63_ptr = 0;
	*key_last_32_63_ptr  = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Keys for row: %lu key_first_00_31_ptr=%08lx key_last_00_31_ptr=%08lxu",
		(unsigned long int) row,
		(unsigned long int) *key_first_00_31_ptr,
		(unsigned long int) *key_last_00_31_ptr
	);

	return(0);
};
#endif // SUPPORT_DB_IPV4_REG


/*
 * get registry number of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp) {
	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Given IPv4 address: %08x", (unsigned int) ipv4);

	int result = IPV4_ADDR_REGISTRY_UNKNOWN;

#ifdef SUPPORT_DB_IPV4_REG
	int match = -1;

	match = libipv6calc_db_wrapper_get_entry_generic(
		NULL,							// pointer to data
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
		0,							// key format (not relevant)
		32,							// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		MAXENTRIES_ARRAY(dbipv4addr_assignment),		// number of rows
		ipv4,							// lookup key MSB
		0,							// lookup key LSB
		NULL,							// data ptr (not used in IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY)
		libipv6calc_db_wrapper_BuiltIn_get_row_dbipv4addr_assignment	// function pointer
	);

	if (match > -1) {
		result = dbipv4addr_assignment[match].registry;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished with success result (dbipv4addr_assignment): match=%d reg=%d", match, result);
		BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_IPV4_REGISTRY);
	};

	if (result == IPV4_ADDR_REGISTRY_UNKNOWN) {
		// IANA fallback
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Nothing found in dbipv4addr_assignment, fallback now to dbipv4addr_assignment_iana");

		match = libipv6calc_db_wrapper_get_entry_generic(
			NULL,							// pointer to data
			IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY,			// type of data_ptr
			IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
			0,							// key format (not relevant)
			32,							// key length
			IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
			MAXENTRIES_ARRAY(dbipv4addr_assignment_iana),		// number of rows
			ipv4,							// lookup key MSB
			0,							// lookup key LSB
			NULL,							// data ptr (not used in IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY)
			libipv6calc_db_wrapper_BuiltIn_get_row_dbipv4addr_assignment_iana	// function pointer
		);

		if (match > -1) {
			result = dbipv4addr_assignment_iana[match].registry;
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished with success result (dbipv4addr_assignment_iana): match=%d reg=%d", match, result);
			BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_IPV4_REGISTRY);
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished without success");
		};
	};
#endif // SUPPORT_DB_IPV4_REG

	return(result);
};


/*
 * get info of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * in:  char* and size to fill
 * out: 0 = found, -1 = no result
 */
int libipv6calc_db_wrapper_BuiltIn_info_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, char *string, const size_t string_len) {
	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Given IPv4 address: %08x", (unsigned int) ipv4);

	int result = -1;

#ifdef SUPPORT_DB_IPV4_REG
	int match = -1;

	match = libipv6calc_db_wrapper_get_entry_generic(
		NULL,							// pointer to data
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST,		// key type
		0,							// key format (not relevant)
		32,							// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		MAXENTRIES_ARRAY(dbipv4addr_info),			// number of rows
		ipv4,							// lookup key MSB
		0,							// lookup key LSB
		NULL,							// data ptr (not used in IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY)
		libipv6calc_db_wrapper_BuiltIn_get_row_dbipv4addr_info	// function pointer
	);

	if (match > -1) {
		snprintf(string, string_len, "%s", dbipv4addr_info[match].info);
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished with success result (dbipv4addr_info): match=%d info=\"%s\"", match, string);
		BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_IPV4_REGISTRY);
		result = 0;
	};

#else  // SUPPORT_DB_IPV4_REG
	snprintf(string, string_len, "%s", "");
#endif // SUPPORT_DB_IPV4_REG

	return(result);
};


#ifdef SUPPORT_DB_IPV6_REG
/*
 * dbipv6addr_assignment / get row (callback function for retrieving value from array)
 */
int libipv6calc_db_wrapper_BuiltIn_get_row_dbipv6addr_assignment(const uint32_t row, uint32_t *key_base_00_31_ptr, uint32_t *key_base_32_63_ptr, uint32_t *key_mask_00_31_ptr, uint32_t *key_mask_32_63_ptr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called fetching row: %lu", (unsigned long int) row);

	if (row >= MAXENTRIES_ARRAY(dbipv6addr_assignment)) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "row out of range: %lu (maximum: %lu)", (unsigned long int) row, (unsigned long int) MAXENTRIES_ARRAY(dbipv6addr_assignment) - 1);
		return(-1);
	};

	*key_base_00_31_ptr = dbipv6addr_assignment[row].ipv6addr_00_31;
	*key_base_32_63_ptr = dbipv6addr_assignment[row].ipv6addr_32_63;
	*key_mask_00_31_ptr = dbipv6addr_assignment[row].ipv6mask_00_31;
	*key_mask_32_63_ptr = dbipv6addr_assignment[row].ipv6mask_32_63;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Keys for row: %lu key_base_00_31_ptr=%08lx key_base_32_63_ptr=%08lx key_mask_00_31_ptr=%08lxu key_mask_32_63_ptr=%08lxu seqlongest=%d",
		(unsigned long int) row,
		(unsigned long int) *key_base_00_31_ptr,
		(unsigned long int) *key_base_32_63_ptr,
		(unsigned long int) *key_mask_00_31_ptr,
		(unsigned long int) *key_mask_32_63_ptr,
		dbipv6addr_assignment[row].prefixlength
	);

	return(dbipv6addr_assignment[row].prefixlength);
};


/*
 * dbipv6addr_info / get row (callback function for retrieving value from array)
 */
int libipv6calc_db_wrapper_BuiltIn_get_row_dbipv6addr_info(const uint32_t row, uint32_t *key_base_00_31_ptr, uint32_t *key_base_32_63_ptr, uint32_t *key_mask_00_31_ptr, uint32_t *key_mask_32_63_ptr) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Called fetching row: %lu", (unsigned long int) row);

	if (row >= MAXENTRIES_ARRAY(dbipv6addr_info)) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "row out of range: %lu (maximum: %lu)", (unsigned long int) row, (unsigned long int) MAXENTRIES_ARRAY(dbipv6addr_info) - 1);
		return(-1);
	};

	*key_base_00_31_ptr = dbipv6addr_info[row].ipv6addr_00_31;
	*key_base_32_63_ptr = dbipv6addr_info[row].ipv6addr_32_63;
	*key_mask_00_31_ptr = dbipv6addr_info[row].ipv6mask_00_31;
	*key_mask_32_63_ptr = dbipv6addr_info[row].ipv6mask_32_63;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Keys for row: %lu key_base_00_31_ptr=%08lx key_base_32_63_ptr=%08lx key_mask_00_31_ptr=%08lxu key_mask_32_63_ptr=%08lxu seqlongest=%d",
		(unsigned long int) row,
		(unsigned long int) *key_base_00_31_ptr,
		(unsigned long int) *key_base_32_63_ptr,
		(unsigned long int) *key_mask_00_31_ptr,
		(unsigned long int) *key_mask_32_63_ptr,
		dbipv6addr_info[row].prefixlength
	);

	return(dbipv6addr_info[row].prefixlength);
};
#endif // SUPPORT_DB_IPV6_REG


/*
 * get registry number of an IPv6 address
 *
 * in:  ipv6addr = IPv6 address structure
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t ipv6_00_31 = ipv6addr_getdword(ipv6addrp, 0);
	uint32_t ipv6_32_63 = ipv6addr_getdword(ipv6addrp, 1);
	
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Given ipv6 prefix: %08x%08x", (unsigned int) ipv6_00_31, (unsigned int) ipv6_32_63);

	if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6BONE) != 0) {
		return(IPV6_ADDR_REGISTRY_6BONE);
	};

	int result = IPV6_ADDR_REGISTRY_UNKNOWN;

#ifdef SUPPORT_DB_IPV6_REG
	int match = -1;

	match = libipv6calc_db_wrapper_get_entry_generic(
		NULL,							// pointer to data
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK,		// key type
		0,							// key format (not relevant)
		64,							// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST,		// search type
		MAXENTRIES_ARRAY(dbipv6addr_assignment),		// number of rows
		ipv6_00_31,						// lookup key MSB
		ipv6_32_63,						// lookup key LSB
		NULL,							// data ptr (not used in IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY)
		libipv6calc_db_wrapper_BuiltIn_get_row_dbipv6addr_assignment	// function pointer
	);

	/* result */
	if ( match > -1 ) {
		result = dbipv6addr_assignment[match].registry;
		BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_IPV6_REGISTRY);
	};
#endif
	return(result);
};


/*
 * get info of an IPv6 address
 *
 * in:  ipv6addr = IPv6 address structure
 * in:  char* and size to fill
 * out: 0 = found, -1 = no result
 */
int libipv6calc_db_wrapper_BuiltIn_info_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, char *string, const size_t string_len) {
	uint32_t ipv6_00_31 = ipv6addr_getdword(ipv6addrp, 0);
	uint32_t ipv6_32_63 = ipv6addr_getdword(ipv6addrp, 1);
	
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Given ipv6 prefix: %08x%08x", (unsigned int) ipv6_00_31, (unsigned int) ipv6_32_63);

	int result = -1;

#ifdef SUPPORT_DB_IPV6_REG
	int match = -1;

	match = libipv6calc_db_wrapper_get_entry_generic(
		NULL,							// pointer to data
		IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY,			// type of data_ptr
		IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK,		// key type
		0,							// key format (not relevant)
		64,							// key length
		IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY,		// search type
		MAXENTRIES_ARRAY(dbipv6addr_info),			// number of rows
		ipv6_00_31,						// lookup key MSB
		ipv6_32_63,						// lookup key LSB
		NULL,							// data ptr (not used in IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY)
		libipv6calc_db_wrapper_BuiltIn_get_row_dbipv6addr_info	// function pointer
	);

	if (match > -1) {
		snprintf(string, string_len, "%s", dbipv6addr_info[match].info);
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper_BuiltIn, "Finished with success result (dbipv6addr_info): match=%d info=\"%s\"", match, string);
		BUILTIN_DB_USAGE_MAP_TAG(BUILTIN_DB_IPV6_REGISTRY);
		result = 0;
	};

#else  // SUPPORT_DB_IPV6_REG
	snprintf(string, string_len, "%s", "");
#endif // SUPPORT_DB_IPV6_REG

	return(result);
};

#endif		// SUPPORT_BUILTIN
