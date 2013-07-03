/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper.c
 * Version    : $Id: libipv6calc_db_wrapper.c,v 1.6 2013/07/03 05:50:28 ds6peter Exp $
 * Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  ipv6calc database wrapper (for decoupling databases from main binary)
 */

#include <stdio.h>

#include "libipv6calcdebug.h"

#include "libipv6calc_db_wrapper.h"
#include "libipv6calc_db_wrapper_GeoIP.h"

static int wrapper_GeoIP_status = 0;


/*
 * function initialise the main wrapper
 *
 * in : (nothing)
 * out: 0=ok, 1=error
 */
int libipv6calc_db_wrapper_init(void) {
	int result = 0, r;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	// Call GeoIP wrapper
	r = libipv6calc_db_wrapper_GeoIP_wrapper_init();
	if (r != 0) {
		result = 1;
	} else {
		wrapper_GeoIP_status = 1; // ok
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Result: %d\n", __FILE__, __func__, result);
	};
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

	// Call GeoIP wrapper
	r = libipv6calc_db_wrapper_GeoIP_wrapper_cleanup();
	if (r != 0) {
		result = 1;
	};

	return(result);
};

/* function get info string */
void libipv6calc_db_wrapper_info(char * string, const size_t size) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	// Call GeoIP wrapper
	libipv6calc_db_wrapper_GeoIP_wrapper_info(string, size);

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Result: %s\n", __FILE__, __func__, string);
	};
	return;
};

/* function print db info */
void libipv6calc_db_wrapper_print_db_info(void) {
	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	// Call GeoIP wrapper
	libipv6calc_db_wrapper_GeoIP_wrapper_print_db_info();

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Return\n", __FILE__, __func__);
	};
	return;
};

/*********************************************
 *  * Abstract functions
 * *******************************************/

const char * libipv6calc_db_wrapper_country_code_by_addr(const char *addr, const int proto) {
	const char * result_char_ptr = NULL;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	if (wrapper_GeoIP_status == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call now GeoIP\n", __FILE__, __func__);
		};

		result_char_ptr = libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(addr, proto);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Result: %s\n", __FILE__, __func__, result_char_ptr);
	};

	return(result_char_ptr);
};


char * libipv6calc_db_wrapper_asnum_by_addr(const char *addr, const int proto) {
	char * result_char_ptr = NULL;

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Called\n", __FILE__, __func__);
	};

	if (wrapper_GeoIP_status == 1) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
			fprintf(stderr, "%s/%s: Call now GeoIP\n", __FILE__, __func__);
		};

		result_char_ptr = libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(addr, proto);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr_db_wrapper) != 0 ) {
		fprintf(stderr, "%s/%s: Result: %s\n", __FILE__, __func__, result_char_ptr);
	};

	return(result_char_ptr);
};
