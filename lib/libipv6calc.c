/*
 * Project    : ipv6calc/lib
 * File       : libipv6calc.c
 * Version    : $Id: libipv6calc.c,v 1.37 2014/02/03 20:48:04 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for some tools
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ipv6calctypes.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "librfc1924.h"
#include "librfc2874.h"
#include "librfc1886.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"

/*
 * function converts chars in a string to upcase
 * in : pointer to a string
 */
void string_to_upcase(char *string) {
	int i;

	if (strlen(string) > 0) {
		for (i = 0; i < (int) strlen(string); i++) {
			string[i] = toupper(string[i]);
		};
	};

	return;
};


/*
 * function converts chars in a string to lowcase
 * in : pointer to a string
 */
void string_to_lowcase(char *string) {
	int i;

	if (strlen(string) > 0) {
		for (i = 0; i < (int) strlen(string); i++) {
			string[i] = tolower(string[i]);
		};
	};

	return;
};


/*
 * reverse string
 * in : pointer to a string
 */
void string_to_reverse(char *string) {
	int i;
	char helpchar;
	size_t length;

	length = strlen(string);

	if (length < 2) {
		/* nothing to do */
		return;
	};

	for (i = 0; i < ( (int) (length >> 1)); i++) {
		helpchar = string[i];
		string[i] = string[length - i - 1];
		string[length - i - 1] = helpchar;
	};

	return;
};


/*
 * dotted-reverse string
 * in : pointer to a string
 */
void string_to_reverse_dotted(char *string) {
	char resultstring[NI_MAXHOST], tempstring[NI_MAXHOST];
	char *token, *cptr, **ptrptr;
	int flag_first = 1;
	
	ptrptr = &cptr;

	/* clear result string */
	snprintf(resultstring, sizeof(resultstring) - 1 , "%s", "");

	/* check for starting dot */
	if ( string[0] == '.' ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "%s.", resultstring);
		snprintf(resultstring, sizeof(resultstring) - 1, "%s", tempstring);
	};

	token = strtok_r(string, ".", ptrptr);

	while (token != NULL) {
		if (flag_first == 1) {
			snprintf(tempstring, sizeof(tempstring) - 1, "%s%s", token, resultstring);
			flag_first = 0;
		} else {
			snprintf(tempstring, sizeof(tempstring) - 1, "%s.%s", token, resultstring);
		};
		snprintf(resultstring, sizeof(resultstring) - 1, "%s", tempstring);

		token = strtok_r(NULL, ".", ptrptr);
	};
	
	if ( string[strlen(string) - 1] == '.' ) {
		snprintf(tempstring, sizeof(tempstring) - 1, ".%s", resultstring);
		snprintf(resultstring, sizeof(resultstring) - 1, "%s", tempstring);
	};

	snprintf(string, NI_MAXHOST - 1, ".%s", resultstring);
	
	return;
};


/*
 * function converts chars in a string to upcase
 * in : pointer to a string
 * ret: format number
 */
uint32_t libipv6calc_autodetectinput(const char *string) {
	uint32_t type = FORMAT_auto_noresult;
	int xdl, i, j = 0, result;
	int numdots = 0, numcolons = 0, numdigits = 0, numxdigits = 0, numdashes = 0, numspaces = 0, numslashes = 0, numalnums = 0, numchar_s = 0, numpercents = 0, numcolonsdouble = 0, xdigitlen_max = 0, xdigitlen_min = 0;
	char resultstring[NI_MAXHOST];
	size_t length;

	length = strlen(string);

	if (length == 0) {
		/* input is empty */
		goto END_libipv6calc_autodetectinput;
	};

	xdl = 0;
	for (i = 0; i < (int) length; i++) {
		if (string[i] == '.') { numdots++; };
		if (string[i] == ':') {
			numcolons++;
			if (i < (int) length + 1) {
				/* check for double colons */
				if (string[i+1] == ':') {
					numcolonsdouble++;
					numcolons++;
					i++;
				};
			};
		};
		if (string[i] == '-') { numdashes++; };
		if (string[i] == '/') { numslashes++; };
		if (string[i] == ' ') { numspaces++; };
		if (string[i] == '%') { numpercents++; };
		if (string[i] == 's') { numchar_s++; };
		if (isdigit((int) string[i])) { numdigits++; };
		if (isxdigit((int) string[i])) {
			numxdigits++;
			xdl++;
		} else {
			if (xdigitlen_max == 0 && xdigitlen_min == 0) {
				// init
				xdigitlen_max = xdl;
				xdigitlen_min = xdl;
			};
			if (xdl > xdigitlen_max) {
				xdigitlen_max = xdl;
			};
			if (xdl < xdigitlen_min) {
				xdigitlen_min = xdl;
			};
			xdl = 0;
		};
		if (isalnum((int) string[i])) { numalnums++; };
	};

	DEBUGPRINT_NA(DEBUG_libipv6calc, "Autodetection source:");
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numdots        :%d", numdots);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numcolons      :%d", numcolons);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numcolonsdouble:%d", numcolonsdouble);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numdashes      :%d", numdashes);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numspaces      :%d", numspaces);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numslashes     :%d", numslashes);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numdigits      :%d", numdigits);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numxdigits     :%d", numxdigits);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numalnums      :%d", numalnums);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numpercents    :%d", numpercents);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " numchar_s      :%d", numchar_s);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " xdigit len max :%d", xdigitlen_max);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " xdigit len min :%d", xdigitlen_min);
	DEBUGPRINT_WA(DEBUG_libipv6calc, " length         :%d", (int) length);

	if ( length == 20 && numdots == 0 && numcolons == 0 ) {
	        /* probably a base85 one */
		DEBUGPRINT_NA(DEBUG_libipv6calc, " check FORMAT_base85");
		result = librfc1924_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: base85 */
			type = FORMAT_base85;
			goto END_libipv6calc_autodetectinput;
		} else DEBUGPRINT_WA(DEBUG_libipv6calc, " check FORMAT_base85 not successful, result: %s", resultstring);
	};
	
	if (length >= 7 && length <= 15 && numdots == 3 && numcolons == 0 && numdigits == numxdigits && numdigits >= 4 && numdigits <= 12 && numslashes <= 1 && (numdots + numdigits + numslashes) == length) {
		/* IPv4: ddd.ddd.ddd.ddd */
		type = FORMAT_ipv4addr;
		goto END_libipv6calc_autodetectinput;
	};

	if ( strncmp(string, "\\[", 2) == 0 ) {
		/* check for Bitstring label: \[x..../dd] */
		DEBUGPRINT_NA(DEBUG_libipv6calc, " check FORMAT_bitstring");
		result = librfc2874_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: bitstring label */
			type = FORMAT_bitstring;
			goto END_libipv6calc_autodetectinput;
		} else DEBUGPRINT_WA(DEBUG_libipv6calc, " check FORMAT_bitstring not successful, result: %s", resultstring);
	};
	
	if (length == 32 && numxdigits == 32 && numdots == 0 && numcolons == 0) {
		/* ifinet6 xxxx..xxxx  */
		type = FORMAT_ifinet6;
		goto END_libipv6calc_autodetectinput;
	};
	
	if (((length == 8 && numxdigits == 8) || (length == 7 && numxdigits == 7)) && numdots == 0 && numcolons == 0) {
		/* IPv4 hexadecimal: xxxxxxxx or xxxxxxx */
		type = FORMAT_ipv4hex;
		goto END_libipv6calc_autodetectinput;
	};
	
	if ((length >= 11 && length <= 17 && numxdigits >= 6 && numxdigits <= 12 && numdots == 0 && ( (numcolons == 5 && numdashes == 0 && numspaces == 0)
	   	 || (numcolons == 0 && numdashes == 5 && numspaces == 0)
		    || (numcolons == 0 && numdashes == 0 && numspaces == 5))
	    )
	    || (length == 13 && numcolons == 0 && numdashes == 1 && numspaces == 0 && numxdigits == 12)
	    || (length == 12 && numcolons == 0 && numdashes == 0 && numspaces == 0 && numxdigits == 12)
	    || (length == 14 && numdots == 2 && numxdigits ==12 && xdigitlen_min == 4 && xdigitlen_max == 4)
	   ) {
		/* MAC 00:00:00:00:00:00 or 00-00-00-00-00-00 or "xx xx xx xx xx xx" or "xxxxxx-xxxxxx" or xxxxxxxxxxxx or xxxx.xxxx.xxxx */

	    	if (length == 14 && numdots == 2 && numxdigits ==12 && xdigitlen_min == 4 && xdigitlen_max == 4) {
			// xxxx.xxxx.xxxx
			type = FORMAT_mac;
			DEBUGPRINT_NA(DEBUG_libipv6calc, "Autodetection found type: mac");
			goto END_libipv6calc_autodetectinput;
		};

		DEBUGPRINT_NA(DEBUG_libipv6calc, " check FORMAT_mac");

		/* Check whether minimum 1 xdigit is between colons, dashes, spaces */
		if (numcolons == 0 && numdashes == 1 && numspaces == 0 && numxdigits == 12) {

			/* Check xxxxxx-xxxxxx */
			j = 0;
			for (i = 0; i < (int) length; i++) {
				if (isxdigit((int) string[i])) {
					j++;
					if ( j > 6 ) {
						/* more than 6 xdigits */
						j = -1;
						break;
					};
					continue;
				} else if (string[i] == '-' ) {
					if ( j == 0 ) {
						/* dash follow dash */
						j = -1;
						break;
					};
					j = 0;
					continue;
				};
				/* normally not reached */
				j = -1;
				break;
			};

		} else if (numcolons == 0 && numdashes == 0 && numspaces == 0 && numxdigits == 12) {
			/* nothing more to check */
		} else {

			j = 0;
			for (i = 0; i < (int) length; i++) {
				if (isxdigit((int) string[i])) {
					j++;
					if ( j > 2 ) {
						/* more than 2 xdigits */
						j = -1;
						break;
					};
					continue;
				} else if (string[i] == ':' || string[i] == '-' || string[i] == ' ') {
					if ( j == 0 ) {
						/* colon/dash/space follows colon/dash/space */
						j = -1;
						break;
					};
					j = 0;
					continue;
				};
				/* normally not reached */
				j = -1;
				break;
			};

		}; /* end of if */

		if ( j != -1 ) {
			type = FORMAT_mac;
			DEBUGPRINT_NA(DEBUG_libipv6calc, "Autodetection found type: mac");
			goto END_libipv6calc_autodetectinput;
		};
	};

	if ((length >= 15 && length <= 23 && numxdigits >= 8 && numxdigits <= 16 && numdots == 0 && ( (numcolons == 7 && numdashes == 0 && numspaces == 0) || (numcolons == 0 && numdashes == 7 && numspaces == 0) || (numcolons == 0 && numdashes == 0 && numspaces == 7))) || (length == 16 && numcolons == 0 && numdashes == 0 && numspaces == 0 && numxdigits == 16)) {
		/* EUI-64 00:00:00:00:00:00:00:00 or 00-00-00-00-00-00-00-00 or "xx xx xx xx xx xx xx xx" or xxxxxxxxxxxxxxxx */

		DEBUGPRINT_NA(DEBUG_libipv6calc, " check FORMAT_eui64");

		if (numcolons == 0 && numdashes == 0 && numspaces == 0 && numxdigits == 16) {
			/* nothing more to check */
		} else {

			j = 0;
			for (i = 0; i < (int) length; i++) {
				if (isxdigit((int) string[i])) {
					j++;
					if ( j > 2 ) {
						/* more than 2 xdigits */
						j = -1;
						break;
					};
					continue;
				} else if (string[i] == ':' || string[i] == '-' || string[i] == ' ') {
					if ( j == 0 ) {
						/* colon/dash/space follows colon/dash/space */
						j = -1;
						break;
					};
					j = 0;
					continue;
				};
				/* normally not reached */
				j = -1;
				break;
			};

		}; /* end of if */

		if ( j != -1 ) {
			type = FORMAT_eui64;
			DEBUGPRINT_NA(DEBUG_libipv6calc, "Autodetection found type: eui64");
			goto END_libipv6calc_autodetectinput;
		};
	};
	
	if (numcolons == 0 && numdots > 0 && numslashes == 0 && numspaces == 0 && (numalnums + numdots) == length) {
		/* check for reverse nibble string */
		DEBUGPRINT_NA(DEBUG_libipv6calc, " check FORMAT_revnibbels_int");
		result = librfc1886_formatcheck(string, resultstring);	
	        if ( result == 0 ) {
			/* ok: reverse nibble string */
			type = FORMAT_revnibbles_int;
			goto END_libipv6calc_autodetectinput;
		} else DEBUGPRINT_WA(DEBUG_libipv6calc, " check FORMAT_revnibbels_int not successful, result: %s", resultstring);
	};
	
	if ((numcolons == 3) && (numcolonsdouble == 0) && numdots == 0 && numslashes == 0 && numpercents == 0 && ((numcolons + numxdigits) == length)) {
		/* IID */
		type = FORMAT_iid;
		goto END_libipv6calc_autodetectinput;
	};

	if (numcolons != 0 && numdots <= 3 && numslashes <= 1 && ((numpercents == 0 && (numdots + numcolons + numxdigits + numslashes) == length) || (numpercents == 1 && (numdots + numcolons + numxdigits + numslashes + numpercents) <= length))) {
		/* hopefully an IPv6 address */
		/* fe80::1 */
		/* fe80::1%eth0 */
		type = FORMAT_ipv6addr;
		goto END_libipv6calc_autodetectinput;
	};

	if (numcolons == 0 && numdots == 2 && numslashes == 0 && numdashes >= 3 && ((numchar_s == 0 && (numdashes + numdots + numxdigits + 10) == length) || (numchar_s == 1 && (numdashes + numdots + numxdigits + numchar_s + 10) <= length))) {
		/* hopefully an IPv6 literal address (e.g. 2001-DB8--1.IPV6-LITERAL.NET) IPV6-LITERAL.NET has 10 chars which are not xdigit  */
		/* also supported with scope: fe80--218-8bff-fe17-a226s4.ipv6-literal.net */
		type = FORMAT_ipv6literal;
		goto END_libipv6calc_autodetectinput;
	};
	
END_libipv6calc_autodetectinput:	
	if (type != FORMAT_auto_noresult) {
		DEBUGPRINT_WA(DEBUG_libipv6calc, "Autodetection found type: 0x%08x", (unsigned int) type);
	} else {
		DEBUGPRINT_NA(DEBUG_libipv6calc, "Autodetection not successful");
	};
	return (type);
};


/*
 * clear filter master address
 *
 * in : *filter    = filter structure
 */
void libipv6calc_filter_clear(s_ipv6calc_filter_master *filter_master) {
	ipv4addr_filter_clear(&filter_master->filter_ipv4addr);
	ipv6addr_filter_clear(&filter_master->filter_ipv6addr);
        macaddr_filter_clear(&filter_master->filter_macaddr);
	return;
};


/*
 * function parses ipv6calc filter expression
 *
 * in : pointer to a string
 * mod: master filter structure
 * ret: success
 */
int libipv6calc_filter_parse(const char *expression, s_ipv6calc_filter_master *filter_master) {
	char tempstring[NI_MAXHOST] = "";
	char *charptr, *cptr, **ptrptr;
	ptrptr = &cptr;
	int r, token_used, result = 0;

	snprintf(tempstring, NI_MAXHOST - 1, "%s", expression);

	/* split expression */
	charptr = strtok_r(tempstring, ",", ptrptr);
	while (charptr != NULL) {
		token_used = 0;

		r = ipv4addr_filter_parse(&filter_master->filter_ipv4addr, charptr);
		if (r == 0) {
			token_used = 1;
		} else if (r == 2) {
			result = 1;
			fprintf(stderr, "Unrecognized filter token: %s\n", charptr);
		};

		r = ipv6addr_filter_parse(&filter_master->filter_ipv6addr, charptr);
		if (r == 0) {
			token_used = 1;
		} else if (r == 2) {
			result = 1;
			fprintf(stderr, "Unrecognized filter token: %s\n", charptr);
		};

		// r += macaddr_filter_parse(&filter_macaddr, charptr);

		/* overall check */
		if (token_used == 0) {
			result = 1;
			fprintf(stderr, "Unrecognized filter token: %s\n", charptr);
		};

		charptr = strtok_r(NULL, ",", ptrptr);
	};

	return (result);
};


/*
 * return proper anonymization set by name
 *
 * in : *name = name of anonymization set
 *      *ipv6calc_anon_set = pointer to anonymization set to be filled
 * return: 1=not found, 0=success
 */
int libipv6calc_anon_set_by_name(s_ipv6calc_anon_set *ipv6calc_anon_set, const char *name) {
	int i;

	if (strlen(name) == 0) {
		fprintf(stderr, "Name of anonymization set is empty\n");
		return 1;
	};

	DEBUGPRINT_WA(DEBUG_libipv6calc, "search for anonymization set with name: %s", name);

	for (i = 0; i < sizeof(ipv6calc_anon_set_list) / sizeof(s_ipv6calc_anon_set); i++) {
		DEBUGPRINT_WA(DEBUG_libipv6calc, "compare name: %s ? %s", name, ipv6calc_anon_set_list[i].name);

		if ((strcmp(name, ipv6calc_anon_set_list[i].name) == 0) || (strcmp(name, ipv6calc_anon_set_list[i].name_short) == 0)) {
			DEBUGPRINT_WA(DEBUG_libipv6calc, "hit name: %s = %s", name, ipv6calc_anon_set_list[i].name);

			memcpy(ipv6calc_anon_set, &ipv6calc_anon_set_list[i], sizeof(s_ipv6calc_anon_set));
			return 0;
		};
	};

	return 1;
};


/*
 * get name of anonymization settings
 *
 * in : s_ipv6calc_anon_set = anonymization set
 * return: char *
 */
const char *libipv6calc_anon_method_name(const s_ipv6calc_anon_set *ipv6calc_anon_set) {
	int i;

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_anon_methods); i++) {
		if (ipv6calc_anon_methods[i].method == ipv6calc_anon_set->method) {
			return(ipv6calc_anon_methods[i].name);
			break;
		};
	};

	return(NULL);
};


/*
 * create string of anonymization settings
 *
 * in : *string = string to be filled
 *      s_ipv6calc_anon_set = anonymization set
 * return: void
 */
void libipv6calc_anon_infostring(char *string, const int stringlength, const s_ipv6calc_anon_set *ipv6calc_anon_set) {
	const char *method_name = libipv6calc_anon_method_name(ipv6calc_anon_set);

	snprintf(string, stringlength - 1, "set=%s,mask-ipv6=%d,mask-ipv4=%d,mask-eui64=%d,mask-mac=%d,method=%s", ipv6calc_anon_set->name, ipv6calc_anon_set->mask_ipv6, ipv6calc_anon_set->mask_ipv4, ipv6calc_anon_set->mask_eui64, ipv6calc_anon_set->mask_mac, (method_name == NULL ? "unknown" : method_name));

	return;
};


/*
 * check whether anonymization method is supported
 *
 * in : s_ipv6calc_anon_set = anonymization set
 * return:
 *   2: special check succeeded
 *   1: no special checks needed
 *   0: not supported
 */
int libipv6calc_anon_supported(const s_ipv6calc_anon_set *ipv6calc_anon_set) {
	/* check requirements */
	if (ipv6calc_anon_set->method == ANON_METHOD_KEEPTYPEASNCC) {
		// check for support
		if (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB | ANON_METHOD_KEEPTYPEASNCC_IPV6_REQ_DB) == 1) {
			return(2);
		} else {
			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_CC) != 1) {
				fprintf(stderr, "ipv6calc anonymization method not supported, missing included/available database: IPv4->CountryCode (GeoIP)\n");
			};
			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_CC) != 1) {
				fprintf(stderr, "ipv6calc anonymization method not supported, missing included/available database: IPv6->CountryCode (GeoIP)\n");
			};
			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_AS) != 1) {
				fprintf(stderr, "ipv6calc anonymization method not supported, missing included/available database: IPv4->AutonomousSystemNumber (GeoIP)\n");
			};
			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_AS) != 1) {
				fprintf(stderr, "ipv6calc anonymization method not supported, missing included/available database: IPv6->AutonomousSystemNumber (GeoIP)\n");
			};
			return(0);
		};
	} else {
		return(1);
	};
};


/*
 * return pointer to registry name by number
 */
const char *libipv6calc_registry_string_by_num(const int registry) {
	int j = -1, i;
	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_registries); i++ ) {
		if (ipv6calc_registries[i].number == registry) {
			j = i;
			break;
		};
	};

	if (j == -1) {
		fprintf(stderr, "ERROR - undefined registry number: %d (this should not happen)\n", registry);
		exit(1);
	};

	return(ipv6calc_registries[j].token);
};
