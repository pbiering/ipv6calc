/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.c
 * Version    : $Id: libipv6calc_db_wrapper.c,v 1.24 2013/10/11 06:06:35 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
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
#include "libipv6calc_db_wrapper_BuiltIn.h"

static int wrapper_GeoIP_disable = 0;
static int wrapper_IP2Location_disable = 0;

static int wrapper_GeoIP_status = 0;
static int wrapper_IP2Location_status = 0;
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

	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Called");

	if (wrapper_GeoIP_disable != 1) {
#ifdef SUPPORT_GEOIP
		// Call GeoIP wrapper
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Call libipv6calc_db_wrapper_GeoIP_wrapper_init");

		r = libipv6calc_db_wrapper_GeoIP_wrapper_init();

		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "GeoIP_wrapper_init result: %d wrapper_features=0x%08x", r, wrapper_features);

		if (r != 0) {
#ifndef SUPPORT_GEOIP_DYN
			// only non-dynamic-load results in a problem
			result = 1;
#endif
		} else {
			wrapper_GeoIP_status = 1; // ok
		};
#endif // SUPPORT_GEOIP
	};

	if (wrapper_IP2Location_disable != 1) {
#ifdef SUPPORT_IP2LOCATION
		// Call IP2Location wrapper
		DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Call libipv6calc_db_wrapper_IP2Location_wrapper_init");

		r = libipv6calc_db_wrapper_IP2Location_wrapper_init();

		DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "IP2Location_wrapper_init result: %d wrapper_features=0x%08x", r, wrapper_features);

		if (r != 0) {
#ifndef SUPPORT_IP2LOCATION_DYN
			// only non-dynamic-load results in a problem
			result = 1;
#endif
		} else {
			wrapper_IP2Location_status = 1; // ok
		};
#endif // SUPPORT_IP2LOCATION
	};

#ifdef SUPPORT_BUILTIN
	// Call BuiltIn wrapper
	DEBUGPRINT_NA(DEBUG_libipv6addr_db_wrapper, "Call libipv6calc_db_wrapper_BuiltIn_wrapper_init");

	r = libipv6calc_db_wrapper_BuiltIn_wrapper_init();

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "BuiltIn_wrapper_init result: %d wrapper_features=0x%08x", r, wrapper_features);

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

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

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
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

#ifdef SUPPORT_GEOIP
	// Call GeoIP wrapper
	libipv6calc_db_wrapper_GeoIP_wrapper_info(string, size);
#endif

#ifdef SUPPORT_IP2LOCATION
	// Call IP2Location wrapper
	libipv6calc_db_wrapper_IP2Location_wrapper_info(string, size);
#endif

#ifdef SUPPORT_BUILTIN
	// Call BuiltIn wrapper
	libipv6calc_db_wrapper_BuiltIn_wrapper_info(string, size);
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Result: %s\n", __FILE__, __func__, string);
	};

	return;
};


/* function get feature string */
void libipv6calc_db_wrapper_features(char *string, const size_t size) {
	int i;
	char tempstring[NI_MAXHOST];
	char *separator;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

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

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Return\n", __FILE__, __func__);
	};

	return;
};


/* function print db info */
void libipv6calc_db_wrapper_print_db_info(const int level_verbose, const char *prefix_string) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	printf("DB features: 0x%08x\n", wrapper_features);

#ifdef SUPPORT_GEOIP
	// Call GeoIP wrapper
	libipv6calc_db_wrapper_GeoIP_wrapper_print_db_info(level_verbose, prefix_string);
#endif

#ifdef SUPPORT_IP2LOCATION
	// Call IP2Location wrapper
	libipv6calc_db_wrapper_IP2Location_wrapper_print_db_info(level_verbose, prefix_string);
#endif

#ifdef SUPPORT_BUILTIN
	// Call BuiltIn wrapper
	libipv6calc_db_wrapper_BuiltIn_wrapper_print_db_info(level_verbose, prefix_string);
#endif

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Return\n", __FILE__, __func__);
	};

	return;
};


/* function query for feature set
 * ret=-1: unknown
 * 0 : not matching
 * 1 : ok
 */
int libipv6calc_db_wrapper_has_features(uint32_t features) {
	int result = -1;
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called with feature value to test: 0x%08x\n", __FILE__, __func__, features);
	};

	if ((wrapper_features & features) == features) {
		result = 1;
	} else {
		result = 0;
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Return with result: %d\n", __FILE__, __func__, result);
	};
	return(result);
};


/*********************************************
 * Option handling
 * return < 0: error
 *********************************************/

int libipv6calc_db_wrapper_options(const int opt, const char *optarg, const struct option longopts[]) {
	int result = -1;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	switch(opt) {
		case DB_ip2location_disable:
			NONQUIETPRINT_NA("Support for IP2Location disabled by option");
			wrapper_IP2Location_disable = 1;
			result = 0;
			break;

		case DB_geoip_disable:
			NONQUIETPRINT_NA("Support for GeoIP disabled by option");
			wrapper_GeoIP_disable = 1;
			result = 0;
			break;

		case DB_ip2location_lib:
#ifdef SUPPORT_IP2LOCATION_DYN
			result = snprintf(ip2location_lib_file, sizeof(ip2location_lib_file), optarg);
#else
			NONQUIETPRINT_WA("Support for IP2Location dyn-load not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_geoip_lib:
#ifdef SUPPORT_GEOIP_DYN
			result = snprintf(geoip_lib_file, sizeof(geoip_lib_file), optarg);
#else
			NONQUIETPRINT_WA("Support for GeoIP dyn-load not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_ip2location_dir:
#ifdef SUPPORT_IP2LOCATION
			result = snprintf(ip2location_db_dir, sizeof(ip2location_db_dir), optarg);
#else
			NONQUIETPRINT_WA("Support for IP2Location not compiled-in, skipping option: --%s", ipv6calcoption_name(opt, longopts));
#endif
			result = 0;
			break;

		case DB_geoip_dir:
#ifdef SUPPORT_GEOIP
			result = snprintf(geoip_db_dir, sizeof(geoip_db_dir), optarg);
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

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Return with result: %d\n", __FILE__, __func__, result);
	};

	return(result);
};


/*********************************************
 * Abstract functions
 *********************************************/

/*
 * get Registry number by AS number
 */
int libipv6calc_db_wrapper_registry_num_by_as_num32(const uint32_t as_num32) {
	// currently only supported by BuiltIn
	return(libipv6calc_db_wrapper_BuiltIn_registry_num_by_as_num32(as_num32));
};


/*
 * get CountryCode in text form
 */
char *libipv6calc_db_wrapper_country_code_by_addr(const char *addr, const int proto) {
	char *result_char_ptr = NULL;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	if (wrapper_GeoIP_status == 1) {
#ifdef SUPPORT_GEOIP
		DEBUGPRINT(DEBUG_libipv6addr_db_wrapper, "%s/%s: Call now GeoIP\n", __FILE__, __func__);

		result_char_ptr = (char *) libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(addr, proto);
#endif
	} else {
		// fallback
		if (wrapper_IP2Location_status == 1) {
#ifdef SUPPORT_IP2LOCATION
			DEBUGPRINT(DEBUG_libipv6addr_db_wrapper, "%s/%s: Call now IP2Location\n", __FILE__, __func__);;

			result_char_ptr = libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr((char *) addr, proto);
#endif
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Result: %s\n", __FILE__, __func__, result_char_ptr);
	};

	return(result_char_ptr);
};


/*
 * get CountryCode in special internal form (index) [A-Z] (26) x [0-9A-Z] (36)
 */
uint16_t libipv6calc_db_wrapper_cc_index_by_addr(const char *addr, const int proto) {
	uint16_t index = COUNTRYCODE_INDEX_UNKNOWN;

	const char *cc_text = libipv6calc_db_wrapper_country_code_by_addr(addr, proto);
	uint8_t c1, c2;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

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

			if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
				fprintf(stderr, "%s/%s: c1=%d c2=%d index=%d (0x%03x) -> test: %c%c\n", __FILE__, __func__, c1, c2, index, index, COUNTRYCODE_INDEX_TO_CHAR1(index), COUNTRYCODE_INDEX_TO_CHAR2(index));
			};

			if (index >= COUNTRYCODE_INDEX_MAX) {
				index = COUNTRYCODE_INDEX_UNKNOWN; // failsafe
				fprintf(stderr, "%s/%s: unexpected index (too high): %d\n", __FILE__, __func__, index);
			};
		};
	};

END_libipv6calc_db_wrapper_cc_index_by_addr:
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Finished with %d (0x%x)\n", __FILE__, __func__, index, index);
	};

	return(index);
};


/*
 * get AS information in text form
 */
char *libipv6calc_db_wrapper_as_text_by_addr(const char *addr, const int proto) {
	char * result_char_ptr = NULL;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	if (wrapper_GeoIP_status == 1) {
#ifdef SUPPORT_GEOIP
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call now GeoIP\n", __FILE__, __func__);
		};

		result_char_ptr = libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(addr, proto);
#endif
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Result: %s\n", __FILE__, __func__, result_char_ptr);
	};

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

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "Called: addr=%s proto=%d", addr, proto);

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

	DEBUGPRINT_WA(DEBUG_libipv6addr_db_wrapper, "Result: %d (0x%08x)", as_num32, as_num32);

	return(as_num32);
};


/*
 * get AS 16-bit number
 */
uint16_t libipv6calc_db_wrapper_as_num16_by_addr(const char *addr, const int proto) {
	uint16_t as_num16 = 0;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called: addr=%s proto=%d\n", __FILE__, __func__, addr, proto);
	};

	// get 32-bit ASN
	uint32_t as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(addr, proto);

	as_num16 = (uint16_t) (as_num32 < 65536 ? as_num32 : ASNUM_AS_TRANS);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Result: %d (0x%04x)\n", __FILE__, __func__, as_num16, as_num16);
	};

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
