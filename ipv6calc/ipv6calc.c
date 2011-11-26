/*
 * Project    : ipv6calc
 * File       : ipv6calc.c
 * Version    : $Id: ipv6calc.c,v 1.54 2011/11/26 16:07:23 peter Exp $
 * Copyright  : 2001-2011 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Central program (main)
 *  This program print out different formats of an given IP(v4/v6) address
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h>

#include "ipv6calc.h"
#include "libipv6calc.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calchelp.h"

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"

#include "showinfo.h"

#include "librfc1884.h"
#include "librfc1886.h"
#include "librfc1924.h"
#include "libifinet6.h"
#include "librfc2874.h"
#include "librfc3056.h"
#include "librfc3041.h"
#include "librfc5569.h"
#include "libeui64.h"
#include "config.h"
#include "libieee.h"
#include "../databases/ieee-oui/dbieee_oui.h"
#include "../databases/ieee-iab/dbieee_iab.h"
#include "libipv4addr.h"
#include "../databases/ipv4-assignment/dbipv4addr_assignment.h"
#include "libipv6addr.h"
#include "../databases/ipv6-assignment/dbipv6addr_assignment.h"

#ifdef SUPPORT_IP2LOCATION
#include "IP2Location.h"

/* 
 *  * API_VERSION is defined as a bareword in IP2Location.h, 
 *   * we need this trick to stringify it. Blah.
 *    */
#define makestr(x) #x
#define xmakestr(x) makestr(x)
#endif


#ifdef SUPPORT_GEOIP
#include "GeoIP.h"
#endif


long int ipv6calc_debug = 0;

/* pipe support */
int input_is_pipe = 0;
#define LINEBUFFER      16384

/* anonymization default values */
int mask_ipv4 = 24;
int mask_ipv6 = 48;
int mask_iid = 1;

int  use_ip2location_ipv4 = 0; /* if set to 1, IP2Location IPv4 is enabled by option(s) */
int  use_ip2location_ipv6 = 0; /* if set to 1, IP2Location IPv6 is enabled by option(s) */
#ifdef SUPPORT_IP2LOCATION
#ifdef IP2LOCATION_DEFAULT_FILE_IPV4
char file_ip2location_ipv4[NI_MAXHOST] = IP2LOCATION_DEFAULT_FILE_IPV4;
#else
char file_ip2location_ipv4[NI_MAXHOST] = "";
#endif
#ifdef IP2LOCATION_DEFAULT_FILE_IPV6
char file_ip2location_ipv6[NI_MAXHOST] = IP2LOCATION_DEFAULT_FILE_IPV6;
#else
char file_ip2location_ipv6[NI_MAXHOST] = "";
#endif
#endif

int  use_geoip_ipv4 = 0; /* if set to 1, GeoIP IPv4 is enabled by option(s) */
int  use_geoip_ipv6 = 0; /* if set to 1, GeoIP IPv6 is enabled by option(s) */
#ifdef SUPPORT_GEOIP
#ifdef GEOIP_DEFAULT_FILE_IPV4
char file_geoip_ipv4[NI_MAXHOST] = GEOIP_DEFAULT_FILE_IPV4;
#else
char file_geoip_ipv4[NI_MAXHOST] = "";
#endif
#ifdef GEOIP_DEFAULT_FILE_IPV6
char file_geoip_ipv6[NI_MAXHOST] = GEOIP_DEFAULT_FILE_IPV6;
#else
char file_geoip_ipv6[NI_MAXHOST] = "";
#endif
#endif

void printversion(void) {
	fprintf(stderr, "%s: version %s", PROGRAM_NAME, PACKAGE_VERSION);
#ifdef SUPPORT_IP2LOCATION
	fprintf(stderr, " IP2Location");
#endif
#ifdef SUPPORT_GEOIP
	fprintf(stderr, " GeoIP");
#ifdef SUPPORT_GEOIP_V6
	fprintf(stderr, " GeoIPv6");
#endif
#endif
#ifdef SUPPORT_DB_IEEE
	fprintf(stderr, " DB_IEEE");
#endif
#ifdef SUPPORT_DB_IPV4
	fprintf(stderr, " DB_IPV4");
#endif
#ifdef SUPPORT_DB_IPV6
	fprintf(stderr, " DB_IPV6");
#endif
	fprintf(stderr, "\n");
};

void printversion_verbose(void) {
	printversion();
	fprintf(stderr, "\n");

#ifdef SUPPORT_DB_IEEE
	fprintf(stderr, "IEEE database included: %s %s\n", libieee_iab_status, libieee_oui_status);
#else
	fprintf(stderr, "IEEE database not included\n");
#endif

#ifdef SUPPORT_DB_IPV4
	fprintf(stderr, "IPv4 database included: %s\n", dbipv4addr_registry_status);
#else
	fprintf(stderr, "IPv4 database not included\n");
#endif

#ifdef SUPPORT_DB_IPV6
	fprintf(stderr, "IPv6 database included: %s\n", dbipv6addr_registry_status);
#else
	fprintf(stderr, "IPv6 database not included\n");
#endif

	fprintf(stderr, "\n");

#ifdef SUPPORT_GEOIP
#ifdef SUPPORT_GEOIP_V6
#if defined (SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6) && defined (SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6)
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 & IPv6 support\n");
#else
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 & IPv6 support (in compatibilty mode)\n");
#endif
#else
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 support only\n");
#endif
#ifdef SUPPORT_GEOIP_LIB_VERSION
	fprintf(stderr, "GeoIP dynamic library version (on this system): %s\n", GeoIP_lib_version());
#else
	fprintf(stderr, "GeoIP dynamic library version (on this system): compiled without detection\n");
#endif
	if (file_geoip_ipv4 != NULL && strlen(file_geoip_ipv4) > 0) {
		fprintf(stderr, "GeoIP IPv4 default file: %s\n", file_geoip_ipv4);
	} else {
		fprintf(stderr, "GeoIP IPv4 default file: not configured\n");
	};
#ifdef SUPPORT_GEOIP_V6
	if (file_geoip_ipv6 != NULL && strlen(file_geoip_ipv6) > 0) {
		fprintf(stderr, "GeoIP IPv6 default file: %s\n", file_geoip_ipv6);
	} else {
		fprintf(stderr, "GeoIP IPv6 default file: not configured\n");
	};
#endif
#else
	fprintf(stderr, "GeoIP support not enabled\n");
#endif

	fprintf(stderr, "\n");

#ifdef SUPPORT_IP2LOCATION
	fprintf(stderr, "IP2Location support enabled, compiled with API version: %s\n", xmakestr(API_VERSION));
	if (file_ip2location_ipv4 != NULL && strlen(file_ip2location_ipv4) > 0) {
		fprintf(stderr, "IP2Location IPv4 default file: %s\n", file_ip2location_ipv4);
	} else {
		fprintf(stderr, "IP2Location IPv4 default file: not configured\n");
	};
	if (file_ip2location_ipv6 != NULL && strlen(file_ip2location_ipv6) > 0) {
		fprintf(stderr, "IP2Location IPv6 default file: %s\n", file_ip2location_ipv6);
	} else {
		fprintf(stderr, "IP2Location IPv6 default file: not configured\n");
	};
#else
	fprintf(stderr, "IP2Location support not enabled\n");
#endif

};

void printcopyright(void) {
	fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program formats and calculates IPv6 addresses and can do many more tricky things\n");
	fprintf(stderr, "See '%s -?|-h|--help or -v -v' for more details\n\n", PROGRAM_NAME);
};


/**************************************************/
/* main */
#define DEBUG_function_name "ipv6calc/main"
int main(int argc, char *argv[]) {
	char resultstring[NI_MAXHOST] = "";
	char resultstring2[NI_MAXHOST] = "";
	char resultstring3[NI_MAXHOST] = "";
	int retval = 1, i, j, lop;
	unsigned long int command = 0;
	int bit_start = 0, bit_end = 0, force_prefix = 0;
	char *input1 = NULL, *input2 = NULL;
	int inputc;
	int inputtype_given = 0, outputtype_given = 0, action_given = 0;

	/* new option style storage */	
	uint32_t inputtype  = FORMAT_undefined;
	uint32_t outputtype = FORMAT_undefined;
	uint32_t action     = ACTION_undefined;

	/* format options storage */
	uint32_t formatoptions = 0;

	/* used structures */
	ipv6calc_ipv6addr ipv6addr, ipv6addr2, ipv6addr3, ipv6addr4;
	ipv6calc_ipv4addr ipv4addr, ipv4addr2;
	ipv6calc_macaddr  macaddr;

	/* clear address structures */
	ipv6addr_clearall(&ipv6addr);
	ipv6addr_clearall(&ipv6addr2);
	ipv6addr_clearall(&ipv6addr3);
	ipv6addr_clearall(&ipv6addr4);
	ipv4addr_clearall(&ipv4addr);
	mac_clearall(&macaddr);

	/* pipe mode */
	char linebuffer[LINEBUFFER];
	char token[LINEBUFFER];
	char *charptr, *cptr, **ptrptr;
	ptrptr = &cptr;
	int linecounter = 0;
	int flush_mode = 0;

	if (argc <= 1) {
		printinfo();
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, ipv6calc_shortopts, ipv6calc_longopts, &lop)) != EOF) {
		switch (i) {
			case -1:
				break;

			case 'v':
				if ((command & CMD_printversion) != 0) {
					// second time '-v'
					command |= CMD_printversion_verbose;
				} else {
					command |= CMD_printversion;
				};
				break;

			case 'f':
				flush_mode = 1;
				break;

			case 'h':
			case '?':
				command |= CMD_printhelp;
				break;
				
			case 'd':
				ipv6calc_debug = atol(optarg);
				break;

			case DB_ip2location_ipv4:
#ifdef SUPPORT_IP2LOCATION
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got IP2Location IPv4 database file: %s\n", DEBUG_function_name, optarg);
				};
				strncpy(file_ip2location_ipv4, optarg, sizeof(file_ip2location_ipv4) -1 );
				use_ip2location_ipv4 = 1;
#else
				if ((formatoptions & FORMATOPTION_quiet) == 0) {
					fprintf(stderr, " Support for option '--db-ip2location-ipv4 <IP2Location IPv4 database file>' not compiled in, IP2Location support disabled\n");
				};
#endif
				break;

			case DB_ip2location_ipv6:
#ifdef SUPPORT_IP2LOCATION
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got IP2Location IPv6 database file: %s\n", DEBUG_function_name, optarg);
				};
				strncpy(file_ip2location_ipv6, optarg, sizeof(file_ip2location_ipv6) -1 );
				use_ip2location_ipv6 = 1;
#else
				if ((formatoptions & FORMATOPTION_quiet) == 0) {
					fprintf(stderr, " Support for option '--db-ip2location-ipv6 <IP2Location IPv6 database file>' not compiled in, IP2Location support disabled\n");
				};
#endif
				break;

			case 'L':
#ifdef IP2LOCATION_DEFAULT_FILE_IPV4
				use_ip2location_ipv4 = 1;
#endif
#ifdef IP2LOCATION_DEFAULT_FILE_IPV6
				use_ip2location_ipv6 = 1;
#endif
				if (use_ip2location_ipv4 == 0 && use_ip2location_ipv6 == 0) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, " Neither support for IP2Location IPv4 or IPv6 default database is compiled in, IP2Location support disabled\n");
					};
				};
				break;

			case DB_ip2location_ipv4_default:
#ifdef IP2LOCATION_DEFAULT_FILE_IPV4
				use_ip2location_ipv4 = 1;
#else
				if ((formatoptions & FORMATOPTION_quiet) == 0) {
					fprintf(stderr, " Support for option '--db-ip2location-ipv4-default' not compiled in, IP2Location IPv4 support disabled\n");
				};
#endif
				break;

			case DB_ip2location_ipv6_default:
#ifdef IP2LOCATION_DEFAULT_FILE_IPV6
				use_ip2location_ipv6 = 1;
#else
				if ((formatoptions & FORMATOPTION_quiet) == 0) {
					fprintf(stderr, " Support for option '--db-ip2location-ipv6-default' not compiled in, IP2Location IPv6 support disabled\n");
				};
#endif
				break;

			case DB_geoip_ipv4:
#ifdef SUPPORT_GEOIP
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got GeoIP IPv4 database file: %s\n", DEBUG_function_name, optarg);
				};
				strncpy(file_geoip_ipv4, optarg, sizeof(file_geoip_ipv4) -1 );
				use_geoip_ipv4 = 1;
#else
				if ((formatoptions & FORMATOPTION_quiet) == 0) {
					fprintf(stderr, " Support for option '--db-geoip-ipv4 <GeoIP IPv4 database file>' not compiled in, GeoIP support disabled\n");
				};
#endif
				break;

			case DB_geoip_ipv6:
#ifdef SUPPORT_GEOIP_V6
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got GeoIP IPv6 database file: %s\n", DEBUG_function_name, optarg);
				};
				strncpy(file_geoip_ipv6, optarg, sizeof(file_geoip_ipv6) -1 );
				use_geoip_ipv6 = 1;
#else
				if ((formatoptions & FORMATOPTION_quiet) == 0) {
					fprintf(stderr, " Support for option '--db-geo-ipv6 <GeoIP IPv6 database file>' not compiled in, GeoIP(v6) support disabled\n");
				};
#endif
				break;

			case 'G':
#ifdef GEOIP_DEFAULT_FILE_IPV4
				use_geoip_ipv4 = 1;
#endif
#ifdef GEOIP_DEFAULT_FILE_IPV6
				use_geoip_ipv6 = 1;
#endif
				if (use_geoip_ipv4 == 0 && use_geoip_ipv6 == 0) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, " Neither support for GeoIP IPv4 or IPv6 default database is compiled in, GeoIP support disabled\n");
					};
				};
				break;

			case DB_geoip_ipv4_default:
#ifdef GEOIP_DEFAULT_FILE_IPV4
				use_geoip_ipv4 = 1;
#else
				if ((formatoptions & FORMATOPTION_quiet) == 0) {
					fprintf(stderr, " Support for option '--db-geoip-ipv4-default' not compiled in, GeoIP IPv4 support disabled\n");
				};
#endif
				break;

			case DB_geoip_ipv6_default:
#ifdef GEOIP_DEFAULT_FILE_IPV6
#ifdef SUPPORT_GEOIP_V6
				use_geoip_ipv6 = 1;
#endif
#else
				if ((formatoptions & FORMATOPTION_quiet) == 0) {
					fprintf(stderr, " Support for option '--db-geoip-ipv6-default' not compiled in, GeoIP IPv6 support disabled\n");
				};
#endif
				break;

			case CMD_printexamples:
				command = CMD_printexamples;
				break;

			case CMD_printoldoptions:
				command = CMD_printoldoptions;
				break;


			/* backward compatibility/shortcut commands */
			case 'r':
			case CMD_addr_to_ip6int:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_auto;
				outputtype = FORMAT_revnibbles_int;
				break;

			case 'a':
			case CMD_addr_to_ip6arpa:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_auto;
				outputtype = FORMAT_revnibbles_arpa;
				break;

			case 'b':
			case CMD_addr_to_bitstring:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_bitstring;
				break;
				
			case CMD_addr_to_compressed:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_addr_to_uncompressed:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case CMD_addr_to_base85:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_base85;
				break;
				
			case CMD_base85_to_addr:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_base85;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;

			case CMD_mac_to_eui64:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_mac;
				outputtype = FORMAT_eui64;
				action = ACTION_mac_to_eui64;
				break;
				
			case CMD_addr_to_fulluncompressed:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case CMD_addr_to_ifinet6:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ifinet6;
				break;

			case CMD_ifinet6_to_compressed:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_ifinet6;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_ipv4_to_6to4addr:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_ipv4addr;
				outputtype = FORMAT_ipv6addr;
				action = ACTION_ipv4_to_6to4addr;
				break;
				
			case CMD_eui64_to_privacy:
				if (inputtype != FORMAT_undefined || outputtype != FORMAT_undefined) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_iid_token;
				outputtype = FORMAT_iid_token;
				action = ACTION_iid_token_to_privacy;
				break;

			case 'i':
			case CMD_showinfo:
				command = CMD_showinfo;
				break;

			case CMD_showinfotypes:
				command = CMD_showinfotypes;
				break;

			case CMD_ANON_NO_MASK_IID:
				mask_iid = 0;
				break;

			case CMD_ANON_MASK_IPV4:
				mask_ipv4 = atoi(optarg);
				if (mask_ipv4 < 0 || mask_ipv4 > 32) {
					fprintf(stderr, " value for option 'mask-ipv4' out-of-range  [0-32]\n");
					exit(EXIT_FAILURE);
				};
				break;

			case CMD_ANON_MASK_IPV6:
				mask_ipv6 = atoi(optarg);
				if (mask_ipv6 < 0 || mask_ipv6 > 32) {
					fprintf(stderr, " value for option 'mask-ipv4' out-of-range  [0-64]\n");
					exit(EXIT_FAILURE);
				};
				break;

			case CMD_ANON_PRESET_STANDARD:
				mask_ipv4 = 24;
				mask_ipv6 = 48;
				mask_iid = 1;
				break;

			case CMD_ANON_PRESET_CAREFUL:
				mask_ipv4 = 20;
				mask_ipv6 = 40;
				mask_iid = 1;
				break;

			case CMD_ANON_PRESET_PARANOID:
				mask_ipv4 = 16;
				mask_ipv6 = 32;
				mask_iid = 1;
				break;

			case CMD_6rd_relay_prefix:
				retval = addr_to_ipv4addrstruct(optarg, resultstring, &ipv4addr2);
				break;

			case CMD_6rd_prefix:
				retval = addr_to_ipv6addrstruct(optarg, resultstring, &ipv6addr);
				break;

			/* format options */
			case 'C':
			case FORMATOPTION_NUM_printcompressed + FORMATOPTION_NUM_HEAD:
	       			formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case 'U':
			case FORMATOPTION_NUM_printuncompressed + FORMATOPTION_NUM_HEAD:
	       			formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case 'F':
			case FORMATOPTION_NUM_printfulluncompressed + FORMATOPTION_NUM_HEAD:
	       			formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case FORMATOPTION_NUM_printprefix + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printprefix;
				break;
				
			case FORMATOPTION_NUM_printsuffix + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printsuffix;
				break;
				
			case FORMATOPTION_NUM_maskprefix + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_maskprefix;
				break;
				
			case FORMATOPTION_NUM_masksuffix + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_masksuffix;
				break;
				
			case 'l':	
			case FORMATOPTION_NUM_printlowercase + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printlowercase;
				break;
				
			case 'u':	
			case FORMATOPTION_NUM_printuppercase + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printuppercase;
				break;
				
			case FORMATOPTION_NUM_forceprefix + FORMATOPTION_NUM_HEAD:
				if (ipv6calc_debug != 0) fprintf(stderr, "%s: format option 'forceprefix' selected\n", DEBUG_function_name);
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					force_prefix = atoi(optarg);
					formatoptions |= FORMATOPTION_forceprefix;
				} else {
					fprintf(stderr, " Argument of option 'forceprefix' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case FORMATOPTION_NUM_printstart + FORMATOPTION_NUM_HEAD:
				if (ipv6calc_debug != 0) fprintf(stderr, "%s: format option 'printstart' selected\n", DEBUG_function_name);
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_start = atoi(optarg);
					formatoptions |= FORMATOPTION_printstart;
				} else {
					fprintf(stderr, " Argument of option 'printstart' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case FORMATOPTION_NUM_printend + FORMATOPTION_NUM_HEAD:
				if (ipv6calc_debug != 0) fprintf(stderr, "%s: format option 'printend' selected\n", DEBUG_function_name);
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_end = atoi(optarg);
					formatoptions |= FORMATOPTION_printend;
				} else {
					fprintf(stderr, " Argument of option 'printend' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case 'm':	
			case FORMATOPTION_NUM_machinereadable + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_machinereadable;
				break;
				
			case 'q':	
			case FORMATOPTION_NUM_quiet + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_quiet;
				break;
				
			case FORMATOPTION_NUM_printmirrored + FORMATOPTION_NUM_HEAD:
				formatoptions |= FORMATOPTION_printmirrored;
				break;

			/* new options */
			case 'I':	
			case CMD_inputtype:
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got input string: %s\n", DEBUG_function_name, optarg);
				};
				if ((strcmp(optarg, "-?") == 0) || (strcmp(optarg, "-h") == 0) || (strcmp(optarg, "--help") == 0) ) {
					inputtype = FORMAT_auto;
					command = CMD_printhelp;
					break;
				};
				
				inputtype = ipv6calctypes_checktype(optarg);
				
				if (inputtype == FORMAT_undefined) {
					fprintf(stderr, " Input option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				inputtype_given = 1;
				break;	
				
			case 'O':	
			case CMD_outputtype:
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got output string: %s\n", DEBUG_function_name, optarg);
				};
				if ((strcmp(optarg, "-?") == 0) || (strcmp(optarg, "-h") == 0) || (strcmp(optarg, "--help") == 0) ) {
					outputtype = FORMAT_auto;
					command = CMD_printhelp;
					break;
				};
				
				outputtype = ipv6calctypes_checktype(optarg);
				if (outputtype == FORMAT_undefined) {
					fprintf(stderr, " Output option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				outputtype_given = 1;
				break;	

			case 'A':	
			case CMD_actiontype:
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got action string: %s\n", DEBUG_function_name, optarg);
				};
				if ((strcmp(optarg, "-?") == 0) || (strcmp(optarg, "-h") == 0) || (strcmp(optarg, "--help") == 0) ) {
					action = ACTION_auto;
					command = CMD_printhelp;
					break;
				};
				action = ipv6calctypes_checkaction(optarg);
				if (action == ACTION_undefined) {
					fprintf(stderr, " Action option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				action_given = 1;
				break;
				
			default:
				fprintf(stderr, "Usage: (see '%s --command -?|-h|--help' for more help)\n", PROGRAM_NAME);
				printhelp();
				exit(EXIT_FAILURE);
		};
	};
	argv += optind;
	argc -= optind;

	/* print help handling */
	if (command == CMD_printhelp) {
		if ( (outputtype == FORMAT_undefined) && (inputtype == FORMAT_undefined) && (action == ACTION_undefined)) {
			printhelp();
			exit(EXIT_FAILURE);
		} else if (outputtype == FORMAT_auto) {
			if (inputtype == FORMAT_undefined) {
				inputtype = FORMAT_auto;
			};
			printhelp_outputtypes(inputtype, formatoptions);
			exit(EXIT_FAILURE);
		} else if (inputtype == FORMAT_auto) {
			printhelp_inputtypes(formatoptions);
			exit(EXIT_FAILURE);
		} else if (action == ACTION_auto) {
			printhelp_actiontypes(formatoptions);
			exit(EXIT_FAILURE);
		};

	} else if (command == CMD_printexamples) {
		printhelp_output_dispatcher(outputtype);
		exit(EXIT_FAILURE);
	} else if (command == CMD_showinfotypes) {
		showinfo_availabletypes();
		exit(EXIT_FAILURE);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Debug value:%08lx command:%08lx inputtype:%08lx outputtype:%08lx action:%08lx formatoptions:%08lx\n", (unsigned long) ipv6calc_debug, command, (unsigned long) inputtype, (unsigned long) outputtype, (unsigned long) action, (unsigned long) formatoptions); 
	};
	
	/* do work depending on selection */
	if ((command & CMD_printversion) != 0) {
		if ((command & CMD_printversion_verbose) != 0) {
			printversion_verbose();
		} else {
			printversion();
		};
		exit(EXIT_SUCCESS);
	};

	if (command == CMD_printoldoptions) {
		printhelp_oldoptions();
		exit(EXIT_FAILURE);
	};
	
	if (argc > 0) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, argv[0]);
		};
	} else {
		if (isatty (STDIN_FILENO)) {
		} else {
			input_is_pipe = 1;
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Input is a pipe\n", DEBUG_function_name);
			};
			formatoptions |= FORMATOPTION_quiet; // force quiet mode
		};
	};

	/***** input type handling *****/
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of input type handling\n", DEBUG_function_name);
	};

	/* check length of input */
	if (argc > 0) {
		if (input_is_pipe == 1) {
			fprintf(stderr, "Additional input found in pipe mode\n");
			exit(EXIT_FAILURE);
		};
		for (i = 0; i < argc; i++) {
			if ( argv[i] != NULL ) {
				if ( strlen(argv[i]) >= NI_MAXHOST ) {
					/* that's not good for size limited buffers...*/
					fprintf(stderr, "Argument %d is too long\n", i + 1);
					exit(EXIT_FAILURE);
				};
			};
		};
	};

	/* loop for pipe */
	if (input_is_pipe == 1) {
PIPE_input:
                /* read line from stdin */
                charptr = fgets(linebuffer, LINEBUFFER, stdin);

		if (charptr == NULL) {
			/* end of input */
			exit(retval);
		};

		linecounter++;

		if (linecounter == 1) {
			if (ipv6calc_debug == 1) {
				fprintf(stderr, "Ok, proceeding stdin...\n");
			};
		};
		
		if (ipv6calc_debug == 1) {
			fprintf(stderr, "Line: %d\r", linecounter);
		};

		if (strlen(linebuffer) >= NI_MAXHOST) {
			fprintf(stderr, "Line too long: %d\n", linecounter);
			exit(EXIT_FAILURE);
		};
		
		if (strlen(linebuffer) == 0) {
			fprintf(stderr, "Line empty: %d\n", linecounter);
			goto PIPE_input;
		};
		
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: stdin got line: '%s'\n", DEBUG_function_name, linebuffer);
		};

		if (linebuffer[strlen(linebuffer) - 1] == '\n') {
			/* remove trailing \n */
			linebuffer[strlen(linebuffer) - 1] = '\0';
		};

		/* look for first token */
		charptr = strtok_r(linebuffer, " \t\n", ptrptr);

		if ( charptr == NULL ) {
			fprintf(stderr, "Line contains no token: %d\n", linecounter);
			goto PIPE_input;
		};

		if ( strlen(charptr) >=  LINEBUFFER) {
			fprintf(stderr, "Line too strange: %d\n", linecounter);
			goto PIPE_input;
		};

		snprintf(token, sizeof(token) - 1, "%s", charptr);
		
		input1 = token;
		inputc = 1;

		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Token 1: '%s'\n", DEBUG_function_name, input1);
		};

		/* check for second token */
		if (*ptrptr[0] != '\0') {
			input2 = *ptrptr;
			inputc = 2;

			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Token 2: '%s'\n", DEBUG_function_name, input2);
			};
		};

	} else {
		inputc = argc;
		if (argc >= 1) {
			input1 = argv[0];
		};
		if (argc >= 2) {
			input2 = argv[1];
		};
	};
	
	/* autodetection */
	if ((inputtype == FORMAT_undefined || inputtype == FORMAT_auto) && inputc > 0) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Call input type autodetection\n", DEBUG_function_name);
		};
		/* no input type specified or automatic selected */
		if ((formatoptions & FORMATOPTION_quiet) == 0) {
			fprintf(stderr, "No input type specified, try autodetection...");
		};
		
		inputtype = libipv6calc_autodetectinput(input1);

		if ( inputtype != FORMAT_undefined ) {
			for (i = 0; i < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); i++) {
				if (inputtype == ipv6calc_formatstrings[i].number) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, "found type: %s\n", ipv6calc_formatstrings[i].token);
					};
					break;
				};
			};
		} else {
			fprintf(stderr, "no result!\n");
		};
	};

	/* auto set of output type*/
	if (inputtype == FORMAT_revnibbles_int || inputtype == FORMAT_revnibbles_arpa || inputtype == FORMAT_ipv6literal) {
		if ( outputtype == FORMAT_undefined ) {
			outputtype = FORMAT_ipv6addr;
		};
	};

	/***** automatic action handling *****/
	if (action == ACTION_undefined) {
		if ( (inputtype == FORMAT_mac && (input_is_pipe == 0 || inputtype_given == 1)) && (outputtype == FORMAT_eui64 && outputtype_given == 1) ) {
			action = ACTION_mac_to_eui64;
		} else if ( (inputtype == FORMAT_iid_token && (input_is_pipe == 0 || inputtype_given == 1)) && (outputtype == FORMAT_iid_token && outputtype_given == 1) ) {
			action = ACTION_iid_token_to_privacy;
		} else if ( inputtype == FORMAT_prefix_mac && (input_is_pipe == 0 || inputtype_given == 1)) {
			action = ACTION_prefix_mac_to_ipv6;
			if ( outputtype == FORMAT_undefined ) {
				outputtype = FORMAT_ipv6addr;
			};
		};
	};

	if (input_is_pipe == 0 && action_given == 0) {
		if ( action != ACTION_undefined ) {
			if ((formatoptions & FORMATOPTION_quiet) == 0) {
				fprintf(stderr, "No action type specified, try autodetection...");
			};
			for (i = 0; i < (int) (sizeof(ipv6calc_actionstrings) / sizeof(ipv6calc_actionstrings[0])); i++) {
				if (action == ipv6calc_actionstrings[i].number) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, "found type: %s\n", ipv6calc_actionstrings[i].token);
					};
					break;
				};
			};
		};
	};

	/* check formatoptions for validity */
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: check format option of output type: 0x%08lx\n", DEBUG_function_name, (unsigned long) outputtype);
	};
		
	for (i = 0; i < (int) (sizeof(ipv6calc_outputformatoptionmap) / sizeof(ipv6calc_outputformatoptionmap[0])); i++) {
		if (outputtype != ipv6calc_outputformatoptionmap[i][0]) {
			continue;
		};

		if ( (ipv6calc_outputformatoptionmap[i][1] & (formatoptions & ~ FORMATOPTION_quiet) ) == (formatoptions & ~ FORMATOPTION_quiet ) ) {
			/* all options valid */
			break;
		};
		
		fprintf(stderr, " Unsupported format option(s):\n");

		/* run through format options */
		for (j = 0; j < (int) (sizeof(ipv6calc_formatoptionstrings) / sizeof (ipv6calc_formatoptionstrings[0])); j++) {
			if ( (ipv6calc_formatoptionstrings[j].number & FORMATOPTION_quiet) != 0 ) {
				/* skip format option "quiet" on check */
				continue;
			};
			if ((((~ ipv6calc_outputformatoptionmap[i][1]) & formatoptions) & ipv6calc_formatoptionstrings[j].number) != 0) {
				fprintf(stderr, "  %s: %s\n", ipv6calc_formatoptionstrings[j].token, ipv6calc_formatoptionstrings[j].explanation);
			};
		};
		exit(EXIT_FAILURE);
	};
	
	/* proceed input depending on type */	
	retval = -1; /* default */

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Process input (inputtype: 0x%08lx, count=%d)\n", DEBUG_function_name, (unsigned long) inputtype, inputc);
	};

	switch (inputtype) {
		case FORMAT_ipv6addr:
		case FORMAT_ipv6literal:
		case FORMAT_ipv4addr:
		case FORMAT_ipv4hex:
		case FORMAT_ipv4revhex:
		case FORMAT_base85:
		case FORMAT_mac:
		case FORMAT_ifinet6:
		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
		case FORMAT_bitstring:
			if (inputc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			break;

		case FORMAT_iid_token:
		case FORMAT_prefix_mac:
			if (inputc < 2) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			break;

		case FORMAT_auto_noresult:
		default:
			fprintf(stderr, " Input-type isn't autodetected\n");
			exit(EXIT_FAILURE);
	};

	switch (inputtype) {
		case FORMAT_ipv6addr:
			retval = addr_to_ipv6addrstruct(input1, resultstring, &ipv6addr);
			argc--;
			break;

		case FORMAT_ipv6literal:
			retval = addrliteral_to_ipv6addrstruct(input1, resultstring, &ipv6addr);
			argc--;
			break;

		case FORMAT_ipv4addr:
			retval = addr_to_ipv4addrstruct(input1, resultstring, &ipv4addr);
			argc--;
			break;
			
		case FORMAT_ipv4hex:
			retval = addrhex_to_ipv4addrstruct(input1, resultstring, &ipv4addr, 0);
			argc--;
			break;

		case FORMAT_ipv4revhex:
			retval = addrhex_to_ipv4addrstruct(input1, resultstring, &ipv4addr, 1);
			argc--;
			break;

		case FORMAT_base85:
			retval = base85_to_ipv6addrstruct(input1, resultstring, &ipv6addr);
			argc--;
			break;
			
		case FORMAT_mac:
			retval = mac_to_macaddrstruct(input1, resultstring, &macaddr);
			argc--;
			break;

		case FORMAT_ifinet6:
			if (inputc < 2) {
				retval = libifinet6_ifinet6_to_ipv6addrstruct(input1, resultstring, &ipv6addr);
			} else {
				retval = libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(input1, input2, resultstring, &ipv6addr);
			};
			break;

		case FORMAT_iid_token:
			/* Get first interface identifier */
			retval = identifier_to_ipv6addrstruct(input1, resultstring, &ipv6addr);
			if (retval != 0) { break; };
			
			/* Get second token */
			retval = tokenlsb64_to_ipv6addrstruct(input2, resultstring, &ipv6addr2);
			break;
			
		case FORMAT_prefix_mac:
			/* Get first: IPv6 prefix */
			retval = addr_to_ipv6addrstruct(input1, resultstring, &ipv6addr);
			if (retval != 0) { break; };
			
			/* Get second: MAC address */
			retval = mac_to_macaddrstruct(input2, resultstring, &macaddr);
			break;

		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			retval = librfc1886_nibblestring_to_ipv6addrstruct(input1, &ipv6addr, resultstring);
			argc--;
			break;
			
		case FORMAT_bitstring:
			retval = librfc2874_bitstring_to_ipv6addrstruct(input1, &ipv6addr, resultstring);
			argc--;
			break;
			
		case FORMAT_auto_noresult:
			fprintf(stderr, " Input-type isn't autodetected\n");
			exit(EXIT_FAILURE);

		default:
			fprintf(stderr, " Input-type isn't implemented\n");
			exit(EXIT_FAILURE);
	};

	if (retval != 0) {
		if (strlen(resultstring) > 0) fprintf(stderr, "%s\n", resultstring);
		exit(EXIT_FAILURE);
	};
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: End of input type handling\n", DEBUG_function_name);
	};

	/***** postprocessing input *****/
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of postprocessing input\n", DEBUG_function_name);
	};

	if (ipv6addr.flag_valid == 1) {
		/* force prefix */
		if ((formatoptions & (FORMATOPTION_forceprefix)) != 0) {
			ipv6addr.flag_prefixuse = 1;
			ipv6addr.prefixlength = force_prefix;
		};
		
		/* mask bits */
		if ((formatoptions & (FORMATOPTION_maskprefix | FORMATOPTION_masksuffix)) != 0) {
			if (ipv6addr.flag_prefixuse == 1) {
				if ((formatoptions & FORMATOPTION_maskprefix) != 0) {
					ipv6addrstruct_maskprefix(&ipv6addr);
				} else if ((formatoptions & FORMATOPTION_masksuffix) != 0) {
					ipv6addrstruct_masksuffix(&ipv6addr);
				};
			} else {
				fprintf(stderr, " Error: mask option used without specifying a prefix length\n");
				exit(EXIT_FAILURE);
			};
		};
		
		/* start bit */
		if ((formatoptions & FORMATOPTION_printstart) != 0) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Set bit start to: %d\n", DEBUG_function_name, bit_start);
			};
			ipv6addr.bit_start = (uint8_t) bit_start;
			ipv6addr.flag_startend_use = 1;
		} else {
			ipv6addr.bit_start = 1;
		};
		
		/* end bit */
		if ((formatoptions & FORMATOPTION_printend) != 0) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Set bit end to: %d\n", DEBUG_function_name, bit_end);
			};
			ipv6addr.bit_end = (uint8_t) bit_end;
			ipv6addr.flag_startend_use = 1;
		} else {
			/* default */
			ipv6addr.bit_end = 128;
		};

		/* prefix+suffix */
		if ((formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix)) != 0) {
			if ( ipv6addr.flag_prefixuse == 0 ) {
				fprintf(stderr, " Error: missing prefix length for printing prefix/suffix\n");
				exit(EXIT_FAILURE);
			} else {
				if ( ipv6addr.flag_startend_use == 0 ) {
					/* only print[prefix|suffix] */
					if ((formatoptions & FORMATOPTION_printprefix) != 0) {
						ipv6addr.bit_start = 1;
						ipv6addr.bit_end = ipv6addr.prefixlength;
						ipv6addr.flag_startend_use = 1;
					} else if ((formatoptions & FORMATOPTION_printsuffix) != 0) {
						ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						ipv6addr.bit_end = 128;
						ipv6addr.flag_startend_use = 1;
					};
				} else {
					/* mixed */
					if ((formatoptions & FORMATOPTION_printprefix) != 0) {
						if ( ipv6addr.prefixlength < ipv6addr.bit_start ) {
							fprintf(stderr, " Error: prefix length '%u' lower than given start bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_start);
							exit(EXIT_FAILURE);
						} else if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%u' higher than given end bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_end);
							exit(EXIT_FAILURE);
						} else {
							ipv6addr.bit_end = ipv6addr.prefixlength;
						};
					} else if ((formatoptions & FORMATOPTION_printsuffix) != 0) {
						if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%u' higher than or eqal to given end bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_end);
							exit(EXIT_FAILURE);
						} else if (ipv6addr.prefixlength >= ipv6addr.bit_start) {
							fprintf(stderr, " Error: prefix length '%u' higher than or equal to given start bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_start);
							exit(EXIT_FAILURE);
						} else {
							ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						};
					};
				};
			};
		};

		/* check start/end */
		if ( ipv6addr.flag_startend_use == 1 ) {
			if ( ipv6addr.bit_start > ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit bigger than end bit\n");
				exit(EXIT_FAILURE);
			} else if ( ipv6addr.bit_start == ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit equal to end bit\n");
				exit(EXIT_FAILURE);
			};
		};
	};

	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: result of 'inputtype': %d\n", DEBUG_function_name, retval);
	};

	if (retval != 0) {
		fprintf(stderr, "%s\n", resultstring);
		exit(EXIT_FAILURE);
	};
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: End of postprocessing input\n", DEBUG_function_name);
	};
	
	/***** action *****/
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of action\n", DEBUG_function_name);
	};

	/***** automatic output handling *****/
	if ( outputtype == FORMAT_undefined ) {
		if ((formatoptions & FORMATOPTION_quiet) == 0) {
			fprintf(stderr, "No output type specified, try autodetection...");
		};

		if ( (inputtype == FORMAT_ipv4addr) && (action == ACTION_ipv4_to_6to4addr) ) {
			outputtype = FORMAT_ipv6addr;
		} else if ( (inputtype == FORMAT_mac) && (action == ACTION_mac_to_eui64) ) {
			outputtype = FORMAT_eui64;
			formatoptions |= FORMATOPTION_printfulluncompressed;
		} else if ( (inputtype == FORMAT_ipv6addr) && ( (action == ACTION_undefined) || (action == ACTION_anonymize) ) ) {
			outputtype = FORMAT_ipv6addr;
		} else if ( (inputtype == FORMAT_ipv4addr) && (action == ACTION_anonymize) ) {
			outputtype = FORMAT_ipv4addr;
		} else if ( ((inputtype == FORMAT_ipv4addr) || (inputtype == FORMAT_ipv4hex) || (inputtype == FORMAT_ipv4revhex)) && (action == ACTION_undefined) ) {
			outputtype = FORMAT_ipv4addr;
		} else if ( (inputtype == FORMAT_mac) ) {
			outputtype = FORMAT_mac;
		} else if ( (inputtype == FORMAT_bitstring) || (inputtype == FORMAT_base85) ) {
			outputtype = FORMAT_ipv6addr;
		} else if ( (inputtype == FORMAT_ipv4addr) && (action == ACTION_6rd_local_prefix) ) {
			outputtype = FORMAT_ipv6addr;
		};

		if ( outputtype != FORMAT_undefined ) {
			for (i = 0; i < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); i++) {
				if (outputtype == ipv6calc_formatstrings[i].number) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, "found type: %s\n", ipv6calc_formatstrings[i].token);
					};
					break;
				};
			};
		} else {
			if ((formatoptions & FORMATOPTION_quiet) == 0) {
				fprintf(stderr, "no result!\n");
			};
		};
	};

	if ( (outputtype & (FORMAT_revnibbles_int | FORMAT_revnibbles_arpa)) != 0 ) {
		/* workaround for reverse IPv4 */
		 if (ipv6addr.flag_valid == 0 && ipv4addr.flag_valid == 1) {
			outputtype = FORMAT_revipv4;
		};
	};

	/* clear resultstring */
	snprintf(resultstring, sizeof(resultstring) - 1, "%s", "");

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Process action (action: 0x%08lx)\n", DEBUG_function_name, (unsigned long) action);
	};
	
	switch (action) {
		case ACTION_mac_to_eui64:
			if (macaddr.flag_valid != 1) {
				fprintf(stderr, "No valid MAC address given!\n");
				exit(EXIT_FAILURE);
			};
			retval = create_eui64_from_mac(&ipv6addr, &macaddr);
			break;
			
		case ACTION_ipv4_to_6to4addr:
			if (inputtype == FORMAT_ipv4addr && outputtype == FORMAT_ipv6addr) {
				/* IPv4 -> IPv6 */
				if (ipv4addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv4 address given!\n");
					exit(EXIT_FAILURE);
				};
				retval = librfc3056_ipv4addr_to_ipv6to4addr(&ipv6addr, &ipv4addr);
			} else if (inputtype == FORMAT_ipv6addr && outputtype == FORMAT_ipv4addr) {
				/* IPv6 -> IPv4 */
				if (ipv6addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv6 address given!\n");
					exit(EXIT_FAILURE);
				};
				retval = librfc3056_ipv6addr_to_ipv4addr(&ipv4addr, &ipv6addr, resultstring);
			} else {
				fprintf(stderr, "Unsupported 6to4 conversion!\n");
				exit(EXIT_FAILURE);
			};
			break;

		case ACTION_anonymize:
			if (inputtype == FORMAT_ipv4addr && outputtype == FORMAT_ipv4addr) {
				/* anonymize IPv4 address */
				if (ipv4addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv4 address given!\n");
					exit(EXIT_FAILURE);
				};
				libipv4addr_anonymize(&ipv4addr, mask_ipv4);
			} else if (inputtype == FORMAT_ipv6addr && outputtype == FORMAT_ipv6addr) {
				/* anonymize IPv6 address */
				if (ipv6addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv6 address given!\n");
					exit(EXIT_FAILURE);
				};
				libipv6addr_anonymize(&ipv6addr, mask_iid, mask_ipv6, mask_ipv4);
			} else {
				fprintf(stderr, "Unsupported anonymization!\n");
				exit(EXIT_FAILURE);
			};
			break;
			
		case ACTION_iid_token_to_privacy:
			if (ipv6addr.flag_valid != 1 || ipv6addr2.flag_valid != 1) {
				fprintf(stderr, "No valid interface identifiers given!\n");
				exit(EXIT_FAILURE);
			};
			retval = librfc3041_calc(&ipv6addr, &ipv6addr2, &ipv6addr3, &ipv6addr4);
			ipv6addr_copy(&ipv6addr, &ipv6addr3);
			ipv6addr_copy(&ipv6addr2, &ipv6addr4);
			break;

		case ACTION_prefix_mac_to_ipv6:
			/* check IPv6 prefix */
			if ( ipv6addr.flag_valid != 1 ) {
				fprintf(stderr, "No valid IPv6 address given!\n");
				exit(EXIT_FAILURE);
			};

			/* check MAC */
			if (macaddr.flag_valid != 1) {
				fprintf(stderr, "No valid MAC address given!\n");
				exit(EXIT_FAILURE);
			};
			
			/* check for empty IID, otherwise display warning */
			for ( i = 8; i <= 15; i++ ) {
				if (ipv6addr.in6_addr.s6_addr[i] != 0) {
					fprintf(stderr, "Warning: given prefix is not 0 in 64 LSBs!\n");
					break;	
				};
			};
			
			/* convert MAC to IID */
			retval = create_eui64_from_mac(&ipv6addr2, &macaddr);

			/* put IID into address */
			for ( i = 8; i <= 15; i++ ) {
				ipv6addr.in6_addr.s6_addr[i] = ipv6addr2.in6_addr.s6_addr[i];
			};
			break;

		case ACTION_6rd_local_prefix:
			/* check formats */
			if ( outputtype != FORMAT_ipv6addr ) {
				fprintf(stderr, "output type incompatible (no IPv6 address)!\n");
				exit(EXIT_FAILURE);
			};
	
			if ( inputtype != FORMAT_ipv4addr ) {
				fprintf(stderr, "input type incompatible (no IPv4 address)!\n");
				exit(EXIT_FAILURE);
			};
	
			/* check IPv6 prefix */
			if ( ipv6addr.flag_valid != 1 ) {
				fprintf(stderr, "No valid IPv6 address given as 6rd prefix!\n");
				exit(EXIT_FAILURE);
			}

			if ( !ipv6addr.flag_prefixuse ) {
				fprintf(stderr, "Specified IPv6 address has no prefix length!\n");
				exit(EXIT_FAILURE);
			}

			/* check IPv4 local ip */
			if ( ipv4addr.flag_valid != 1 ) {
				fprintf(stderr, "No valid IPv4 address given as local IP!\n");
				exit(EXIT_FAILURE);
			}
		
			if ( ipv4addr.flag_prefixuse ) {
				fprintf(stderr, "No prefix length should be specified for the local IP!\n");
				exit(EXIT_FAILURE);
			}

			/* check IPv4 relay prefix */
			if ( ipv4addr2.flag_valid != 1 ) {
				fprintf(stderr, "No valid IPv4 address given as 6rd relay prefix!\n");
				exit(EXIT_FAILURE);
			}

			if ( !ipv4addr2.flag_prefixuse ) {
				fprintf(stderr, "Specified relay prefix IPv4 address has no prefix length!\n");
				exit(EXIT_FAILURE);
			}

			retval = librfc5569_calc_6rd_local_prefix(&ipv6addr, &ipv4addr2, &ipv4addr, resultstring);
			break;

		case ACTION_undefined:
			/* no action selected */
			break;

		default:
			fprintf(stderr, " Action-type isn't implemented\n");
			exit(EXIT_FAILURE);
			/* break; */
	};

	if (retval != 0) {
		fprintf(stderr, "Problem occurs during action: %s\n", resultstring);
		exit(EXIT_FAILURE);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: End of action\n", DEBUG_function_name);
	};

	/***** output type *****/
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Process output (outputtype: 0x%08lx)\n", DEBUG_function_name, (unsigned long) outputtype);
	};
	
	/* catch showinfo */	
	if (command == CMD_showinfo) {
		if (ipv6addr.flag_valid == 1) {
			retval = showinfo_ipv6addr(&ipv6addr, formatoptions);
	       	} else if (ipv4addr.flag_valid == 1) {
			retval = showinfo_ipv4addr(&ipv4addr, formatoptions);
	       	} else if (macaddr.flag_valid == 1) {
			retval = showinfo_eui48(&macaddr, formatoptions);
		} else {
		       	fprintf(stderr, "No valid or supported input address given!\n");
			retval = 1;
		};
		if (retval != 0) {
			fprintf(stderr, "Problem occurs during selection of showinfo\n");
			exit(EXIT_FAILURE);
		};
		goto RESULT_print;
	};

	switch (outputtype) {
		case -1:
			/* old implementation */
			break;

		case FORMAT_base85:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_base85\n", DEBUG_function_name); }
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = ipv6addrstruct_to_base85(&ipv6addr, resultstring);
			break;
				
		case FORMAT_bitstring:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_bitstring\n", DEBUG_function_name); }
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Call 'librfc2874_addr_to_bitstring'\n", DEBUG_function_name);
			};
			retval = librfc2874_addr_to_bitstring(&ipv6addr, resultstring, formatoptions);
			break;
			
		case FORMAT_octal:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_octal\n", DEBUG_function_name); }
			if (ipv4addr.flag_valid == 1) {
				retval = libipv4addr_to_octal(&ipv4addr, resultstring, formatoptions);
			} else if (ipv6addr.flag_valid == 1) {
				retval = libipv6addr_to_octal(&ipv6addr, resultstring, formatoptions);
			};
			break;
				
		case FORMAT_hex:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_hex\n", DEBUG_function_name); }
			if (ipv4addr.flag_valid == 1) {
				retval = libipv4addr_to_hex(&ipv4addr, resultstring, formatoptions);
			} else if (ipv6addr.flag_valid == 1) {
				retval = libipv6addr_to_hex(&ipv6addr, resultstring, formatoptions);
			};
			break;
				
		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_revnibbles_int/arpa\n", DEBUG_function_name); }
			if (ipv6addr.flag_valid == 1) {
				switch (outputtype) {
					case FORMAT_revnibbles_int:
						retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, formatoptions, "ip6.int.");
						break;
					case FORMAT_revnibbles_arpa:
						retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, formatoptions, "ip6.arpa.");
						break;
				};
			} else {
			       fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE);
			};
			break;
				
		case FORMAT_revipv4:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_revipv4\n", DEBUG_function_name); }
			if (ipv4addr.flag_valid == 1) {
				retval = libipv4addr_to_reversestring(&ipv4addr, resultstring, formatoptions);
			} else {
			       fprintf(stderr, "No valid IPv4 address given!\n"); exit(EXIT_FAILURE);
		       	};
			break;
			
		case FORMAT_ifinet6:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_ifinet6\n", DEBUG_function_name); }
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = libifinet6_ipv6addrstruct_to_ifinet6(&ipv6addr, resultstring);
			break;

		case FORMAT_ipv6addr:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_ipv6addr\n", DEBUG_function_name); }
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if ((formatoptions & (FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed | FORMATOPTION_printprefix | FORMATOPTION_printsuffix)) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, formatoptions);
			};
			break;

		case FORMAT_ipv6literal:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_ipv6literal\n", DEBUG_function_name); }
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if ((formatoptions & (FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed)) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions | FORMATOPTION_literal);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, formatoptions | FORMATOPTION_literal);
			};
			break;
			
			
		case FORMAT_ipv4addr:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_ipv4addr\n", DEBUG_function_name); }
			if (ipv4addr.flag_valid != 1) { fprintf(stderr, "No valid IPv4 address given!\n"); exit(EXIT_FAILURE); };
			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, formatoptions);
			break;
			
		case FORMAT_eui64:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_eui64\n", DEBUG_function_name); }
			if (action != ACTION_mac_to_eui64) { fprintf(stderr, "Specify proper action or input for output format: eui64\n"); exit(EXIT_FAILURE); };
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid EUI-64 identifier given!\n"); exit(EXIT_FAILURE); };
			if (ipv6addr.prefixlength != 64) { fprintf(stderr, "No valid EUI-64 identifier given!\n"); exit(EXIT_FAILURE); };
			formatoptions |= FORMATOPTION_printsuffix;
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			break;
			
		case FORMAT_mac:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_mac\n", DEBUG_function_name); }
			if (macaddr.flag_valid != 1) { fprintf(stderr, "No valid MAC address given!\n"); exit(EXIT_FAILURE); };
			retval = macaddrstruct_to_string(&macaddr, resultstring, formatoptions);
			break;
			
		case FORMAT_iid_token:
			if (ipv6calc_debug != 0) { fprintf(stderr, "%s: Start of output handling for FORMAT_iid_token\n", DEBUG_function_name); }
			if (ipv6addr.flag_valid != 1 || ipv6addr2.flag_valid != 1) { fprintf(stderr, "No valid IPv6 addresses given!\n"); exit(EXIT_FAILURE); };
			/* get interface identifier */
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring2, formatoptions | FORMATOPTION_printsuffix);
			if (retval != 0) { break; };
			
			/* get token */
			retval = libipv6addr_ipv6addrstruct_to_tokenlsb64(&ipv6addr2, resultstring3, formatoptions);
			
			/* cat together */
			snprintf(resultstring, sizeof(resultstring) - 1, "%s %s", resultstring2, resultstring3);
			break;


		default:
			fprintf(stderr, " Outputtype isn't implemented: %8lx\n", (unsigned long) outputtype);
			exit(EXIT_FAILURE);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: End of output handling\n", DEBUG_function_name);
	};

RESULT_print:
	/* print result */
	if ( strlen(resultstring) > 0 ) {
		if ( retval == 0 ) {
			fprintf(stdout, "%s\n", resultstring);
		} else {
			fprintf(stderr, "%s\n", resultstring);
		};
	};

	if (input_is_pipe == 1) {
		if (flush_mode == 1) {
			fflush(stdout);
		};
		goto PIPE_input;
	};

	exit(retval);
};
#undef DEBUG_function_name
