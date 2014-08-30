/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.c
 * Version    : $Id: libipv6calc_db_wrapper.c,v 1.43 2014/08/30 00:38:53 ds6peter Exp $
 * Copyright  : 2013-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc database wrapper (for decoupling databases from main binary)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
#endif

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

#ifdef SUPPORT_DBIP
	snprintf(tempstring, sizeof(tempstring), "%s%sDBIP", string, strlen(string) > 0 ? " " : "");
	snprintf(string, size, "%s", tempstring);
#endif

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
	};
	printf("\n");
#endif

#ifdef SUPPORT_IP2LOCATION
	if (wrapper_IP2Location_disable != 1) {
		// Call IP2Location wrapper
		libipv6calc_db_wrapper_IP2Location_wrapper_print_db_info(level_verbose, prefix_string);
	};
	printf("\n");
#endif

#ifdef SUPPORT_DBIP
	// Call DBIP wrapper
	libipv6calc_db_wrapper_DBIP_wrapper_print_db_info(level_verbose, prefix_string);
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

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	switch(opt) {
		case DB_ip2location_disable:
			wrapper_IP2Location_disable = 1;
			result = 0;
			break;

		case DB_geoip_disable:
			wrapper_GeoIP_disable = 1;
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
 */
char *libipv6calc_db_wrapper_country_code_by_addr(const char *addr, const int proto) {
	char *result_char_ptr = NULL;

	DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Called");

	if (wrapper_GeoIP_status == 1) {
#ifdef SUPPORT_GEOIP
		DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now GeoIP");

		result_char_ptr = (char *) libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(addr, proto);
#endif
	};

	if (result_char_ptr == NULL) {
		// fallback
		if (wrapper_IP2Location_status == 1) {
#ifdef SUPPORT_IP2LOCATION
			DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now IP2Location");;

			result_char_ptr = libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr((char *) addr, proto);
#endif
		};

		if (result_char_ptr == NULL) {
			// fallback
			if (wrapper_DBIP_status == 1) {
#ifdef SUPPORT_DBIP
				DEBUGPRINT_NA(DEBUG_libipv6calc_db_wrapper, "Call now DBIP");;

				result_char_ptr = libipv6calc_db_wrapper_DBIP_wrapper_country_code_by_addr((char *) addr, proto);
#endif
			};
	};
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Result: %s", result_char_ptr);

	return(result_char_ptr);
};


/*
 * get CountryCode in special internal form (index) [A-Z] (26) x [0-9A-Z] (36)
 */
uint16_t libipv6calc_db_wrapper_cc_index_by_addr(const char *addr, const int proto) {
	uint16_t index = COUNTRYCODE_INDEX_UNKNOWN;

	const char *cc_text = libipv6calc_db_wrapper_country_code_by_addr(addr, proto);
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
 * generic Berkeley DB lookup function for IPv4
 * 
 * in:  DB, ipv4addrp
 * out: database entry matching ipv4addrp
 *
 * database format
 * Version 1
 *  RECNO: ipv4_first;ipv4_last;data
 */
int libipv6calc_db_wrapper_get_dbentry_by_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, DB *dbp, const int db_format, char *resultstring, const size_t resultstring_length) {
	int retval = -1;

	char datastring[NI_MAXHOST];

	if (db_format != 1) {
		ERRORPRINT_WA("unsupported database format: %d", db_format);
		goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv4addr;
	};

	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given IPv4 address: %08x", (unsigned int) ipv4);

	int i = -1;
	int match = -1;
	int i_min, i_max, i_old, max = 0;

	DBT key, data;
	DBC *dbcp;
	int ret;
	long unsigned int recno, recno_max;

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	int token_counter;
	char *db_data = NULL;

	uint32_t ipv4_first = 0, ipv4_last = 0;

	i_min = 0; i_max = max; i_old = -1;

	// get amount of entries in database
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	/* Acquire a cursor for the database. */
	if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
		dbp->err(dbp, ret, "DB->cursor");
		goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv4addr;
	};

	/* Walk through the database and print out the key/data pairs. */
	if ((ret = dbcp->c_get(dbcp, &key, &data, DB_LAST)) != 0) {
		dbp->err(dbp, ret, "DB->cursor/DB_LAST");
		goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv4addr;
	};

	/* Close the cursor. */
	if ((ret = dbcp->c_close(dbcp)) != 0) {
		dbp->err(dbp, ret, "DBcursor->close");
		goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv4addr;
	};

	recno_max = *(long unsigned int *)key.data;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database amount of entries found: %lu", recno_max);

	max = recno_max - 1;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Called with ipv4=%08x max=%d", ipv4, max);

	// binary search in DB
	i = max / 2;
	while (i_old != i) {
		memset(&key, 0, sizeof(key));
		memset(&data, 0, sizeof(data));

		recno = i + 2; // 1 is info

		key.data = &recno;
		key.size = sizeof(recno);

		if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) != 0) {
			dbp->err(dbp, ret, "DB->get");
			goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv4addr;
                };

		snprintf(datastring, (data.size + 1) >= sizeof(datastring) ? sizeof(datastring) : data.size + 1, "%s", (char *) data.data);

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database entry %lu: %s (%d)", recno, datastring, data.size);

		// split data string
		token = strtok_r(datastring, ";", ptrptr);
		token_counter = 0;
		while (token != NULL) {
			token_counter++;
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "token %d found: %s", token_counter, token);

			if (token_counter == 1) {
				ipv4_first = atoi(token);
			} else if (token_counter == 2) {
				ipv4_last = atoi(token);
			} else if (token_counter == 3) {
				db_data = token;
			} else {
				ERRORPRINT_WA("corrupted database, data has more than expected tokens: %d (%s)", token_counter, (char *) data.data);
				exit(1);
				//goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv4addr;
			};

			/* get next token */
			token = strtok_r(NULL, ";", ptrptr);
		};

		if (token_counter != 3) {
			ERRORPRINT_WA("corrupted database, data has less than expected tokens: %d (%s)", token_counter, (char *) data.data);
			exit(1);
			//goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv4addr;
		};

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Search in DB for ipv4=%08x first=%08x last=%08x i=%d i_min=%d i_max=%d", ipv4, (unsigned int) ipv4_first, (unsigned int) ipv4_last, i, i_min, i_max);

		if (ipv4 < ipv4_first) {
			// to high in array, jump down
			i_max = i;
		} else if (ipv4 > ipv4_last) {
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
		if (db_data != NULL) {
			snprintf(resultstring, resultstring_length, "%s", db_data);
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished with success result (DB): match=%d data=%s", match, resultstring);
			retval = 0;
		};
	};

END_libipv6calc_db_wrapper_get_dbentry_by_ipv4addr:
	return (retval);
};


/*
 * generic Berkeley DB lookup function for IPv6
 * 
 * in:  DB, ipv6addrp
 * out: database entry matching ipv6addrp
 *
 * database format
 * Version 1
 *  RECNO: ipv6_first_0_31;ipv6_first_32_63;ipv6_last_0_31;ipv6_last_32_63;data
 */
int libipv6calc_db_wrapper_get_dbentry_by_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp, DB *dbp, const int db_format, char *resultstring, const size_t resultstring_length) {
	int retval = -1;

	char datastring[NI_MAXHOST];

	if (db_format != 1) {
		ERRORPRINT_WA("unsupported database format: %d", db_format);
		goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv6addr;
	};

	uint32_t ipv6_00_31 = ipv6addr_getdword(ipv6addrp, 0);
	uint32_t ipv6_32_63 = ipv6addr_getdword(ipv6addrp, 1);

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Given ipv6 prefix: %08x%08x", (unsigned int) ipv6_00_31, (unsigned int) ipv6_32_63);

	int i = -1;
	int match = -1;
	// int i_min, i_max, i_old, max = 0; // TODO: implmement binary search

	DBT key, data;
	DBC *dbcp;
	int ret;
	long unsigned int recno, recno_max;

	char *token, *cptr, **ptrptr;
	ptrptr = &cptr;

	int token_counter;
	char *db_data = NULL;

	uint32_t ipv6_first_00_31 = 0, ipv6_first_32_63 = 0, ipv6_last_00_31 = 0, ipv6_last_32_63 = 0;

	// get amount of entries in database
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	/* Acquire a cursor for the database. */
	if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
		dbp->err(dbp, ret, "DB->cursor");
		goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv6addr;
	};

	/* Walk through the database and print out the key/data pairs. */
	if ((ret = dbcp->c_get(dbcp, &key, &data, DB_LAST)) != 0) {
		dbp->err(dbp, ret, "DB->cursor/DB_LAST");
		goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv6addr;
	};

	/* Close the cursor. */
	if ((ret = dbcp->c_close(dbcp)) != 0) {
		dbp->err(dbp, ret, "DBcursor->close");
		goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv6addr;
	};

	recno_max = *(long unsigned int *)key.data;

	DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database amount of entries found: %lu", recno_max);

	for (i = 0; i < recno_max - 1; i++) {
		/* run sequentially through database array */

		memset(&key, 0, sizeof(key));
		memset(&data, 0, sizeof(data));

		recno = i + 2; // 1 is info

		key.data = &recno;
		key.size = sizeof(recno);

		if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) != 0) {
			dbp->err(dbp, ret, "DB->get");
			goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv6addr;
                };

		snprintf(datastring, (data.size + 1) >= sizeof(datastring) ? sizeof(datastring) : data.size + 1, "%s", (char *) data.data);

		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Database entry %lu: %s (%d)", recno, datastring, data.size);

		// split data string
		token = strtok_r(datastring, ";", ptrptr);
		token_counter = 0;
		while (token != NULL) {
			token_counter++;
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "token %d found: %s", token_counter, token);

			if (token_counter == 1) {
				ipv6_first_00_31 = atoi(token);
			} else if (token_counter == 2) {
				ipv6_first_32_63 = atoi(token);
			} else if (token_counter == 3) {
				ipv6_last_00_31 = atoi(token);
			} else if (token_counter == 4) {
				ipv6_last_32_63 = atoi(token);
			} else if (token_counter == 5) {
				db_data = token;
			} else {
				ERRORPRINT_WA("corrupted database, data has more than expected tokens: %d (%s)", token_counter, (char *) data.data);
				exit(1);
				//ERRORPRINT_WA("corrupted database, data has more than expected tokens: %d", token_counter);
				//goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv6addr;
			};

			/* get next token */
			token = strtok_r(NULL, ";", ptrptr);
		};

		if (token_counter != 5) {
			ERRORPRINT_WA("corrupted database, data has less than expected tokens: %d (%s)", token_counter, (char *) data.data);
			exit(1);
			//goto END_libipv6calc_db_wrapper_get_dbentry_by_ipv6addr;
		};

		//DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Search in DB for ipv6=%08x%08x first=%08x%08x last=%08x%08x i=%d i_min=%d i_max=%d", // TODO Binary search
		DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Search in DB for ipv6=%08x%08x first=%08x%08x last=%08x%08x i=%d",
			(unsigned int) ipv6_00_31,
			(unsigned int) ipv6_32_63,
			(unsigned int) ipv6_first_00_31,
			(unsigned int) ipv6_first_32_63,
			(unsigned int) ipv6_last_00_31,
			(unsigned int) ipv6_last_32_63,
			//i, i_min, i_max); // TODO Binary search
			i);

		if ((ipv6_00_31 < ipv6_first_00_31) || (ipv6_00_31 > ipv6_last_00_31)) {
			/* MSB 00-31 too less or too high */
			continue;
		};

		if ((ipv6_00_31 == ipv6_first_00_31) && (ipv6_32_63 < ipv6_first_32_63)) {
			/* MSB 00-31 match first, but MSB 32-63 too less */
			continue;
		};

		if ((ipv6_00_31 == ipv6_last_00_31) && (ipv6_32_63 > ipv6_last_32_63)) {
			/* MSB 00-31 match last, but MSB 32-63 too high */
			continue;
		};

		match = i;
		break;
	};

	if (match != -1) {
		if (db_data != NULL) {
			snprintf(resultstring, resultstring_length, "%s", db_data);
			DEBUGPRINT_WA(DEBUG_libipv6calc_db_wrapper, "Finished with success result (DB): match=%d data=%s", match, resultstring);
			retval = 0;
		};
	};

END_libipv6calc_db_wrapper_get_dbentry_by_ipv6addr:
	return (retval);
};



#endif // HAVE_BERKELEY_DB_SUPPORT
