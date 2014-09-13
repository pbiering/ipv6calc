/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.c
 * Version    : $Id: libipv6calc_db_wrapper.c,v 1.47 2014/09/13 21:15:06 ds6peter Exp $
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc database wrapper (for decoupling databases from main binary)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "config.h"

#include "libipv6calcdebug.h"
#include "libipv6calc.h"

#define _ipv6calcoptions_h_ 1	// don't read options
#include "ipv6calcoptions.h"

#include "libipv6calc_db_wrapper.h"
#include "libipv6calc_db_wrapper_GeoIP.h"
#include "libipv6calc_db_wrapper_IP2Location.h"
#include "libipv6calc_db_wrapper_DBIP.h"
#include "libipv6calc_db_wrapper_BuiltIn.h"

static int wrapper_GeoIP_disable = 0;
static int wrapper_IP2Location_disable = 0;
static int wrapper_DBIP_disable = 0;

static int wrapper_GeoIP_status = 0;
static int wrapper_IP2Location_status = 0;
static int wrapper_DBIP_status = 0;
static int wrapper_BuiltIn_status = 0;

uint32_t wrapper_features = 0;


/*
 * function initialise the main wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_init(void) {
	int result = 0, r;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

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
#endif // SUPPORT_GEOIP
	} else {
		NONQUIETPRINT_NA("Support for GeoIP disabled by option");
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
#endif // SUPPORT_IP2LOCATION
	} else {
		NONQUIETPRINT_NA("Support for IP2Location disabled by option");
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
#endif // SUPPORT_DBIP
	} else {
		NONQUIETPRINT_NA("Support for db-ip.com disabled by option");
	};

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

	return(result);
};


/*
 * function cleanup the main wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_cleanup(void) {
	int result = 0, r;

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
	char tempstring[NI_MAXHOST];

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

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Return");

	return;
};

/* function print feature string help */
void libipv6calc_db_wrapper_features_help(void) {
	int i;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_db_features); i++) {
		fprintf(stderr, "%-22s%c %s\n", ipv6calc_db_features[i].token, (wrapper_features & ipv6calc_db_features[i].number) ? '+' : '-', ipv6calc_db_features[i].explanation);
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Return");

	return;


};


/* function print db info */
void libipv6calc_db_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	printf("%sDB features: 0x%08x\n\n", prefix_string, wrapper_features);

#ifdef SUPPORT_GEOIP
	if (wrapper_GeoIP_disable != 1) {
		// Call GeoIP wrapper
		libipv6calc_db_wrapper_GeoIP_wrapper_print_db_info(level_verbose, prefix_string);
	} else {
		printf("GeoIP support available but disabled by option\n");
	};
	printf("\n");
#endif

#ifdef SUPPORT_IP2LOCATION
	if (wrapper_IP2Location_disable != 1) {
		// Call IP2Location wrapper
		libipv6calc_db_wrapper_IP2Location_wrapper_print_db_info(level_verbose, prefix_string);
	} else {
		printf("IP2Location support available but disabled by option\n");
	};
	printf("\n");
#endif

#ifdef SUPPORT_DBIP
	if (wrapper_DBIP_disable != 1) {
		// Call DBIP wrapper
		libipv6calc_db_wrapper_DBIP_wrapper_print_db_info(level_verbose, prefix_string);
	} else {
		printf("db-ip.com support available but disabled by option\n");
	};
	printf("\n");
#endif

#ifdef SUPPORT_BUILTIN
	// Call BuiltIn wrapper
	libipv6calc_db_wrapper_BuiltIn_wrapper_print_db_info(level_verbose, prefix_string);
	printf("\n");
#endif

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
 * get CountryCode in text form
 * TODO: add support for DB priority
 */
char *libipv6calc_db_wrapper_country_code_by_addr(const char *addr, const int proto, unsigned int *data_source_ptr) {
	char *result_char_ptr = NULL;
	unsigned int data_source = IPV6CALC_DB_UNKNOWN;

#ifdef SUPPORT_DBIP
	static char country_code[256] = "";
#endif

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	if (wrapper_GeoIP_status == 1) {
#ifdef SUPPORT_GEOIP
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now GeoIP");

		result_char_ptr = (char *) libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(addr, proto);

		if (result_char_ptr != NULL) {
			data_source = IPV6CALC_DB_GEOIP;
			goto END_libipv6calc_db_wrapper; // ok
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called GeoIP did not return a valid country_code");
		};
#endif
	};

	if (wrapper_IP2Location_status == 1) {
#ifdef SUPPORT_IP2LOCATION
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now IP2Location");

		result_char_ptr = libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr((char *) addr, proto);

		if (result_char_ptr != NULL) {
			data_source = IPV6CALC_DB_IP2LOCATION;
			goto END_libipv6calc_db_wrapper; // ok
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called IP2Location did not return a valid country_code");
		};
#endif
	};

	if (wrapper_DBIP_status == 1) {
#ifdef SUPPORT_DBIP
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now DBIP");

		country_code[0] = '\0'; // clear contents			

		int ret = libipv6calc_db_wrapper_DBIP_wrapper_country_code_by_addr((char *) addr, proto, country_code, sizeof(country_code));
		if (ret == 0) {
			result_char_ptr = country_code;
		};

		if (result_char_ptr != NULL) {
			data_source = IPV6CALC_DB_DBIP;
			goto END_libipv6calc_db_wrapper; // ok
		} else {
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called db-ip.com did not return a valid country_code");
		};
#endif
	};

	goto END_libipv6calc_db_wrapper; // dummy goto in case no db is enabled

END_libipv6calc_db_wrapper:
	if (data_source_ptr != NULL) {
		// set data_source if pointer not NULL
		*data_source_ptr = data_source;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: %s (data_source=%d)", result_char_ptr, data_source);

	return(result_char_ptr);
};


/*
 * get CountryCode in special internal form (index) [A-Z] (26) x [0-9A-Z] (36)
 */
uint16_t libipv6calc_db_wrapper_cc_index_by_addr(const char *addr, const int proto, unsigned int *data_source_ptr) {
	uint16_t index = COUNTRYCODE_INDEX_UNKNOWN;

	const char *cc_text = libipv6calc_db_wrapper_country_code_by_addr(addr, proto, data_source_ptr);
	uint8_t c1, c2;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	if ((cc_text != NULL) && (strlen(cc_text) == 2)) {
		if (isalpha(cc_text[0]) && isalnum(cc_text[1])) {
			c1 = toupper(cc_text[0]);
			if (! (c1 >= 'A' && c1 <= 'Z')) {
				goto END_libipv6calc_db_wrapper_cc_index_by_addr; // something wrong
			};
			c1 -= 'A';

			c2 = toupper(cc_text[1]);
			if (c2 >= '0' && c2 <= '9') {
				c2 -= '0';
			} else if (c2 >= 'A' && c2 <= 'Z') {
				c2 -= 'A';
				c2 += 10;
			} else {
				goto END_libipv6calc_db_wrapper_cc_index_by_addr; // something wrong
			};

			index = c1 + c2 * COUNTRYCODE_LETTER1_MAX;

			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "c1=%d c2=%d index=%d (0x%03x) -> test: %c%c", c1, c2, index, index, COUNTRYCODE_INDEX_TO_CHAR1(index), COUNTRYCODE_INDEX_TO_CHAR2(index));

			if (index >= COUNTRYCODE_INDEX_MAX) {
				index = COUNTRYCODE_INDEX_UNKNOWN; // failsafe
				fprintf(stderr, "%s/%s: unexpected index (too high): %d\n", __FILE__, __func__, index);
			};
		};
	};

END_libipv6calc_db_wrapper_cc_index_by_addr:
	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished with %d (0x%x)", index, index);

	return(index);
};


/*
 * get country code string by index
 */
int libipv6calc_db_wrapper_country_code_by_cc_index(char *string, int length, const uint16_t cc_index) {
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
char *libipv6calc_db_wrapper_as_text_by_addr(const char *addr, const int proto) {
	char * result_char_ptr = NULL;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	if (wrapper_GeoIP_status == 1) {
#ifdef SUPPORT_GEOIP
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now GeoIP");

		result_char_ptr = libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(addr, proto);
#endif
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: %s", result_char_ptr);

	return(result_char_ptr);
};



/*
 * get AS 32-bit number
 */
uint32_t libipv6calc_db_wrapper_as_num32_by_addr(const char *addr, const int proto) {
	char *as_text;
	char as_number_string[11];  // max: 4294967295 = 10 digits + \0
	uint32_t as_num32 = ASNUM_AS_UNKNOWN; // default
	int i, valid = 1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%s proto=%d", addr, proto);

	// TODO: switch mechanism depending on backend (GeoIP supports AS only by text representation)
	as_text = libipv6calc_db_wrapper_as_text_by_addr(addr, proto);

	if ((as_text != NULL) && (strncmp(as_text, "AS", 2) == 0) && (strlen(as_text) > 2)) {
		// catch AS....
		for (i = 0; i < (strlen(as_text) - 2); i++) {
			if ((as_text[i+2] == ' ') || (as_text[i+2] == '\0')) {
				break;
			} else if (isdigit(as_text[i+2])) {
				continue;
			} else {
				// something wrong
				valid = 0;
				break;
			};
		};

		if (i > 10) {
			// too many digits
			valid = 0;
		};

		if (valid == 1) {
			snprintf(as_number_string, 11, "%s", as_text + 2);
			as_num32 = atol(as_number_string);
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: %d (0x%08x)", as_num32, as_num32);

	return(as_num32);
};


/*
 * get AS 16-bit number
 */
uint16_t libipv6calc_db_wrapper_as_num16_by_addr(const char *addr, const int proto) {
	uint16_t as_num16 = 0;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called: addr=%s proto=%d", addr, proto); 

	// get 32-bit ASN
	uint32_t as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(addr, proto);

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


/*
 * get registry string of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = Registry string
 * ret: 0: ok, 1: unknown, 2: reserved
 */
int libipv6calc_db_wrapper_registry_string_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length) {
	int retval = 1;
#ifdef SUPPORT_BUILTIN
	retval = libipv6calc_db_wrapper_BuiltIn_registry_string_by_ipv4addr(ipv4addrp, resultstring, resultstring_length);
#endif
	return (retval);
};


/*
 * get registry number of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_registry_num_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp) {
	int retval = -1;
#ifdef SUPPORT_BUILTIN
	retval = libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv4addr(ipv4addrp);
#endif
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
#ifdef SUPPORT_BUILTIN
	retval = libipv6calc_db_wrapper_BuiltIn_registry_string_by_ipv6addr(ipv6addrp, resultstring, resultstring_length);
#endif
	return (retval);
};


/*
 * get registry number of an IPv6 address
 *
 * in:  ipv6addr = IPv6 address structure
 * out: assignment number (-1 = no result)
 */
int libipv6calc_db_wrapper_registry_num_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp) {
	int retval = -1;
#ifdef SUPPORT_BUILTIN
	retval = libipv6calc_db_wrapper_BuiltIn_registry_num_by_ipv6addr(ipv6addrp);
#endif
	return (retval);
};


#ifdef HAVE_BERKELEY_DB_SUPPORT
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
		"%lu;%lu;%[^%]",			// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x2
		"%lu;%lu;%lu;%lu;%[^%]",		// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x4
		"%lx;%lx;%[^%]",			// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2
		"%lx;%lx;%lx;%lx;%[^%]",		// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4
		"0x%lx;0x%lx;%[^%]",			// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_PREF_32x2
		"0x%lx;0x%lx;0x%lx;0x%lx;%[^%]",	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_WITH_PREF_32x4
	};

	const int db_format_values[] = {
		3,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x2
		5,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x4
		3,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x2
		5,	// IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_HEX_32x4
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

	long int recno = row;

	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	key.data = &recno;
	key.size = sizeof(recno);

	int ret;

	// DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Try to get row");

	ret = db_ptr->get(db_ptr, NULL, &key, &data, 0);

	// DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "get executed");

	if (ret != 0) {
		db_ptr->err(db_ptr, ret, "DB->get");
		goto END_libipv6calc_db_wrapper_bdb_fetch_row;
	};

	// DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Got row");

	snprintf(datastring, (data.size + 1) >= sizeof(datastring) ? sizeof(datastring) : data.size + 1, "%s", (char *) data.data);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "database row %lu: %s (size=%d) data_ptr=%p", recno, datastring, data.size, data_ptr);

	if (db_format_values[db_format] == 3) {
		ret = sscanf(datastring, db_format_row[db_format], data_1_00_31_ptr, data_2_00_31_ptr, (char *) data_ptr);
		*data_1_32_63_ptr = 0;
		*data_2_32_63_ptr = 0;
	} else if (db_format_values[db_format] == 5) {
		ret = sscanf(datastring, db_format_row[db_format], data_1_00_31_ptr, data_1_32_63_ptr, data_2_00_31_ptr, data_2_32_63_ptr, (char *) data_ptr);
	} else {
		ERRORPRINT_WA("unsupported db_format_values (FIX CODE): %u", db_format_values[db_format]);
		exit(EXIT_FAILURE);
	};

	if (ret != db_format_values[db_format]) {
		ERRORPRINT_WA("row parsing error, corrupted database: %s (ret=%d format=%s expectedvalues=%d)", datastring, ret, db_format_row[db_format], db_format_values[db_format]);
		goto END_libipv6calc_db_wrapper_bdb_fetch_row;
	};

	retval = 0; // ok

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
int libipv6calc_db_wrapper_get_entry_generic(
	void 		*db_ptr,		// pointer to database in case of IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB, otherwise NULL
	const uint8_t	data_ptr_type,		// type of data_ptr
	const uint8_t	data_key_type,		// key type
	const uint8_t	data_key_length,	// key length
	const uint8_t	data_search_type,	// search type
	const long int	data_key_row_min,	// number of first usable row (begin)
	const long int	data_key_row_max,	// number of last usable row (end)
	const uint32_t	lookup_key_00_31,	// lookup key MSB
	const uint32_t	lookup_key_32_63,	// lookup key LSB
	void            *data_ptr,		// pointer to DB data in case of IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB, otherwise NULL
	int  (*get_array_row)()			// function to get array row
	) {

	int retval = -1;

	uint32_t value_first_00_31 = 0, value_last_00_31 = 0;
	uint32_t value_first_32_63 = 0, value_last_32_63 = 0;

#ifdef HAVE_BERKELEY_DB_SUPPORT
	uint8_t	db_format = 255;

	DB *dbp = NULL;

	char db_value[NI_MAXHOST];
#endif // HAVE_BERKELEY_DB_SUPPORT

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with data_ptr_type=%u data_key_type=%u data_key_length=%u data_key_row_min=%lu data_key_row_max=%lu lookup_key_00_31=%08lx lookup_key_32_63=%08lx db_ptr=%p, data_ptr=%p",
		data_ptr_type,
		data_key_type,
		data_key_length,
		(long unsigned int) data_key_row_min,
		(long unsigned int) data_key_row_max,
		(long unsigned int) lookup_key_00_31,
		(long unsigned int) lookup_key_32_63,
		db_ptr,
		data_ptr
	);

	/* check row min/max */
	if (data_key_row_min < 0) {
		ERRORPRINT_WA("unsupported data_key_row_min (FIX CODE): %ld", data_key_row_min);
		exit(EXIT_FAILURE);
	};

	if (data_key_row_max < 1) {
		ERRORPRINT_WA("unsupported data_key_row_max (FIX CODE): %ld", data_key_row_max);
		exit(EXIT_FAILURE);
	};

	if (data_key_row_max < data_key_row_min) {
		ERRORPRINT_WA("unsupported data_key_row_max < data_key_row_min (FIX CODE): %ld / %ld", data_key_row_max, data_key_row_min);
		exit(EXIT_FAILURE);
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

	/* check data_ptr_type */
	if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY) {
		if (get_array_row == NULL) {
			ERRORPRINT_NA("get_array_row function is unexpected NULL (FIX CODE)");
			exit(EXIT_FAILURE);
		};

		if (data_ptr != NULL) {
			ERRORPRINT_NA("data_ptr is unexpected NOT NULL - not supported on IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY (FIX CODE)");
			exit(EXIT_FAILURE);
		};
#ifdef HAVE_BERKELEY_DB_SUPPORT
	} else if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB) {
		if (get_array_row != NULL) {
			ERRORPRINT_NA("get_array_row function is unexpected NOT NULL (FIX CODE)");
			exit(EXIT_FAILURE);
		};

		// supported
		dbp = (DB *) db_ptr; // map db_ptr to DB ptr

		if (data_key_length == 32) {
			db_format = IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x2;
		} else if (data_key_length == 64) {
			db_format = IPV6CALC_DB_LOOKUP_DATA_DBD_FORMAT_SEMICOLON_SEP_DEC_32x4;
		};
#endif // HAVE_BERKELEY_DB_SUPPORT
	} else {
		ERRORPRINT_WA("unsupported data_ptr_type (FIX CODE): %u", data_ptr_type);
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


	int i = -1;
	int match = -1;
	int seqlongest = -1;
	int i_min, i_max, i_old, max = 0;

	int ret;

	long unsigned int recno;

#ifdef HAVE_BERKELEY_DB_SUPPORT
	// empty
#endif // HAVE_BERKELEY_DB_SUPPORT

	i_min = 0; i_max = max; i_old = -1;

	max = data_key_row_max;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Start binary search over entries: max=%d", max);

	if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
		// binary search in provided data
		i_max = max;
		i = i_max / 2;
	} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
		// sequential search in provided data
		i_old = max;
		i_max = max;
		i = 0;
	};

	while (i_old != i) {
		recno = i + data_key_row_min;
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Analyze recno=%ld", recno);

		if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY) {
			ret = get_array_row(recno, &value_first_00_31, &value_first_32_63, &value_last_00_31, &value_last_32_63);
			if (ret < 0) {
				ERRORPRINT_WA("can't retrieve keys from array for row: %lu", recno);
				exit(EXIT_FAILURE);
			};	
#ifdef HAVE_BERKELEY_DB_SUPPORT	
		} else if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Try to get row from Berkeley DB dbp=%p recno=%ld", dbp, recno);
			ret = libipv6calc_db_wrapper_bdb_fetch_row(
				dbp,			// pointer to DB
				db_format,		// DB format
				recno,			// row number
				&value_first_00_31,	// data 1 (MSB in case of 64 bits)
				&value_first_32_63,	// data 1 (LSB in case of 64 bits)
				&value_last_00_31,	// data 2 (MSB in case of 64 bits)
				&value_last_32_63,	// data 2 (LSB in case of 64 bits)
				data_ptr		// pointer to data
			);

			if (ret != 0) {
				ERRORPRINT_WA("can't retrieve keys from data for row: %lu", recno);
				exit(EXIT_FAILURE);
			};
#endif // HAVE_BERKELEY_DB_SUPPORT
		};

		if (data_key_length == 32) {
			if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Test %08x range %08x - %08x i=%d i_min=%d i_max=%d",
					(unsigned int) lookup_key_00_31,
					(unsigned int) value_first_00_31,
					(unsigned int) value_last_00_31,
					i, i_min, i_max);
			} else if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Test %08x mask %08x/%08x i=%d i_min=%d i_max=%d",
					(unsigned int) lookup_key_00_31,
					(unsigned int) value_first_00_31,
					(unsigned int) value_last_00_31,
					 i, i_min, i_max);
			};
		} else if (data_key_length == 64) {
			if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Test %08x:%08x range %08x:%08x - %08x:%08x i=%d i_min=%d i_max=%d",
					(unsigned int) lookup_key_00_31,
					(unsigned int) lookup_key_32_63,
					(unsigned int) value_first_00_31,
					(unsigned int) value_first_32_63,
					(unsigned int) value_last_00_31,
					(unsigned int) value_last_32_63,
					 i, i_min, i_max);
			} else if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK) {
				DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Test %08x:%08x mask %08x:%08x/%08x:%08x i=%d i_min=%d i_max=%d",
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
					i_max = i;
				} else if (lookup_key_00_31 > value_last_00_31) {
					// to low in array, jump up
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
							seqlongest = ret;
						};
					};
				};
			} else if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK) {
				if ((lookup_key_00_31 & value_last_00_31) < value_first_00_31) {
					// to high in array, jump down
					i_max = i;
				} else if ((lookup_key_00_31 & value_last_00_31) > value_first_00_31) {
					// to low in array, jump up
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
							seqlongest = ret;
						};
					};
				};
			};
		} else if (data_key_length == 64) {
			if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_FIRST_LAST) {
				if (lookup_key_00_31 < value_first_00_31) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						/* to high in array, jump down */
						i_max = i;
					};
				} else if (lookup_key_00_31 > value_last_00_31) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// to low in array, jump up
						i_min = i;
					};
				} else if ((lookup_key_00_31 == value_first_00_31) && (lookup_key_32_63 < value_first_32_63)) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						/* to high in array, jump down */
						i_max = i;
					};
				} else if ((lookup_key_00_31 == value_last_00_31) && (lookup_key_32_63 > value_last_32_63)) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// to low in array, jump up
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
							seqlongest = ret;
						};
					};
				};
			} else if (data_key_type == IPV6CALC_DB_LOOKUP_DATA_KEY_TYPE_BASE_MASK) {
				if ((lookup_key_00_31 & value_last_00_31) < value_first_00_31) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						/* to high in array, jump down */
						i_max = i;
					};
				} else if ((lookup_key_00_31 & value_last_00_31) > value_first_00_31) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// to low in array, jump up
						i_min = i;
					};
				} else if (((lookup_key_00_31 & value_last_00_31) == value_first_00_31) && ((lookup_key_32_63 & value_last_32_63) < value_first_32_63)) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						/* to high in array, jump down */
						i_max = i;
					};
				} else if (((lookup_key_00_31 & value_last_00_31) == value_first_00_31) && ((lookup_key_32_63 & value_last_32_63) > value_first_32_63)) {
					if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
						// to low in array, jump up
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
							seqlongest = ret;
						};
					};
				};
			};
		};

		if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
			// binary search in provided data
			i_old = i;
			i = (i_max - i_min) / 2 + i_min;
		} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
			// sequential search in provided data
			i++;
		};
	};

	if (match != -1) {
		retval = match;

		if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_BINARY) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Matched: %d", i);
			if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_ARRAY) {
#ifdef HAVE_BERKELEY_DB_SUPPORT
			} else if (data_ptr_type == IPV6CALC_DB_LOOKUP_DATA_PTR_TYPE_BDB) {
				data_ptr = db_value;
#endif // HAVE_BERKELEY_DB_SUPPORT
			};
		} else if (data_search_type == IPV6CALC_DB_LOOKUP_DATA_SEARCH_TYPE_SEQLONGEST) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Matched: %d", match);
		};

		if (data_ptr != NULL) {
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished with success result (DB): match=%d", match);
		};
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Finished with NO SUCCESS result (DB)");
	};

//END_libipv6calc_db_wrapper_get_entry_generic:
	return(retval);
};
