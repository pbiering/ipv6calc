/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.c
 * Version    : $Id$
 * Copyright  : 2013-2017 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc database wrapper (for decoupling databases from main binary)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>

#include "config.h"

#include "libipv6calcdebug.h"
#include "libipv6calc.h"

#include "ipv6calcoptions.h"

#include "libipv6calc_db_wrapper.h"
#include "libipv6calc_db_wrapper_GeoIP.h"
#include "libipv6calc_db_wrapper_IP2Location.h"
#include "libipv6calc_db_wrapper_DBIP.h"
#include "libipv6calc_db_wrapper_External.h"
#include "libipv6calc_db_wrapper_BuiltIn.h"

#ifdef DOMAIN
// fallback for IP2Location.h < 8.0.0 where "DOMAIN" is defined
// code taken from https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c
static int uint64_log2(uint64_t n) {
	#define S(k) if (n >= (UINT64_C(1) << k)) { i += k; n >>= k; }

	int i = -(n == 0); S(32); S(16); S(8); S(4); S(2); S(1); return i;

	#undef S
};
#else
#include <math.h>
#define uint64_log2	log2
#endif

static int wrapper_GeoIP_disable       = 0;
static int wrapper_IP2Location_disable = 0;
static int wrapper_DBIP_disable        = 0;
static int wrapper_External_disable    = 0;
static int wrapper_BuiltIn_disable     = 0;

static int wrapper_GeoIP_status = 0;
static int wrapper_IP2Location_status = 0;
static int wrapper_DBIP_status = 0;
static int wrapper_External_status = 0;
static int wrapper_BuiltIn_status = 0;

uint32_t wrapper_features = 0;
uint32_t wrapper_features_by_source[IPV6CALC_DB_SOURCE_MAX + 1];
uint32_t wrapper_features_by_source_implemented[IPV6CALC_DB_SOURCE_MAX + 1];

int wrapper_features_selector[IPV6CALC_DB_FEATURE_NUM_MAX + 1][IPV6CALC_DB_PRIO_MAX];
unsigned int wrapper_source_priority_selector[IPV6CALC_DB_SOURCE_MAX + 1];
int wrapper_source_priority_selector_by_option = -1; // -1: uninitialized, 0: initialized, > 0: touched by option


/*
 * function initialise the main wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_init(const char *prefix_string) {
	int result = 0, f, p, s, j;

#if defined SUPPORT_GEOIP || defined SUPPORT_IP2LOCATION || defined SUPPORT_DBIP || defined SUPPORT_EXTERNAL || defined SUPPORT_BUILTIN
	int r;
#endif
	s = strlen(prefix_string); // make compiler happy (avoid unused "...")

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	// clear feature selector
	for (f = IPV6CALC_DB_FEATURE_NUM_MIN; f <= IPV6CALC_DB_FEATURE_NUM_MAX; f++) {
		for (p = 0; p < IPV6CALC_DB_PRIO_MAX; p++) {
			wrapper_features_selector[f][p] = 0;
		};
	};

	for (s = 0; s <= IPV6CALC_DB_SOURCE_MAX; s++) {
		wrapper_features_by_source[s] = 0;
	};

	// initialize priority selector
	if (wrapper_source_priority_selector_by_option < 0) {
		for (s = 0; s <= IPV6CALC_DB_SOURCE_MAX; s++) {
			wrapper_source_priority_selector[s] = 0;
		};
		wrapper_source_priority_selector_by_option = 0;
	};

	// initialize priority
	if (wrapper_source_priority_selector_by_option == 0) {
		// default
		for (s = IPV6CALC_DB_SOURCE_MIN; s <= IPV6CALC_DB_SOURCE_MAX; s++) {
			wrapper_source_priority_selector[s] = s;
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database priorization already given by option, fill missing ones: %d", wrapper_source_priority_selector_by_option);

		for (j = IPV6CALC_DB_SOURCE_MIN; j <= wrapper_source_priority_selector_by_option; j++) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database priorization/defined by option entry %d: %s", j, libipv6calc_db_wrapper_get_data_source_name_by_number(wrapper_source_priority_selector[j]));
		};

		// already touched by option, fill not mentioned ones
		for (j = 0; j < MAXENTRIES_ARRAY(data_sources); j++) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database priorization/test source: %s", data_sources[j].shortname);
			for (s = IPV6CALC_DB_SOURCE_MIN; s <= wrapper_source_priority_selector_by_option; s++) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database priorization/check: %s", libipv6calc_db_wrapper_get_data_source_name_by_number(wrapper_source_priority_selector[s]));
				if (wrapper_source_priority_selector[s] == data_sources[j].number) {
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database priorization/source already defined: %s", data_sources[j].shortname);
					// already set, skip
					break;
				};
			};

			if (s > wrapper_source_priority_selector_by_option) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database priorization/source missing, add: %s", data_sources[j].shortname);
				wrapper_source_priority_selector_by_option++;
				wrapper_source_priority_selector[wrapper_source_priority_selector_by_option] = data_sources[j].number;
			};
		};
	};

	for (j = IPV6CALC_DB_SOURCE_MIN; j <= wrapper_source_priority_selector_by_option; j++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database priorization/defined by option entry %d: %s", j, libipv6calc_db_wrapper_get_data_source_name_by_number(wrapper_source_priority_selector[j]));
	};

	if (wrapper_GeoIP_disable != 1) {
#ifdef SUPPORT_GEOIP
		// Call GeoIP wrapper
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call libipv6calc_db_wrapper_GeoIP_wrapper_init");

		r = libipv6calc_db_wrapper_GeoIP_wrapper_init();

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "GeoIP_wrapper_init result: %d wrapper_features=0x%08x", r, wrapper_features);

		if (r != 0) {
#ifndef SUPPORT_GEOIP_DYN
			// only non-dynamic-load results in a problem
			result = 1;
#endif
		} else {
			wrapper_GeoIP_status = 1; // ok
		};
	} else {
		NONQUIETPRINT_WA("%sSupport for GeoIP disabled by option", prefix_string);
#endif // SUPPORT_GEOIP
	};

	if (wrapper_IP2Location_disable != 1) {
#ifdef SUPPORT_IP2LOCATION
		// Call IP2Location wrapper
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call libipv6calc_db_wrapper_IP2Location_wrapper_init");

		r = libipv6calc_db_wrapper_IP2Location_wrapper_init();

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "IP2Location_wrapper_init result: %d wrapper_features=0x%08x", r, wrapper_features);

		if (r != 0) {
#ifndef SUPPORT_IP2LOCATION_DYN
			// only non-dynamic-load results in a problem
			result = 1;
#endif
		} else {
			wrapper_IP2Location_status = 1; // ok
		};
	} else {
		NONQUIETPRINT_WA("%sSupport for IP2Location disabled by option", prefix_string);
#endif // SUPPORT_IP2LOCATION
	};

	if (wrapper_DBIP_disable != 1) {
#ifdef SUPPORT_DBIP
		// Call DBIP wrapper
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call libipv6calc_db_wrapper_DBIP_wrapper_init");

		r = libipv6calc_db_wrapper_DBIP_wrapper_init();

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "DBIP_wrapper_init result: %d wrapper_features=0x%08x", r, wrapper_features);

		if (r != 0) {
			result = 1;
		} else {
			wrapper_DBIP_status = 1; // ok
		};
	} else {
		NONQUIETPRINT_WA("%sSupport for db-ip.com disabled by option", prefix_string);
#endif // SUPPORT_DBIP
	};

	if (wrapper_External_disable != 1) {
#ifdef SUPPORT_EXTERNAL
		// Call External wrapper
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call libipv6calc_db_wrapper_External_wrapper_init");

		r = libipv6calc_db_wrapper_External_wrapper_init();

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "External_wrapper_init result: %d wrapper_features=0x%08x", r, wrapper_features);

		if (r != 0) {
			result = 1;
		} else {
			wrapper_External_status = 1; // ok
		};
	} else {
		NONQUIETPRINT_WA("%sSupport for External disabled by option", prefix_string);
#endif // SUPPORT_EXTERNAL
	};


	if (wrapper_BuiltIn_disable != 1) {
#ifdef SUPPORT_BUILTIN
		// Call BuiltIn wrapper
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call libipv6calc_db_wrapper_BuiltIn_wrapper_init");

		r = libipv6calc_db_wrapper_BuiltIn_wrapper_init();

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "BuiltIn_wrapper_init result: %d wrapper_features=0x%08x", r, wrapper_features);

		if (r != 0) {
			result = 1;
		} else {
			wrapper_BuiltIn_status = 1; // ok
		};
#endif // SUPPORT_BUILTIN
	};

	// select source for feature by standard priority (from last to first in list)
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "select source for feature by standard priority");
	// run through feature numbers
	for (f = IPV6CALC_DB_FEATURE_NUM_MIN; f <= IPV6CALC_DB_FEATURE_NUM_MAX; f++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "check feature f=%d", f);
		// run through sources
		int sp;
		for (sp = IPV6CALC_DB_SOURCE_MIN; sp <= IPV6CALC_DB_SOURCE_MAX; sp++) {
			s = wrapper_source_priority_selector[sp];
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "check feature by source f=%d s=%d sp=%d", f, s, sp);
			if ((wrapper_features_by_source[s] & (1 << f)) != 0) {
				// supported, run through prio array
				for (p = 0; p < IPV6CALC_DB_PRIO_MAX; p++) {
					if (wrapper_features_selector[f][p] == 0) {
						DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "select feature with source f=%d s=%d p=%d", f, s, p);
						wrapper_features_selector[f][p] = s;
						break;
					} else {
#if defined SUPPORT_BUILTIN && defined SUPPORT_EXTERNAL
						// special handling for BuiltIn/External (no subsequent calls)
						if ((wrapper_BuiltIn_disable != 1) && (wrapper_External_disable != 1)) {
							// IPv4 -> Registry
							if (((1 << f) == IPV6CALC_DB_IPV4_TO_REGISTRY) && \
								( (s == IPV6CALC_DB_SOURCE_BUILTIN && (wrapper_features_selector[f][p] == IPV6CALC_DB_SOURCE_EXTERNAL)) \
								  || (s == IPV6CALC_DB_SOURCE_EXTERNAL && (wrapper_features_selector[f][p] == IPV6CALC_DB_SOURCE_BUILTIN))
								)) {
									// BuiltIn & External have feature enabled and one of them is already selected for p=0
									time_t db_unixtime_BuiltIn  = libipv6calc_db_wrapper_BuiltIn_db_unixtime_by_feature(IPV6CALC_DB_IPV4_TO_REGISTRY);
									time_t db_unixtime_External = libipv6calc_db_wrapper_External_db_unixtime_by_feature(IPV6CALC_DB_IPV4_TO_REGISTRY);

									if ((db_unixtime_BuiltIn > 0) && (db_unixtime_External > 0)) {
										if (db_unixtime_BuiltIn < db_unixtime_External) {
											wrapper_features_selector[f][p] = IPV6CALC_DB_SOURCE_EXTERNAL;
											DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "BuiltIn has older DB than External for IPV6CALC_DB_IPV4_TO_REGISTRY");
										} else {
											wrapper_features_selector[f][p] = IPV6CALC_DB_SOURCE_BUILTIN;
											DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "External has older DB than BuiltIn for IPV6CALC_DB_IPV4_TO_REGISTRY");
									};
									break; // no further extension of priority list
								};
							};

							// IPv6 -> Registry
							if (((1 << f) == IPV6CALC_DB_IPV6_TO_REGISTRY) && \
								( (s == IPV6CALC_DB_SOURCE_BUILTIN && (wrapper_features_selector[f][p] == IPV6CALC_DB_SOURCE_EXTERNAL)) \
								  || (s == IPV6CALC_DB_SOURCE_EXTERNAL && (wrapper_features_selector[f][p] == IPV6CALC_DB_SOURCE_BUILTIN))
								)) {
									// BuiltIn & External have feature enabled and one of them is already selected for p=0
									time_t db_unixtime_BuiltIn  = libipv6calc_db_wrapper_BuiltIn_db_unixtime_by_feature(IPV6CALC_DB_IPV6_TO_REGISTRY);
									time_t db_unixtime_External = libipv6calc_db_wrapper_External_db_unixtime_by_feature(IPV6CALC_DB_IPV6_TO_REGISTRY);

									if ((db_unixtime_BuiltIn > 0) && (db_unixtime_External > 0)) {
										if (db_unixtime_BuiltIn < db_unixtime_External) {
											wrapper_features_selector[f][p] = IPV6CALC_DB_SOURCE_EXTERNAL;
											DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "BuiltIn has older DB than External for IPV6CALC_DB_IPV6_TO_REGISTRY");
										} else {
											wrapper_features_selector[f][p] = IPV6CALC_DB_SOURCE_BUILTIN;
											DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "External has older DB than BuiltIn for IPV6CALC_DB_IPV6_TO_REGISTRY");
									};
									break; // no further extension of priority list
								};
							};
						};
#endif // SUPPORT_BUILTIN && SUPPORT_EXTERNAL
					};
				};
			};
		};
	};

#ifdef TEST
#if defined SUPPORT_BUILTIN && defined SUPPORT_EXTERNAL
	// IPv4 -> Registry
	if ((wrapper_BuiltIn_status == 1) && (wrapper_External_status == 1)) {
		if (libipv6calc_db_wrapper_External_has_features(IPV6CALC_DB_IPV4_TO_REGISTRY) == 1 \
			&& libipv6calc_db_wrapper_BuiltIn_has_features(IPV6CALC_DB_IPV4_TO_REGISTRY) == 1) {
			// BuiltIn & External have feature enabled (both)
			time_t db_unixtime_BuiltIn  = libipv6calc_db_wrapper_BuiltIn_db_unixtime_by_feature(IPV6CALC_DB_IPV4_TO_REGISTRY);
			time_t db_unixtime_External = libipv6calc_db_wrapper_External_db_unixtime_by_feature(IPV6CALC_DB_IPV4_TO_REGISTRY);

			if ((db_unixtime_BuiltIn > 0) && (db_unixtime_External > 0)) {
				if (db_unixtime_BuiltIn < db_unixtime_External) {
					wrapper_features_selector[IPV6CALC_DB_FEATURE_NUM_IPV4_TO_REGISTRY][0] = IPV6CALC_DB_SOURCE_EXTERNAL;
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "BuiltIn has older DB than External for IPV6CALC_DB_IPV4_TO_REGISTRY, disable it further");
				} else {
					wrapper_features_selector[IPV6CALC_DB_FEATURE_NUM_IPV4_TO_REGISTRY][0] = IPV6CALC_DB_SOURCE_BUILTIN;
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "External has older DB than BuiltIn for IPV6CALC_DB_IPV4_TO_REGISTRY, disable it further");
				};
			};
		};
	};
#endif // SUPPORT_BUILTIN && SUPPORT_EXTERNAL
#endif

	return(result);
};


/*
 * function cleanup the main wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_cleanup(void) {
	int result = 0;

#if defined SUPPORT_GEOIP || defined SUPPORT_IP2LOCATION || defined SUPPORT_DBIP || defined SUPPORT_EXTERNAL || defined SUPPORT_BUILTIN
	int r;
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_GEOIP
	// Call GeoIP wrapper
	r = libipv6calc_db_wrapper_GeoIP_wrapper_cleanup();
	if (r != 0) {
		result = 1;
	};
#endif

#ifdef SUPPORT_IP2LOCATION
	// Call IP2Location wrapper
	r = libipv6calc_db_wrapper_IP2Location_wrapper_cleanup();
	if (r != 0) {
		result = 1;
	};
#endif

#ifdef SUPPORT_DBIP
	// Call DBIP wrapper
	r = libipv6calc_db_wrapper_DBIP_wrapper_cleanup();
	if (r != 0) {
		result = 1;
	};
#endif

#ifdef SUPPORT_BUILTIN
	// Call BuiltIn wrapper
	r = libipv6calc_db_wrapper_BuiltIn_wrapper_cleanup();
	if (r != 0) {
		result = 1;
	};
#endif

	return(result);
};


/* function get info strings */
void libipv6calc_db_wrapper_info(char *string, const size_t size) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

#ifdef SUPPORT_GEOIP
	// Call GeoIP wrapper
	libipv6calc_db_wrapper_GeoIP_wrapper_info(string, size);
#endif

#ifdef SUPPORT_IP2LOCATION
	// Call IP2Location wrapper
	libipv6calc_db_wrapper_IP2Location_wrapper_info(string, size);
#endif

#ifdef SUPPORT_DBIP
	// Call DBIP wrapper
	libipv6calc_db_wrapper_BuiltIn_wrapper_info(string, size);
#endif

#ifdef SUPPORT_BUILTIN
	// Call BuiltIn wrapper
	libipv6calc_db_wrapper_BuiltIn_wrapper_info(string, size);
#endif

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: %s", string);

	return;
};


/* function get feature string */
void libipv6calc_db_wrapper_features(char *string, const size_t size) {
	int i;
	char tempstring[NI_MAXHOST];
	char *separator;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_db_features); i++) {
		if (wrapper_features & ipv6calc_db_features[i].number) {
			if (strlen(string) == 0) {
				separator = "";
			} else {
				separator = " ";
			};
			snprintf(tempstring, sizeof(tempstring), "%s%s%s", string, separator, ipv6calc_db_features[i].token);
			snprintf(string, size, "%s", tempstring);
		};
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Return");

	return;
};


/* function get capability string */
void libipv6calc_db_wrapper_capabilities(char *string, const size_t size) {
#if defined SUPPORT_GEOIP || defined SUPPORT_IP2LOCATION || defined SUPPORT_DBIP || defined SUPPORT_EXTERNAL || defined SUPPORT_BUILTIN
	char tempstring[NI_MAXHOST];
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	if (wrapper_GeoIP_disable != 1) {
#ifdef SUPPORT_GEOIP
#ifdef SUPPORT_GEOIP_DYN
		snprintf(tempstring, sizeof(tempstring), "%s%sGeoIP(dyn-load)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#else // SUPPORT_GEOIP_DYN
#ifdef SUPPORT_GEOIP_STATIC
		snprintf(tempstring, sizeof(tempstring), "%s%sGeoIP(static)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#else // SUPPORT_GEOIP_STATIC
		snprintf(tempstring, sizeof(tempstring), "%s%sGeoIP(linked)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#endif // SUPPORT_GEOIP_STATIC
#endif // SUPPORT_GEOIP_DYN
#endif // SUPPORT_GEOIP
	};

	if (wrapper_IP2Location_disable != 1) {
#ifdef SUPPORT_IP2LOCATION
#ifdef SUPPORT_IP2LOCATION_DYN
		snprintf(tempstring, sizeof(tempstring), "%s%sIP2Location(dyn-load)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#else // SUPPORT_IP2LOCATION_DYN
#ifdef SUPPORT_IP2LOCATION_STATIC
		snprintf(tempstring, sizeof(tempstring), "%s%sIP2Location(static)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#else // SUPPORT_IP2LOCATION_STATIC
		snprintf(tempstring, sizeof(tempstring), "%s%sIP2Location(linked)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#endif // SUPPORT_IP2LOCATION_STATIC
#endif //SUPPORT_IP2LOCATION_DYN
#endif // SUPPORT_IP2LOCATION
	};

	if (wrapper_DBIP_disable != 1) {
#ifdef SUPPORT_DBIP
		snprintf(tempstring, sizeof(tempstring), "%s%sDBIP", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#endif
	};

	if (wrapper_External_disable != 1) {
#ifdef SUPPORT_EXTERNAL
		snprintf(tempstring, sizeof(tempstring), "%s%sExternal", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#endif
	};

	if (wrapper_BuiltIn_disable != 1) {
#ifdef SUPPORT_BUILTIN
		snprintf(tempstring, sizeof(tempstring), "%s%sDB_AS_REG(BuiltIn)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
		snprintf(tempstring, sizeof(tempstring), "%s%sDB_CC_REG(BuiltIn)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);

#ifdef SUPPORT_DB_IPV4_REG
		snprintf(tempstring, sizeof(tempstring), "%s%sDB_IPV4_REG(BuiltIn)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#endif

#ifdef SUPPORT_DB_IPV6_REG
		snprintf(tempstring, sizeof(tempstring), "%s%sDB_IPV6_REG(BuiltIn)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#endif

#ifdef SUPPORT_DB_IEEE
		snprintf(tempstring, sizeof(tempstring), "%s%sDB_IEEE(BuiltIn)", string, strlen(string) > 0 ? " " : "");
		snprintf(string, size, "%s", tempstring);
#endif
#endif // SUPPORT_BUILTIN
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Return");

	return;
};


/* function get source name by number
 * in: source number
 * out: source name
 */
const char *libipv6calc_db_wrapper_get_data_source_name_by_number(const unsigned int number) {
	int i;

	for (i = 0; i < MAXENTRIES_ARRAY(data_sources); i++) {
		if (data_sources[i].number == number) {
			return(data_sources[i].name);
		};
	};

	ERRORPRINT_WA("unsupported data_source number: %d (FIX CODE)\n", number);
	exit(1);
};


/* function get feature index by number
 * in: feature number
 * out: index
 */
static int libipv6calc_db_wrapper_get_feature_index_by_feature(const uint32_t feature) {
	int i;

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_db_features); i++) {
		if (ipv6calc_db_features[i].number == feature) {
			return(i);
		};
	};

	ERRORPRINT_WA("unsupported feature: %d (FIX CODE)\n", feature);
	exit(1);
};



/* function print feature string help */
void libipv6calc_db_wrapper_features_help(void) {
	int i, s, first;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_db_features); i++) {
		fprintf(stderr, "%-22s%c %s", ipv6calc_db_features[i].token, (wrapper_features & ipv6calc_db_features[i].number) ? '+' : '-', ipv6calc_db_features[i].explanation);

		first = 1;
		for (s = IPV6CALC_DB_SOURCE_MIN; s <= IPV6CALC_DB_SOURCE_MAX; s++) {
			if ((wrapper_features_by_source_implemented[s] & ipv6calc_db_features[i].number) != 0) {
				fprintf(stderr, "%s%s", (first == 1) ? " (provided by " : ",", libipv6calc_db_wrapper_get_data_source_name_by_number(s));
				first = 0;
			};
		};
		fprintf(stderr, "%s\n", (first == 1) ? "" : ")");
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Return");

	return;


};


/* function print db info */
void libipv6calc_db_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	int f, p, f_index;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	fprintf(stderr, "%sDB features: 0x%08x\n\n", prefix_string, wrapper_features);

#ifdef SUPPORT_GEOIP
	if (wrapper_GeoIP_disable != 1) {
		// Call GeoIP wrapper
		libipv6calc_db_wrapper_GeoIP_wrapper_print_db_info(level_verbose, prefix_string);
	} else {
		fprintf(stderr, "%sGeoIP support available but disabled by option\n", prefix_string);
	};
	fprintf(stderr, "\n");
#endif

#ifdef SUPPORT_IP2LOCATION
	if (wrapper_IP2Location_disable != 1) {
		// Call IP2Location wrapper
		libipv6calc_db_wrapper_IP2Location_wrapper_print_db_info(level_verbose, prefix_string);
	} else {
		fprintf(stderr, "%sIP2Location support available but disabled by option\n", prefix_string);
	};
	fprintf(stderr, "\n");
#endif

#ifdef SUPPORT_DBIP
	if (wrapper_DBIP_disable != 1) {
		// Call DBIP wrapper
		libipv6calc_db_wrapper_DBIP_wrapper_print_db_info(level_verbose, prefix_string);
	} else {
		fprintf(stderr, "%sdb-ip.com support available but disabled by option\n", prefix_string);
	};
	fprintf(stderr, "\n");
#endif

#ifdef SUPPORT_EXTERNAL
	if (wrapper_External_disable != 1) {
		// Call External wrapper
		libipv6calc_db_wrapper_External_wrapper_print_db_info(level_verbose, prefix_string);
	} else {
		fprintf(stderr, "%sExternal support available but disabled by option\n", prefix_string);
	};
	fprintf(stderr, "\n");
#endif

#ifdef SUPPORT_BUILTIN
	if (wrapper_BuiltIn_disable != 1) {
		// Call BuiltIn wrapper
		libipv6calc_db_wrapper_BuiltIn_wrapper_print_db_info(level_verbose, prefix_string);
	} else {
		fprintf(stderr, "%sBuiltIn support available but disabled by option\n", prefix_string);
	};
	fprintf(stderr, "\n");
#endif

	// summary
	fprintf(stderr, "%sDatabase selection or priorization ('->': subsequential calls)\n", prefix_string);

	fprintf(stderr, "%sDatabase priorization %s: "
		, prefix_string
		, (wrapper_source_priority_selector_by_option > 0) ? "by option" : "default"
	);

	int sp;
	for (sp = IPV6CALC_DB_SOURCE_MIN; sp <= IPV6CALC_DB_SOURCE_MAX; sp++) {
		fprintf(stderr, "%s%s"
			, (sp == IPV6CALC_DB_SOURCE_MIN) ? "" : "->"
			, libipv6calc_db_wrapper_get_data_source_name_by_number(wrapper_source_priority_selector[sp])
		);
	};
	fprintf(stderr, "\n");

	for (f = IPV6CALC_DB_FEATURE_NUM_MIN; f <= IPV6CALC_DB_FEATURE_NUM_MAX; f++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "f=%d", f);

		f_index = libipv6calc_db_wrapper_get_feature_index_by_feature(1 << f);

		fprintf(stderr, "%s%s (%s): ", prefix_string, ipv6calc_db_features[f_index].token, ipv6calc_db_features[f_index].explanation);

		if (wrapper_features_selector[f][0] == 0) {
			fprintf(stderr, "NO-DATABASE");
		} else {
			for (p = 0; p < IPV6CALC_DB_PRIO_MAX; p++) {
				if (wrapper_features_selector[f][p] != 0) {
					fprintf(stderr, "%s%s", (p == 0) ? "" : "->", libipv6calc_db_wrapper_get_data_source_name_by_number(wrapper_features_selector[f][p]));
				};
			};
		};
		fprintf(stderr, "\n");
	};

	fprintf(stderr, "\n");

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Return");

	return;
};


/* function query for feature set
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_has_features(uint32_t features) {
	int result = -1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with feature value to test: 0x%08x", features);

	if ((wrapper_features & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Return with result: %d", result);

	return(result);
};


/*********************************************
 * Option handling
 * return < 0: error
 *********************************************/
int libipv6calc_db_wrapper_options(const int opt, const char *optarg, const struct option longopts[]) {
	int result = -1;
	int s;

	// initialize priority selector
	if (wrapper_source_priority_selector_by_option < 0) {
		for (s = 0; s <= IPV6CALC_DB_SOURCE_MAX; s++) {
			wrapper_source_priority_selector[s] = 0;
		};
		wrapper_source_priority_selector_by_option = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with option: %08x", opt);

	switch(opt) {
		case DB_ip2location_disable:
			wrapper_IP2Location_disable = 1;
			result = 0;
			break;

		case DB_geoip_disable:
			wrapper_GeoIP_disable = 1;
			result = 0;
			break;

		case DB_dbip_disable:
			wrapper_DBIP_disable = 1;
			result = 0;
			break;

		case DB_external_disable:
			wrapper_External_disable = 1;
			result = 0;
			break;

		case DB_builtin_disable:
			wrapper_BuiltIn_disable = 1;
			result = 0;
			break;

		case DB_ip2location_lib:
#ifdef SUPPORT_IP2LOCATION_DYN
			result = snprintf(ip2location_lib_file, sizeof(ip2location_lib_file), "%s", optarg);
#else
			NONQUIETPRINT_WA("Support for IP2Location dyn-load not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_geoip_lib:
#ifdef SUPPORT_GEOIP_DYN
			result = snprintf(geoip_lib_file, sizeof(geoip_lib_file), "%s", optarg);
#else
			NONQUIETPRINT_WA("Support for GeoIP dyn-load not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_ip2location_dir:
#ifdef SUPPORT_IP2LOCATION
			result = snprintf(ip2location_db_dir, sizeof(ip2location_db_dir), "%s", optarg);
#else
			NONQUIETPRINT_WA("Support for IP2Location not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_geoip_dir:
#ifdef SUPPORT_GEOIP
			result = snprintf(geoip_db_dir, sizeof(geoip_db_dir), "%s", optarg);
#else
			NONQUIETPRINT_WA("Support for GeoIP not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_dbip_dir:
#ifdef SUPPORT_DBIP
			result = snprintf(dbip_db_dir, sizeof(dbip_db_dir), "%s", optarg);
#else
			NONQUIETPRINT_WA("Support for db-ip.com not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_dbip_only_type:
#ifdef SUPPORT_DBIP
			if ((atoi(optarg) >= 1) && (atoi(optarg) <= DBIP_DB_MAX)) {
				dbip_db_only_type = atoi(optarg);
			} else {
				fprintf(stderr, " Argument of option 'db-dbip-only-type' is out or range (1-%d): %d\n", DBIP_DB_MAX, atoi(optarg));
				exit(EXIT_FAILURE);
			};
#else
			NONQUIETPRINT_WA("Support for db-ip.com not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_dbip_comm_to_free_switch_min_delta_months:
#ifdef SUPPORT_DBIP
			if ((atoi(optarg) >= 0) && (atoi(optarg) <= 99999)) {
				dbip_db_comm_to_free_switch_min_delta_months = atoi(optarg);
			} else {
				fprintf(stderr, " Argument of option 'db-dbip-comm-to-free-switch-min-delta-months' is out or range (0-99999): %d\n", atoi(optarg));
				exit(EXIT_FAILURE);
			};
#else
			NONQUIETPRINT_WA("Support for db-ip.com not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_external_dir:
#ifdef SUPPORT_EXTERNAL
			result = snprintf(external_db_dir, sizeof(external_db_dir), "%s", optarg);
#else
			NONQUIETPRINT_WA("Support for external not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		/* obsolete options */
		case DB_ip2location_ipv4:
		case DB_ip2location_ipv6:
			NONQUIETPRINT_WA("Obsolete option skipped: --%s <file>, use instead: --%s <dir>", ipv6calcoption_name(opt, longopts), ipv6calcoption_name(DB_ip2location_dir, longopts));
			result = 0;
			break;

		case DB_geoip_ipv4:
		case DB_geoip_ipv6:
			NONQUIETPRINT_WA("Obsolete option skipped: --%s <file>, use instead: --%s <dir>", ipv6calcoption_name(opt, longopts), ipv6calcoption_name(DB_geoip_dir, longopts));
			result = 0;
			break;


		case DB_ip2location_lite_to_sample_autoswitch_max_delta_months:
#ifdef SUPPORT_IP2LOCATION
			if ((atoi(optarg) >= 0) && (atoi(optarg) <= 99999)) {
				ip2location_db_lite_to_sample_autoswitch_max_delta_months = atoi(optarg);
			} else {
				fprintf(stderr, " Argument of option 'db-ip2location-lite-to-sample-autoswitch-max-delta-months' is out or range (0-99999): %d\n", atoi(optarg));
				exit(EXIT_FAILURE);
			};
#else
			NONQUIETPRINT_WA("Support for IP2Location not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_ip2location_comm_to_lite_switch_min_delta_months:
#ifdef SUPPORT_IP2LOCATION
			if ((atoi(optarg) >= 0) && (atoi(optarg) <= 99999)) {
				ip2location_db_comm_to_lite_switch_min_delta_months = atoi(optarg);
			} else {
				fprintf(stderr, " Argument of option 'db-ip2location-comm-to-lite-switch-min-delta-months' is out or range (0-99999): %d\n", atoi(optarg));
				exit(EXIT_FAILURE);
			};
#else
			NONQUIETPRINT_WA("Support for IP2Location not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_ip2location_only_type:
#ifdef SUPPORT_IP2LOCATION
			if ((atoi(optarg) >= 1) && (atoi(optarg) <= IP2LOCATION_DB_MAX)) {
				ip2location_db_only_type = atoi(optarg);
			} else {
				fprintf(stderr, " Argument of option 'db-ip2location-only-type' is out or range (1-%d): %d\n", IP2LOCATION_DB_MAX, atoi(optarg));
				exit(EXIT_FAILURE);
			};
#else
			NONQUIETPRINT_WA("Support for IP2Location not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_ip2location_allow_softlinks:
#ifdef SUPPORT_IP2LOCATION
			ip2location_db_allow_softlinks = 1;
#else
			NONQUIETPRINT_WA("Support for IP2Location not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_common_priorization:
#if defined SUPPORT_EXTERNAL || defined SUPPORT_DBIP || defined SUPPORT_GEOIP || SUPPORT_IP2LOCATION
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Parse database priorization string: %s", optarg);
			char tempstring[NI_MAXHOST];
			char *token, *cptr, **ptrptr;
			ptrptr = &cptr;
			int i, j;
			snprintf(tempstring, sizeof(tempstring), "%s", optarg);

			s = IPV6CALC_DB_SOURCE_MIN;

			token = strtok_r(tempstring, ":", ptrptr);
			while (token != NULL) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Parsing of token: %s", token);
				for (i = 0; i < MAXENTRIES_ARRAY(data_sources); i++) {
					if (strcasecmp(data_sources[i].shortname, token) == 0) {
						DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Token found: %s (%d)", token, data_sources[i].number);
						break;
					};
				};

				if (i == MAXENTRIES_ARRAY(data_sources)) {
					ERRORPRINT_WA("Database priorization token not supported: %s", token);
					exit(EXIT_FAILURE);
				};

				/* check for duplicate */
				if (s > IPV6CALC_DB_SOURCE_MIN) {
					DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Check for duplicate: %s", token);
					for (j = IPV6CALC_DB_SOURCE_MIN; j <= s; j++) {
						if (wrapper_source_priority_selector[j] == data_sources[i].number) {
							ERRORPRINT_WA("Database duplicate priorization token found: %s", token);
							exit(EXIT_FAILURE);
						};
					};
				};

				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Set token on entry %d: %s", s, token);
				wrapper_source_priority_selector[s] = data_sources[i].number;
				wrapper_source_priority_selector_by_option = s;
				s++;
	
				/* get next token */
				token = strtok_r(NULL, ":", ptrptr);
			};

			for (j = IPV6CALC_DB_SOURCE_MIN; j <= wrapper_source_priority_selector_by_option; j++) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database priorization/defined by option entry %d: %s", j, libipv6calc_db_wrapper_get_data_source_name_by_number(wrapper_source_priority_selector[j]));
			};
#else
			NONQUIETPRINT_WA("Support for database priorization not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
			s = strlen(optarg); // make compiler happy (avoid unused "...")
#endif
			result = 0;
			break;
	};

	if (result > 0) {
		result = 0;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Return with result: %d", result);

	return(result);
};


/*********************************************
 * Abstract functions
 *********************************************/

/*
 * get registry number by AS number
 */
int libipv6calc_db_wrapper_registry_num_by_as_num32(const uint32_t as_num32) {
	// currently only supported by BuiltIn
	return(libipv6calc_db_wrapper_BuiltIn_registry_num_by_as_num32(as_num32));
};


/*
 * get registry number by CC index
 */
int libipv6calc_db_wrapper_registry_num_by_cc_index(const uint16_t cc_index) {
	// currently only supported by BuiltIn
	return(libipv6calc_db_wrapper_BuiltIn_registry_num_by_cc_index(cc_index));
};

/*
 * get registry number by IP address
 */
int libipv6calc_db_wrapper_registry_num_by_ipaddr(const ipv6calc_ipaddr *ipaddrp) {
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		CONVERT_IPADDRP_IPV4ADDR(ipaddrp, ipv4addr)
		return(libipv6calc_db_wrapper_registry_num_by_ipv4addr(&ipv4addr));
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		CONVERT_IPADDRP_IPV6ADDR(ipaddrp, ipv6addr)
		return(libipv6calc_db_wrapper_registry_num_by_ipv6addr(&ipv6addr));
	} else {
		ERRORPRINT_WA("unsupported proto=%d (FIX CODE)", ipaddrp->proto);
		exit(EXIT_FAILURE);
	};
};

/*
 * get registry string by IP address
 */
int libipv6calc_db_wrapper_registry_string_by_ipaddr(const ipv6calc_ipaddr *ipaddrp, char *resultstring, const size_t resultstring_length) {
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		CONVERT_IPADDRP_IPV4ADDR(ipaddrp, ipv4addr)
		return(libipv6calc_db_wrapper_registry_string_by_ipv4addr(&ipv4addr, resultstring, resultstring_length));
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		CONVERT_IPADDRP_IPV6ADDR(ipaddrp, ipv6addr)
		return(libipv6calc_db_wrapper_registry_string_by_ipv6addr(&ipv6addr, resultstring, resultstring_length));
	} else {
		ERRORPRINT_WA("unsupported proto=%d (FIX CODE)", ipaddrp->proto);
		exit(EXIT_FAILURE);
	};
};


/*
 * get CountryCode in text form
 * in: ipaddrp, length
 * mod: string, data_source_ptr (if != NULL)
 * return: 0=ok
 */
int libipv6calc_db_wrapper_country_code_by_addr(char *string, const int length, const ipv6calc_ipaddr *ipaddrp, unsigned int *data_source_ptr) {
	unsigned int data_source = IPV6CALC_DB_SOURCE_UNKNOWN;
	int f = 0, p, result = -1;

#if defined SUPPORT_GEOIP || defined SUPPORT_IP2LOCATION
	char tempstring[IPV6CALC_ADDR_STRING_MAX] = "";
	char *result_char_ptr = NULL;
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	if (string == NULL) {
		ERRORPRINT_NA("given pointer 'string' is NULL (FIX CODE)");
		exit(EXIT_FAILURE);
	};

	if (length < 3) {
		ERRORPRINT_NA("given 'length' < 3 (FIX CODE)");
		exit(EXIT_FAILURE);
	};

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		f = IPV6CALC_DB_FEATURE_NUM_IPV4_TO_CC;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address: %08x", (unsigned int) ipaddrp->addr[0]);
		if ((ipaddrp->typeinfo1 & IPV4_ADDR_RESERVED) != 0) {
			// reserved IPv4 address has no country
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address: %08x is reserved (skip CountryCode lookup)", (unsigned int) ipaddrp->addr[0]);
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		f = IPV6CALC_DB_FEATURE_NUM_IPV6_TO_CC;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv6 address prefix (0-63): %08x%08x", (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
		if ((ipaddrp->typeinfo1 & IPV6_ADDR_RESERVED) != 0) {
			// reserved IPv4 address has no country
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv6 address prefix (0-63): %08x%08x is reserved (skip CountryCode lookup)", (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		ERRORPRINT_WA("unsupported proto=%d (FIX CODE)", ipaddrp->proto);
		exit(EXIT_FAILURE);
	};

	// run through priorities
	for (p = 0; p < IPV6CALC_DB_PRIO_MAX; p++) {
#if defined SUPPORT_GEOIP || defined SUPPORT_IP2LOCATION
		switch(wrapper_features_selector[f][p]) {
		    case IPV6CALC_DB_SOURCE_GEOIP:
		    case IPV6CALC_DB_SOURCE_IP2LOCATION:
			// need IP address as string
			if (strlen(tempstring) == 0) {
				libipaddr_ipaddrstruct_to_string(ipaddrp, tempstring, sizeof(tempstring), 0);
			};
		};
#endif

		switch(wrapper_features_selector[f][p]) {
		    case 0:
			// last
			goto END_libipv6calc_db_wrapper; // ok
			break;

		    case IPV6CALC_DB_SOURCE_GEOIP:
			if (wrapper_GeoIP_status == 1) {
#ifdef SUPPORT_GEOIP
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call now GeoIP with %s", tempstring);

				result_char_ptr = (char *) libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(tempstring, ipaddrp->proto);

				if (result_char_ptr != NULL) {
					snprintf(string, length, "%s", result_char_ptr);
					result = 0;
					data_source = IPV6CALC_DB_SOURCE_GEOIP;
					goto END_libipv6calc_db_wrapper; // ok
				} else {
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called GeoIP did not return a valid country_code");
				};
#endif
			};
			break;

		    case IPV6CALC_DB_SOURCE_IP2LOCATION:
			if (wrapper_IP2Location_status == 1) {
#ifdef SUPPORT_IP2LOCATION
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call now IP2Location with %s", tempstring);

				result_char_ptr = libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr(tempstring, ipaddrp->proto);
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called IP2Location returned: %s", result_char_ptr);

				if (result_char_ptr != NULL) {
					snprintf(string, length, "%s", result_char_ptr);
					result = 0;
					data_source = IPV6CALC_DB_SOURCE_IP2LOCATION;
					goto END_libipv6calc_db_wrapper; // ok
				} else {
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called IP2Location did not return a valid country_code");
				};
#endif
			};
			break;

		    case IPV6CALC_DB_SOURCE_DBIP:
			if (wrapper_DBIP_status == 1) {
#ifdef SUPPORT_DBIP
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now DBIP");

				int ret = libipv6calc_db_wrapper_DBIP_wrapper_country_code_by_addr(ipaddrp, string, length);
				if (ret == 0) {
					result = 0;
					data_source = IPV6CALC_DB_SOURCE_DBIP;
					goto END_libipv6calc_db_wrapper; // ok
				} else {
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called db-ip.com did not return a valid country_code");
				};
#endif
			};
			break;

		    case IPV6CALC_DB_SOURCE_EXTERNAL:
			if (wrapper_External_status == 1) {
#ifdef SUPPORT_EXTERNAL
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now External");

				int ret = libipv6calc_db_wrapper_External_country_code_by_addr(ipaddrp, string, length);

				if (ret == 0) {
					result = 0;
					data_source = IPV6CALC_DB_SOURCE_EXTERNAL;
					goto END_libipv6calc_db_wrapper; // ok
				} else {
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called External did not return a valid country_code");
				};
#endif
			};
			break;

		    default:
			goto END_libipv6calc_db_wrapper; // dummy goto in case no db is enabled
			break;
		};
	};

END_libipv6calc_db_wrapper:
	if (result == 0) {
		if (data_source_ptr != NULL) {
			// set data_source if pointer not NULL
			*data_source_ptr = data_source;
		};
	} else {
		// clear string
		snprintf(string, length, "%s", "");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: %s (data_source=%d)", string, data_source);

	return(result);
};


/*
 * get CountryCode in special internal form (index) [A-Z] (26) x [0-9A-Z] (36)
 */
uint16_t libipv6calc_db_wrapper_cc_index_by_addr(const ipv6calc_ipaddr *ipaddrp, unsigned int *data_source_ptr) {
	uint16_t index = COUNTRYCODE_INDEX_UNKNOWN;
	char cc_text[256] = "";
	uint8_t c1, c2;
	int r;

	int cache_hit = 0;

	static ipv6calc_ipaddr ipaddr_cache_lastused;
	static uint16_t cc_index_lastused;
	static unsigned int data_source_lastused = IPV6CALC_DB_SOURCE_UNKNOWN;
	static int ipaddr_cache_lastused_valid = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%08x%08x%08x%08x proto=%d", ipaddrp->addr[0], ipaddrp->addr[1], ipaddrp->addr[2], ipaddrp->addr[3], ipaddrp->proto);

	if ((ipaddr_cache_lastused_valid == 1)
	    &&	(ipaddr_cache_lastused.proto == ipaddrp->proto)
	    && 	(ipaddr_cache_lastused.addr[0] == ipaddrp->addr[0])
	    && 	(ipaddr_cache_lastused.addr[1] == ipaddrp->addr[1])
	    && 	(ipaddr_cache_lastused.addr[2] == ipaddrp->addr[2])
	    && 	(ipaddr_cache_lastused.addr[3] == ipaddrp->addr[3])
	) {
		index = cc_index_lastused;

		// set only data_source from cache if caller request it
		if (data_source_ptr != NULL) {
			*data_source_ptr = data_source_lastused;
		};

		cache_hit = 1;
		goto END_libipv6calc_db_wrapper_cached;
	} else {
		// retrieve always data_source for caching
		r = libipv6calc_db_wrapper_country_code_by_addr(cc_text, sizeof(cc_text), ipaddrp, &data_source_lastused);
		if (r != 0) {
			goto END_libipv6calc_db_wrapper_cached; // something wrong
		};

		if (strlen(cc_text) == 2) {
			if (isalpha(cc_text[0]) && isalnum(cc_text[1])) {
				c1 = toupper(cc_text[0]);
				if (! (c1 >= 'A' && c1 <= 'Z')) {
					goto END_libipv6calc_db_wrapper_cached; // something wrong
				};
				c1 -= 'A';

				c2 = toupper(cc_text[1]);
				if (c2 >= '0' && c2 <= '9') {
					c2 -= '0';
				} else if (c2 >= 'A' && c2 <= 'Z') {
					c2 -= 'A';
					c2 += 10;
				} else {
					goto END_libipv6calc_db_wrapper_cached; // something wrong
				};

				index = c1 + c2 * COUNTRYCODE_LETTER1_MAX;

				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "c1=%d c2=%d index=%d (0x%03x) -> test: %c%c", c1, c2, index, index, COUNTRYCODE_INDEX_TO_CHAR1(index), COUNTRYCODE_INDEX_TO_CHAR2(index));

				if (index >= COUNTRYCODE_INDEX_MAX) {
					index = COUNTRYCODE_INDEX_UNKNOWN; // failsafe
					ERRORPRINT_WA("unexpected index (too high): %d", index);
					goto END_libipv6calc_db_wrapper_cached; // something wrong
				};
			};
		} else {
			ERRORPRINT_WA("returned cc_text has not 2 chars: %s", cc_text);
			goto END_libipv6calc_db_wrapper_cached; // something wrong
		};

		// store in last used cache
		ipaddr_cache_lastused_valid = 1;
		cc_index_lastused = index;
		ipaddr_cache_lastused = *ipaddrp;
	};

END_libipv6calc_db_wrapper_cached:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: addr=%08x%08x%08x%08x cc_index=%d (0x%03x) %c%c%s", ipaddrp->addr[0], ipaddrp->addr[1], ipaddrp->addr[2], ipaddrp->addr[3], index, index, COUNTRYCODE_INDEX_TO_CHAR1(index), COUNTRYCODE_INDEX_TO_CHAR2(index), (cache_hit == 1 ? " (cached)" : ""));

	return(index);
};


/*
 * get country code string by index
 */
int libipv6calc_db_wrapper_country_code_by_cc_index(char *string, const int length, const uint16_t cc_index) {
	int result = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with cc_index=%d", cc_index);

	if (cc_index <= COUNTRYCODE_INDEX_LETTER_MAX) {
		snprintf(string, length, "%c%c", COUNTRYCODE_INDEX_TO_CHAR1(cc_index), COUNTRYCODE_INDEX_TO_CHAR2(cc_index));
	} else if (cc_index == COUNTRYCODE_INDEX_UNKNOWN) {
		snprintf(string, length, "unknown");
	} else if ((cc_index >= COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN) && (cc_index <= COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MAX)) {
		snprintf(string, length, "unknown");
	} else {
		snprintf(string, length, "unsupported");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Return country code: %s", string);
	return(result);
};


/*
 * get AS information in text form
 */
char *libipv6calc_db_wrapper_as_text_by_addr(const ipv6calc_ipaddr *ipaddrp) {
	char *result_char_ptr = NULL;

#if defined SUPPORT_GEOIP
	char tempstring[IPV6CALC_ADDR_STRING_MAX] = "";
#endif

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%04x%04x%04x%04x proto=%d", ipaddrp->addr[0], ipaddrp->addr[1], ipaddrp->addr[2], ipaddrp->addr[3], ipaddrp->proto);

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		if ((ipaddrp->typeinfo1 & IPV4_ADDR_RESERVED) != 0) {
			// reserved IPv4 address has no AS
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address: %08x is reserved (skip AS lookup)", (unsigned int) ipaddrp->addr[0]);
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		if ((ipaddrp->typeinfo1 & IPV6_ADDR_RESERVED) != 0) {
			// reserved IPv4 address has no AS
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv6 address prefix (0-63): %08x%08x is reserved (skip AS lookup)", (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		ERRORPRINT_WA("unsupported proto=%d (FIX CODE)", ipaddrp->proto);
		exit(EXIT_FAILURE);
	};

	if (wrapper_GeoIP_status == 1) {
#ifdef SUPPORT_GEOIP
		// conversion sto string needed for GeoIP
		if (strlen(tempstring) == 0) {
			libipaddr_ipaddrstruct_to_string(ipaddrp, tempstring, sizeof(tempstring), 0);
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Call now GeoIP with addr=%s proto=%d)", tempstring, ipaddrp->proto);

		result_char_ptr = libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(tempstring, ipaddrp->proto);
#endif
	};

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: %s", result_char_ptr);

	return(result_char_ptr);
};



/*
 * get AS 32-bit number
 */
uint32_t libipv6calc_db_wrapper_as_num32_by_addr(const ipv6calc_ipaddr *ipaddrp) {
	char *as_text;
	char as_number_string[11];  // max: 4294967295 = 10 digits + \0
	uint32_t as_num32 = ASNUM_AS_UNKNOWN; // default
	int valid = 1;
	unsigned int s;

	int cache_hit = 0;

	static ipv6calc_ipaddr ipaddr_cache_lastused;
	static uint32_t as_num32_lastused;
	static int ipaddr_cache_lastused_valid = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%08x%08x%08x%08x proto=%d", ipaddrp->addr[0], ipaddrp->addr[1], ipaddrp->addr[2], ipaddrp->addr[3], ipaddrp->proto);

	if (ipaddrp->proto == IPV6CALC_PROTO_IPV4) {
		if ((ipaddrp->typeinfo1 & IPV4_ADDR_RESERVED) != 0) {
			// reserved IPv4 address has no AS
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address: %08x is reserved (skip AS lookup)", (unsigned int) ipaddrp->addr[0]);
			goto END_libipv6calc_db_wrapper;
		};
	} else if (ipaddrp->proto == IPV6CALC_PROTO_IPV6) {
		if ((ipaddrp->typeinfo1 & IPV6_ADDR_RESERVED) != 0) {
			// reserved IPv4 address has no AS
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv6 address prefix (0-63): %08x%08x is reserved (skip AS lookup)", (unsigned int) ipaddrp->addr[0], (unsigned int) ipaddrp->addr[1]);
			goto END_libipv6calc_db_wrapper;
		};
	} else {
		ERRORPRINT_WA("unsupported proto=%d (FIX CODE)", ipaddrp->proto);
		exit(EXIT_FAILURE);
	};

	if ((ipaddr_cache_lastused_valid == 1)
	    &&	(ipaddr_cache_lastused.proto == ipaddrp->proto)
	    && 	(ipaddr_cache_lastused.addr[0] == ipaddrp->addr[0])
	    && 	(ipaddr_cache_lastused.addr[1] == ipaddrp->addr[1])
	    && 	(ipaddr_cache_lastused.addr[2] == ipaddrp->addr[2])
	    && 	(ipaddr_cache_lastused.addr[3] == ipaddrp->addr[3])
	) {
		as_num32 = as_num32_lastused;
		cache_hit = 1;
	} else {
		// TODO: switch mechanism depending on backend (GeoIP supports AS only by text representation)
		as_text = libipv6calc_db_wrapper_as_text_by_addr(ipaddrp);

		if ((as_text != NULL) && (strncmp(as_text, "AS", 2) == 0) && (strlen(as_text) > 2)) {
			// catch AS....
			for (s = 0; s < (strlen(as_text) - 2); s++) {
				if ((as_text[s+2] == ' ') || (as_text[s+2] == '\0')) {
					break;
				} else if (isdigit(as_text[s+2])) {
					continue;
				} else {
					// something wrong
					valid = 0;
					break;
				};
			};

			if (s > 10) {
				// too many digits
				valid = 0;
			};

			if (valid == 1) {
				snprintf(as_number_string, 11, "%s", as_text + 2);
				as_num32 = atol(as_number_string);
			};
		};

		// store in last used cache
		ipaddr_cache_lastused_valid = 1;
		as_num32_lastused = as_num32;
		ipaddr_cache_lastused = *ipaddrp;
	};

END_libipv6calc_db_wrapper:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: addr=%08x%08x%08x%08x as_num32=%d (0x%08x)%s", ipaddrp->addr[0], ipaddrp->addr[1], ipaddrp->addr[2], ipaddrp->addr[3], as_num32, as_num32, (cache_hit == 1 ? " (cached)" : ""));

	return(as_num32);
};


/*
 * get AS 16-bit number
 */
uint16_t libipv6calc_db_wrapper_as_num16_by_addr(const ipv6calc_ipaddr *ipaddrp) {
	uint16_t as_num16 = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%08x%08x%08x%08x proto=%d", ipaddrp->addr[0], ipaddrp->addr[1], ipaddrp->addr[2], ipaddrp->addr[3], ipaddrp->proto);

	// get 32-bit ASN
	uint32_t as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(ipaddrp);

	as_num16 = (uint16_t) (as_num32 < 65536 ? as_num32 : ASNUM_AS_TRANS);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: %d (0x%04x)", as_num16, as_num16);

	// return 16-bit ASN or AS_TRANS in case of > 16-bit
	return(as_num16);
};


/*
 * compress AS 32-bit number to 17 bit
 */
uint32_t libipv6calc_db_wrapper_as_num32_comp17(const uint32_t as_num32) {;
	uint32_t as_num32_comp17 = 0;
	uint32_t as_num32_comp17_reg = 0;
	uint32_t as_num32_comp17_asn = 0;

	if (as_num32 <= 0xffff) {
		as_num32_comp17 = as_num32;
	} else {
		if ((as_num32 & 0x00070000) == (as_num32 & 0xfff70000)) {
			// 3 of 16 MSB bits active (which are  at least in 2013 maped 1:1 to related registry)
			as_num32_comp17_reg = (as_num32 & 0x0007000) >> 3;
		} else {
			// map to unknown registry
			as_num32_comp17_reg = 0;
		};

		if ((as_num32 & 0x0fff) == (as_num32 & 0xffff)) {
			// only 12 of 16 LSB bits active
			as_num32_comp17_asn = as_num32 & 0x0fff;
		} else {
			// more than 12 bits are in use, unspecified result, but keeping registry and set special flag
			as_num32_comp17_asn = 0x1000;
		};

		// fill compressed value and set flag
		as_num32_comp17 = as_num32_comp17_reg | as_num32_comp17_asn | 0x00010000;
	};

	return(as_num32_comp17);
};


/*
 * Decompress AS 32-bit number from 17 bit
 */
uint32_t libipv6calc_db_wrapper_as_num32_decomp17(const uint32_t as_num32_comp17) {;
	uint32_t as_num32 = ASNUM_AS_UNKNOWN;

	if ((as_num32_comp17 & 0x00010000) == 0x00000000) {
		as_num32 = as_num32_comp17;
	} else {
		if ((as_num32_comp17 & 0xe000) == 0x0000) {
			as_num32 = ASNUM_AS_UNKNOWN;
		} else {
			as_num32 |= (as_num32_comp17 & 0xe000) << 3;

			if ((as_num32_comp17 & 0x1000) == 0x1000) {
				// keep only ASN registry
			} else {
				as_num32 |= (as_num32_comp17 & 0x0fff);
			};
		};
	};

	return(as_num32);
};


/*
 * Get IEEE vendor string
 * in:  macaddrp
 * mod: resultstring
 * out: 0=found, 1=not found
 */
int libipv6calc_db_wrapper_ieee_vendor_string_by_macaddr(char *resultstring, const size_t resultstring_length, const ipv6calc_macaddr *macaddrp) {
	int retval = 1;
#ifdef SUPPORT_BUILTIN
	retval = libipv6calc_db_wrapper_BuiltIn_ieee_vendor_string_by_macaddr(resultstring, resultstring_length, macaddrp);
#endif
	return (retval);
};


/*
 * Get short IEEE vendor string
 * in:  macaddrp
 * mod: resultstring
 * out: 0=found, 1=not found
 */
int libipv6calc_db_wrapper_ieee_vendor_string_short_by_macaddr(char *resultstring, const size_t resultstring_length, const ipv6calc_macaddr *macaddrp) {
	int retval = 1;
#ifdef SUPPORT_BUILTIN
	retval = libipv6calc_db_wrapper_BuiltIn_ieee_vendor_string_short_by_macaddr(resultstring, resultstring_length, macaddrp);
#endif
	return (retval);
};


/********************************************
 * IPv4/IPv6 -> Registry lookup
 ********************************************/

/*
 * Get reserved IPv4 address information as string
 * ret: NULL: not reserved, !=NULL: pointer to string
 */
static const char *libipv6calc_db_wrapper_reserved_string_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp) {
	const char *info = NULL;

	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address: %08x", (unsigned int) ipv4);

	// see also: https://en.wikipedia.org/wiki/Reserved_IP_addresses

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
 * Get reserved IPv6 address information as string
 * ret: NULL: not reserved, !=NULL: pointer to string
 */
static const char *libipv6calc_db_wrapper_reserved_string_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp) {
	const char *info = NULL;

	uint32_t ipv6_00_31 = ipv6addr_getdword(ipv6addrp, 0);
	uint32_t ipv6_32_63 = ipv6addr_getdword(ipv6addrp, 1);
	uint32_t ipv6_64_95 = ipv6addr_getdword(ipv6addrp, 2);
	uint32_t ipv6_96_127 = ipv6addr_getdword(ipv6addrp, 3);
	
	uint16_t ipv6_00_15 = ipv6addr_getword(ipv6addrp, 0);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%08x%08x%08x%08x"
		, (unsigned int) ipv6_00_31
		, (unsigned int) ipv6_32_63
		, (unsigned int) ipv6_64_95
		, (unsigned int) ipv6_96_127
	);

	// see also: https://en.wikipedia.org/wiki/Reserved_IP_addresses
	//
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
	} else if ((ipv6_00_31 == 0x01000000) && (ipv6_32_63 == 0)) {
		// 0100::0/64 (RFC 6666)
		info = "reserved(RFC6666)";
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
 * get registry string of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = Registry string
 * ret: 0: ok, 1: unknown, 2: reserved
 */
int libipv6calc_db_wrapper_registry_string_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length) {
	int retval = 1;
	int registry;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	const char *info = libipv6calc_db_wrapper_reserved_string_by_ipv4addr(ipv4addrp);

	if (info != NULL) {
		// is reserved
		snprintf(resultstring, resultstring_length, "%s", info);
		retval = 2;
	} else {
		registry = libipv6calc_db_wrapper_registry_num_by_ipv4addr(ipv4addrp);
		snprintf(resultstring, resultstring_length, "%s", libipv6calc_registry_string_by_num(registry));
		if (registry != REGISTRY_UNKNOWN) {
			retval = 0;
		} else {
			retval = 1;
		};
	};

	return (retval);
};


/*
 * get registry number of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: registry number
 */
int libipv6calc_db_wrapper_registry_num_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp) {
	int retval = REGISTRY_UNKNOWN, p, f;

	int cache_hit = 0;

	static ipv6calc_ipv4addr cache_lu_ipv4addr;
	static uint32_t cache_lu_ipv4addr_registry_num;
	static int      cache_lu_ipv4addr_valid = 0;

#if defined SUPPORT_EXTERNAL
	ipv6calc_ipaddr ipaddr;
#endif

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%08x", ipv4addr_getdword(ipv4addrp));

	if ((cache_lu_ipv4addr_valid == 1)
	    && 	(memcmp(&cache_lu_ipv4addr.in_addr, &ipv4addrp->in_addr, sizeof(struct in_addr)) == 0)
	) {
		retval= cache_lu_ipv4addr_registry_num;
		cache_hit = 1;
		goto END_libipv6calc_db_wrapper_cached;
	};

	const char *info = libipv6calc_db_wrapper_reserved_string_by_ipv4addr(ipv4addrp);

	if (info != NULL) {
		retval = REGISTRY_RESERVED;
		goto END_libipv6calc_db_wrapper;
	};

	f = IPV6CALC_DB_FEATURE_NUM_IPV4_TO_REGISTRY;

	// run through priorities
	for (p = 0; p < IPV6CALC_DB_PRIO_MAX; p++) {
		switch(wrapper_features_selector[f][p]) {
		    case 0:
			// last
			goto END_libipv6calc_db_wrapper; // ok
			break;

		    case IPV6CALC_DB_SOURCE_BUILTIN:
			if (wrapper_BuiltIn_status == 1) {
#ifdef SUPPORT_BUILTIN
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now BuiltIn");

				retval = libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv4addr(ipv4addrp);
#endif
			};
			break;

		    case IPV6CALC_DB_SOURCE_EXTERNAL:
			if (wrapper_External_status == 1) {
#ifdef SUPPORT_EXTERNAL
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now External");
				CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr);
				retval = libipv6calc_db_wrapper_External_registry_num_by_addr(&ipaddr);
#endif
			};
			break;

		    default:
			goto END_libipv6calc_db_wrapper; // dummy goto in case no db is enabled
			break;
		};
	};

END_libipv6calc_db_wrapper:
	// store in last used cache
	cache_lu_ipv4addr_valid = 1;
	cache_lu_ipv4addr_registry_num = retval;
	cache_lu_ipv4addr.in_addr = ipv4addrp->in_addr;

END_libipv6calc_db_wrapper_cached:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: addr=%08x reg=%d%s"
		, ipv4addr_getdword(ipv4addrp)
		, retval
		, (cache_hit == 1 ? " (cached)" : "")
	);
	return (retval);
};


/*
 * get registry string of an IPv6 address
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = Registry string
 * ret: 0: ok, 1: unknown, 2: reserved
 */
int libipv6calc_db_wrapper_registry_string_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length) {
	int retval = 1;
	int registry;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	const char *info = libipv6calc_db_wrapper_reserved_string_by_ipv6addr(ipv6addrp);

	if (info != NULL) {
		// is reserved
		snprintf(resultstring, resultstring_length, "%s", info);
		retval = 2;
	} else {
		registry = libipv6calc_db_wrapper_registry_num_by_ipv6addr(ipv6addrp);
		snprintf(resultstring, resultstring_length, "%s", libipv6calc_registry_string_by_num(registry));

		if (registry != REGISTRY_UNKNOWN) {
			retval = 0;
		} else {
			retval = 1;
		};
	};

	return (retval);
};


/*
 * get registry number of an IPv6 address
 *
 * in:  ipv6addr = IPv6 address structure
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_registry_num_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp) {
	int retval = REGISTRY_UNKNOWN, p, f;

	int cache_hit = 0;

	static ipv6calc_ipv6addr cache_lu_ipv6addr;
	static uint32_t cache_lu_ipv6addr_registry_num;
	static int      cache_lu_ipv6addr_valid = 0;

#if defined SUPPORT_EXTERNAL
	ipv6calc_ipaddr ipaddr;
#endif

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%08x%08x%08x%08x", ipv6addr_getdword(ipv6addrp, 0), ipv6addr_getdword(ipv6addrp, 1), ipv6addr_getdword(ipv6addrp, 2), ipv6addr_getdword(ipv6addrp, 3));

	if ((cache_lu_ipv6addr_valid == 1)
	    && 	(memcmp(&cache_lu_ipv6addr.in6_addr, &ipv6addrp->in6_addr, sizeof(struct in6_addr)) == 0)
	) {
		retval= cache_lu_ipv6addr_registry_num;
		cache_hit = 1;
		goto END_libipv6calc_db_wrapper_cached;
	};

	const char *info = libipv6calc_db_wrapper_reserved_string_by_ipv6addr(ipv6addrp);

	if (info != NULL) {
		retval = REGISTRY_RESERVED;
		goto END_libipv6calc_db_wrapper;
	};

	if (ipv6addr_getword(ipv6addrp, 0) == 0x3ffe) {
		// special handling of 6BONE
		retval = REGISTRY_6BONE;
		goto END_libipv6calc_db_wrapper;
	};

	f = IPV6CALC_DB_FEATURE_NUM_IPV6_TO_REGISTRY;

	// run through priorities
	for (p = 0; p < IPV6CALC_DB_PRIO_MAX; p++) {
		switch(wrapper_features_selector[f][p]) {
		    case 0:
			// last
			goto END_libipv6calc_db_wrapper; // ok
			break;

		    case IPV6CALC_DB_SOURCE_BUILTIN:
			if (wrapper_BuiltIn_status == 1) {
#ifdef SUPPORT_BUILTIN
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now BuiltIn");

				retval = libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv6addr(ipv6addrp);
#endif
			};
			break;

		    case IPV6CALC_DB_SOURCE_EXTERNAL:
			if (wrapper_External_status == 1) {
#ifdef SUPPORT_EXTERNAL
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now External");
				CONVERT_IPV6ADDRP_IPADDR(ipv6addrp, ipaddr);
				retval = libipv6calc_db_wrapper_External_registry_num_by_addr(&ipaddr);
#endif
			};
			break;

		    default:
			goto END_libipv6calc_db_wrapper; // dummy goto in case no db is enabled
			break;
		};
	};

END_libipv6calc_db_wrapper:
	// store in last used cache
	cache_lu_ipv6addr_valid = 1;
	cache_lu_ipv6addr_registry_num = retval;
	cache_lu_ipv6addr.in6_addr = ipv6addrp->in6_addr;

END_libipv6calc_db_wrapper_cached:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: addr=%08x%08x%08x%08x reg=%d%s"
		, ipv6addr_getdword(ipv6addrp, 0)
		, ipv6addr_getdword(ipv6addrp, 1)
		, ipv6addr_getdword(ipv6addrp, 2)
		, ipv6addr_getdword(ipv6addrp, 3)
		, retval
		, (cache_hit == 1 ? " (cached)" : "")
	);
	return (retval);
};


/*
 * get info string of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = Registry string
 * ret: 0: ok
 */
int libipv6calc_db_wrapper_info_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, char *string, const size_t string_len) {
	int retval = 1, f, p;

#if defined SUPPORT_EXTERNAL
	ipv6calc_ipaddr ipaddr;
#endif

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%08x", ipv4addr_getdword(ipv4addrp));

	f = IPV6CALC_DB_FEATURE_NUM_IPV4_TO_INFO;

	// run through priorities
	for (p = 0; p < IPV6CALC_DB_PRIO_MAX; p++) {
		switch(wrapper_features_selector[f][p]) {
		    case 0:
			// last
			goto END_libipv6calc_db_wrapper; // ok
			break;

		    case IPV6CALC_DB_SOURCE_BUILTIN:
			if (wrapper_BuiltIn_status == 1) {
#ifdef SUPPORT_BUILTIN
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now BuiltIn");

				retval = libipv6calc_db_wrapper_BuiltIn_info_by_ipv4addr(ipv4addrp, string, string_len);
#endif
			};
			break;

		    case IPV6CALC_DB_SOURCE_EXTERNAL:
			if (wrapper_External_status == 1) {
#ifdef SUPPORT_EXTERNAL
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now External");
				CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr);
				retval = libipv6calc_db_wrapper_External_info_by_ipaddr(&ipaddr, string, string_len);
#endif
			};
			break;

		    default:
			goto END_libipv6calc_db_wrapper; // dummy goto in case no db is enabled
			break;
		};
	};

END_libipv6calc_db_wrapper:
	return (retval);
};


/*
 * get info string of an IPv6 address
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = Registry string
 * ret: 0: ok
 */
int libipv6calc_db_wrapper_info_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, char *string, const size_t string_len) {
	int retval = 1, p, f;

#if defined SUPPORT_EXTERNAL
	ipv6calc_ipaddr ipaddr;
#endif

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%08x%08x%08x%08x", ipv6addr_getdword(ipv6addrp, 0), ipv6addr_getdword(ipv6addrp, 1), ipv6addr_getdword(ipv6addrp, 2), ipv6addr_getdword(ipv6addrp, 3));

	f = IPV6CALC_DB_FEATURE_NUM_IPV6_TO_INFO;

	// run through priorities
	for (p = 0; p < IPV6CALC_DB_PRIO_MAX; p++) {
		switch(wrapper_features_selector[f][p]) {
		    case 0:
			// last
			goto END_libipv6calc_db_wrapper; // ok
			break;

		    case IPV6CALC_DB_SOURCE_BUILTIN:
			if (wrapper_BuiltIn_status == 1) {
#ifdef SUPPORT_BUILTIN
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now BuiltIn");

				retval = libipv6calc_db_wrapper_BuiltIn_info_by_ipv6addr(ipv6addrp, string, string_len);
#endif
			};
			break;

		    case IPV6CALC_DB_SOURCE_EXTERNAL:
			if (wrapper_External_status == 1) {
#ifdef SUPPORT_EXTERNAL
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now External");
				CONVERT_IPV6ADDRP_IPADDR(ipv6addrp, ipaddr);
				retval = libipv6calc_db_wrapper_External_info_by_ipaddr(&ipaddr, string, string_len);
#endif
			};
			break;

		    default:
			goto END_libipv6calc_db_wrapper; // dummy goto in case no db is enabled
			break;
		};
	};

END_libipv6calc_db_wrapper:
	return (retval);
};



#ifdef HAVE_BERKELEY_DB_SUPPORT
/********************************************
 * some generic Berkeley DB helper functions
 ********************************************/

/*
 * get value of token from Berkeley DB
 * in : DB pointer, token, max length of value
 * out: value
 * ret: 0=ok -1=error
 */
int libipv6calc_db_wrapper_bdb_get_data_by_key(DB *dbp, char *token, char *value, const size_t value_size) {
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: dbp=%p token=%s", dbp, token);

	DBT key, data;
	int ret;

	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	key.data = token;
	key.size = strlen(token);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "try to get key.data=%p key.size=%d", key.data, key.size);

	if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) != 0) {
		dbp->err(dbp, ret, "DB->get token=%s", token);
		return(-1);
	};

	snprintf(value, (data.size + 1) >= value_size ? value_size : data.size + 1, "%s", (char *) data.data);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Return: dbp=%p token=%s value=%s", dbp, token, value);

	return(0);
};



/*
 * generic fetch of a Berkeley DB row
 */
int libipv6calc_db_wrapper_bdb_fetch_row(
	DB 			*db_ptr,		// pointer to DB
	const uint8_t		db_format,		// DB format
	const long int		row,			// row number
	uint32_t		*data_1_00_31_ptr,	// data 1 (MSB in case of 64 bits)
	uint32_t		*data_1_32_63_ptr,	// data 1 (LSB in case of 64 bits)
	uint32_t		*data_2_00_31_ptr,	// data 2 (MSB in case of 64 bits)
	uint32_t		*data_2_32_63_ptr,	// data 2 (LSB in case of 64 bits)
	void			*data_ptr		// pointer to data
	) {

	int retval = -1;

	const char *db_format_row[] = {
		"%u;%u;%[^%]",			// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x2
		"%u;%u;%u;%u;%[^%]",		// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x4
		"%x;%x;%[^%]",			// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2
		"%x;%x;%x;%x;%[^%]",		// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4
		"%x;%x;%d;%[^%]",			// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x2
		"%x;%x;%x;%x;%d;%[^%]",		// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x4
		"0x%x;0x%x;%[^%]",			// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_PREF_32x2
		"0x%x;0x%x;0x%x;0x%x;%[^%]",	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_PREF_32x4
	};

	const int db_format_values[] = {
		3,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x2
		5,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x4
		3,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2
		5,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4
		4,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x2
		6,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x4
		3,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_PREF_32x2
		5,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_PREF_32x4
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called db_ptr=%p, data_ptr=%p", db_ptr, data_ptr);

	if (MAXENTRIES_ARRAY(db_format_row) != MAXENTRIES_ARRAY(db_format_values)) {
		ERRORPRINT_NA("inconsistent array definition (FIX CODE)");
		exit(EXIT_FAILURE);
	};

	if (db_format >= MAXENTRIES_ARRAY(db_format_row)) {
		ERRORPRINT_WA("unsupported db_format (FIX CODE): %u", db_format);
		exit(EXIT_FAILURE);
	};

	char datastring[NI_MAXHOST];

	DBT key, data;

	db_recno_t recno = row;

	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	key.data = &recno;
	key.size = sizeof(recno);

	int ret, value;

	// DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Try to get row");

	ret = db_ptr->get(db_ptr, NULL, &key, &data, 0);

	// DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "get executed");

	if (ret != 0) {
		db_ptr->err(db_ptr, ret, "DB->get");
		goto END_libipv6calc_db_wrapper_bdb_fetch_row;
	};

	// DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Got row");

	snprintf(datastring, (data.size + 1) >= sizeof(datastring) ? sizeof(datastring) : data.size + 1, "%s", (char *) data.data);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "start parsing database row %u: %s (size=%d) data_ptr=%p values=%u format=%s", recno, datastring, data.size, data_ptr, db_format_values[db_format], db_format_row[db_format]);

	if (db_format_values[db_format] == 3) {
		ret = sscanf(datastring, db_format_row[db_format], data_1_00_31_ptr, data_2_00_31_ptr, (char *) data_ptr);
		*data_1_32_63_ptr = 0;
		*data_2_32_63_ptr = 0;
	} else if (db_format_values[db_format] == 4) {
		ret = sscanf(datastring, db_format_row[db_format], data_1_00_31_ptr, data_2_00_31_ptr, &value, (char *) data_ptr);
		*data_1_32_63_ptr = 0;
		*data_2_32_63_ptr = 0;
	} else if (db_format_values[db_format] == 5) {
		ret = sscanf(datastring, db_format_row[db_format], data_1_00_31_ptr, data_1_32_63_ptr, data_2_00_31_ptr, data_2_32_63_ptr, (char *) data_ptr);
	} else if (db_format_values[db_format] == 6) {
		ret = sscanf(datastring, db_format_row[db_format], data_1_00_31_ptr, data_1_32_63_ptr, data_2_00_31_ptr, data_2_32_63_ptr, &value, (char *) data_ptr);
	} else {
		ERRORPRINT_WA("unsupported db_format_values (FIX CODE): %u", db_format_values[db_format]);
		exit(EXIT_FAILURE);
	};

	if (ret != db_format_values[db_format]) {
		ERRORPRINT_WA("row parsing error, corrupted database: %s (ret=%d format=%s expectedvalues=%d)", datastring, ret, db_format_row[db_format], db_format_values[db_format]);
		goto END_libipv6calc_db_wrapper_bdb_fetch_row;
	};

	if ((db_format == IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x4) \
	  || (db_format == IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_VALUE_32x4)) {
		retval = value; // for Longest Match
	} else {
		retval = 0; // ok
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "database row %lu parsed: data_1_00_31=%08x data_1_32_63=%08x data_2_00_31=%08x data_2_32_63=%08x value=%s)", row, *data_1_00_31_ptr, *data_1_32_63_ptr, *data_2_00_31_ptr, *data_2_32_63_ptr, (char *) data_ptr);
	
END_libipv6calc_db_wrapper_bdb_fetch_row:
	return(retval);
};
#endif // HAVE_BERKELEY_DB_SUPPORT


/*
 * generic internal/external database lookup function
 * return:	 -1 : no lookup result
 * 		>= 0: matching row
 */
long int libipv6calc_db_wrapper_get_entry_generic(
	void 		*db_ptr,		// pointer to database in case of IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB, otherwise NULL
	const uint8_t	data_ptr_type,		// type of data_ptr
	const uint8_t	data_key_type,		// key type
	const uint8_t	data_key_format,	// key format
	const uint8_t	data_key_length,	// key length
	const uint8_t	data_search_type,	// search type
	const uint32_t	data_num_rows,		// number of rows
	const uint32_t	lookup_key_00_31,	// lookup key MSB
	const uint32_t	lookup_key_32_63,	// lookup key LSB
	void            *data_ptr,		// pointer to DB data in case of IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB, otherwise NULL
	int  (*get_array_row)()			// function to get array row
	) {

	int retval = -1;

	uint32_t value_first_00_31 = 0, value_last_00_31 = 0;
	uint32_t value_first_32_63 = 0, value_last_32_63 = 0;

#ifdef HAVE_BERKELEY_DB_SUPPORT
	DB *dbp = NULL;
#endif // HAVE_BERKELEY_DB_SUPPORT

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with data_ptr_type=%u data_key_type=%u data_key_format=%u, data_key_length=%u data_num_rows=%u lookup_key_00_31=%08lx lookup_key_32_63=%08lx db_ptr=%p, data_ptr=%p",
		data_ptr_type,
		data_key_type,
		data_key_format,
		data_key_length,
		data_num_rows,
		(long unsigned int) lookup_key_00_31,
		(long unsigned int) lookup_key_32_63,
		db_ptr,
		data_ptr
	);

	if (data_num_rows < 1) {
		ERRORPRINT_WA("unsupported data_key_num_rows (FIX CODE): %u", data_num_rows);
		exit(EXIT_FAILURE);
	};

	if (data_num_rows > INT32_MAX) {
		ERRORPRINT_WA("unsupported data_key_num_rows (FIX CODE): %u", data_num_rows);
		exit(EXIT_FAILURE);
	};

	/* check data_ptr_type */
	switch(data_ptr_type) {
	    case IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY:
		if (get_array_row == NULL) {
			ERRORPRINT_NA("get_array_row function is unexpected NULL (FIX CODE)");
			exit(EXIT_FAILURE);
		};

		if (data_ptr != NULL) {
			ERRORPRINT_NA("data_ptr is unexpected NOT NULL - not supported on IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY (FIX CODE)");
			exit(EXIT_FAILURE);
		};
		break;

#ifdef HAVE_BERKELEY_DB_SUPPORT
	    case IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB:
		if (get_array_row != NULL) {
			ERRORPRINT_NA("get_array_row function is unexpected NOT NULL (FIX CODE)");
			exit(EXIT_FAILURE);
		};

		if (data_ptr == NULL) {
			ERRORPRINT_NA("data_ptr is unexpected NULL - not supported on IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB (FIX CODE)");
			exit(EXIT_FAILURE);
		};

		// supported
		dbp = (DB *) db_ptr; // map db_ptr to DB ptr
		break;
#endif // HAVE_BERKELEY_DB_SUPPORT

	    default:
		ERRORPRINT_WA("unsupported data_ptr_type (FIX CODE): %u", data_ptr_type);
		exit(EXIT_FAILURE);
		break;
	};

	/* check data_key_length */
	switch(data_key_length) {
	    case 32:
	    case 64:
		// supported
		break;

	    default:
		ERRORPRINT_WA("unsupported data_key_length (FIX CODE): %d", data_key_length);
		exit(EXIT_FAILURE);
	};

	/* check data search type */
	switch(data_search_type) {
	    case IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY:
	    case IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST:
		// supported
		break;

	    default:
		ERRORPRINT_WA("unsupported data_search_type (FIX CODE): %d", data_search_type);
		exit(EXIT_FAILURE);
	};

	/* check data_key_type */
	switch(data_key_type) {
	    case IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST:
	    case IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK:
		// supported
		break;

	    default:
		ERRORPRINT_WA("unsupported data_key_type (FIX CODE): %u", data_key_type);
		exit(EXIT_FAILURE);
	};


	long int i = -1;
	long int match = -1;
	int seqlongest = -1;
	long int i_min, i_max, i_old, i_old2;
	int search_binary_count = 0;
	int search_binary_count_max = (int) uint64_log2(data_num_rows) + 1;

	int ret = -1;

	i_min = 0; i_max = data_num_rows - 1; i_old = -1; i_old2 = -1;


	if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
		// binary search in provided data
		i = i_max / 2;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Start binary search over entries: data_num_rows=%u", data_num_rows);
	} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
		// sequential search in provided data
		i_old = i_max;
		i = 0;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Start sequential search over entries: data_num_rows=%u", data_num_rows);
	};

	while (i_old != i) {
		if ((i >= (int32_t) data_num_rows) || (i < 0)) {
			ERRORPRINT_WA("i out of range (FIX CODE): i=%ld data_num_rows=%u", i, data_num_rows);
			exit(EXIT_FAILURE);
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Analyze entry i=%ld", i);

		if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY) {
			ret = get_array_row(i, &value_first_00_31, &value_first_32_63, &value_last_00_31, &value_last_32_63);
			if (ret < 0) {
				ERRORPRINT_WA("can't retrieve keys from array for row: %lu", i);
				exit(EXIT_FAILURE);
			};	
#ifdef HAVE_BERKELEY_DB_SUPPORT	
		} else if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Try to get row from Berkeley DB dbp=%p row=%ld", dbp, i + 1);
			ret = libipv6calc_db_wrapper_bdb_fetch_row(
				dbp,				// pointer to DB
				data_key_format,		// DB format
				(long unsigned int) i + 1,	// row number (BDB starts always with 1, so add offset)
				&value_first_00_31,		// data 1 (MSB in case of 64 bits)
				&value_first_32_63,		// data 1 (LSB in case of 64 bits)
				&value_last_00_31,		// data 2 (MSB in case of 64 bits)
				&value_last_32_63,		// data 2 (LSB in case of 64 bits)
				data_ptr			// pointer to data
			);

			if (ret < 0) {
				ERRORPRINT_WA("can't retrieve keys from data for row: %ld", i);
				exit(EXIT_FAILURE);
			};
#endif // HAVE_BERKELEY_DB_SUPPORT
		};

		if (data_key_length == 32) {
			if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Test %08x range %08x - %08x i=%ld i_min=%ld i_max=%ld",
					(unsigned int) lookup_key_00_31,
					(unsigned int) value_first_00_31,
					(unsigned int) value_last_00_31,
					i, i_min, i_max);
			} else if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Test %08x base/mask %08x/%08x i=%ld i_min=%ld i_max=%ld",
					(unsigned int) lookup_key_00_31,
					(unsigned int) value_first_00_31,
					(unsigned int) value_last_00_31,
					 i, i_min, i_max);
			};
		} else if (data_key_length == 64) {
			if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Test %08x:%08x range %08x:%08x - %08x:%08x i=%ld i_min=%ld i_max=%ld",
					(unsigned int) lookup_key_00_31,
					(unsigned int) lookup_key_32_63,
					(unsigned int) value_first_00_31,
					(unsigned int) value_first_32_63,
					(unsigned int) value_last_00_31,
					(unsigned int) value_last_32_63,
					 i, i_min, i_max);
			} else if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Test %08x:%08x base/mask %08x:%08x/%08x:%08x i=%ld i_min=%ld i_max=%ld",
					(unsigned int) lookup_key_00_31,
					(unsigned int) lookup_key_32_63,
					(unsigned int) value_first_00_31,
					(unsigned int) value_first_32_63,
					(unsigned int) value_last_00_31,
					(unsigned int) value_last_32_63,
					 i, i_min, i_max);
			};
		};

		if (data_key_length == 32) {
			if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST) {
				if (lookup_key_00_31 < value_first_00_31) {
					// to high in array, jump down
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to high in array, jump down");
					i_max = i;
				} else if (lookup_key_00_31 > value_last_00_31) {
					// to low in array, jump up
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to low in array, jump up");
					i_min = i;
				} else {
					// hit
					match = i;
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// binary search in provided data
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "matching");
						break;
					} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
						// sequential search in provided data
						if (ret > seqlongest) {
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "matching, seqlongest old=%d new=%d", seqlongest, ret);
							seqlongest = ret;
						} else {
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "matching, keep seqlongest=%d", seqlongest);
						};
					};
				};
			} else if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK) {
				if ((lookup_key_00_31 & value_last_00_31) < value_first_00_31) {
					// to high in array, jump down
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to high in array, jump down");
					i_max = i;
				} else if ((lookup_key_00_31 & value_last_00_31) > value_first_00_31) {
					// to low in array, jump up
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to low in array, jump up");
					i_min = i;
				} else {
					// hit
					match = i;
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// binary search in provided data
						break;
					} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
						// sequential search in provided data
						if (ret > seqlongest) {
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "matching, seqlongest old=%d new=%d", seqlongest, ret);
							seqlongest = ret;
						} else {
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "matching, keep seqlongest=%d", seqlongest);
						};
					};
				};
			};
		} else if (data_key_length == 64) {
			if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST) {
				if (lookup_key_00_31 < value_first_00_31) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						/* to high in array, jump down */
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to high in array, jump down");
						i_max = i;
					};
				} else if (lookup_key_00_31 > value_last_00_31) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// to low in array, jump up
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to low in array, jump up");
						i_min = i;
					};
				} else if ((lookup_key_00_31 == value_first_00_31) && (lookup_key_32_63 < value_first_32_63)) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						/* to high in array, jump down */
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to high in array, jump down");
						i_max = i;
					};
				} else if ((lookup_key_00_31 == value_last_00_31) && (lookup_key_32_63 > value_last_32_63)) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// to low in array, jump up
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to low in array, jump up");
						i_min = i;
					};
				} else {
					// hit
					match = i;
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// binary search in provided data
						break;
					} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
						// sequential search in provided data
						if (ret > seqlongest) {
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "matching, seqlongest old=%d new=%d", seqlongest, ret);
							seqlongest = ret;
						} else {
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "matching, keep seqlongest=%d", seqlongest);
						};
					};
				};
			} else if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK) {
				if ((lookup_key_00_31 & value_last_00_31) < value_first_00_31) {
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "(lookup_key_00_31 & value_last_00_31) < value_first_00_31");
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						/* to high in array, jump down */
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to high in array, jump down");
						i_max = i;
					};
				} else if ((lookup_key_00_31 & value_last_00_31) > value_first_00_31) {
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "(lookup_key_00_31 & value_last_00_31) > value_first_00_31");
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// to low in array, jump up
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to low in array, jump up");
						i_min = i;
					};
				} else if (((lookup_key_00_31 & value_last_00_31) == value_first_00_31) && ((lookup_key_32_63 & value_last_32_63) < value_first_32_63)) {
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "(lookup_key_32_63 & value_last_32_63) < value_first_32_63");
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						/* to high in array, jump down */
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to high in array, jump down");
						i_max = i;
					};
				} else if (((lookup_key_00_31 & value_last_00_31) == value_first_00_31) && ((lookup_key_32_63 & value_last_32_63) > value_first_32_63)) {
					DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "(lookup_key_32_63 & value_last_32_63) > value_first_32_63");
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// to low in array, jump up
						DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "to low in array, jump up");
						i_min = i;
					};
				} else {
					// hit
					match = i;
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// binary search in provided data
						break;
					} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
						// sequential search in provided data
						if (ret > seqlongest) {
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "matching, seqlongest old=%d new=%d", seqlongest, ret);
							seqlongest = ret;
						} else {
							DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "matching, keep seqlongest=%d", seqlongest);
						};
					};
				};
			};
		};

		if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
			// binary search in provided data
			i_old2 = i_old;
			i_old = i;
			i = (i_max - i_min) / 2 + i_min;
			search_binary_count++;

			// jump to last entry in special way if needed, otherwise it's not reachable
			if ((i == i_old) && ((i + 1) == ((int32_t) data_num_rows - 1))) {
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "workaround for last entry activated");
				i = i_max;
			};

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "count=%d/%d i_old2=%ld i_old=%ld i_min=%ld i_max=%ld i=%ld", search_binary_count, search_binary_count_max, i_old2, i_old, i_min, i_max, i);

			if (i_old2 == i) {
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "loop in binary search detected, no match found");
				break;
			};

			if (search_binary_count > search_binary_count_max) {
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "limit of binary search reached, no match found");
				break;
			};

		} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
			// sequential search in provided data
			i++;
		};
	};

	if (match != -1) {
		retval = match;

		if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Entry matched: %ld", i);
			if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY) {
				// currently nothing to do
#ifdef HAVE_BERKELEY_DB_SUPPORT
			} else if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB) {
				// currently nothing to do
#endif // HAVE_BERKELEY_DB_SUPPORT
			};
		} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finally entry matched: %ld", match);
			if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY) {
				// currently nothing to do
#ifdef HAVE_BERKELEY_DB_SUPPORT
			} else if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB) {
				// fetch matching row
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Try to get row from Berkeley DB dbp=%p recno=%ld", dbp, match);
				ret = libipv6calc_db_wrapper_bdb_fetch_row(
					dbp,			// pointer to DB
					data_key_format,	// DB format
					match + 1,		// row number
					&value_first_00_31,	// data 1 (MSB in case of 64 bits)
					&value_first_32_63,	// data 1 (LSB in case of 64 bits)
					&value_last_00_31,	// data 2 (MSB in case of 64 bits)
					&value_last_32_63,	// data 2 (LSB in case of 64 bits)
					data_ptr		// pointer to data
				);

				if (ret < 0) {
					ERRORPRINT_WA("can't retrieve keys from data for row: %lu", match);
					exit(EXIT_FAILURE);
				};
#endif // HAVE_BERKELEY_DB_SUPPORT
			};
		};

		if (data_ptr != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished with success result (DB): match=%ld", match);
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished with NO SUCCESS result (DB)");
	};

//END_libipv6calc_db_wrapper_get_entry_generic:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Return: %d", retval);
	return(retval);
};


/*********** generic function **********************/
uint16_t libipv6calc_db_cc_to_index(const char *cc_text) {
	uint16_t index = COUNTRYCODE_INDEX_UNKNOWN;
	uint8_t c1, c2;

	if (cc_text == NULL) {
		ERRORPRINT_NA("input is NULL");
		goto END_libipv6calc_db_cc_to_index; // something wrong
	};

	if (strlen(cc_text) != 2) {
		ERRORPRINT_WA("input is not string with length 2: %s", cc_text);
		goto END_libipv6calc_db_cc_to_index; // something wrong
	};

	if ((! isalpha(cc_text[0])) || (! isalnum(cc_text[1]))) {
		ERRORPRINT_WA("input is not valid country code: %s", cc_text);
		goto END_libipv6calc_db_cc_to_index; // something wrong
	};

	c1 = toupper(cc_text[0]);
	if (! (c1 >= 'A' && c1 <= 'Z')) {
		ERRORPRINT_WA("input char 1 is not part of a valid country code: %s", cc_text);
		goto END_libipv6calc_db_cc_to_index; // something wrong
	};
	c1 -= 'A';

	c2 = toupper(cc_text[1]);
	if (c2 >= '0' && c2 <= '9') {
		c2 -= '0';
	} else if (c2 >= 'A' && c2 <= 'Z') {
		c2 -= 'A';
		c2 += 10;
	} else {
		ERRORPRINT_WA("input char 2 is not part of a valid country code: %s", cc_text);
		goto END_libipv6calc_db_cc_to_index; // something wrong
	};

	index = c1 + c2 * COUNTRYCODE_LETTER1_MAX;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "c1=%d c2=%d index=%d (0x%03x) -> test: %c%c", c1, c2, index, index, COUNTRYCODE_INDEX_TO_CHAR1(index), COUNTRYCODE_INDEX_TO_CHAR2(index));

	if (index >= COUNTRYCODE_INDEX_MAX) {
		index = COUNTRYCODE_INDEX_UNKNOWN; // failsafe
		ERRORPRINT_WA("unexpected index (too high): %d", index);
	};

END_libipv6calc_db_cc_to_index:
	return(index);
};


/***********************************************************/
/*********** filter based on database **********************/
/***********************************************************/

/*********** DB CC **********************/

/*
 * parse filter DB CC
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:skip 2:problem
 */
int libipv6calc_db_cc_filter_parse(s_ipv6calc_filter_db_cc *filter, const char *token, const int negate_flag) {
	int result = 1, negate = negate_flag, offset = 0;
	const char *filter_token = "cc=";
	const char *prefixdot = "db.";

	uint16_t cc_index;

	if (token == NULL) {
		return (result);
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "input: %s", token);

	if (token[0] == '^') {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "found negate prefix in token: %s", token);

		negate = 1;
		offset += 1;
	};

	if (strncmp(token + offset, prefixdot, strlen(prefixdot)) == 0) {
		/* prefix with dot found */
		offset += strlen(prefixdot);
		result = 2; /* token with prefix, result into problem if not found */

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "token with prefix, suffix: %s", token + offset);
	} else {
		/* prefix dot is required */
		// no match
		goto END_ipv6calc_db_cc_filter_parse;
	};

	if (strncmp(token + offset, filter_token, strlen(filter_token)) == 0) {
		/* filter token found */
		offset += strlen(filter_token);
		result = 2; /* filter token found */

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "filter token found, suffix: %s", token + offset);
	} else {
		// no match
		goto END_ipv6calc_db_cc_filter_parse;
	};

	if (strcmp(token + offset, "unknown") == 0) {
		cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	} else {
		if (strlen(token + offset) != 2) {
			ERRORPRINT_WA("filter token 'cc=' requires 2 char country code: %s:", token + offset);
			goto END_ipv6calc_db_cc_filter_parse;
		};

		cc_index = libipv6calc_db_cc_to_index(token + offset);

		if (cc_index == COUNTRYCODE_INDEX_UNKNOWN) {
			ERRORPRINT_WA("filter token 'cc=' requires a valid country code: %s:", token + offset);
			goto END_ipv6calc_db_cc_filter_parse;
		};
	};

	if (negate == 1) {
		if (filter->cc_may_not_have_max < IPV6CALC_FILTER_DB_CC_MAX) {
			filter->cc_may_not_have[filter->cc_may_not_have_max] = cc_index;
			filter->cc_may_not_have_max++;
		} else {
			ERRORPRINT_WA("filter token 'cc=' maxmimum reached for 'may not have': %d", filter->cc_may_not_have_max);
			goto END_ipv6calc_db_cc_filter_parse;
		};
	} else {
		if (filter->cc_must_have_max < IPV6CALC_FILTER_DB_CC_MAX) {
			filter->cc_must_have[filter->cc_must_have_max] = cc_index;
			filter->cc_must_have_max++;
		} else {
			ERRORPRINT_WA("filter token 'cc=' maxmimum reached for 'must have': %d", filter->cc_must_have_max);
			goto END_ipv6calc_db_cc_filter_parse;
		};
	};
	filter->active = 1;
	result = 0;

END_ipv6calc_db_cc_filter_parse:
	return (result);
};


/*
 * check filter DB CC
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:problem
 */
int libipv6calc_db_cc_filter_check(const s_ipv6calc_filter_db_cc *filter, const int proto) {
	int result = 0, r;
	char cc[IPV6CALC_COUNTRYCODE_STRING_MAX];

	DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper)
	char tempstring[NI_MAXHOST];
	char tempstring2[NI_MAXHOST];
	int i;

	if (filter->cc_must_have_max > 0) {
		tempstring2[0] = '\0';
		for (i = 0; i < filter->cc_must_have_max; i++) {
			libipv6calc_db_wrapper_country_code_by_cc_index(cc, sizeof(cc), filter->cc_must_have[i]);
			snprintf(tempstring, sizeof(tempstring), "%s%s%s", tempstring2, (i > 0) ? " " : "", cc);
			snprintf(tempstring2, sizeof(tempstring2), "%s", tempstring);
		};
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.cc filter 'must_have'   : %s", tempstring2);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.cc filter 'must_have'   : --"); 
	};

	if (filter->cc_may_not_have_max > 0) {
		tempstring2[0] = '\0';
		for (i = 0; i < filter->cc_may_not_have_max; i++) {
			libipv6calc_db_wrapper_country_code_by_cc_index(cc, sizeof(cc), filter->cc_may_not_have[i]);
			snprintf(tempstring, sizeof(tempstring), "%s%s%s", tempstring2, (i > 0) ? " " : "", cc);
			snprintf(tempstring2, sizeof(tempstring2), "%s", tempstring);
		};
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.cc filter 'may_not_have': %s", tempstring2);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.cc filter 'may_not_have': --"); 
	};
	DEBUGSECTION_END

	if (proto == IPV6CALC_PROTO_IPV4) {
		r = libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_CC);
	} else if (proto == IPV6CALC_PROTO_IPV6) {
		r = libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_CC);
	} else {
		ERRORPRINT_WA("unsupported proto (FIX CODE): %d", proto);
		result = 1;
		goto END_ipv6calc_db_cc_filter_check;
	};

	if (r == 1) {
		// ok
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "database layer supports IPv%d->CountryCode", proto);
	} else {
		ERRORPRINT_WA("database layer don't support IPv%d->CountryCode", proto);
		result = 1;
	};

END_ipv6calc_db_cc_filter_check:
	return(result);
};


/*
 * filter for CC
 *
 * in : cc_index   = country code
 * in : *filter    = filter structure
 * ret: 0=match 1=not match -1=neutral
 */
int libipv6calc_db_cc_filter(const uint16_t cc_index, const s_ipv6calc_filter_db_cc *filter) {
	int i, result = -1;

	char cc1[IPV6CALC_COUNTRYCODE_STRING_MAX], cc2[IPV6CALC_COUNTRYCODE_STRING_MAX];

	libipv6calc_db_wrapper_country_code_by_cc_index(cc1, sizeof(cc1), cc_index);

	if (filter->cc_must_have_max > 0) {
		result = 1;

		for (i = 0; i < filter->cc_must_have_max; i++) {
			libipv6calc_db_wrapper_country_code_by_cc_index(cc2, sizeof(cc2), filter->cc_must_have[i]);

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.cc filter: check %s against must-have: %s", cc1, cc2);
			if (cc_index == filter->cc_must_have[i]) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.cc filter: %s hits must-have: %s", cc1, cc2);
				// match MUST-HAVE
				result = 0;
				break;
			};
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.cc filter: no must-have defined");
	};

	if (filter->cc_may_not_have_max > 0) {
		for (i = 0; i < filter->cc_may_not_have_max; i++) {
			libipv6calc_db_wrapper_country_code_by_cc_index(cc2, sizeof(cc2), filter->cc_may_not_have[i]);

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.cc filter: check %s against may-not-have: %s", cc1, cc2);
			if (cc_index == filter->cc_may_not_have[i]) {
				// match MAY-NOT-HAVE
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.cc filter: %s hits may-not-have: %s", cc1, cc2);
				result = 1;
				break;
			};
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.cc filter: no may-not-have defined");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.cc filter final result: %d", result);
	return (result);
};


/*********** DB ASN **********************/

/*
 * parse filter DB ASN
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:skip 2:problem
 */
int libipv6calc_db_asn_filter_parse(s_ipv6calc_filter_db_asn *filter, const char *token, const int negate_flag) {
	int result = 1, negate = negate_flag, offset = 0;
	const char *filter_token = "asn=";
	const char *prefixdot = "db.";

	uint32_t asn;

	if (token == NULL) {
		return (result);
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "input: %s", token);

	if (token[0] == '^') {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "found negate prefix in token: %s", token);

		negate = 1;
		offset += 1;
	};

	if (strncmp(token + offset, prefixdot, strlen(prefixdot)) == 0) {
		/* prefix with dot found */
		offset += strlen(prefixdot);
		result = 2; /* token with prefix, result into problem if not found */

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "token with prefix, suffix: %s", token + offset);
	} else {
		/* prefix dot is required */
		// no match
		goto END_ipv6calc_db_asn_filter_parse;
	};

	if (strncmp(token + offset, filter_token, strlen(filter_token)) == 0) {
		/* filter token found */
		offset += strlen(filter_token);
		result = 2; /* filter token found */

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "filter token found, suffix: %s", token + offset);
	} else {
		// no match
		goto END_ipv6calc_db_asn_filter_parse;
	};

	if (strcmp(token + offset, "unknown") == 0) {
		asn = ASNUM_AS_UNKNOWN;
	} else {
		asn = (uint32_t) strtol(token + offset, NULL, 10);

		if (errno == ERANGE) {
			ERRORPRINT_WA("filter token 'asn=' requires a valid decimal number between 0 and %ul: %s:", (2^32) - 1, token + offset);
			goto END_ipv6calc_db_asn_filter_parse;
		};
	};

	if (negate == 1) {
		if (filter->asn_may_not_have_max < IPV6CALC_FILTER_DB_ASN_MAX) {
			filter->asn_may_not_have[filter->asn_may_not_have_max] = asn;
			filter->asn_may_not_have_max++;
		} else {
			ERRORPRINT_WA("filter token 'asn=' maxmimum reached for 'may not have': %d", filter->asn_may_not_have_max);
			goto END_ipv6calc_db_asn_filter_parse;
		};
	} else {
		if (filter->asn_must_have_max < IPV6CALC_FILTER_DB_ASN_MAX) {
			filter->asn_must_have[filter->asn_must_have_max] = asn;
			filter->asn_must_have_max++;
		} else {
			ERRORPRINT_WA("filter token 'asn=' maxmimum reached for 'must have': %d", filter->asn_must_have_max);
			goto END_ipv6calc_db_asn_filter_parse;
		};
	};
	filter->active = 1;
	result = 0;

END_ipv6calc_db_asn_filter_parse:
	return (result);
};


/*
 * check filter DB ASN
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:problem
 */
int libipv6calc_db_asn_filter_check(const s_ipv6calc_filter_db_asn *filter, const int proto) {
	int result = 0, r;

	DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper)
	char tempstring[NI_MAXHOST];
	char tempstring2[NI_MAXHOST];
	int i;
	if (filter->asn_must_have_max > 0) {
		tempstring2[0] = '\0';
		for (i = 0; i < filter->asn_must_have_max; i++) {
			snprintf(tempstring, sizeof(tempstring), "%s%s%u", tempstring2, (i > 0) ? " " : "", filter->asn_must_have[i]);
			snprintf(tempstring2, sizeof(tempstring2), "%s", tempstring);
		};
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.asn filter 'must_have'   : %s", tempstring2);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.asn filter 'must_have'   : --"); 
	};

	if (filter->asn_may_not_have_max > 0) {
		tempstring2[0] = '\0';
		for (i = 0; i < filter->asn_may_not_have_max; i++) {
			snprintf(tempstring, sizeof(tempstring), "%s%s%u", tempstring2, (i > 0) ? " " : "", filter->asn_may_not_have[i]);
			snprintf(tempstring2, sizeof(tempstring2), "%s", tempstring);
		};
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.asn filter 'may_not_have': %s", tempstring2);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.asn filter 'may_not_have': --"); 
	};
	DEBUGSECTION_END

	if (proto == IPV6CALC_PROTO_IPV4) {
		r = libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_AS);
	} else if (proto == IPV6CALC_PROTO_IPV6) {
		r = libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_AS);
	} else {
		ERRORPRINT_WA("unsupported proto (FIX CODE): %d", proto);
		result = 1;
		goto END_ipv6calc_db_asn_filter_check;
	};

	if (r == 1) {
		// ok
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "database layer supports IPv%d->ASN", proto);
	} else {
		ERRORPRINT_WA("database layer don't support IPv%d->ASN", proto);
		result = 1;
	};

END_ipv6calc_db_asn_filter_check:
	return(result);
};


/*
 * filter ASN
 *
 * in : asn        = ASN
 * in : *filter    = filter structure
 * ret: 0=match 1=not match -1=neutral
 */
int libipv6calc_db_asn_filter(const uint32_t asn, const s_ipv6calc_filter_db_asn *filter) {
	int i, result = -1;

	if (filter->asn_must_have_max > 0) {
		result = 1;

		for (i = 0; i < filter->asn_must_have_max; i++) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.asn filter: check %u against must-have: %u", asn, filter->asn_must_have[i]);
			if (asn == filter->asn_must_have[i]) {
				// match MUST-HAVE
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.asn filter: %u hits must-have: %u", asn, filter->asn_must_have[i]);
				result = 0;
			};
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.asn filter: no must-have defined");
	};

	if (filter->asn_may_not_have_max > 0) {
		for (i = 0; i < filter->asn_may_not_have_max; i++) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.asn filter: check %u against must-have: %u", asn, filter->asn_may_not_have[i]);
			if (asn == filter->asn_may_not_have[i]) {
				// match MAY-NOT-HAVE
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.asn filter: %u hits may-not-have: %u", asn, filter->asn_may_not_have[i]);
				result = 1;
			};
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.asn filter: no may-not-have defined");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.asn filter final result: %d", result);
	return (result);
};


/*********** DB Registry **********************/

/*
 * parse filter DB Registry
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:skip 2:problem
 */
int libipv6calc_db_registry_filter_parse(s_ipv6calc_filter_db_registry *filter, const char *token, const int negate_flag) {
	int result = 1, negate = negate_flag, offset = 0;
	const char *filter_token = "reg=";
	const char *prefixdot = "db.";

	int registry = 0, i;

	if (token == NULL) {
		return (result);
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "input: %s", token);

	if (token[0] == '^') {
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "found negate prefix in token: %s", token);

		negate = 1;
		offset += 1;
	};

	if (strncmp(token + offset, prefixdot, strlen(prefixdot)) == 0) {
		/* prefix with dot found */
		offset += strlen(prefixdot);
		result = 2; /* token with prefix, result into problem if not found */

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "token with prefix, suffix: %s", token + offset);
	} else {
		/* prefix dot is required */
		// no match
		goto END_ipv6calc_db_registry_filter_parse;
	};

	if (strncmp(token + offset, filter_token, strlen(filter_token)) == 0) {
		/* filter token found */
		offset += strlen(filter_token);
		result = 2; /* filter token found */

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "filter token found, suffix: %s", token + offset);
	} else {
		// no match
		goto END_ipv6calc_db_registry_filter_parse;
	};

	if (strcmp(token + offset, "unknown") == 0) {
		registry = REGISTRY_UNKNOWN;
	} else {
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_registries); i++) {
			if (strcasecmp(token + offset, ipv6calc_registries[i].token) == 0) {
				registry = ipv6calc_registries[i].number;
				break;
			};
		};

		if (registry == 0) {
			ERRORPRINT_WA("filter token 'reg=' requires a valid registry token: %s:", token + offset);
			goto END_ipv6calc_db_registry_filter_parse;
		};
	};

	if (negate == 1) {
		if (filter->registry_may_not_have_max < IPV6CALC_FILTER_DB_REGISTRY_MAX) {
			filter->registry_may_not_have[filter->registry_may_not_have_max] = registry;
			filter->registry_may_not_have_max++;
		} else {
			ERRORPRINT_WA("filter token 'reg=' maxmimum reached for 'may not have': %d", filter->registry_may_not_have_max);
			goto END_ipv6calc_db_registry_filter_parse;
		};
	} else {
		if (filter->registry_must_have_max < IPV6CALC_FILTER_DB_REGISTRY_MAX) {
			filter->registry_must_have[filter->registry_must_have_max] = registry;
			filter->registry_must_have_max++;
		} else {
			ERRORPRINT_WA("filter token 'reg=' maxmimum reached for 'must have': %d", filter->registry_must_have_max);
			goto END_ipv6calc_db_registry_filter_parse;
		};
	};
	filter->active = 1;
	result = 0;

END_ipv6calc_db_registry_filter_parse:
	return (result);
};


/*
 * check filter DB Registry
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:problem
 */
int libipv6calc_db_registry_filter_check(const s_ipv6calc_filter_db_registry *filter, const int proto) {
	int result = 0, r;

	DEBUGSECTION_BEGIN(DEBUG_libipv6calc_db_wrapper)
	char tempstring[NI_MAXHOST];
	char tempstring2[NI_MAXHOST];
	int i;
	if (filter->registry_must_have_max > 0) {
		tempstring2[0] = '\0';
		for (i = 0; i < filter->registry_must_have_max; i++) {
			snprintf(tempstring, sizeof(tempstring), "%s%s%s", tempstring2, (i > 0) ? " " : "", libipv6calc_registry_string_by_num(filter->registry_must_have[i]));
			snprintf(tempstring2, sizeof(tempstring2), "%s", tempstring);
		};
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.registry filter 'must_have'   : %s", tempstring2);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.registry filter 'must_have'   : --"); 
	};

	if (filter->registry_may_not_have_max > 0) {
		tempstring2[0] = '\0';
		for (i = 0; i < filter->registry_may_not_have_max; i++) {
			snprintf(tempstring, sizeof(tempstring), "%s%s%s", tempstring2, (i > 0) ? " " : "", libipv6calc_registry_string_by_num(filter->registry_may_not_have[i]));
			snprintf(tempstring2, sizeof(tempstring2), "%s", tempstring);
		};
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.registry filter 'may_not_have': %s", tempstring2);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.registry filter 'may_not_have': --"); 
	};
	DEBUGSECTION_END

	if (proto == IPV6CALC_PROTO_IPV4) {
		r = libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_REGISTRY);
	} else if (proto == IPV6CALC_PROTO_IPV6) {
		r = libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_REGISTRY);
	} else {
		ERRORPRINT_WA("unsupported proto (FIX CODE): %d", proto);
		result = 1;
		goto END_ipv6calc_db_registry_filter_check;
	};

	if (r == 1) {
		// ok
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "database layer supports IPv%d->Registry", proto);
	} else {
		ERRORPRINT_WA("database layer don't support IPv%d->Registry", proto);
		result = 1;
	};

END_ipv6calc_db_registry_filter_check:
	return(result);
};



/*
 * filter Registry
 *
 * in : registry        = Registry
 * in : *filter    = filter structure
 * ret: 0=match 1=not match -1=neutral
 */
int libipv6calc_db_registry_filter(const uint32_t registry, const s_ipv6calc_filter_db_registry *filter) {
	int i, result = -1;

	if (filter->registry_must_have_max > 0) {
		result = 1;

		for (i = 0; i < filter->registry_must_have_max; i++) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.reg filter: check %s against must-have: %s", libipv6calc_registry_string_by_num(registry), libipv6calc_registry_string_by_num(filter->registry_must_have[i]));
			if (registry == filter->registry_must_have[i]) {
				// match MUST-HAVE
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.reg filter: %s hits must-have: %s", libipv6calc_registry_string_by_num(registry), libipv6calc_registry_string_by_num(filter->registry_must_have[i]));
				result = 0;
			};
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.registry filter: no must-have defined");
	};

	if (filter->registry_may_not_have_max > 0) {
		for (i = 0; i < filter->registry_may_not_have_max; i++) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.reg filter: check %s against must-have: %s", libipv6calc_registry_string_by_num(registry), libipv6calc_registry_string_by_num(filter->registry_may_not_have[i]));
			if (registry == filter->registry_may_not_have[i]) {
				// match MAY-NOT-HAVE
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.reg filter: %s hits may-not-have: %s", libipv6calc_registry_string_by_num(registry), libipv6calc_registry_string_by_num(filter->registry_may_not_have[i]));
				result = 1;
			};
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "db.reg filter: no may-not-have defined");
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "db.reg filter final result: %d", result);
	return (result);
};


/*
 * return library version numeric
 */
uint32_t libipv6calc_db_lib_version_numeric(void) {
	return(IPV6CALC_PACKAGE_VERSION_NUMERIC);
}; 


/*
 * return library version as string
 */
const char *libipv6calc_db_lib_version_string(void) {
	return(IPV6CALC_PACKAGE_VERSION_STRING);

};


/*
 * return API version numeric
 */
uint32_t libipv6calc_db_api_version_numeric(void) {
	return(IPV6CALC_API_VERSION_NUMERIC);
};


/*
 * return API version as string
 */
const char *libipv6calc_db_api_version_string(void) {
	return(IPV6CALC_API_VERSION_STRING);
};
