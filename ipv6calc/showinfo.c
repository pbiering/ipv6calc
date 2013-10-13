/*
 * Project    : ipv6calc
 * File       : showinfo.c
 * Version    : $Id: showinfo.c,v 1.91 2013/10/13 16:18:44 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Function to show information about a given IPv6 address
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ipv6calc.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"
#include "libipv6addr.h"
#include "libipv4addr.h"
#include "libipv6calc.h"
#include "libieee.h"
#include "libmac.h"
#include "libeui64.h"
#include "config.h"
#include "showinfo.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"
#include "../databases/lib/libipv6calc_db_wrapper_GeoIP.h"
#include "../databases/lib/libipv6calc_db_wrapper_IP2Location.h"

#ifdef SUPPORT_IP2LOCATION
#include "IP2Location.h"

/* 
 * API_VERSION is defined as a bareword in IP2Location.h, 
 * we need this trick to stringify it. Blah.
 */
#define makestr(x) #x
#define xmakestr(x) makestr(x)

extern int use_ip2location_ipv4;
extern int use_ip2location_ipv6;
extern char file_ip2location_ipv4[NI_MAXHOST];
extern char file_ip2location_ipv6[NI_MAXHOST];
#endif

#ifdef SUPPORT_GEOIP
#include "GeoIP.h"
#include "GeoIPCity.h"
extern int use_geoip_ipv4;
extern int use_geoip_ipv6;
extern char file_geoip_ipv4[NI_MAXHOST];
extern char file_geoip_ipv6[NI_MAXHOST];
#endif

/* from anonymizer */
extern int mask_ipv6;
extern int mask_ipv4;
extern int mask_iid;
extern s_ipv6calc_anon_set ipv6calc_anon_set;

/*
 * show available types on machine readable format
 */
void showinfo_availabletypes(void) {
	int i;

	fprintf(stderr, "\nAvailable tokens for machine-readable output (printed in one line):\n");
	fprintf(stderr, " IPV6_TYPE=...                 : type of IPv6 address (commata separated)\n");
	fprintf(stderr, " ");
	for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
		fprintf(stderr, " %s", ipv6calc_ipv6addrtypestrings[i].token);
	};
	fprintf(stderr, "\n");
	fprintf(stderr, " IPV6=...                      : given IPv6 address full uncompressed\n");
	fprintf(stderr, " IPV6_ANON=...                 : given anonymized IPv6 address full uncompressed\n");
	fprintf(stderr, " IPV6_REGISTRY=...             : registry token of given IPv6 address\n");
	fprintf(stderr, " IPV6_PREFIXLENGTH=ddd         : given prefix length\n");
	fprintf(stderr, " IPV6_AS_NUM=...               : AS number of (anonymized) IPv6 address\n");
	fprintf(stderr, " IPV6_COUNTRYCODE=...          : Country Code of (anonymized) IPv6 address\n");
	fprintf(stderr, " IPV4=ddd.ddd.ddd.ddd          : native IPv4 address\n");
	fprintf(stderr, " IPV4_ANON=ddd.ddd.ddd.ddd     : native anonymized IPv4 address\n");
	fprintf(stderr, " IPV4_REGISTRY=...             : registry token of native IPv4 address\n");
	fprintf(stderr, " IPV4_PREFIXLENGTH=ddd         : given prefix length of native IPv4 address\n");
	fprintf(stderr, " IPV4[...]=ddd.ddd.ddd.ddd     : included IPv4 address in IID or SLA (e.g. ISATAP, TEREDO, NAT64, 6to4)\n");
	fprintf(stderr, " IPV4_ANON[...]=ddd.ddd.ddd.ddd: included anonymized IPv4 address in IID or SLA (e.g. ISATAP, TEREDO, NAT64, 6to4)\n");
	fprintf(stderr, " IPV4_REGISTRY[...]=...        : registry token of included IPv4 address\n");
	fprintf(stderr, " IPV4_SOURCE[...]=...          : source of IPv4 address\n");
	fprintf(stderr, " IPV4_AS_NUM[...]=...          : AS number of (anonymized) IPv4 address\n");
	fprintf(stderr, " IPV4_COUNTRYCODE[...]=...     : Country Code of (anonymized) IPv4 address\n");
	fprintf(stderr, "  ISATAP|TEREDO-SERVER|TEREDO-CLIENT|6TO4|LINK-LOCAL-IID\n");
	fprintf(stderr, " SLA=xxxx                      : an included SLA\n");
	fprintf(stderr, " IID=xxxx:xxxx:xxxx:xxxx       : an included interface identifier\n");
	fprintf(stderr, " EUI48=xx:xx:xx:xx:xx:xx       : an included EUI-48 (MAC) identifier\n");
	fprintf(stderr, " EUI48_SCOPE=local|global      : scope of EUI-48 identifier\n");
	fprintf(stderr, " EUI48_TYPE=...                : type of EUI-48 identifier\n");
	fprintf(stderr, "  unicast|multicast|broadcast\n");
	fprintf(stderr, " EUI64=xx:xx:xx:xx:xx:xx:xx:xx : an included EUI-64 identifier\n");
	fprintf(stderr, " EUI64_SCOPE=local-*|global    : scope of EUI-64 identifier\n");
	fprintf(stderr, " OUI=\"...\"                     : OUI string, if available\n");
	fprintf(stderr, " TEREDO_PORT_CLIENT=...        : port of Teredo client (NAT outside)\n");
#ifdef SUPPORT_IP2LOCATION
	fprintf(stderr, " IP2LOCATION_COUNTRY_SHORT=... : Country code of IP address\n");
	fprintf(stderr, " IP2LOCATION_COUNTRY_LONG=...  : Country of IP address\n");
	fprintf(stderr, " IP2LOCATION_REGION=...        : Region of IP address\n");
	fprintf(stderr, " IP2LOCATION_CITY=...          : City of IP address\n");
	fprintf(stderr, " IP2LOCATION_ISP=...           : ISP of IP address\n");
	fprintf(stderr, " IP2LOCATION_LATITUDE=...      : Latitude of IP address\n");
	fprintf(stderr, " IP2LOCATION_LONGITUDE=...     : Longitude of IP address\n");
	fprintf(stderr, " IP2LOCATION_DOMAIN=...        : Domain of IP address\n");
	fprintf(stderr, " IP2LOCATION_ZIPCODE=...       : Zip code of IP address\n");
	fprintf(stderr, " IP2LOCATION_DATABASE_INFO_IPV4=... : Information about the used IPv4 database\n");
	fprintf(stderr, " IP2LOCATION_DATABASE_INFO_IPV6=... : Information about the used IPv6 database\n");
#endif
#ifdef SUPPORT_GEOIP
	fprintf(stderr, " GEOIP_COUNTRY_SHORT=...       : Country code of IP address\n");
	fprintf(stderr, " GEOIP_COUNTRY_LONG=...        : Country of IP address\n");
	fprintf(stderr, " GEOIP_REGION=...              : Region of IP address\n");
	fprintf(stderr, " GEOIP_CITY=...                : City of IP address\n");
	fprintf(stderr, " GEOIP_ZIPCODE=...             : Zip code of IP address\n");
	fprintf(stderr, " GEOIP_LATITUDE=...            : Latitude of IP address\n");
	fprintf(stderr, " GEOIP_LONGITUDE=...           : Longitude of IP address\n");
	fprintf(stderr, " GEOIP_AS_TEXT=...             : Autonomous System information\n");
	fprintf(stderr, " GEOIP_DATABASE_INFO_IPV4=...  : Information about the used IPv4 database\n");
	fprintf(stderr, " GEOIP_DATABASE_INFO_IPV6=...  : Information about the used IPv6 database\n");
#endif
	fprintf(stderr, " IPV6CALC_NAME=name            : Name of ipv6calc\n");
	fprintf(stderr, " IPV6CALC_VERSION=x.y          : Version of ipv6calc\n");
	fprintf(stderr, " IPV6CALC_COPYRIGHT=\"...\"      : Copyright string\n");
	fprintf(stderr, " IPV6CALC_OUTPUT_VERSION=x     : Version of output format\n");
	fprintf(stderr, " IPV6CALC_FEATURES=\"...\"       : Feature string of ipv6calc -v\n");
	fprintf(stderr, " IPV6CALC_SETTINGS_ANON=...    : Anonymizer settings\n");
};

/*
 * print one information
 */
static void printout(const char *string) {
	const char *prefix = "";
	/* const char *prefix = "ipv6calc_"; */
	const char *suffix = "\n";
	
	fprintf(stdout, "%s%s%s", prefix, string, suffix);
};

static void printfooter(const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST] = "";
	char tempstring2[NI_MAXHOST] = "";
	int i;

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6CALC_NAME=%s", PROGRAM_NAME);
		printout(tempstring);
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6CALC_VERSION=%s", PACKAGE_VERSION);
		printout(tempstring);
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6CALC_COPYRIGHT=\"%s\"", PROGRAM_COPYRIGHT);
		printout(tempstring);
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6CALC_OUTPUT_VERSION=%d", IPV6CALC_OUTPUT_VERSION);
		printout(tempstring);

		libipv6calc_anon_infostring(tempstring2, sizeof(tempstring2), &ipv6calc_anon_set);
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6CALC_SETTINGS_ANON=%s", tempstring2);
		printout(tempstring);

		tempstring[0] = '\0'; /* clear tempstring */

#ifdef SUPPORT_IP2LOCATION
#ifdef SUPPORT_IP2LOCATION_DYN
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%s IP2Location(dyn-load)", tempstring);
#else
#ifdef SUPPORT_IP2LOCATION_STATIC
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%s IP2Location(static)", tempstring);
#else
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%s IP2Location", tempstring);
#endif
#endif
		snprintf(tempstring, sizeof(tempstring) - 1, "%s", tempstring2);
#endif

#ifdef SUPPORT_GEOIP
#ifdef SUPPORT_GEOIP_DYN
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%s GeoIP(dyn-load)", tempstring);
#else
#ifdef SUPPORT_GEOIP_STATIC
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%s GeoIP(static)", tempstring);
#else
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%s GeoIP", tempstring);
#endif
#endif
		snprintf(tempstring, sizeof(tempstring) - 1, "%s", tempstring2);
#ifdef SUPPORT_GEOIP_V6
		snprintf(tempstring2, sizeof(tempstring2) - 1, "%s GeoIPv6", tempstring);
		snprintf(tempstring, sizeof(tempstring) - 1, "%s", tempstring2);
#endif
#endif
		/* cut off first char */
		for (i = 0; i < strlen(tempstring); i++) {
			tempstring[i] = tempstring[i+1];
		};

		libipv6calc_db_wrapper_features(tempstring, sizeof(tempstring));
	
		snprintf(tempstring2, sizeof(tempstring2) - 1, "IPV6CALC_FEATURES=\"%s\"", tempstring);
		printout(tempstring2);
	};
};


#ifdef SUPPORT_IP2LOCATION
/* print IP2Location information */
#define DEBUG_function_name "showinfo/print_ip2location"
static void print_ip2location(char *addrstring, const uint32_t formatoptions, const char *additionalstring, int version) {
	DEBUGPRINT_NA(DEBUG_showinfo, "Called");

	if (wrapper_features_IP2Location == 0) {
		DEBUGPRINT_NA(DEBUG_showinfo, "IP2Location support not active");
		return;
	};

	int ip2location_type = 0;

	IP2Location *IP2LocationObj;

	static int flag_ip2location_info_shown_ipv4 = 0;
	static int flag_ip2location_info_shown_ipv6 = 0;

	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable);
	char tempstring[NI_MAXHOST] = "";
	char *CountryCode, *CountryName;
	static int flag_info_show = 0;

	if ( (ipv6calc_debug & DEBUG_showinfo) != 0 ) {
		fprintf(stderr, "%s/%s: Called addrstring=%s formatoptions=0x%08x additionalstring=%s version=%d\n", __FILE__, __func__, addrstring, formatoptions, additionalstring, version);
	};

	if (version == 4) {
		if (libipv6calc_db_wrapper_IP2Location_has_features(IPV6CALC_DB_IPV4_TO_CC) != 1) {
			return;
		};
		ip2location_type = IP2LOCATION_DB_IP_COUNTRY;
	} else if (version == 6) {
		if (libipv6calc_db_wrapper_IP2Location_has_features(IPV6CALC_DB_IPV6_TO_CC) != 1) {
			return;
		};
		ip2location_type = IP2LOCATION_DB_IPV6_COUNTRY;
	} else {
		fprintf(stderr, " IP2Location for IPv%d is not supported\n", version);
		return;
	};

	CountryCode = libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr(addrstring, version);
	CountryName = libipv6calc_db_wrapper_IP2Location_wrapper_country_name_by_addr(addrstring, version);

	if (machinereadable != 0) {
		if (CountryCode != NULL) {
			snprintf(tempstring, sizeof(tempstring), "IP2LOCATION_COUNTRY_SHORT%s=%s", additionalstring, CountryCode);
			printout(tempstring);
			flag_info_show = 1;
		};

		if (CountryName != NULL) {
			snprintf(tempstring, sizeof(tempstring), "IP2LOCATION_COUNTRY_LONG%s=%s", additionalstring, CountryName);
			printout(tempstring);
			flag_info_show = 1;
		};
	} else {
		if (strlen(additionalstring) > 0) {
			snprintf(tempstring, sizeof(tempstring), "IP2Location reports for %s country %s (%s)", additionalstring, (CountryName != 0 ? CountryName : "UNKNOWN"), (CountryCode != NULL ? CountryCode : "??"));
		} else {
			snprintf(tempstring, sizeof(tempstring), "IP2Location reports country %s (%s)", (CountryName != 0 ? CountryName : "UNKNOWN"), (CountryCode != NULL ? CountryCode : "??"));
		};
		printout(tempstring);
	};

	if ( (ipv6calc_debug & DEBUG_showinfo) != 0 ) {
		fprintf(stderr, "%s: IP2Location IPv%d try to open database by type: %s\n", DEBUG_function_name, version, libipv6calc_db_wrapper_IP2Location_dbdescription(ip2location_type));
	};

	if (libipv6calc_db_wrapper_IP2Location_db_avail(ip2location_type) != 1) {
		if ( (ipv6calc_debug & DEBUG_showinfo) != 0 ) {
			fprintf(stderr, "%s: IP2Location IPv%d database by type not available: %s\n", DEBUG_function_name, version, libipv6calc_db_wrapper_IP2Location_dbdescription(ip2location_type));
		};
		// stay silent
		return;
	};

	IP2LocationObj = libipv6calc_db_wrapper_IP2Location_open_type(ip2location_type);

	if (flag_info_show != 0) {
		if (machinereadable != 0) {
			snprintf(tempstring, sizeof(tempstring), "IP2LOCATION_DATABASE_INFO_IPV%d=%s", version, libipv6calc_db_wrapper_IP2Location_database_info(IP2LocationObj));
			printout(tempstring);
		};
	};

	IP2LocationRecord *record = libipv6calc_db_wrapper_IP2Location_get_all(IP2LocationObj, (char*) addrstring);

	if ((version == 4) && (flag_ip2location_info_shown_ipv4 == 0)) {
		flag_ip2location_info_shown_ipv4 = 1;
		flag_info_show = 1;
	} else if ((version == 6) && (flag_ip2location_info_shown_ipv6 == 0)) {
		flag_ip2location_info_shown_ipv6 = 1;
		flag_info_show = 1;
	};

	if (record != NULL) {
		if ( machinereadable != 0 ) {
#define TEST_IP2LOCATION_AVAILABLE(v)	((strstr(v, "unavailable") == NULL) && (strstr(v, "demo database") == NULL))

			if (TEST_IP2LOCATION_AVAILABLE(record->region)) {
				snprintf(tempstring, sizeof(tempstring) - 1, "IP2LOCATION_REGION%s=%s", additionalstring, record->region);
				printout(tempstring);
			};

			if (TEST_IP2LOCATION_AVAILABLE(record->city)) {
				snprintf(tempstring, sizeof(tempstring) - 1, "IP2LOCATION_CITY%s=%s", additionalstring, record->city);
				printout(tempstring);
			};

			if (TEST_IP2LOCATION_AVAILABLE(record->isp)) {
				snprintf(tempstring, sizeof(tempstring) - 1, "IP2LOCATION_ISP%s=%s", additionalstring, record->isp);
				printout(tempstring);
			};

			if ((record->latitude != 0) && (record->longitude != 0)) {
				snprintf(tempstring, sizeof(tempstring) - 1, "IP2LOCATION_LATITUDE%s=%f", additionalstring, record->latitude);
				printout(tempstring);
				snprintf(tempstring, sizeof(tempstring) - 1, "IP2LOCATION_LONGITUDE%s=%f", additionalstring, record->longitude);
				printout(tempstring);
			};

			if (TEST_IP2LOCATION_AVAILABLE(record->domain)) {
				snprintf(tempstring, sizeof(tempstring) - 1, "IP2LOCATION_DOMAIN%s=%s", additionalstring, record->domain);
				printout(tempstring);
			};

			if (TEST_IP2LOCATION_AVAILABLE(record->zipcode)) {
				if (strcmp(record->zipcode, "-") != 0) {
					snprintf(tempstring, sizeof(tempstring) - 1, "IP2LOCATION_ZIPCODE%s=%s", additionalstring, record->zipcode);
					printout(tempstring);
				};
			};
		} else {
			fprintf(stderr, " IP2Location not machinereadable output currently not supported\n");
		};

		libipv6calc_db_wrapper_IP2Location_free_record(record);
	} else {
		if ((formatoptions & FORMATOPTION_quiet) == 0) {
			fprintf(stderr, "%s: IP2Location returned no record for address: %s\n", DEBUG_function_name, addrstring);
		};
	};

	libipv6calc_db_wrapper_IP2Location_close(IP2LocationObj);
};
#undef DEBUG_function_name
#endif

#ifdef SUPPORT_GEOIP
#define DEBUG_function_name "showinfo/print_geoip"
/* print GeoIP information */
static void print_geoip(const char *addrstring, const uint32_t formatoptions, const char *additionalstring, int version) {
	DEBUGPRINT_NA(DEBUG_showinfo, "Called");

	if (wrapper_features_GeoIP == 0) {
		DEBUGPRINT_NA(DEBUG_showinfo, "GeoIP support not active");
		return;
	};

	GeoIP *gi = NULL;

	const char *returnedCountry = NULL;
	const char *returnedCountryName = NULL;
	char *as_text;
	GeoIPRecord *gir = NULL;
	int flag_geoip_ok = 0;
	static int flag_geoip_info_shown = 0;

	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable);
	char tempstring[NI_MAXHOST] = "";

	as_text = libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(addrstring, version);
	if (as_text != NULL) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_AS_TEXT%s=%s", additionalstring, as_text);
			printout(tempstring);
		};
	};

	gir = libipv6calc_db_wrapper_GeoIP_wrapper_record_city_by_addr(addrstring, version);
	if (gir != NULL) {
		flag_geoip_ok = 1;

		if ( (ipv6calc_debug & DEBUG_showinfo) != 0 ) {
			fprintf(stderr, "%s: GeoIP IPv%d city database result\n", DEBUG_function_name, version);
		};

		if ( machinereadable != 0 ) {
			if (gir->region != NULL) {
				snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_REGION%s=%s", additionalstring, gir->region);
				printout(tempstring);
			};
			if (gir->city != NULL) {
				snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_CITY%s=%s", additionalstring, gir->city);
				printout(tempstring);
			};
			if (gir->postal_code != NULL) {
				snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_ZIPCODE%s=%s", additionalstring, gir->postal_code);
				printout(tempstring);
			};
			snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_LATITUDE%s=%f", additionalstring, gir->latitude);
			printout(tempstring);
			snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_LONGITUDE%s=%f", additionalstring, gir->longitude);
			printout(tempstring);
			/*
			snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_DMACODE%s=%d", additionalstring, gir->dma_code);
			printout(tempstring);
			snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_AREACODE%s=%d", additionalstring, gir->area_code);
			printout(tempstring);
			*/
		} else {
			fprintf(stderr, " GeoIP not machinereadable output currently not supported\n");
		};
		libipv6calc_db_wrapper_GeoIPRecord_delete(gir);
	};

	returnedCountry     = libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(addrstring, version);
	returnedCountryName = libipv6calc_db_wrapper_GeoIP_wrapper_country_name_by_addr(addrstring, version);

	if (returnedCountry != NULL) {
		flag_geoip_ok = 1;

		if ( (ipv6calc_debug & DEBUG_showinfo) != 0 ) {
			fprintf(stderr, "%s: GeoIP IP%d country database result\n", DEBUG_function_name, version);
		};

		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_COUNTRY_SHORT%s=%s", additionalstring, returnedCountry);
			printout(tempstring);

			if (returnedCountryName != NULL) {
				snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_COUNTRY_LONG%s=%s", additionalstring, returnedCountryName);
				printout(tempstring);
			} else {
				DEBUGPRINT_NA(DEBUG_showinfo, "returnedCountryName=NULL");
			};
		} else {
			if (returnedCountryName != NULL) {
				fprintf(stderr, " GeoIP country name and code: %s (%s)\n", returnedCountryName, returnedCountry);
			} else {
				fprintf(stderr, " GeoIP country code: %s\n", returnedCountry);
			};
		};
	};

	if (flag_geoip_ok == 1) {
		if (flag_geoip_info_shown == 0) {
			flag_geoip_info_shown = 1;

			if ( machinereadable != 0 ) {
				snprintf(tempstring, sizeof(tempstring) - 1, "GEOIP_DATABASE_INFO_IPV%d=%s apiversion=system", version, libipv6calc_db_wrapper_GeoIP_database_info(gi));
				printout(tempstring);
			};
		};
	} else {
		if ( (ipv6calc_debug & DEBUG_showinfo) != 0 ) {
			fprintf(stderr, "%s: GeoIP returned no record for address: %s\n", DEBUG_function_name, addrstring);
		};
	};

	libipv6calc_db_wrapper_GeoIP_delete(gi);
};
#undef DEBUG_function_name
#endif


/* print IPv4 address */
#define DEBUG_function_name "showinfo/print_ipv4addr"
static void print_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions, const char *string) {
	char tempstring[NI_MAXHOST] = "", tempstring2[NI_MAXHOST] = "", helpstring[NI_MAXHOST] = "";
	char tempipv4string[NI_MAXHOST] = "";
	char embeddedipv4string[NI_MAXHOST] = "";
	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable), as_num32 = ASNUM_AS_UNKNOWN;
	int retval, i, j, retval_anon = 1;
	uint32_t typeinfo;
	ipv6calc_ipv4addr ipv4addr_anon, *ipv4addr_anon_ptr;
	uint16_t cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	char *as_text = NULL;

	ipv4addr_anon_ptr = &ipv4addr_anon;

	typeinfo = ipv4addr_gettype(ipv4addrp);

	DEBUGPRINT_WA(DEBUG_showinfo, "result of 'ipv4addr_gettype': 0x%08x", (unsigned int) typeinfo);

	retval = libipv4addr_ipv4addrstruct_to_string(ipv4addrp, tempipv4string, 0);
	if ( retval != 0 ) {
		fprintf(stderr, "Error converting IPv4 address: %s\n", tempipv4string);
	};

	if ((formatoptions & FORMATOPTION_printembedded) != 0) {
		snprintf(embeddedipv4string, sizeof(embeddedipv4string) - 1, "[%s]", tempipv4string);
	};

	ipv4addr_copy(ipv4addr_anon_ptr, ipv4addrp); /* copy structure */

	if ((typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {	
		retval_anon = libipv4addr_anonymize(ipv4addr_anon_ptr, ipv6calc_anon_set.mask_ipv4, ANON_METHOD_KEEPTYPEASNCC);

		if (retval_anon == 0) {
			retval = libipv4addr_ipv4addrstruct_to_string(ipv4addr_anon_ptr, tempstring2, 0);
			if ( retval != 0 ) {
				fprintf(stderr, "Error uncompressing IPv4 address: %s\n", tempstring2);
				retval = 1;
				retval_anon = 2;
			};
		};
	};
	
	if ( machinereadable != 0 ) {
		/* given source string */
		if ((string != NULL) && (strlen(string) > 0)) {
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_SOURCE%s=%s", embeddedipv4string, string);
			printout(tempstring);
		};

		/* address */
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV4%s=%s", embeddedipv4string, tempipv4string);
		printout(tempstring);

		if ((typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {	
			if (retval_anon == 0 ) {	
				/* anonymized address */
				snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_ANON%s=%s", embeddedipv4string, tempstring2);
				printout(tempstring);
			} else {
				// TODO: only show this on verbose (must be implemented)
				// snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_ANON%s=(unsupported, too less DB features available)", embeddedipv4string);
				// printout(tempstring);
			};
		};

		if (ipv4addrp->flag_prefixuse == 1) {	
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_PREFIXLENGTH%s=%d", embeddedipv4string, (int) ipv4addrp->prefixlength);
			printout(tempstring);
		};

		j = 0;
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_TYPE%s=", embeddedipv4string);
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv4addrtypestrings); i++ ) {
			if ( (typeinfo & ipv6calc_ipv4addrtypestrings[i].number) != 0 ) {
				if (j != 0) {
					snprintf(helpstring, sizeof(helpstring) - 1, "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring) - 1, "%s", helpstring);
				};
				snprintf(helpstring, sizeof(helpstring) - 1, "%s%s", tempstring, ipv6calc_ipv4addrtypestrings[i].token);
				snprintf(tempstring, sizeof(tempstring) - 1, "%s", helpstring);
				j = 1;
			};
		};
		printout(tempstring);
	} else {
		fprintf(stdout, "IPv4 address: %s\n", tempipv4string);

		fprintf(stdout, "IPv4 address type: ");
		j = 0;
		for (i = 0; i < (int) (sizeof(ipv6calc_ipv4addrtypestrings) / sizeof(ipv6calc_ipv4addrtypestrings[0])); i++ ) {
			if ( (typeinfo & ipv6calc_ipv4addrtypestrings[i].number) != 0 ) {
				if ( j != 0 ) { fprintf(stdout, ", "); };
				fprintf(stdout, "%s", ipv6calc_ipv4addrtypestrings[i].token);
				j = 1;
			};
		};
		fprintf(stdout, "\n");
	};	

	/* get AS Information */
	if ((typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
		DEBUGPRINT_NA(DEBUG_showinfo, "Call ipv4addr_anonymized_get_as_num32");
		as_num32 = ipv4addr_anonymized_get_as_num32(ipv4addrp);
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_AS) == 1) {
			as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(tempipv4string, 4);
			as_text = libipv6calc_db_wrapper_as_text_by_addr(tempipv4string, 4);
		};
	};

	if (as_num32 != ASNUM_AS_UNKNOWN) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_AS_NUM%s=%d", embeddedipv4string, as_num32);
			printout(tempstring);

			if (as_text != NULL) {
				snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_AS_TEXT%s=%s", embeddedipv4string, as_text);
				printout(tempstring);
			};
		} else {
			if (as_text != NULL) {
				fprintf(stdout, "Autonomous System Information: %s\n", as_text);
			} else {
				fprintf(stdout, "Autonomous System Number (32-bit): %d\n", as_num32);
			};
		};
	} else {
		DEBUGPRINT_NA(DEBUG_showinfo, "Skip AS print: as_num32=ASNUM_AS_UNKNOWN");
	};

	/* get CountryCode Information */
	if ((typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
		cc_index = ipv4addr_anonymized_get_cc_index(ipv4addrp);
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_CC) == 1) {
			cc_index = libipv6calc_db_wrapper_cc_index_by_addr(tempipv4string, 4);
		};
	};

	if (cc_index != COUNTRYCODE_INDEX_UNKNOWN) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_COUNTRYCODE%s=%c%c", embeddedipv4string, COUNTRYCODE_INDEX_TO_CHAR1(cc_index), COUNTRYCODE_INDEX_TO_CHAR2(cc_index));
			printout(tempstring);
		} else {
			fprintf(stdout, "Country Code: %c%c\n", COUNTRYCODE_INDEX_TO_CHAR1(cc_index), COUNTRYCODE_INDEX_TO_CHAR2(cc_index));
		};
	};

	if ((typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {
		/* get registry string */
		// TODO: get registry by AS, if possible
		retval = libipv4addr_get_registry_string(ipv4addrp, helpstring);
		if (retval == 1) {
			if (machinereadable == 0) {
				fprintf(stderr, "Error getting registry string for IPv4 address: %s (%s)\n", helpstring, tempipv4string);
			};
		} else {
			if (machinereadable != 0) {
					snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_REGISTRY%s=%s", embeddedipv4string, helpstring);
					printout(tempstring);
			} else {
				fprintf(stdout, "IPv4 registry%s: %s\n", embeddedipv4string, helpstring);
			};
		};
	} else if (as_num32 != ASNUM_AS_UNKNOWN) {
		int registry = libipv6calc_db_wrapper_registry_num_by_cc_index(cc_index);

		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV4_REGISTRY%s=%s", embeddedipv4string, libipv6calc_registry_string_by_num(registry));
			printout(tempstring);
		} else {
			fprintf(stdout, "Registry for address: %s\n", ipv6calc_registries[j].token);
		};
	};


	if ((typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {
#ifdef SUPPORT_IP2LOCATION
		/* IP2Location information */
		print_ip2location(tempipv4string, formatoptions, embeddedipv4string, 4);
#endif

#ifdef SUPPORT_GEOIP
		/* GeoIP information */
		if (use_geoip_ipv4 != 0) {
			print_geoip(tempipv4string, formatoptions, embeddedipv4string, 4);
		};
#endif
	};

	return;
};
#undef DEBUG_function_name


/*
 * print EUI-48/MAC information
 */
#define DEBUG_function_name "showinfo/print_eui48"
static void print_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST], helpstring[NI_MAXHOST];
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	int i, result;
	ipv6calc_ipv4addr ipv4addr;

	/* EUI-48/MAC address */
	snprintf(helpstring, sizeof(helpstring) - 1, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) macaddrp->addr[0], (unsigned int) macaddrp->addr[1], (unsigned int) macaddrp->addr[2], (unsigned int) macaddrp->addr[3], (unsigned int) macaddrp->addr[4], (unsigned int) macaddrp->addr[5]);

	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "EUI48=%s", helpstring);
		printout(tempstring);
	} else {
		fprintf(stdout, "EUI-48/MAC address: %s\n", helpstring);
	};

	/* scope */	
	if ( (macaddrp->addr[0] & 0x02) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI48_SCOPE=global");
		} else {
			fprintf(stdout, "MAC is a global unique one\n");
		};
	} else {
		if ( machinereadable != 0 ) {
			printout("EUI48_SCOPE=local");
		} else {
			fprintf(stdout, "MAC is a local one\n");
		};
	};
	
	/* unicast/multicast/broadcast */	
	if ( (macaddrp->addr[0] & 0x01) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI48_TYPE=unicast");
		} else {
			fprintf(stdout, "MAC is an unicast one\n");
		};
	} else {
		if ( (macaddrp->addr[0] == 0xff) && (macaddrp->addr[1] == 0xff) && (macaddrp->addr[2] == 0xff) && (macaddrp->addr[3] == 0xff) && (macaddrp->addr[4] == 0xff) && (macaddrp->addr[5] == 0xff) ) {
			if ( machinereadable != 0 ) {
				printout("EUI48_TYPE=broadcast");
			} else {
				fprintf(stdout, "MAC is a broadcast one\n");
			};
		} else {
			if ( machinereadable != 0 ) {
				printout("EUI48_TYPE=multicast");
			} else {
				fprintf(stdout, "MAC is a multicast one\n");
			};

		};
	};
	
	/* vendor string */
	result = libieee_get_vendor_string(helpstring, macaddrp);
	if (result == 0) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "OUI=\"%s\"", helpstring);
			printout(tempstring);
		} else {
			fprintf(stdout, "OUI is: %s\n", helpstring);
		};
	};

	/* check for Linux ISDN-NET/PLIP */
	if ( (macaddrp->addr[0] == 0xfc) && (macaddrp->addr[1] == 0xfc) ) {
		/* copy address */
		for ( i = 0; i <= 3; i++ ) {
			ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) macaddrp->addr[i + 2]);
		};
		ipv4addr.scope = ipv4addr_gettype(&ipv4addr);

		if ( machinereadable != 0 ) {
			/* no additional hint */
		} else {
			fprintf(stdout, "Address type contains IPv4 address:\n");
		};
		print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "ISDN-NET/PLIP");
	};

	return;
};
#undef DEBUG_function_name


/*
 * print EUI-64 information
 */
#define DEBUG_function_name "showinfo/print_eui64"
static void print_eui64(const ipv6calc_eui64addr *eui64addrp, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST], helpstring[NI_MAXHOST];
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	int result, i;
	ipv6calc_macaddr macaddr;

	/* EUI-64 address */
	snprintf(helpstring, sizeof(helpstring) - 1, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) eui64addrp->addr[0], (unsigned int) eui64addrp->addr[1], (unsigned int) eui64addrp->addr[2], (unsigned int) eui64addrp->addr[3], (unsigned int) eui64addrp->addr[4], (unsigned int) eui64addrp->addr[5], (unsigned int) eui64addrp->addr[6], (unsigned int) eui64addrp->addr[7]);

	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "EUI64=%s", helpstring);
		printout(tempstring);
	} else {
		fprintf(stdout, "EUI-64 identifier: %s\n", helpstring);
	};
	
	/* scope */	
	if ( (eui64addrp->addr[0] & 0x02) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI64_SCOPE=global");
		} else {
			fprintf(stdout, "EUI-64 identifier is a global unique one\n");
		};
	} else {
		if ( machinereadable != 0 ) {
			printout("EUI64_SCOPE=local");
		} else {
			fprintf(stdout, "EUI-64 identifier is a local one\n");
		};
	};

	/* get vendor string */
	for (i = 0; i < 6; i++) {	
		macaddr.addr[i] = eui64addrp->addr[i];
	};

	result = libieee_get_vendor_string(helpstring, &macaddr);
	if (result == 0) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "OUI=\"%s\"", helpstring);
			printout(tempstring);
		} else {
			fprintf(stdout, "OUI is: %s\n", tempstring);
		};
	};
	
	return;
};
#undef DEBUG_function_name

/*
 * function shows information about a given IPv6 address
 *
 * in : *ipv6addrp = pointer to IPv6 address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "showinfo_ipv6addr"
int showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp1, const uint32_t formatoptions) {
	int retval = 1, i, j, flag_prefixuse, registry, r, retval_anon = 1;;
	char tempstring[NI_MAXHOST] = "", tempstring2[NI_MAXHOST] = "", helpstring[NI_MAXHOST] = "";
	char ipv6addrstring[NI_MAXHOST] = "";
	const char *cc;
	ipv6calc_ipv6addr ipv6addr, ipv6addr_anon, *ipv6addrp, *ipv6addr_anon_ptr;
	ipv6calc_ipv4addr ipv4addr, ipv4addr2;
	ipv6calc_macaddr macaddr;
	ipv6calc_eui64addr eui64addr;
	uint16_t port;
	uint32_t typeinfo;
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	uint32_t payload, as_num32, cc_index;

	ipv6addr_anon_ptr = &ipv6addr_anon;

	ipv6addrp = &ipv6addr;
	ipv6addr_copy(ipv6addrp, ipv6addrp1); /* copy structure */

	typeinfo = ipv6addr_gettype(ipv6addrp);
	registry = ipv6addr_getregistry(ipv6addrp);

	if ( (ipv6calc_debug & DEBUG_showinfo) != 0) {
		fprintf(stderr, "%s: result of 'ipv6addr_getregistry': %d\n", DEBUG_function_name, registry);
		fprintf(stderr, "%s:       %08x (result of 'ipv6addr_gettype')\n", DEBUG_function_name, (unsigned int) typeinfo);
	};

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++ ) {
		if ( (ipv6calc_debug & DEBUG_showinfo) != 0) {
			fprintf(stderr, "%s: test: %08x : %s\n", DEBUG_function_name, (unsigned int) ipv6calc_ipv6addrtypestrings[i].number, ipv6calc_ipv6addrtypestrings[i].token);
		};
	};	

	/* get full uncompressed IPv6 address */
	flag_prefixuse = ipv6addrp->flag_prefixuse;
	ipv6addrp->flag_prefixuse = 0;
	retval = libipv6addr_ipv6addrstruct_to_uncompaddr(ipv6addrp, ipv6addrstring, FORMATOPTION_printfulluncompressed);
	if ( retval != 0 ) {
		fprintf(stderr, "Error uncompressing IPv6 address: %s\n", ipv6addrstring);
		retval = 1;
		goto END;
	};	

	ipv6addrp->flag_prefixuse = flag_prefixuse;

	if ((typeinfo & (IPV6_ADDR_ANONYMIZED_PREFIX | IPV6_ADDR_ANONYMIZED_IID)) == 0 ) {
		ipv6addr_copy(ipv6addr_anon_ptr, ipv6addrp); /* copy structure */
		retval_anon = libipv6addr_anonymize(ipv6addr_anon_ptr, &ipv6calc_anon_set);

		if (retval_anon == 0) {
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(ipv6addr_anon_ptr, tempstring2, FORMATOPTION_printfulluncompressed);
			if ( retval != 0 ) {
				fprintf(stderr, "Error uncompressing IPv6 address: %s\n", tempstring2);
				retval = 1;
				retval_anon = 2;
			};
		};
	};
	
	if ( machinereadable != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6=%s", ipv6addrstring);
		printout(tempstring);
	
		if ((typeinfo & (IPV6_ADDR_ANONYMIZED_PREFIX | IPV6_ADDR_ANONYMIZED_IID | IPV6_ADDR_LOOPBACK)) == 0 ) {
			if (retval_anon == 0) {
				snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_ANON=%s", tempstring2);
			} else {
				// TODO: only show this on verbose (must be implemented)
				// snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_ANON=(unsupported, too less DB features available)");
			};
			printout(tempstring);
		};

		if (ipv6addrp->flag_prefixuse == 1) {	
			snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_PREFIXLENGTH=%d", (int) ipv6addrp->prefixlength);
			printout(tempstring);
		};

		j = 0;
		snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_TYPE=");
		for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
			if ( (typeinfo & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
				if (j != 0) {
					snprintf(helpstring, sizeof(helpstring) - 1, "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring) - 1, "%s", helpstring);
				};
				snprintf(helpstring, sizeof(helpstring) - 1, "%s%s", tempstring, ipv6calc_ipv6addrtypestrings[i].token);
				snprintf(tempstring, sizeof(tempstring) - 1, "%s", helpstring);
				j = 1;
			};
		};
		printout(tempstring);
	} else {
		fprintf(stdout, "Address type: ");
		j = 0;
		for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
			if ( (typeinfo & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
				if ( j != 0 ) { fprintf(stdout, ", "); };
				fprintf(stdout, "%s", ipv6calc_ipv6addrtypestrings[i].token);
				j = 1;
			};
		};
		fprintf(stdout, "\n");
	};	

	if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, 1);

		if (r == 0) {
			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, helpstring, 0);
			if ( retval != 0 ) {
				fprintf(stderr, "Error converting IPv4 address to string\n");
				retval = 1;
				goto END;
			};	

			if ( machinereadable != 0 ) {
				print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "6TO4");
			} else {
				fprintf(stdout, "Address type is 6to4 and included IPv4 address is: %s\n", helpstring);
			};

			/* get registry string */
			retval = libipv4addr_get_registry_string(&ipv4addr, helpstring);
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "IPv4 registry for 6to4 address: %s\n", helpstring);
			};
		};
	};

	if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
		/* extract Teredo client IPv4 address */
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, 1);

		if (r == 0) {
			r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr2, 2);

			if (r == 0) {
				print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "TEREDO-CLIENT");
				print_ipv4addr(&ipv4addr2, formatoptions | FORMATOPTION_printembedded, "TEREDO-SERVER");

				retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr2, helpstring, 0);
				if ( retval != 0 ) {
					fprintf(stderr, "Error converting IPv4 address to string\n");
					retval = 1;
					goto END;
				};	

				/* extract Teredo client UDP port */
				port = (uint16_t) (((uint16_t) ipv6addr_getoctet(ipv6addrp, (unsigned int) 10) << 8 | (uint16_t) ipv6addr_getoctet(ipv6addrp, (unsigned int) 11)) ^ 0xffff);

				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring) - 1, "TEREDO_PORT_CLIENT=%u", (unsigned int) port);
					printout(tempstring);
				} else {
					fprintf(stdout, "Address type is Teredo and included IPv4 server address is: %s and client port: %u\n", helpstring, (unsigned int) port);
				};

				/* get registry string */
				retval = libipv4addr_get_registry_string(&ipv4addr2, helpstring);
				
				if ( machinereadable != 0 ) {
				} else {
					fprintf(stdout, "IPv4 registry for Teredo server address: %s\n", helpstring);
				};
			};
		};
	};

	if ( (typeinfo & IPV6_NEW_ADDR_NAT64) != 0 )  {
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, 0);

		if (r == 0) {
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "IPv4 registry for NAT64 address: %s\n", helpstring);
			};
			print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "NAT64");
		};
	};

	/* SLA prefix included? */
	if ( ((typeinfo & ( IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID | IPV6_ADDR_ANONYMIZED_PREFIX)) == 0)) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "SLA=%04x", (unsigned int) ipv6addr_getword(ipv6addrp, 3));
			printout(tempstring);
		} else {
			fprintf(stdout, "Address type has SLA: %04x\n", (unsigned int) ipv6addr_getword(ipv6addrp, 3));
		};
	};
	
	/* IPv6 Registry, CountryCode, AS? */
	if ((typeinfo & IPV6_ADDR_ANONYMIZED_PREFIX) == 0) {
		/* no anonymized address */
		if ( (ipv6calc_debug & DEBUG_showinfo) != 0) {
			fprintf(stderr, "%s: Check registry: %d\n", DEBUG_function_name, registry);
		};

		/* get registry string */
		retval = libipv6addr_get_registry_string(ipv6addrp, helpstring);
		if ( retval == 1  && machinereadable == 0 ) {
			fprintf(stderr, "Error getting registry string for IPv6 address: %s\n", helpstring);
		} else {
			if ( machinereadable != 0 ) {
				snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_REGISTRY=%s", helpstring);
				printout(tempstring);
			} else {
				fprintf(stdout, "Registry for address: %s\n", helpstring);
			};
		};

		if ((typeinfo & IPV6_NEW_ADDR_AGU) != 0) {
			/* global address */

			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_CC) == 1) {
				/* get country code */
				cc = libipv6calc_db_wrapper_country_code_by_addr(ipv6addrstring, 6);
				if ((cc == NULL)  && (machinereadable == 0)) {
					fprintf(stderr, "Error getting country code for IPv6 address\n");
				} else {
					if ( machinereadable != 0 ) {
						snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_COUNTRYCODE=%s", (cc != NULL ? cc : "(unknown)"));
						printout(tempstring);
					} else {
						fprintf(stdout, "Country Code: %s\n", cc);
					};
				};
			};

			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_AS) == 1) {
				/* get AS Number */
				as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(ipv6addrstring, 6);
				if ((as_num32 == 0)  && (machinereadable == 0)) {
					fprintf(stderr, "Error getting AS number for IPv6 address\n");
				} else {
					if ( machinereadable != 0 ) {
						if (as_num32 == 0) {
							snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_AS_NUM=(unknown)");
						} else {
							snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_AS_NUM=%d", as_num32);
						};
						printout(tempstring);
					} else {
						fprintf(stdout, "ASN for address: %d\n", as_num32);
					};
				};
			};
		};
	} else {
		/* retrieve ASN from anonymization value */
		r = ipv6addr_get_payload_anonymized_prefix(ipv6addrp, ANON_PREFIX_PAYLOAD_ASN32, &as_num32);

		if (r == 0) {
			if ((as_num32 == 0) && (machinereadable == 0)) {
				fprintf(stderr, "Error getting AS number from anonymized IPv6 address\n");
			} else {
				if ( machinereadable != 0 ) {
					if (as_num32 == 0) {
						snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_AS_NUM=(unknown)");
					} else {
						snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_AS_NUM=%d", as_num32);
					};
					printout(tempstring);
				} else {
					fprintf(stdout, "ASN for address: %d\n", as_num32);
				};

				registry = libipv6calc_db_wrapper_registry_num_by_as_num32(as_num32);

				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_REGISTRY=%s", libipv6calc_registry_string_by_num(registry));
					printout(tempstring);
				} else {
					fprintf(stdout, "Registry for address: %s\n", libipv6calc_registry_string_by_num(registry));
				};
			};
		};

		/* retrieve CountryCodeIndex from anonymization value */
		r = ipv6addr_get_payload_anonymized_prefix(ipv6addrp, ANON_PREFIX_PAYLOAD_CCINDEX, &cc_index);

		if (r == 0) {
			if (cc_index > COUNTRYCODE_INDEX_MAX) {
				if ( machinereadable != 0 ) {
					fprintf(stderr, "Error getting CountryCode from anonymized IPv6 address\n");
				};
			} else if (cc_index == COUNTRYCODE_INDEX_UNKNOWN) {
				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_COUNTRYCODE=(unknown)");
					printout(tempstring);
				} else {
					fprintf(stdout, "Country Code: (unknown)\n");
				};
			} else if (cc_index <= COUNTRYCODE_INDEX_LETTER_MAX) {
				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_COUNTRYCODE=%c%c", COUNTRYCODE_INDEX_TO_CHAR1(cc_index), COUNTRYCODE_INDEX_TO_CHAR2(cc_index));
					printout(tempstring);
				} else {
					fprintf(stdout, "Country Code: %c%c\n", COUNTRYCODE_INDEX_TO_CHAR1(cc_index), COUNTRYCODE_INDEX_TO_CHAR2(cc_index));
				};
			} else {
				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring) - 1, "IPV6_COUNTRYCODE=(unsupported)");
					printout(tempstring);
				} else {
					fprintf(stdout, "Country Code: (unsupported)\n");
				};
			};
		};

  	};

	/* Proper solicited node link-local multicast address? */
	if ( (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0 ) {
		if ( (typeinfo & (IPV6_ADDR_LINKLOCAL & IPV6_ADDR_MULTICAST)) != 0 ) {
			/* address is ok */
		} else {
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "Address is not a proper 'solicited-node link-local multicast' address!\n");
				retval = 1;
				goto END;
			};
		};
	};
	
	/* Compat or mapped */
	if ( (typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) {
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, 0);

		if (r == 0) {
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "Address type is compat/mapped and include an IPv4 address\n");
			};

			print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "COMPAT/MAPPED");
		};
	};

	/* Interface identifier included */
	if ( ( ((typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0) || ((typeinfo & (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) == (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) ) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID)) == 0) ) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring) - 1, "IID=%04x:%04x:%04x:%04x", (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
			printout(tempstring);
		} else {
			fprintf(stdout, "Interface identifier: %04x:%04x:%04x:%04x\n", (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
		};

		if ((typeinfo & IPV6_ADDR_ANONYMIZED_IID) == IPV6_ADDR_ANONYMIZED_IID) {
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "Interface identifier is an anonymized one\n");
			};
		};

		if ((typeinfo & IPV6_NEW_ADDR_IID_EUI48) == IPV6_NEW_ADDR_IID_EUI48) {
			/* EUI-48 */
			if ((typeinfo & IPV6_ADDR_ANONYMIZED_IID) == IPV6_ADDR_ANONYMIZED_IID) {
				payload = ipv6addr_get_payload_anonymized_iid(ipv6addrp, typeinfo) ^ 0x020000;

				if ((payload & 0x1000000) != 0) {
					libieee_unmap_oui_macaddr(&macaddr, payload);
				} else {
					macaddr.addr[0] = ((payload >> 16) & 0xff) ^ 0x02;
					macaddr.addr[1] = (payload >> 8) & 0xff;
					macaddr.addr[2] = (payload & 0xff);
					macaddr.addr[3] = 0;
					macaddr.addr[4] = 0;
					macaddr.addr[5] = 0;
				};
			} else {
				macaddr.addr[0] = ipv6addr_getoctet(ipv6addrp,  8) ^ 0x02;
				macaddr.addr[1] = ipv6addr_getoctet(ipv6addrp,  9);
				macaddr.addr[2] = ipv6addr_getoctet(ipv6addrp, 10);
				macaddr.addr[3] = ipv6addr_getoctet(ipv6addrp, 13);
				macaddr.addr[4] = ipv6addr_getoctet(ipv6addrp, 14);
				macaddr.addr[5] = ipv6addr_getoctet(ipv6addrp, 15);
			};
			print_eui48(&macaddr, formatoptions);
		} else {
			if ((typeinfo & IPV6_NEW_ADDR_IID_EUI64) == IPV6_NEW_ADDR_IID_EUI64) {
				/* EUI-64 */
				if ((typeinfo & IPV6_ADDR_ANONYMIZED_IID) == IPV6_ADDR_ANONYMIZED_IID) {
					payload = ipv6addr_get_payload_anonymized_iid(ipv6addrp, typeinfo) ^ 0x020000;

					if ((payload & 0x1000000) != 0) {
						libieee_unmap_oui_eui64addr(&eui64addr, payload);
					} else  {
						eui64addr.addr[0] = ((payload >> 16) & 0xff) ^ 0x02;
						eui64addr.addr[1] = (payload >> 8) & 0xff;
						eui64addr.addr[2] = (payload & 0xff);
						eui64addr.addr[3] = 0;
						eui64addr.addr[4] = 0;
						eui64addr.addr[5] = 0;
						eui64addr.addr[6] = 0;
						eui64addr.addr[7] = 0;
					};
				} else {
					eui64addr.addr[0] = ipv6addr_getoctet(ipv6addrp,  8) ^ 0x02;
					eui64addr.addr[1] = ipv6addr_getoctet(ipv6addrp,  9);
					eui64addr.addr[2] = ipv6addr_getoctet(ipv6addrp, 10);
					eui64addr.addr[3] = ipv6addr_getoctet(ipv6addrp, 11);
					eui64addr.addr[4] = ipv6addr_getoctet(ipv6addrp, 12);
					eui64addr.addr[5] = ipv6addr_getoctet(ipv6addrp, 13);
					eui64addr.addr[6] = ipv6addr_getoctet(ipv6addrp, 14);
					eui64addr.addr[7] = ipv6addr_getoctet(ipv6addrp, 15);
				};
				print_eui64(&eui64addr, formatoptions);
			} else {
				if ( (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0 ) {
					if ( machinereadable != 0 ) {
						snprintf(tempstring, sizeof(tempstring) - 1, "EUI64=??:??:??:??:??:%02x:%02x:%02x", (unsigned int) ipv6addr_getoctet(ipv6addrp, 13), (unsigned int) ipv6addr_getoctet(ipv6addrp, 14), (unsigned int) ipv6addr_getoctet(ipv6addrp, 15));
						printout(tempstring);
					} else {
						fprintf(stdout, "Generated from the extension identifier of an EUI-48 (MAC): ...:%02x:%02x:%02x\n", (unsigned int) ipv6addr_getoctet(ipv6addrp, 13), (unsigned int) ipv6addr_getoctet(ipv6addrp, 14), (unsigned int) ipv6addr_getoctet(ipv6addrp, 15));
					};
				} else if ( (typeinfo & (IPV6_NEW_ADDR_IID_ISATAP | IPV6_NEW_ADDR_6TO4_MICROSOFT) ) != 0 )  {
					if ((typeinfo & IPV6_ADDR_IID_32_63_HAS_IPV4) != 0) {
						/* IPv4 address included */

						if ((typeinfo & IPV6_ADDR_ANONYMIZED_IID) != 0) {
							payload = ipv6addr_get_payload_anonymized_iid(ipv6addrp, typeinfo);
							/* IPv4 */
							ipv4addr_setoctet(&ipv4addr, 0, (payload >> 16) & 0xff);
							ipv4addr_setoctet(&ipv4addr, 1, (payload >>  8) & 0xff);
							ipv4addr_setoctet(&ipv4addr, 2, (payload      ) & 0xff);
							ipv4addr_setoctet(&ipv4addr, 3, 0);
						} else {
							for (i = 0; i <= 3; i++) {
								ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
							};
						};

						ipv4addr.scope = ipv4addr_gettype(&ipv4addr);

						if ( (typeinfo & IPV6_NEW_ADDR_IID_ISATAP) != 0 )  {
							print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "ISATAP");
						} else {
							print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "6to4-microsoft");
						};

					} else if ((ipv6addr_getoctet(ipv6addrp, 11) == 0xff) && (ipv6addr_getoctet(ipv6addrp, 12) == 0xfe)) {
						/* Vendor ID included */
						if ( machinereadable != 0 ) {
						} else {
							fprintf(stdout, "ISATAP vendor ID: 0x%02x%02x%02x\n", ipv6addr_getoctet(ipv6addrp, 13), ipv6addr_getoctet(ipv6addrp, 14), ipv6addr_getoctet(ipv6addrp, 15));
						};
					} else {
						/* Extension ID included */
						if ( machinereadable != 0 ) {
						} else {
							fprintf(stdout, "ISATAP extension ID: 0x%02x%02x%02x%02x%02x\n", ipv6addr_getoctet(ipv6addrp, 11), ipv6addr_getoctet(ipv6addrp, 12), ipv6addr_getoctet(ipv6addrp, 13), ipv6addr_getoctet(ipv6addrp, 14), ipv6addr_getoctet(ipv6addrp, 15));
						};
					};
				} else if ( ( ( (typeinfo & IPV6_ADDR_LINKLOCAL) != 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0 && ipv6addr_getword(ipv6addrp, 6) != 0)) )   {
					/* fe80:: must have 0000:0000:xxxx:yyyy where xxxx > 0 */
					if ( machinereadable != 0 ) {
					} else {
						fprintf(stdout, "Address type contains IPv4 address:\n");
					};

					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
					};
					ipv4addr.scope = ipv4addr_gettype(&ipv4addr);

					if ( machinereadable != 0 ) {
						// printout("IPV4_SOURCE=LINK-LOCAL-IID");
					};

					print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "LINK-LOCAL-IID");
				} else {
					if ( machinereadable != 0 ) {
						if ((typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
							printout("EUI64_SCOPE=local-6to4-microsoft");
						} else if ((typeinfo & IPV6_NEW_ADDR_IID_RANDOM) != 0) {
							printout("EUI64_SCOPE=local-random");
						} else {
							printout("EUI64_SCOPE=local");
						};
					} else {
						if ((typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
							fprintf(stdout, "Interface identifier contain only IPv4 address from 6to4 prefix, usually seen on Microsoft OS\n");
						} else if ((typeinfo & IPV6_NEW_ADDR_IID_RANDOM) != 0) {
							fprintf(stdout, "Interface identifier is probably generated by privacy extension\n");
						} else {
							fprintf(stdout, "Interface identifier is probably manual set\n");
						};
					};
				};
			};
		};
	};

	if ( (typeinfo & IPV6_NEW_ADDR_ORCHID) != 0) {
		/* 'extract' hash */
		ipv6addr_setword(ipv6addrp, 0, 0x0000);
		ipv6addr_setword(ipv6addrp, 1, ipv6addr_getword(ipv6addrp, 1) & 0x000F);

		retval = libipv6addr_to_hex(ipv6addrp, ipv6addrstring, 0);

		if ( machinereadable != 0 ) {
		} else {
			if ((typeinfo & IPV6_ADDR_ANONYMIZED_IID) != 0) {
				snprintf(tempstring, sizeof(tempstring) - 1, "n/a (previosly anonymized)");
			} else {
				snprintf(tempstring, sizeof(tempstring) - 1, "%s", ipv6addrstring+7);
			};
			fprintf(stdout, "ORCHID hash (100 bits): %s\n", tempstring);
		};
	};
END:

	if ( ((typeinfo & IPV6_NEW_ADDR_AGU) != 0) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID | IPV6_ADDR_ANONYMIZED_PREFIX)) == 0) ) {
#ifdef SUPPORT_IP2LOCATION
		/* IP2Location information */
		print_ip2location(ipv6addrstring, formatoptions, "", 6);
#endif

#ifdef SUPPORT_GEOIP
		/* GeoIP information */
		if (use_geoip_ipv6 != 0) {
			print_geoip(ipv6addrstring, formatoptions, "", 6);
		};
#endif
	};

	printfooter(formatoptions);
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * shows information about a given IPv4 address
 *
 * in : *ipv4addrp = pointer to IPv4 address
 * ret: ==0: ok, !=0: error
 */
int showinfo_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions) {
	int retval = 1;

	print_ipv4addr(ipv4addrp, formatoptions, "");

	printfooter(formatoptions);
	retval = 0;
	return (retval);
};


/*
 * shows information about a given EUI-48 identifier
 *
 * in : *macaddrp = pointer to MAC address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "showinfo_eui48"
int showinfo_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions) {
	int retval = 1;

	print_eui48(macaddrp, formatoptions);
	printfooter(formatoptions);

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * shows information about a given EUI-64 identifier
 *
 * in : *eui64addrp = pointer to EUI-64 address
 * ret: ==0: ok, !=0: error
 */
int showinfo_eui64(const ipv6calc_eui64addr *eui64addrp, const uint32_t formatoptions) {
	int retval = 1;

	print_eui64(eui64addrp, formatoptions);
	printfooter(formatoptions);

	retval = 0;
	return (retval);
};
