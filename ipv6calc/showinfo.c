/*
 * Project    : ipv6calc
 * File       : showinfo.c
 * Version    : $Id$
 * Copyright  : 2001-2017 by Peter Bieringer <pb (at) bieringer.de>
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
#include "../databases/lib/libipv6calc_db_wrapper_DBIP.h"
#include "../databases/lib/libipv6calc_db_wrapper_External.h"
#include "../databases/lib/libipv6calc_db_wrapper_BuiltIn.h"

#ifdef SUPPORT_IP2LOCATION
#include "IP2Location.h"

/* 
 * API_VERSION is defined as a bareword in IP2Location.h, 
 * we need this trick to stringify it. Blah.
 */
#define makestr(x) #x
#define xmakestr(x) makestr(x)
#endif

#ifdef SUPPORT_GEOIP
#include "GeoIP.h"
#include "GeoIPCity.h"
#endif

/* from anonymizer */
extern s_ipv6calc_anon_set ipv6calc_anon_set;

/*
 * show available types on machine readable format
 */
void showinfo_availabletypes(void) {
	int i;

	fprintf(stderr, "\nAvailable tokens for machine-readable output (printed in one line):\n");
	fprintf(stderr, " IPV6_TYPE=...                 : type of IPv6 address (commata separated)\n");
	fprintf(stderr, " ");
	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++ ) {
		fprintf(stderr, " %s", ipv6calc_ipv6addrtypestrings[i].token);
	};
	fprintf(stderr, "\n");
	fprintf(stderr, " IPV6=...                      : given IPv6 address full uncompressed\n");
	fprintf(stderr, " IPV6_ANON=...                 : given anonymized IPv6 address full uncompressed\n");
	fprintf(stderr, " IPV6_REGISTRY=...             : registry token of given IPv6 address\n");
	fprintf(stderr, " IPV6_PREFIXLENGTH=ddd         : given prefix length\n");
	fprintf(stderr, " IPV6_AS_NUM=...               : AS number of (anonymized) IPv6 address\n");
	fprintf(stderr, " IPV6_COUNTRYCODE=...          : Country Code of (anonymized) IPv6 address\n");
	fprintf(stderr, " IPV6_COUNTRYCODE_SOURCE=...   : Source of Country Code of IPv6 address\n");
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
	fprintf(stderr, " IPV4_COUNTRYCODE_SOURCE[...]=...: Source of Country Code of (anonymized) IPv4 address\n");
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

	fprintf(stderr, " AS_NUM=...                    : Autonomous System Number\n");
	fprintf(stderr, " AS_NUM_REGISTRY=...           : Registry of AS number\n");
#ifdef SUPPORT_IP2LOCATION
	fprintf(stderr, " IP2LOCATION_COUNTRY_SHORT=...     : Country code of IP address\n");
	fprintf(stderr, " IP2LOCATION_COUNTRY_LONG=...      : Country of IP address\n");
	fprintf(stderr, " IP2LOCATION_REGION=...            : Region of IP address\n");
	fprintf(stderr, " IP2LOCATION_CITY=...              : City of IP address\n");
	fprintf(stderr, " IP2LOCATION_ISP=...               : ISP of IP address\n");
	fprintf(stderr, " IP2LOCATION_LATITUDE=...          : Latitude of IP address\n");
	fprintf(stderr, " IP2LOCATION_LONGITUDE=...         : Longitude of IP address\n");
	fprintf(stderr, " IP2LOCATION_DOMAIN=...            : Domain of IP address\n");
	fprintf(stderr, " IP2LOCATION_ZIPCODE=...           : ZIP code of IP address\n");
	fprintf(stderr, " IP2LOCATION_AREACODE=...          : Area code of IP address\n");
	fprintf(stderr, " IP2LOCATION_TIMEZONE=...          : Time zone of IP address\n");
	fprintf(stderr, " IP2LOCATION_NETSPEED=...          : Net speed of IP address\n");
	fprintf(stderr, " IP2LOCATION_IDDCODE=...           : Internation Direct Dialing of IP address\n");
	fprintf(stderr, " IP2LOCATION_WEATHERSTATIONCODE=...: Weather Station Code of IP address\n");
	fprintf(stderr, " IP2LOCATION_WEATHERSTATIONNAME=...: Weather Station Name of IP address\n");
	fprintf(stderr, " IP2LOCATION_USAGETYPE=...         : Usage type of IP address\n");
	fprintf(stderr, " IP2LOCATION_ELEVATION=...         : Elevation of city of IP address\n");
	fprintf(stderr, " IP2LOCATION_MOBILEBRAND=...       : Mobile Brand of IP address\n");
	fprintf(stderr, " IP2LOCATION_MNC=...               : Mobile Network Code of IP address\n");
	fprintf(stderr, " IP2LOCATION_MCC=...               : Mobile Country Code of IP address\n");
	fprintf(stderr, " IP2LOCATION_DATABASE_INFO=...     : Information about the used databases\n");
#endif
#ifdef SUPPORT_GEOIP
	fprintf(stderr, " GEOIP_COUNTRY_SHORT=...       : Country code of IP address\n");
	fprintf(stderr, " GEOIP_COUNTRY_LONG=...        : Country of IP address\n");
	fprintf(stderr, " GEOIP_REGION=...              : Region of IP address\n");
	fprintf(stderr, " GEOIP_CITY=...                : City of IP address\n");
	fprintf(stderr, " GEOIP_ZIPCODE=...             : Zip code of IP address\n");
	fprintf(stderr, " GEOIP_LATITUDE=...            : Latitude of IP address\n");
	fprintf(stderr, " GEOIP_LONGITUDE=...           : Longitude of IP address\n");
	fprintf(stderr, " GEOIP_AREACODE=...            : Area code of IP address\n");
	fprintf(stderr, " GEOIP_DMACODE=...             : DMA/Metro code of IP address\n");
	fprintf(stderr, " GEOIP_AS_TEXT=...             : Autonomous System information\n");
	fprintf(stderr, " GEOIP_DATABASE_INFO=...       : Information about the used databases\n");
#endif
#ifdef SUPPORT_DBIP
	fprintf(stderr, " DBIP_COUNTRY_SHORT=.. .       : Country code of IP address\n");
	fprintf(stderr, " DBIP_CITY=...                 : City of IP address\n");
	fprintf(stderr, " DBIP_REGION=...               : Region of IP address\n");
	fprintf(stderr, " DBIP_DATABASE_INFO=.. .       : Information about the used databases\n");
#endif
#ifdef SUPPORT_EXTERNAL
	fprintf(stderr, " EXTERNAL_COUNTRY_SHORT=.. .   : Country code of IP address\n");
	fprintf(stderr, " EXTERNAL_DATABASE_INFO=.. .   : Information about the used databases\n");
#endif
#ifdef SUPPORT_BUILTIN
	fprintf(stderr, " BUILTIN_DATABASE_INFO=.. .    : Information about the used databases\n");
#endif
	fprintf(stderr, " IPV6CALC_NAME=name            : Name of ipv6calc\n");
	fprintf(stderr, " IPV6CALC_VERSION=x.y.z        : Version of ipv6calc\n");
	fprintf(stderr, " IPV6CALC_COPYRIGHT=\"...\"      : Copyright string\n");
	fprintf(stderr, " IPV6CALC_OUTPUT_VERSION=x     : Version of output format\n");
	fprintf(stderr, " IPV6CALC_FEATURES=\"...\"       : Feature string of ipv6calc -v\n");
	fprintf(stderr, " IPV6CALC_CAPABILITIES=\"...\"   : Capability string of ipv6calc -v -v\n");
	fprintf(stderr, " IPV6CALC_SETTINGS_ANON=\"...\"  : Anonymizer settings\n");

	fprintf(stderr, "\n");
	fprintf(stderr, "Notes: quoting of values can be controlled via\n");
	fprintf(stderr, "          --mrqva (always) --mrqvn (never)\n");
	fprintf(stderr, "       specific token can be selected using --mrst <TOKEN>\n");
	fprintf(stderr, "         even in case of no/empty output: --mrstpa <TOKEN>\n");
	fprintf(stderr, "       matching tokens can be selected using --mrmt <TOKEN>\n");
	fprintf(stderr, "       value of a specific token can be displayed using --mrtvo <TOKEN>\n");
};

/*
 * print one information
 */
static void printout(const char *token, const char *value, const uint32_t formatoptions) {
	int quote = 0;

	if (formatoptions & FORMATOPTION_mr_select_token) {
		// skip not matching token (equal)
		if (strcmp(showinfo_machine_readable_filter, token) != 0) return;

		showinfo_machine_readable_filter_used = 1;
	};

	if (formatoptions & FORMATOPTION_mr_match_token) {
		// skip not matching token (begin)
		if (strncmp(showinfo_machine_readable_filter, token, strlen(showinfo_machine_readable_filter)) != 0) return;
	};

	/* automatic quoting disabled
	if (strstr(value, " ") != NULL) {
		quote = 1;
	};
	*/

	if ((formatoptions & FORMATOPTION_mr_quote_default) != 0) {
		quote = 1;
	};

	if ((formatoptions & FORMATOPTION_mr_quote_always) != 0) {
		quote = 1;
	};

	if ((formatoptions & FORMATOPTION_mr_quote_never) != 0) {
		quote = 0;
	};

	fprintf(stdout, "%s%s%s%s%s\n"
		, (formatoptions & FORMATOPTION_mr_value_only) ? "" : token
		, (formatoptions & FORMATOPTION_mr_value_only) ? "" : "="
		, (quote == 1) ? "\"" : ""
		, value
		, (quote == 1) ? "\"" : ""
	);
};

static void printout2(const char *token, const char *additional, const char *value, const uint32_t formatoptions) {
	int quote = 0;

	if (formatoptions & FORMATOPTION_mr_select_token) {
		// skip in case additional is not empty
		if ((additional != NULL) && (strlen(additional) > 0)) return;

		// skip not matching token
		if (strcmp(showinfo_machine_readable_filter, token) != 0) return;

		showinfo_machine_readable_filter_used = 1;
	};

	if (formatoptions & FORMATOPTION_mr_match_token) {
		// skip in case additional is not empty
		if ((additional != NULL) && (strlen(additional) > 0)) return;

		// skip not matching token (begin)
		if (strncmp(showinfo_machine_readable_filter, token, strlen(showinfo_machine_readable_filter)) != 0) return;
	};

	/* automatic quoting disabled
	if (strstr(value, " ") != NULL) {
		quote = 1;
	};
	*/

	if ((formatoptions & FORMATOPTION_mr_quote_default) != 0) {
		quote = 1;
	};

	if (formatoptions & FORMATOPTION_mr_quote_always) {
		quote = 1;
	};

	if (formatoptions & FORMATOPTION_mr_quote_never) {
		quote = 0;
	};

	fprintf(stdout, "%s%s%s%s%s%s\n"
		, (formatoptions & FORMATOPTION_mr_value_only) ? "" : token
		, additional
		, (formatoptions & FORMATOPTION_mr_value_only) ? "" : "="
		, (quote == 1) ? "\"" : ""
		, value
		, (quote == 1) ? "\"" : ""
	);
};

static void printfooter(const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST] = "";
	char tempstring2[NI_MAXHOST] = "";

	if ((formatoptions & FORMATOPTION_mr_select_token_pa) && (showinfo_machine_readable_filter_used == 0)) {
		printout(showinfo_machine_readable_filter, "", formatoptions);
	};

#if defined SUPPORT_IP2LOCATION || defined SUPPORT_GEOIP || defined SUPPORT_DBIP || defined SUPPORT_EXTERNAL || defined SUPPORT_BUILTIN
	char *string;
#endif

#ifdef SUPPORT_IP2LOCATION
	string = libipv6calc_db_wrapper_IP2Location_wrapper_db_info_used();
	if ((string != NULL) && (strlen(string) > 0)) {
		if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
			printout("IP2LOCATION_DATABASE_INFO", string, formatoptions);
		} else {
			fprintf(stdout, "IP2Location database: %s\n", string);
		};
	};
#endif

#ifdef SUPPORT_GEOIP
	string = libipv6calc_db_wrapper_GeoIP_wrapper_db_info_used();
	if ((string != NULL) && (strlen(string) > 0)) {
		if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
			printout("GEOIP_DATABASE_INFO", string, formatoptions);
		} else {
			fprintf(stdout, "GeoIP database: %s\n", string);
		};
	};
#endif

#ifdef SUPPORT_DBIP
	string = libipv6calc_db_wrapper_DBIP_wrapper_db_info_used();
	if ((string != NULL) && (strlen(string) > 0)) {
		if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
			printout("DBIP_DATABASE_INFO", string, formatoptions);
		} else {
			fprintf(stdout, "DB-IP.com database: %s\n", string);
		};
	};
#endif

#ifdef SUPPORT_EXTERNAL
	string = libipv6calc_db_wrapper_External_wrapper_db_info_used();
	if ((string != NULL) && (strlen(string) > 0)) {
		if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
			printout("EXTERNAL_DATABASE_INFO", string, formatoptions);
		} else {
			fprintf(stdout, "External database: %s\n", string);
		};
	};
#endif

#ifdef SUPPORT_BUILTIN
	string = libipv6calc_db_wrapper_BuiltIn_wrapper_db_info_used();
	if ((string != NULL) && (strlen(string) > 0)) {
		if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
			printout("BUILTIN_DATABASE_INFO", string, formatoptions);
		} else {
			fprintf(stdout, "Built-In database: %s\n", string);
		};
	};
#endif

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		printout("IPV6CALC_NAME"     , PROGRAM_NAME, formatoptions);
		printout("IPV6CALC_VERSION"  , PACKAGE_VERSION, formatoptions);
		printout("IPV6CALC_COPYRIGHT", PROGRAM_COPYRIGHT, formatoptions | FORMATOPTION_mr_quote_default);

		snprintf(tempstring, sizeof(tempstring), "%d", IPV6CALC_OUTPUT_VERSION);
		printout("IPV6CALC_OUTPUT_VERSION", tempstring, formatoptions);

		libipv6calc_anon_infostring(tempstring2, sizeof(tempstring2), &ipv6calc_anon_set);
		printout("IPV6CALC_SETTINGS_ANON", tempstring2, formatoptions | FORMATOPTION_mr_quote_default);

		/* features */
		tempstring[0] = '\0'; /* clear tempstring */

		libipv6calc_db_wrapper_features(tempstring, sizeof(tempstring));

		if (feature_zeroize == 1) {
			snprintf(tempstring2, sizeof(tempstring2), "%s%sANON_ZEROISE", tempstring, strlen(tempstring) > 0 ? " " : "");
			snprintf(tempstring, sizeof(tempstring), "%s", tempstring2);
		};

		if (feature_anon == 1) {
			snprintf(tempstring2, sizeof(tempstring2), "%s%sANON_ANONYMIZE", tempstring, strlen(tempstring) > 0 ? " " : "");
			snprintf(tempstring, sizeof(tempstring), "%s", tempstring2);
		};

		if (feature_kp == 1) {
			snprintf(tempstring2, sizeof(tempstring2), "%s%sANON_KEEP-TYPE-ASN-CC", tempstring, strlen(tempstring) > 0 ? " " : "");
			snprintf(tempstring, sizeof(tempstring), "%s", tempstring2);
		};

		printout("IPV6CALC_FEATURES", tempstring, formatoptions | FORMATOPTION_mr_quote_default);

		/* capabilities */
		tempstring[0] = '\0'; /* clear tempstring */

		libipv6calc_db_wrapper_capabilities(tempstring, sizeof(tempstring));

		printout("IPV6CALC_CAPABILITIES", tempstring, formatoptions | FORMATOPTION_mr_quote_default);
	};
};


#ifdef SUPPORT_IP2LOCATION
/* print IP2Location information */
static void print_ip2location(char *addrstring, const uint32_t formatoptions, const char *additionalstring, int version) {
	DEBUGPRINT_NA(DEBUG_showinfo, "Called");

#define TEST_IP2LOCATION_AVAILABLE(v)	((v != NULL) && (strstr(v, "unavailable") == NULL) && (strstr(v, "demo database") == NULL) && (strstr(v, "This is demo") == NULL) && (strstr(v, "INVALID") == NULL) && (strstr(v, "-") == NULL))

#define HUMAN_READABLE_IP2LOCATION(name, value) \
				if (strlen(additionalstring) > 0) { \
					fprintf(stdout, "IP2Location reports for %s %s: %s\n", additionalstring, name, value); \
				} else { \
					fprintf(stdout, "IP2Location reports %s: %s\n", name, value); \
				};

	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_IP2LOCATION] == 0) {
		DEBUGPRINT_NA(DEBUG_showinfo, "IP2Location support not active");
		return;
	};

	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable);
	char tempstring[NI_MAXHOST];
	char *CountryCode = NULL, *CountryName = NULL;

	DEBUGPRINT_WA(DEBUG_showinfo, "Called addrstring=%s formatoptions=0x%08x additionalstring=%s version=%d", addrstring, formatoptions, additionalstring, version);

	CountryCode = libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr(addrstring, version);
	CountryName = libipv6calc_db_wrapper_IP2Location_wrapper_country_name_by_addr(addrstring, version);

	if (machinereadable != 0) {
		if (CountryCode != NULL) {
			printout2("IP2LOCATION_COUNTRY_SHORT", additionalstring, CountryCode, formatoptions);
		};

		if (CountryName != NULL) {
			printout2("IP2LOCATION_COUNTRY_LONG", additionalstring, CountryName, formatoptions);
		};
	} else {
		if (strlen(additionalstring) > 0) {
			fprintf(stdout, "IP2Location reports for %s country %s (%s)\n", additionalstring, (CountryName != NULL ? CountryName : "UNKNOWN"), (CountryCode != NULL ? CountryCode : "??"));
		} else {
			fprintf(stdout, "IP2Location country name and code: %s (%s)\n", (CountryName != NULL ? CountryName : "UNKNOWN"), (CountryCode != NULL ? CountryCode : "??"));
		};
	};

	IP2LocationRecord *record = libipv6calc_db_wrapper_IP2Location_wrapper_record_city_by_addr((char*) addrstring, version);

	if (record != NULL) {

		if (TEST_IP2LOCATION_AVAILABLE(record->region)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_REGION", additionalstring, record->region, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Region", record->region)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->city)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_CITY", additionalstring, record->city, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("City", record->city)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->isp)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_ISP", additionalstring, record->isp, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("ISP", record->isp)
			};
		};

		if (record->latitude != 0) {
			snprintf(tempstring, sizeof(tempstring), "%f", record->latitude);
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_LATITUDE", additionalstring, tempstring, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Latitude", tempstring)
			};
		};

		if (record->longitude != 0) {
			snprintf(tempstring, sizeof(tempstring), "%f", record->longitude);
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_LONGITUDE", additionalstring, tempstring, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Longitude", tempstring)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->domain)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_DOMAIN", additionalstring, record->domain, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Domain", record->domain)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->zipcode)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_ZIPCODE", additionalstring, record->zipcode, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("ZIP Code", record->zipcode)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->areacode)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_AREACODE", additionalstring, record->areacode, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("AREA Code", record->areacode)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->timezone)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_TIMEZONE", additionalstring, record->timezone, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Time Zone", record->timezone)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->netspeed)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_NETSPEED", additionalstring, record->netspeed, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Network Speed", record->netspeed)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->iddcode)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_IDDCODE", additionalstring, record->iddcode, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("IDD Code", record->iddcode)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->weatherstationcode)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_WEATHERSTATIONCODE", additionalstring, record->weatherstationcode, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Weather Station Code", record->weatherstationcode)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->weatherstationname)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_WEATHERSTATIONNAME", additionalstring, record->weatherstationname, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Weather Station Name", record->weatherstationname)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->mnc)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_MNC", additionalstring, record->mnc, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Mobile Network Code", record->mnc)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->mcc)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_MCC", additionalstring, record->mcc, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Mobile Country Code", record->mcc)
			};
		};

		if (TEST_IP2LOCATION_AVAILABLE(record->mobilebrand)) {
			if ( machinereadable != 0 ) {
				printout2("IP2LOCATION_MOBILEBRAND", additionalstring, record->mobilebrand, formatoptions);
			} else {
				HUMAN_READABLE_IP2LOCATION("Mobile Brand", record->mobilebrand)
			};
		};

#ifndef SUPPORT_IP2LOCATION_ALL_COMPAT
		if (libipv6calc_db_wrapper_IP2Location_library_version_major() > 4) {
			if (TEST_IP2LOCATION_AVAILABLE(record->usagetype)) {
				if ( machinereadable != 0 ) {
					printout2("IP2LOCATION_USAGETYPE", additionalstring, record->usagetype, formatoptions);
				} else {
					// get description
					const char *desc = libipv6calc_db_wrapper_IP2Location_UsageType_description(record->usagetype);
					if (desc != NULL) {
						snprintf(tempstring, sizeof(tempstring), "%s (%s)", record->usagetype, desc);
					} else {
						snprintf(tempstring, sizeof(tempstring), "%s", record->usagetype);
					};
					HUMAN_READABLE_IP2LOCATION("Usage Type", tempstring)
				};
			};

			if (record->elevation != 0) {
				snprintf(tempstring, sizeof(tempstring), "%f", record->elevation);
				if ( machinereadable != 0 ) {
					printout2("IP2LOCATION_ELEVATION", additionalstring, tempstring, formatoptions);
				} else {
					HUMAN_READABLE_IP2LOCATION("Elevation", tempstring)
				};
			};
		};
#endif // SUPPORT_IP2LOCATION_ALL_COMPAT

		libipv6calc_db_wrapper_IP2Location_free_record(record);
	} else {
		DEBUGPRINT_WA(DEBUG_showinfo, "IP2Location returned no record for address: %s", addrstring);
	};
};
#endif

#ifdef SUPPORT_GEOIP
/* print GeoIP information */
static void print_geoip(const char *addrstring, const uint32_t formatoptions, const char *additionalstring, int version) {
	DEBUGPRINT_NA(DEBUG_showinfo, "Called");

#define HUMAN_READABLE_GEOIP(name, value) \
				if (strlen(additionalstring) > 0) { \
					fprintf(stdout, "GeoIP reports for %s %s: %s\n", additionalstring, name, value); \
				} else { \
					fprintf(stdout, "GeoIP reports %s: %s\n", name, value); \
				};

	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_GEOIP] == 0) {
		DEBUGPRINT_NA(DEBUG_showinfo, "GeoIP support not active");
		return;
	};

	const char *returnedCountry = NULL;
	const char *returnedCountryName = NULL;
	char *as_text;
	GeoIPRecord *gir = NULL;

	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable);
	char tempstring[NI_MAXHOST] = "";

	as_text = libipv6calc_db_wrapper_GeoIP_wrapper_asnum_by_addr(addrstring, version);
	if (as_text != NULL) {
		if ( machinereadable != 0 ) {
			printout2("GEOIP_AS_TEXT", additionalstring, as_text, formatoptions);
		};
	};

	returnedCountry     = libipv6calc_db_wrapper_GeoIP_wrapper_country_code_by_addr(addrstring, version);
	returnedCountryName = libipv6calc_db_wrapper_GeoIP_wrapper_country_name_by_addr(addrstring, version);
	if (returnedCountry != NULL) {
		DEBUGPRINT_WA(DEBUG_showinfo, "GeoIP IPv%d country database result", version);

		if ( machinereadable != 0 ) {
			printout2("GEOIP_COUNTRY_SHORT", additionalstring, returnedCountry, formatoptions);

			if (returnedCountryName != NULL) {
				printout2("GEOIP_COUNTRY_LONG", additionalstring, returnedCountryName, formatoptions);
			} else {
				DEBUGPRINT_NA(DEBUG_showinfo, "returnedCountryName=NULL");
			};
		} else {
			if (returnedCountryName != NULL) {
				if (strlen(additionalstring) > 0) {
					fprintf(stdout, "GeoIP country name and code for %s: %s (%s)\n", additionalstring, returnedCountryName, returnedCountry);
				} else {
					fprintf(stdout, "GeoIP country name and code: %s (%s)\n", returnedCountryName, returnedCountry);
				};
			} else {
				if (strlen(additionalstring) > 0) {
					fprintf(stdout, "GeoIP country code for %s: %s\n", additionalstring, returnedCountry);
				} else {
					fprintf(stdout, "GeoIP country code: %s\n", returnedCountry);
				};
			};
		};
	};

	gir = libipv6calc_db_wrapper_GeoIP_wrapper_record_city_by_addr(addrstring, version);
	if (gir != NULL) {
		DEBUGPRINT_WA(DEBUG_showinfo, "GeoIP IPv%d city database result", version);

		if (gir->region != NULL) {
			if ( machinereadable != 0 ) {
				printout2("GEOIP_REGION", additionalstring, gir->region, formatoptions);
			} else {
				HUMAN_READABLE_GEOIP("Region", gir->region)
			};
		};

		if (gir->city != NULL) {
			if ( machinereadable != 0 ) {
				printout2("GEOIP_CITY", additionalstring, gir->city, formatoptions);
			} else {
				HUMAN_READABLE_GEOIP("City", gir->city)
			};
		};

		if (gir->postal_code != NULL) {
			if ( machinereadable != 0 ) {
				printout2("GEOIP_ZIPCODE", additionalstring, gir->postal_code, formatoptions);
			} else {
				HUMAN_READABLE_GEOIP("ZIP Code", gir->postal_code)
			};
		};

		if (gir->latitude != 0) {
			snprintf(tempstring, sizeof(tempstring), "%f", gir->latitude);
			if ( machinereadable != 0 ) {
				printout2("GEOIP_LATITUDE", additionalstring, tempstring, formatoptions);
			} else {
				HUMAN_READABLE_GEOIP("Latitude", tempstring)
			};
		};

		if (gir->longitude != 0) {
			snprintf(tempstring, sizeof(tempstring), "%f", gir->longitude);
			if ( machinereadable != 0 ) {
				printout2("GEOIP_LONGITUDE", additionalstring, tempstring, formatoptions);
			} else {
				HUMAN_READABLE_GEOIP("Longitude", tempstring)
			};
		};

		if (gir->continent_code != NULL) {
			if ( machinereadable != 0 ) {
				printout2("GEOIP_CONTINENTCODE", additionalstring, gir->continent_code, formatoptions);
			} else {
				HUMAN_READABLE_GEOIP("Continent Code", gir->continent_code)
			};
		};

		if (gir->dma_code != 0) {
			snprintf(tempstring, sizeof(tempstring), "%d", gir->dma_code);
			if ( machinereadable != 0 ) {
				printout2("GEOIP_DMACODE", additionalstring, tempstring, formatoptions);
			} else {
				HUMAN_READABLE_GEOIP("DMA Code", tempstring)
			};
		};

		if (gir->area_code != 0) {
			snprintf(tempstring, sizeof(tempstring), "%d", gir->area_code);
			if ( machinereadable != 0 ) {
				printout2("GEOIP_AREACODE", additionalstring, tempstring, formatoptions);
			} else {
				HUMAN_READABLE_GEOIP("AREA Code", tempstring)
			};
		};

		libipv6calc_db_wrapper_GeoIPRecord_delete(gir);
	};
};
#endif

#ifdef SUPPORT_DBIP
/* print DBIP information */
static void print_dbip(const ipv6calc_ipaddr *ipaddrp, const uint32_t formatoptions, const char *additionalstring) {
	DEBUGPRINT_NA(DEBUG_showinfo, "Called");

#define HUMAN_READABLE_DBIP(name, value) \
				if (strlen(additionalstring) > 0) { \
					fprintf(stdout, "DB-IP.com reports for %s %s: %s\n", additionalstring, name, value); \
				} else { \
					fprintf(stdout, "DB-IP.com reports %s: %s\n", name, value); \
				};

	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_DBIP] == 0) {
		DEBUGPRINT_NA(DEBUG_showinfo, "DBIP support not active");
		return;
	};

	int ret;

	char returnedCountry[256] = "";
	char returnedCity[256] = "";
	char returnedRegion[256] = "";

	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable);

	ret = libipv6calc_db_wrapper_DBIP_wrapper_country_code_by_addr(ipaddrp, returnedCountry, sizeof(returnedCountry));
	if ((ret == 0) && (strlen(returnedCountry) > 0)) {
		DEBUGPRINT_WA(DEBUG_showinfo, "DBIP IPv%d country database result", ipaddrp->proto);

		if ( machinereadable != 0 ) {
			printout2("DBIP_COUNTRY_SHORT", additionalstring, returnedCountry, formatoptions);
		} else {
			HUMAN_READABLE_DBIP("Country Code", returnedCountry)
		};
	};

	ret = libipv6calc_db_wrapper_DBIP_wrapper_city_by_addr(ipaddrp, returnedCity, sizeof(returnedCity), returnedRegion, sizeof(returnedRegion));
	if ((ret == 0) && (strlen(returnedCity) > 0)) {
		DEBUGPRINT_WA(DEBUG_showinfo, "DBIP IPv%d city database result", ipaddrp->proto);

		if ( machinereadable != 0 ) {
			printout2("DBIP_CITY", additionalstring, returnedCity, formatoptions);
		} else {
			HUMAN_READABLE_DBIP("City", returnedCity)
		};
	};

	if ((ret == 0) && (strlen(returnedRegion) > 0)) {
		DEBUGPRINT_WA(DEBUG_showinfo, "DBIP IPv%d region database result", ipaddrp->proto);

		if ( machinereadable != 0 ) {
			printout2("DBIP_REGION", additionalstring, returnedRegion, formatoptions);
		} else {
			HUMAN_READABLE_DBIP("Region", returnedRegion)
		};
	};
};
#endif

#ifdef SUPPORT_EXTERNAL
/* print External DB information */
static void print_external(const ipv6calc_ipaddr *ipaddrp, const uint32_t formatoptions, const char *additionalstring) {
	DEBUGPRINT_NA(DEBUG_showinfo, "Called");

	if (wrapper_features_by_source[IPV6CALC_DB_SOURCE_EXTERNAL] == 0) {
		DEBUGPRINT_NA(DEBUG_showinfo, "External DB support not active");
		return;
	};

	int ret;

	char returnedCountry[256] = "";

	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable);

	ret = libipv6calc_db_wrapper_External_country_code_by_addr(ipaddrp, returnedCountry, sizeof(returnedCountry));
	if ((ret == 0) && (strlen(returnedCountry) > 0)) {
		DEBUGPRINT_WA(DEBUG_showinfo, "External IPv%d country database result", ipaddrp->proto);

		if ( machinereadable != 0 ) {
			printout2("EXTERNAL_COUNTRY_SHORT", additionalstring, returnedCountry, formatoptions);
		} else {
			if (strlen(additionalstring) > 0) {
				fprintf(stdout, "External DB country code for %s: %s\n", additionalstring, returnedCountry);
			} else {
				fprintf(stdout, "External DB country code: %s\n", returnedCountry);
			};
		};
	};
};
#endif

/* print IPv4 address */
static void print_ipv4addr(const ipv6calc_ipv4addr *ipv4addrp, const uint32_t formatoptions, const char *string) {
	char tempstring[NI_MAXHOST] = "", tempstring2[NI_MAXHOST] = "", helpstring[NI_MAXHOST] = "";
	char tempipv4string[NI_MAXHOST] = "";
	char embeddedipv4string[NI_MAXHOST] = "";
	uint32_t machinereadable = (formatoptions & FORMATOPTION_machinereadable), as_num32 = ASNUM_AS_UNKNOWN;
	int retval, i, j, retval_anon = 1;
	ipv6calc_ipv4addr ipv4addr_anon, *ipv4addr_anon_ptr;
	uint16_t cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	char *as_text = NULL;
	int registry;
	ipv6calc_ipaddr ipaddr;

	unsigned int data_source = IPV6CALC_DB_SOURCE_UNKNOWN;

	ipv4addr_anon_ptr = &ipv4addr_anon;

	if (ipv4addrp->flag_typeinfo == 0) {
		fprintf(stderr, "FATAL error, typeinfo not valid - FIX CODE of caller\n");
		exit(2);
	};

	retval = libipv4addr_ipv4addrstruct_to_string(ipv4addrp, tempipv4string, sizeof(tempipv4string), 0);
	if ( retval != 0 ) {
		fprintf(stderr, "Error converting IPv4 address: %s\n", tempipv4string);
		return;
	};

	CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr);

	if ((formatoptions & FORMATOPTION_printembedded) != 0) {
		snprintf(embeddedipv4string, sizeof(embeddedipv4string), "[%s]", tempipv4string);
	};

	ipv4addr_copy(ipv4addr_anon_ptr, ipv4addrp); /* copy structure */

	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {
		retval_anon = libipv4addr_anonymize(ipv4addr_anon_ptr, ipv6calc_anon_set.mask_ipv4, ipv6calc_anon_set.method);

		if (retval_anon == 0) {
			retval = libipv4addr_ipv4addrstruct_to_string(ipv4addr_anon_ptr, tempstring2, sizeof(tempstring2), 0);
			if ( retval != 0 ) {
				fprintf(stderr, "Error uncompressing IPv4 address: %s\n", tempstring2);
				retval = 1;
				retval_anon = 2;
			};
		};
	};
	
	if (machinereadable != 0) {
		/* given source string */
		if ((string != NULL) && (strlen(string) > 0)) {
			printout2("IPV4_SOURCE", embeddedipv4string, string, formatoptions);
		};

		/* address */
		printout2("IPV4", embeddedipv4string, tempipv4string, formatoptions);

		if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {
			if (retval_anon == 0 ) {	
				/* anonymized address */
				printout2("IPV4_ANON", embeddedipv4string, tempstring2, formatoptions);
			} else {
				// TODO: only show this on verbose (must be implemented)
				// snprintf(tempstring, sizeof(tempstring), "IPV4_ANON%s=(unsupported, too less DB features available)", embeddedipv4string);
				// printout(tempstring);
			};
		};

		if (ipv4addrp->flag_prefixuse == 1) {	
			snprintf(tempstring, sizeof(tempstring), "%d", (int) ipv4addrp->prefixlength);
			printout2("IPV4_PREFIXLENGTH", embeddedipv4string, tempstring, formatoptions);
		};

		j = 0;

		snprintf(tempstring, sizeof(tempstring), "%s", "");
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv4addrtypestrings); i++ ) {
			if ((ipv4addrp->typeinfo & ipv6calc_ipv4addrtypestrings[i].number) != 0) {
				if (j != 0) {
					snprintf(helpstring, sizeof(helpstring), "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				};
				snprintf(helpstring, sizeof(helpstring), "%s%s", tempstring, ipv6calc_ipv4addrtypestrings[i].token);
				snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				j = 1;
			};
		};
		printout2("IPV4_TYPE", embeddedipv4string, tempstring, formatoptions);
	} else {

		if ((string != NULL) && (strlen(string) > 0)) {
			fprintf(stdout, "IPv4 address: %s (%s)\n", tempipv4string, string);
		} else {
			fprintf(stdout, "IPv4 address: %s\n", tempipv4string);
		};

		fprintf(stdout, "IPv4 address type: ");
		j = 0;
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv4addrtypestrings); i++ ) {
			if ((ipv4addrp->typeinfo & ipv6calc_ipv4addrtypestrings[i].number) != 0) {
				if ( j != 0 ) { fprintf(stdout, ", "); };
				fprintf(stdout, "%s", ipv6calc_ipv4addrtypestrings[i].token);
				j = 1;
			};
		};
		fprintf(stdout, "\n");
	};	

	if ((ipv4addrp->typeinfo & (IPV4_ADDR_GLOBAL)) != 0) {
		/* get AS Information */
		as_num32 = libipv4addr_as_num32_by_addr(ipv4addrp);
		if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {
			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_AS) == 1) {
				as_text = libipv6calc_db_wrapper_as_text_by_addr(&ipaddr);
			};
		};

		if (as_num32 != ASNUM_AS_UNKNOWN) {
			if ( machinereadable != 0 ) {
				snprintf(tempstring, sizeof(tempstring), "%d", as_num32);
				printout2("IPV4_AS_NUM", embeddedipv4string, tempstring, formatoptions);

				if (as_text != NULL) {
					printout2("IPV4_AS_TEXT", embeddedipv4string, as_text, formatoptions);
				};
			} else {
				if (as_text != NULL) {
					if (strlen(embeddedipv4string) > 0) {
						fprintf(stdout, "Autonomous System Information for %s: %s\n", embeddedipv4string, as_text);
					} else {
						fprintf(stdout, "Autonomous System Information: %s\n", as_text);
					};
				} else {
					if (strlen(embeddedipv4string) > 0) {
						fprintf(stdout, "Autonomous System Number (32-bit) for %s: %d\n", embeddedipv4string, as_num32);
					} else {
						fprintf(stdout, "Autonomous System Number (32-bit): %d\n", as_num32);
					};
				};
			};
		} else {
			DEBUGPRINT_NA(DEBUG_showinfo, "Skip AS print: as_num32=ASNUM_AS_UNKNOWN");
		};

		/* get CountryCode Information */
		cc_index = libipv4addr_cc_index_by_addr(ipv4addrp, &data_source);

		if (cc_index < COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN) {
			libipv6calc_db_wrapper_country_code_by_cc_index(tempstring2, sizeof(tempstring2), cc_index);
			if ( machinereadable != 0 ) {
				printout2("IPV4_COUNTRYCODE", embeddedipv4string, tempstring2, formatoptions);

				if (data_source != IPV6CALC_DB_SOURCE_UNKNOWN) {
					for (i = 0; i < MAXENTRIES_ARRAY(data_sources); i++ ) {
						if (data_source == data_sources[i].number) {
							printout2("IPV4_COUNTRYCODE_SOURCE" , embeddedipv4string, data_sources[i].name, formatoptions);
							break;
						};
					};
				};
			} else {
				if (strlen(embeddedipv4string) > 0) {
					fprintf(stdout, "Country Code for %s: %s\n", embeddedipv4string, tempstring2);
				} else {
					fprintf(stdout, "Country Code: %s\n", tempstring2);
				};
			};
		} else {
			DEBUGPRINT_NA(DEBUG_showinfo, "Skip CountryCode print: cc_index>=COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN");
		};
	};


	DEBUGPRINT_NA(DEBUG_showinfo, "get registry");

	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
		registry = libipv4addr_registry_num_by_addr(ipv4addrp);
		snprintf(tempstring2, sizeof(tempstring2), "%s", libipv6calc_registry_string_by_num(registry));
	} else {
		libipv6calc_db_wrapper_registry_string_by_ipv4addr(ipv4addrp, tempstring, sizeof(tempstring));
		snprintf(tempstring2, sizeof(tempstring2), "%s", tempstring);
	};

	if (machinereadable != 0) {
		printout2("IPV4_REGISTRY", embeddedipv4string, tempstring2, formatoptions);
	} else {
		if (strlen(embeddedipv4string) > 0) {
			fprintf(stdout, "IPv4 registry for %s: %s\n", embeddedipv4string, tempstring2);
		} else {
			fprintf(stdout, "IPv4 registry: %s\n", tempstring2);
		};
	};


	if (((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) == 0) && ((ipv4addrp->typeinfo & IPV4_ADDR_GLOBAL) != 0)) {
#ifdef SUPPORT_IP2LOCATION
		/* IP2Location information */
		print_ip2location(tempipv4string, formatoptions, embeddedipv4string, 4);
#endif

#ifdef SUPPORT_GEOIP
		/* GeoIP information */
		print_geoip(tempipv4string, formatoptions, embeddedipv4string, 4);
#endif

#ifdef SUPPORT_DBIP
		/* db-ip.com information */
		print_dbip(&ipaddr, formatoptions, embeddedipv4string);
#endif

#ifdef SUPPORT_EXTERNAL
		/* External DB information */
		print_external(&ipaddr, formatoptions, embeddedipv4string);
#endif
	};

	return;
};


/*
 * print ASN information
 */
static void print_asn(const uint32_t asn, const uint32_t formatoptions) {
	char helpstring[NI_MAXHOST];
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	int registry;

	snprintf(helpstring, sizeof(helpstring), "%d", asn);

	if ( machinereadable != 0 ) {
		printout("AS_NUM", helpstring, formatoptions);
	} else {
		fprintf(stdout, "Autonomous System Number: %s\n", helpstring);
	};

	registry = libipv6calc_db_wrapper_registry_num_by_as_num32(asn);	
	if ( machinereadable != 0 ) {
		printout("AS_NUM_REGISTRY", libipv6calc_registry_string_by_num(registry), formatoptions);
	} else {
		fprintf(stdout, "Registry of Autonomous System Number: %s\n", libipv6calc_registry_string_by_num(registry));
	};
	
	return;
};


/*
 * print EUI-48/MAC information
 */
static void print_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions) {
	char helpstring[NI_MAXHOST];
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	int i, result;
	ipv6calc_ipv4addr ipv4addr;

	/* EUI-48/MAC address */
	snprintf(helpstring, sizeof(helpstring), "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) macaddrp->addr[0], (unsigned int) macaddrp->addr[1], (unsigned int) macaddrp->addr[2], (unsigned int) macaddrp->addr[3], (unsigned int) macaddrp->addr[4], (unsigned int) macaddrp->addr[5]);

	if ( machinereadable != 0 ) {
		printout("EUI48", helpstring, formatoptions);
	} else {
		fprintf(stdout, "EUI-48/MAC address: %s\n", helpstring);
	};

	/* scope */	
	if ( (macaddrp->addr[0] & 0x02) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI48_SCOPE", "global", formatoptions);
		} else {
			fprintf(stdout, "MAC is a global unique one\n");
		};
	} else {
		if ( machinereadable != 0 ) {
			printout("EUI48_SCOPE", "local", formatoptions);
		} else {
			fprintf(stdout, "MAC is a local one\n");
		};
	};
	
	/* unicast/multicast/broadcast */	
	if ( (macaddrp->addr[0] & 0x01) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI48_TYPE", "unicast", formatoptions);
		} else {
			fprintf(stdout, "MAC is an unicast one\n");
		};
	} else {
		if ( (macaddrp->addr[0] == 0xff) && (macaddrp->addr[1] == 0xff) && (macaddrp->addr[2] == 0xff) && (macaddrp->addr[3] == 0xff) && (macaddrp->addr[4] == 0xff) && (macaddrp->addr[5] == 0xff) ) {
			if ( machinereadable != 0 ) {
				printout("EUI48_TYPE", "broadcast", formatoptions);
			} else {
				fprintf(stdout, "MAC is a broadcast one\n");
			};
		} else {
			if ( machinereadable != 0 ) {
				printout("EUI48_TYPE", "multicast", formatoptions);
			} else {
				fprintf(stdout, "MAC is a multicast one\n");
			};

		};
	};
	
	/* vendor string */
	result = libipv6calc_db_wrapper_ieee_vendor_string_by_macaddr(helpstring, sizeof(helpstring), macaddrp);
	if (result == 0) {
		if ( machinereadable != 0 ) {
			printout("OUI", helpstring, formatoptions | FORMATOPTION_mr_quote_default);
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
		ipv4addr_settype(&ipv4addr, 1);
		ipv4addr.flag_valid = 1;

		if ( machinereadable != 0 ) {
			/* no additional hint */
		} else {
			fprintf(stdout, "Address type contains IPv4 address:\n");
		};
		print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "ISDN-NET/PLIP");
	};

	return;
};


/*
 * print EUI-64 information
 */
static void print_eui64(const ipv6calc_eui64addr *eui64addrp, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST], helpstring[NI_MAXHOST];
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	int result, i;
	ipv6calc_macaddr macaddr;

	/* EUI-64 address */
	snprintf(helpstring, sizeof(helpstring), "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) eui64addrp->addr[0], (unsigned int) eui64addrp->addr[1], (unsigned int) eui64addrp->addr[2], (unsigned int) eui64addrp->addr[3], (unsigned int) eui64addrp->addr[4], (unsigned int) eui64addrp->addr[5], (unsigned int) eui64addrp->addr[6], (unsigned int) eui64addrp->addr[7]);

	if ( machinereadable != 0 ) {
		printout("EUI64", helpstring, formatoptions);
	} else {
		fprintf(stdout, "EUI-64 identifier: %s\n", helpstring);
	};
	
	/* scope */	
	if ( (eui64addrp->addr[0] & 0x02) == 0 ) {
		if ( machinereadable != 0 ) {
			printout("EUI64_SCOPE", "global", formatoptions);
		} else {
			fprintf(stdout, "EUI-64 identifier is a global unique one\n");
		};
	} else {
		if ( machinereadable != 0 ) {
			printout("EUI64_SCOPE", "local", formatoptions);
		} else {
			fprintf(stdout, "EUI-64 identifier is a local one\n");
		};
	};

	/* get vendor string */
	for (i = 0; i < 6; i++) {	
		macaddr.addr[i] = eui64addrp->addr[i];
	};

	result = libipv6calc_db_wrapper_ieee_vendor_string_by_macaddr(helpstring, sizeof(helpstring), &macaddr);
	if (result == 0) {
		if ( machinereadable != 0 ) {
			printout("OUI", helpstring, formatoptions | FORMATOPTION_mr_quote_default);
		} else {
			fprintf(stdout, "OUI is: %s\n", tempstring);
		};
	};
	
	return;
};

/*
 * function shows information about a given IPv6 address
 *
 * in : *ipv6addrp = pointer to IPv6 address
 * ret: ==0: ok, !=0: error
 */
int showinfo_ipv6addr(const ipv6calc_ipv6addr *ipv6addrp1, const uint32_t formatoptions) {
	int retval = 1, i, j, flag_prefixuse, registry, r, retval_anon = 1;;
	char tempstring[NI_MAXHOST] = "", tempstring2[NI_MAXHOST] = "", helpstring[NI_MAXHOST] = "";
	char ipv6addrstring[NI_MAXHOST] = "";
	ipv6calc_ipv6addr ipv6addr, ipv6addr_anon, *ipv6addrp, *ipv6addr_anon_ptr;
	ipv6calc_ipv4addr ipv4addr, ipv4addr2;
	ipv6calc_macaddr macaddr;
	ipv6calc_eui64addr eui64addr;
	uint16_t port;
	uint32_t machinereadable = ( formatoptions & FORMATOPTION_machinereadable);
	uint32_t payload, as_num32, cc_index;
	char *as_text = NULL;
	unsigned int data_source = IPV6CALC_DB_SOURCE_UNKNOWN;
	ipv6calc_ipaddr ipaddr;

	ipv6addr_anon_ptr = &ipv6addr_anon;

	ipv6addrp = &ipv6addr;
	ipv6addr_copy(ipv6addrp, ipv6addrp1); /* copy structure */

	if (ipv6addrp->flag_typeinfo == 0) {
		fprintf(stderr, "FATAL error, typeinfo not valid - FIX CODE of caller\n");
		exit(2);
	};

	DEBUGPRINT_WA(DEBUG_showinfo, "typeinfo 0x%08x-0x%08x (from given address)", (unsigned int) ipv6addrp->typeinfo, (unsigned int) ipv6addrp->typeinfo2);

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++ ) {
		DEBUGPRINT_WA(DEBUG_showinfo, "test1: %08x : %s", (unsigned int) ipv6calc_ipv6addrtypestrings[i].number, ipv6calc_ipv6addrtypestrings[i].token);
	};	

	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addr_type2_strings); i++ ) {
		DEBUGPRINT_WA(DEBUG_showinfo, "test2: %08x : %s", (unsigned int) ipv6calc_ipv6addr_type2_strings[i].number, ipv6calc_ipv6addr_type2_strings[i].token);
	};


	/* get full uncompressed IPv6 address */
	flag_prefixuse = ipv6addrp->flag_prefixuse;
	ipv6addrp->flag_prefixuse = 0;
	retval = libipv6addr_ipv6addrstruct_to_uncompaddr(ipv6addrp, ipv6addrstring, sizeof(ipv6addrstring), FORMATOPTION_printfulluncompressed);
	if ( retval != 0 ) {
		fprintf(stderr, "Error uncompressing IPv6 address: %s\n", ipv6addrstring);
		retval = 1;
		goto END;
	};	

	ipv6addrp->flag_prefixuse = flag_prefixuse;

	CONVERT_IPV6ADDRP_IPADDR(ipv6addrp, ipaddr);

	if ((ipv6addrp->typeinfo & (IPV6_ADDR_ANONYMIZED_PREFIX | IPV6_ADDR_ANONYMIZED_IID)) == 0 ) {
		ipv6addr_copy(ipv6addr_anon_ptr, ipv6addrp); /* copy structure */

		retval_anon = libipv6addr_anonymize(ipv6addr_anon_ptr, &ipv6calc_anon_set);

		if (retval_anon == 0) {
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(ipv6addr_anon_ptr, tempstring2, sizeof(tempstring2), FORMATOPTION_printfulluncompressed);
			if ( retval != 0 ) {
				fprintf(stderr, "Error uncompressing IPv6 address: %s\n", tempstring2);
				retval = 1;
				retval_anon = 2;
			};
		};
	};
	
	if ( machinereadable != 0 ) {
		printout("IPV6", ipv6addrstring, formatoptions);
	
		if ((ipv6addrp->typeinfo & (IPV6_ADDR_ANONYMIZED_PREFIX | IPV6_ADDR_ANONYMIZED_IID | IPV6_ADDR_LOOPBACK)) == 0 ) {
			if (retval_anon == 0) {
				printout("IPV6_ANON", tempstring2, formatoptions);
			} else {
				// TODO: only show this on verbose (must be implemented)
				// snprintf(tempstring, sizeof(tempstring), "IPV6_ANON=(unsupported, too less DB features available)");
			};
		};

		if (ipv6addrp->flag_prefixuse == 1) {	
			snprintf(tempstring, sizeof(tempstring), "%d", (int) ipv6addrp->prefixlength);
			printout("IPV6_PREFIXLENGTH", tempstring, formatoptions);
		};

		j = 0;
		snprintf(tempstring, sizeof(tempstring), "%s", "");
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++ ) {
			if ((ipv6addrp->typeinfo & ipv6calc_ipv6addrtypestrings[i].number) != 0) {
				if (j != 0) {
					snprintf(helpstring, sizeof(helpstring), "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				};
				snprintf(helpstring, sizeof(helpstring), "%s%s", tempstring, ipv6calc_ipv6addrtypestrings[i].token);
				snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				j = 1;
			};
		};
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addr_type2_strings); i++ ) {
			if ((ipv6addrp->typeinfo2 & ipv6calc_ipv6addr_type2_strings[i].number) != 0) {
				if (j != 0) {
					snprintf(helpstring, sizeof(helpstring), "%s,", tempstring);
					snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				};
				snprintf(helpstring, sizeof(helpstring), "%s%s", tempstring, ipv6calc_ipv6addr_type2_strings[i].token);
				snprintf(tempstring, sizeof(tempstring), "%s", helpstring);
				j = 1;
			};
		};
		printout("IPV6_TYPE", tempstring, formatoptions);
	} else {
		fprintf(stdout, "Address type: ");
		j = 0;
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++ ) {
			if ((ipv6addrp->typeinfo & ipv6calc_ipv6addrtypestrings[i].number) != 0) {
				if ( j != 0 ) { fprintf(stdout, ", "); };
				fprintf(stdout, "%s", ipv6calc_ipv6addrtypestrings[i].token);
				j = 1;
			};
		};
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addr_type2_strings); i++ ) {
			if ((ipv6addrp->typeinfo2 & ipv6calc_ipv6addr_type2_strings[i].number) != 0) {
				if ( j != 0 ) { fprintf(stdout, ", "); };
				fprintf(stdout, "%s", ipv6calc_ipv6addr_type2_strings[i].token);
				j = 1;
			};
		};
		fprintf(stdout, "\n");
	};

	if ((ipv6addrp->typeinfo & (IPV6_NEW_ADDR_AGU)) != 0) {
		/* CountryCode */
		DEBUGPRINT_NA(DEBUG_showinfo, "get country code");
		cc_index = libipv6addr_cc_index_by_addr(ipv6addrp, &data_source);

		if (cc_index > COUNTRYCODE_INDEX_MAX) {
			if ( machinereadable != 0 ) {
				fprintf(stderr, "Error getting CountryCode for IPv6 address\n");
			};
		} else {
			if (cc_index < COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN) {
				libipv6calc_db_wrapper_country_code_by_cc_index(tempstring2, sizeof(tempstring2),cc_index);
				if ( machinereadable != 0 ) {
					printout("IPV6_COUNTRYCODE" , tempstring2, formatoptions);

					if (data_source != IPV6CALC_DB_SOURCE_UNKNOWN) {
						for (i = 0; i < MAXENTRIES_ARRAY(data_sources); i++ ) {
							if (data_source == data_sources[i].number) {
								printout("IPV6_COUNTRYCODE_SOURCE" , data_sources[i].name, formatoptions);
								break;
							};
						};
					};
				} else {
					fprintf(stdout, "Country Code: %s\n", tempstring2);
				};
			} else {
				DEBUGPRINT_NA(DEBUG_showinfo, "Skip CountryCode print: cc_index>=COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN");
			};
		};

		/* AS */
		DEBUGPRINT_NA(DEBUG_showinfo, "get AS number/text");
		if ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_PREFIX) == 0) {
			if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV6_TO_AS) == 1) {
				as_text = libipv6calc_db_wrapper_as_text_by_addr(&ipaddr);
			};
		};

		as_num32 = libipv6addr_as_num32_by_addr(ipv6addrp);

		if ((as_num32 == 0) && (machinereadable == 0)) {
			// fprintf(stderr, "Error getting AS number from IPv6 address\n");
		} else {
			if (as_num32 != ASNUM_AS_UNKNOWN) {
				if ( machinereadable != 0 ) {
					if (as_num32 == 0) {
						snprintf(tempstring, sizeof(tempstring), "(unknown)");
					} else {
						snprintf(tempstring, sizeof(tempstring), "%d", as_num32);
					};
					printout("IPV6_AS_NUM" ,tempstring, formatoptions);

					if (as_text != NULL) {
						printout("IPV6_AS_TEXT", as_text, formatoptions);
					};

				} else {
					fprintf(stdout, "ASN for address: %d\n", as_num32);
				};
			} else {
				DEBUGPRINT_NA(DEBUG_showinfo, "Skip AS print: as_num32=ASNUM_AS_UNKNOWN");
			};
		};
	};


	/* IPv6 Registry */
	DEBUGPRINT_NA(DEBUG_showinfo, "get registry");
	if (((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_PREFIX) != 0) \
		&& ((ipv6addrp->typeinfo & IPV6_ADDR_HAS_PUBLIC_IPV4_IN_PREFIX) == 0)) {
		registry = libipv6addr_registry_num_by_addr(ipv6addrp);
		snprintf(tempstring2, sizeof(tempstring2), "%s", libipv6calc_registry_string_by_num(registry));
	} else {
		libipv6calc_db_wrapper_registry_string_by_ipv6addr(ipv6addrp, tempstring, sizeof(tempstring));
		snprintf(tempstring2, sizeof(tempstring2), "%s", tempstring);
	};

	if ( machinereadable != 0 ) {
		printout("IPV6_REGISTRY", tempstring2, formatoptions);
	} else {
		fprintf(stdout, "Registry for address: %s\n", tempstring2);
	};


	/* 6to4 */
	if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6TO4) != 0) {
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);

		if (r == 0) {
			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, helpstring, sizeof(helpstring), 0);
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
			retval = libipv6calc_db_wrapper_registry_string_by_ipv4addr(&ipv4addr, helpstring, sizeof(helpstring));
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "IPv4 registry of 6to4 address: %s\n", helpstring);
			};
		};
	};

	/* Teredo */
	if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_TEREDO) != 0) {
		/* extract Teredo client IPv4 address */
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);

		if (r == 0) {
			r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr2, IPV6_ADDR_SELECT_IPV4_TEREDO_SERVER);

			if (r == 0) {
				print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "TEREDO-CLIENT");
				print_ipv4addr(&ipv4addr2, formatoptions | FORMATOPTION_printembedded, "TEREDO-SERVER");

				retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr2, helpstring, sizeof(helpstring), 0);
				if ( retval != 0 ) {
					fprintf(stderr, "Error converting IPv4 address to string\n");
					retval = 1;
					goto END;
				};	

				/* extract Teredo client UDP port */
				port = (uint16_t) (((uint16_t) ipv6addr_getoctet(ipv6addrp, (unsigned int) 10) << 8 | (uint16_t) ipv6addr_getoctet(ipv6addrp, (unsigned int) 11)) ^ 0xffff);

				if ( machinereadable != 0 ) {
					snprintf(tempstring, sizeof(tempstring), "%u", (unsigned int) port);
					printout("TEREDO_PORT_CLIENT", tempstring, formatoptions);
				} else {
					fprintf(stdout, "Address type is Teredo and included IPv4 server address is: %s and client port: %u\n", helpstring, (unsigned int) port);
				};

				/* get registry string */
				retval = libipv6calc_db_wrapper_registry_string_by_ipv4addr(&ipv4addr2, helpstring, sizeof(helpstring));
				
				if ( machinereadable != 0 ) {
				} else {
					fprintf(stdout, "IPv4 registry of Teredo server address: %s\n", helpstring);
				};
			};
		};
	};

	/* 6rd */
	if ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_6RD) != 0) {
		DEBUGPRINT_WA(DEBUG_showinfo, "6rd found with prefix: %d", ipv6addrp->prefix2length);
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_PREFIX2_LENGTH);

		if (r == 0) {
			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, helpstring, sizeof(helpstring), 0);
			if ( retval != 0 ) {
				fprintf(stderr, "Error converting IPv4 address to string\n");
				retval = 1;
				goto END;
			};	

			if ( machinereadable != 0 ) {
				print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "6RD");
			} else {
				fprintf(stdout, "Address type is IPv6 Rapid Deployment and included IPv4 address is: %s\n", helpstring);
			};

			/* get registry string */
			retval = libipv6calc_db_wrapper_registry_string_by_ipv4addr(&ipv4addr, helpstring, sizeof(helpstring));
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "IPv4 registry of IPv6 Rapid Deployment address: %s\n", helpstring);
			};
		};
	};

	/* NAT64 */
	if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_NAT64) != 0)  {
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);

		if (r == 0) {
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "IPv4 registry for NAT64 address: %s\n", helpstring);
			};
			print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "NAT64");
		};
	};

	/* SLA prefix included? */
	if (((ipv6addrp->typeinfo & ( IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0) && ((ipv6addrp->typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID | IPV6_ADDR_ANONYMIZED_PREFIX)) == 0)) {
		uint16_t sla = ipv6addr_getword(ipv6addrp, 3);
		uint16_t sla_mask = 0xffff;

		if ((ipv6addrp->typeinfo2 & IPV6_ADDR_TYPE2_6RD) != 0) {
			DEBUGPRINT_WA(DEBUG_showinfo, "6rd found with prefix: %d", ipv6addrp->prefix2length);
			// reduced SLA, mask value
			if ((ipv6addrp->prefix2length + 32) > 48) {
				// more than 48 bits used for 6RD prefix + included IPv4 address
				sla_mask = sla_mask >> (ipv6addrp->prefix2length - 16);
			};

			sla &= sla_mask;
		};
			
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "%04x", (unsigned int) sla);
			printout("SLA", tempstring, formatoptions);
		} else {
			fprintf(stdout, "Address type has SLA: %04x\n", (unsigned int) sla);
		};
	};

	/* Proper solicited node link-local multicast address? */
	if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0) {
		if ((ipv6addrp->typeinfo & (IPV6_ADDR_LINKLOCAL & IPV6_ADDR_MULTICAST)) != 0) {
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
	if ((ipv6addrp->typeinfo & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0) {
		r = libipv6addr_get_included_ipv4addr(ipv6addrp, &ipv4addr, IPV6_ADDR_SELECT_IPV4_DEFAULT);

		if (r == 0) {
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "Address type is compat/mapped and include an IPv4 address\n");
			};

			print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "COMPAT/MAPPED");
		};
	};

	/* Interface identifier included */
	if ((((ipv6addrp->typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0)
	    || ((ipv6addrp->typeinfo & (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) == (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)))
	  && ((ipv6addrp->typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID)) == 0) ) {
		if ( machinereadable != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "%04x:%04x:%04x:%04x", (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
			printout("IID", tempstring, formatoptions);
		} else {
			fprintf(stdout, "Interface identifier: %04x:%04x:%04x:%04x\n", (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
		};

		if ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_IID) == IPV6_ADDR_ANONYMIZED_IID) {
			if ( machinereadable != 0 ) {
			} else {
				fprintf(stdout, "Interface identifier is an anonymized one\n");
			};
		};

		if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_EUI48) == IPV6_NEW_ADDR_IID_EUI48) {
			/* EUI-48 */
			if ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_IID) == IPV6_ADDR_ANONYMIZED_IID) {
				payload = ipv6addr_get_payload_anonymized_iid(ipv6addrp, ipv6addrp->typeinfo) ^ 0x020000;

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
			if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_EUI64) == IPV6_NEW_ADDR_IID_EUI64) {
				/* EUI-64 */
				if ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_IID) == IPV6_ADDR_ANONYMIZED_IID) {
					payload = ipv6addr_get_payload_anonymized_iid(ipv6addrp, ipv6addrp->typeinfo) ^ 0x020000;

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
				if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0) {
					if ( machinereadable != 0 ) {
						snprintf(tempstring, sizeof(tempstring), "??:??:??:??:??:%02x:%02x:%02x", (unsigned int) ipv6addr_getoctet(ipv6addrp, 13), (unsigned int) ipv6addr_getoctet(ipv6addrp, 14), (unsigned int) ipv6addr_getoctet(ipv6addrp, 15));
						printout("EUI64", tempstring, formatoptions);
					} else {
						fprintf(stdout, "Generated from the extension identifier of an EUI-48 (MAC): ...:%02x:%02x:%02x\n", (unsigned int) ipv6addr_getoctet(ipv6addrp, 13), (unsigned int) ipv6addr_getoctet(ipv6addrp, 14), (unsigned int) ipv6addr_getoctet(ipv6addrp, 15));
					};
				} else if ((ipv6addrp->typeinfo & (IPV6_NEW_ADDR_IID_ISATAP | IPV6_NEW_ADDR_6TO4_MICROSOFT) ) != 0)  {
					if ((ipv6addrp->typeinfo & IPV6_ADDR_IID_32_63_HAS_IPV4) != 0) {
						/* IPv4 address included */

						if ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_IID) != 0) {
							payload = ipv6addr_get_payload_anonymized_iid(ipv6addrp, ipv6addrp->typeinfo);
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
						ipv4addr_settype(&ipv4addr, 1);
						ipv4addr.flag_valid = 1;

						if ( (ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_ISATAP) != 0 )  {
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
				} else if ((((ipv6addrp->typeinfo & IPV6_ADDR_LINKLOCAL) != 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0 && ipv6addr_getword(ipv6addrp, 6) != 0)))   {
					/* fe80:: must have 0000:0000:xxxx:yyyy where xxxx > 0 */
					if ( machinereadable != 0 ) {
					} else {
						fprintf(stdout, "Address type contains IPv4 address:\n");
					};

					for (i = 0; i <= 3; i++) {
						ipv4addr_setoctet(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctet(ipv6addrp, (unsigned int) (i + 12)));
					};
					ipv4addr_settype(&ipv4addr, 1);
					ipv4addr.flag_valid = 1;

					if ( machinereadable != 0 ) {
						// printout("IPV4_SOURCE=LINK-LOCAL-IID");
					};

					print_ipv4addr(&ipv4addr, formatoptions | FORMATOPTION_printembedded, "LINK-LOCAL-IID");
				} else {
					if ( machinereadable != 0 ) {
						if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
							printout("EUI64_SCOPE", "local-6to4-microsoft", formatoptions);
						} else if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_RANDOM) != 0) {
							printout("EUI64_SCOPE", "local-random", formatoptions);
						} else {
							printout("EUI64_SCOPE", "local", formatoptions);
						};
					} else {
						if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
							fprintf(stdout, "Interface identifier contain only IPv4 address from 6to4 prefix, usually seen on Microsoft OS\n");
						} else if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_IID_RANDOM) != 0) {
							fprintf(stdout, "Interface identifier is probably generated by privacy extension\n");
						} else {
							fprintf(stdout, "Interface identifier is probably manual set\n");
						};
					};
				};
			};
		};
	};

	if ((ipv6addrp->typeinfo & IPV6_NEW_ADDR_ORCHID) != 0) {
		/* 'extract' hash */
		ipv6addr_setword(ipv6addrp, 0, 0x0000);
		ipv6addr_setword(ipv6addrp, 1, ipv6addr_getword(ipv6addrp, 1) & 0x000F);

		retval = libipv6addr_to_hex(ipv6addrp, ipv6addrstring, sizeof(ipv6addrstring), 0);

		if ( machinereadable != 0 ) {
		} else {
			if ((ipv6addrp->typeinfo & IPV6_ADDR_ANONYMIZED_IID) != 0) {
				snprintf(tempstring, sizeof(tempstring), "n/a (previosly anonymized)");
			} else {
				snprintf(tempstring, sizeof(tempstring), "%s", ipv6addrstring+7);
			};
			fprintf(stdout, "ORCHID hash (100 bits): %s\n", tempstring);
		};
	};
END:

	i = libipv6calc_db_wrapper_registry_num_by_ipv6addr(ipv6addrp);
	if ((i != IPV6_ADDR_REGISTRY_RESERVED) && (i != IPV6_ADDR_REGISTRY_6BONE)) {
		if (((ipv6addrp->typeinfo & IPV6_NEW_ADDR_AGU) != 0) && ((ipv6addrp->typeinfo & (IPV6_NEW_ADDR_TEREDO | IPV6_NEW_ADDR_ORCHID | IPV6_ADDR_ANONYMIZED_PREFIX)) == 0)) {
#ifdef SUPPORT_IP2LOCATION
			/* IP2Location information */
			print_ip2location(ipv6addrstring, formatoptions, "", 6);
#endif

#ifdef SUPPORT_GEOIP
			/* GeoIP information */
			print_geoip(ipv6addrstring, formatoptions, "", 6);
#endif

#ifdef SUPPORT_DBIP
			/* db-ip.com information */
			print_dbip(&ipaddr, formatoptions, "");
#endif

#ifdef SUPPORT_EXTERNAL
			/* External DB information */
			print_external(&ipaddr, formatoptions, "");
#endif
		};
	};

	printfooter(formatoptions);
	retval = 0;
	return (retval);
};


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
int showinfo_eui48(const ipv6calc_macaddr *macaddrp, const uint32_t formatoptions) {
	int retval = 1;

	print_eui48(macaddrp, formatoptions);
	printfooter(formatoptions);

	retval = 0;
	return (retval);
};


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


/*
 * shows information about a given ASN
 *
 * in : asn
 * ret: ==0: ok, !=0: error
 */
int showinfo_asn(const uint32_t asn, const uint32_t formatoptions) {
	int retval = 1;

	print_asn(asn, formatoptions);
	printfooter(formatoptions);

	retval = 0;
	return (retval);
};
