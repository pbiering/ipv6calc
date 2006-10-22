/*
 * Project    : ipv6calc
 * File       : ipv6calc.c
 * Version    : $Id: ipv6calc.c,v 1.26 2006/10/22 10:59:19 peter Exp $
 * Copyright  : 2001-2006 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Central program (main)
 *  This program print out different formats of an given IP(v4/v6) address
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 

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
#include "libeui64.h"
#include "config.h"

long int ipv6calc_debug = 0;

#ifdef SUPPORT_IP2LOCATION
int  use_ip2location_ipv4 = 0; /* if set to 1, IP2Location IPv4 is enabled by option(s) */
int  use_ip2location_ipv6 = 0; /* if set to 1, IP2Location IPv6 is enabled by option(s) */
char file_ip2location_ipv4[NI_MAXHOST] = "";
char file_ip2location_ipv6[NI_MAXHOST] = "";
#endif

#ifdef SUPPORT_GEOIP
int  use_geoip = 0; /* if set to 1, GeoIP is enabled by option(s) */
char file_geoip[NI_MAXHOST] = "";
#endif

void printversion(void) {
	fprintf(stderr, "%s: version %s\n", PROGRAM_NAME, PACKAGE_VERSION);
};

void printcopyright(void) {
	fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program formats and calculates IPv6 addresses\n");
	fprintf(stderr, "See '%s -?|-h|--help' for more details\n\n", PROGRAM_NAME);
};


/**************************************************/
/* main */
#define DEBUG_function_name "ipv6calc/main"
int main(int argc,char *argv[]) {
	char resultstring[NI_MAXHOST] = "";
	char resultstring2[NI_MAXHOST] = "";
	char resultstring3[NI_MAXHOST] = "";
	int retval = 1, i, j, lop;
	unsigned long int command = 0;
	int bit_start = 0, bit_end = 0;

	/* new option style storage */	
	uint32_t inputtype  = FORMAT_undefined;
	uint32_t outputtype = FORMAT_undefined;
	uint32_t action     = ACTION_undefined;

	/* format options storage */
	uint32_t formatoptions = 0;

	/* used structures */
	ipv6calc_ipv6addr ipv6addr, ipv6addr2, ipv6addr3, ipv6addr4;
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_macaddr  macaddr;

	/* clear address structures */
	ipv6addr_clearall(&ipv6addr);
	ipv6addr_clearall(&ipv6addr2);
	ipv6addr_clearall(&ipv6addr3);
	ipv6addr_clearall(&ipv6addr4);
	ipv4addr_clearall(&ipv4addr);
	mac_clearall(&macaddr);

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
				command |= CMD_printversion;
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
				fprintf(stderr, " Support for option '--db-ip2location-ipv4 <IP2Location IPv4 database file>' not compiled in, IP2Location support disabled\n");
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
				fprintf(stderr, " Support for option '--db-ip2location-ipv6 <IP2Location IPv6 database file>' not compiled in, IP2Location support disabled\n");
#endif
				break;

			case DB_geoip:
#ifdef SUPPORT_GEOIP
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got GeoIP database file: %s\n", DEBUG_function_name, optarg);
				};
				strncpy(file_geoip, optarg, sizeof(file_geoip) -1 );
				use_geoip = 1;
#else
				fprintf(stderr, " Support for option '--db-geoip <GeoIP database file>' not compiled in, GeoIP support disabled\n");
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
				
			case FORMATOPTION_NUM_printstart + FORMATOPTION_NUM_HEAD:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_start = atoi(optarg);
					formatoptions |= FORMATOPTION_printstart;
				} else {
					fprintf(stderr, " Argument of option 'printstart' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case FORMATOPTION_NUM_printend + FORMATOPTION_NUM_HEAD:
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
				break;
				
			default:
				fprintf(stderr, "Usage: (see '%s --command -?|-h|--help' for more help)\n", PROGRAM_NAME);
				printhelp();
				break;
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
			printhelp_outputtypes(inputtype);
			exit(EXIT_FAILURE);
		} else if (inputtype == FORMAT_auto) {
			printhelp_inputtypes();
			exit(EXIT_FAILURE);
		} else if (action == ACTION_auto) {
			printhelp_actiontypes();
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
		fprintf(stderr, "Debug value:%lx  command:%lx  inputtype:%lx   outputtype:%lx  action:%lx  formatoptions:%x\n", (unsigned long) ipv6calc_debug, command, (unsigned long) inputtype, (unsigned long) outputtype, (unsigned long) action, (unsigned) formatoptions); 
	};
	
	/* do work depending on selection */
	if (command == CMD_printversion) {
		printversion();
		exit(EXIT_FAILURE);
	};

	if (command == CMD_printoldoptions) {
		printhelp_oldoptions();
		exit(EXIT_FAILURE);
	};
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, argv[0]);
	};

	/***** automatic action handling *****/
	if ( action == ACTION_undefined ) {
		/*
		if ((formatoptions & FORMATOPTION_quiet) == 0) {
			fprintf(stderr, "No action type specified, try autodetection...");
		};
		*/

		if ( (inputtype == FORMAT_mac) && (outputtype ==FORMAT_eui64) ) {
			action = ACTION_mac_to_eui64;
		} else if ( (inputtype == FORMAT_iid_token) && (outputtype ==FORMAT_iid_token) ) {
			action = ACTION_iid_token_to_privacy;
		} else if ( inputtype == FORMAT_prefix_mac ) {
			action = ACTION_prefix_mac_to_ipv6;
			if ( outputtype == FORMAT_undefined ) {
				outputtype = FORMAT_ipv6addr;
			};
		};
		
		if ( action != FORMAT_undefined ) {
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
		} else {
			/*
			if ((formatoptions & FORMATOPTION_quiet) == 0) {
				fprintf(stderr, "no result!\n");
			};
			*/
		};
	};

	/***** input type handling *****/
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of input type handling\n", DEBUG_function_name);
	};

	/* check length of input */
	if (argc > 0) {
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
	
	/* autodetection */
	if ((inputtype == FORMAT_undefined || inputtype == FORMAT_auto) && argc > 0) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Call input type autodetection\n", DEBUG_function_name);
		};
		/* no input type specified or automatic selected */
		if ((formatoptions & FORMATOPTION_quiet) == 0) {
			fprintf(stderr, "No input type specified, try autodetection...");
		};
		
		inputtype = libipv6calc_autodetectinput(argv[0]);

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

	/* check formatoptions for validity */
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

	switch (inputtype) {
		case FORMAT_ipv6addr:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = addr_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			break;

		case FORMAT_ipv4addr:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = addr_to_ipv4addrstruct(argv[0], resultstring, &ipv4addr);
			argc--;
			break;
			
		case FORMAT_ipv4hex:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = addrhex_to_ipv4addrstruct(argv[0], resultstring, &ipv4addr, 0);
			argc--;
			break;

		case FORMAT_ipv4revhex:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = addrhex_to_ipv4addrstruct(argv[0], resultstring, &ipv4addr, 1);
			argc--;
			break;

		case FORMAT_base85:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = base85_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			break;
			
		case FORMAT_mac:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = mac_to_macaddrstruct(argv[0], resultstring, &macaddr);
			argc--;
			break;

		case FORMAT_ifinet6:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			if (argc < 2) {
				retval = libifinet6_ifinet6_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
				argc--;
			} else {
				retval = libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(argv[0], argv[1], resultstring, &ipv6addr);
				argc -= 2;
			};
			break;

		case FORMAT_iid_token:
			/* Get first interface identifier */
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = identifier_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			if (retval != 0) { break; };
			
			/* Get second token */
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = tokenlsb64_to_ipv6addrstruct(argv[1], resultstring, &ipv6addr2);
			argc--;
			break;
			
		case FORMAT_prefix_mac:
			/* Get first: IPv6 prefix */
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = addr_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			if (retval != 0) { break; };
			
			/* Get second: MAC address */
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = mac_to_macaddrstruct(argv[1], resultstring, &macaddr);
			argc--;
			break;

		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = librfc1886_nibblestring_to_ipv6addrstruct(argv[0], &ipv6addr, resultstring);
			argc--;
			break;
			
		case FORMAT_bitstring:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = librfc2874_bitstring_to_ipv6addrstruct(argv[0], &ipv6addr, resultstring);
			argc--;
			break;
			
		default:
			fprintf(stderr, " Input-type isn't implemented\n");
			exit(EXIT_FAILURE);
	};

	if (retval != 0) {
		fprintf(stderr, "%s\n", resultstring);
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
		} else if ( (inputtype == FORMAT_ipv6addr) && (action == ACTION_undefined) ) {
			outputtype = FORMAT_ipv6addr;
		} else if ( ((inputtype == FORMAT_ipv4addr) || (inputtype == FORMAT_ipv4hex) || (inputtype == FORMAT_ipv4revhex)) && (action == ACTION_undefined) ) {
			outputtype = FORMAT_ipv4addr;
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
		if (ipv4addr.flag_valid == 1) {
			outputtype = FORMAT_revipv4;
		};
	};

	
	/* clear resultstring */
	snprintf(resultstring, sizeof(resultstring) - 1, "%s", "");
	
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
		fprintf(stderr, "%s: Start of output handling\n", DEBUG_function_name);
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
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = ipv6addrstruct_to_base85(&ipv6addr, resultstring);
			break;
				
		case FORMAT_bitstring:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Call 'librfc2874_addr_to_bitstring'\n", DEBUG_function_name);
			};
			retval = librfc2874_addr_to_bitstring(&ipv6addr, resultstring, formatoptions);
			break;
			
		case FORMAT_octal:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = libipv6addr_to_octal(&ipv6addr, resultstring, formatoptions);
			break;
				
		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
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
			if (ipv4addr.flag_valid == 1) {
				retval = libipv4addr_to_reversestring(&ipv4addr, resultstring, formatoptions);
			} else {
			       fprintf(stderr, "No valid IPv4 address given!\n"); exit(EXIT_FAILURE);
		       	};
			break;
			
		case FORMAT_ifinet6:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = libifinet6_ipv6addrstruct_to_ifinet6(&ipv6addr, resultstring);
			break;

		case FORMAT_ipv6addr:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if ((formatoptions & (FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed | FORMATOPTION_printprefix | FORMATOPTION_printsuffix)) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, formatoptions);
			};
			break;
			
		case FORMAT_ipv4addr:
			if (ipv4addr.flag_valid != 1) { fprintf(stderr, "No valid IPv4 address given!\n"); exit(EXIT_FAILURE); };
			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, formatoptions);
			break;
			
		case FORMAT_eui64:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			formatoptions |= FORMATOPTION_printsuffix;
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			break;
			
		case FORMAT_iid_token:
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
			fprintf(stderr, " Outputtype isn't implemented\n");
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
	exit(retval);
};
#undef DEBUG_function_name
