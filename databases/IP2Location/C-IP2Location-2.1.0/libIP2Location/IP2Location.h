/* IP2Location.h
 *
 * Copyright (C) 2005-2006 IP2Location.com  All Rights Reserved.
 *
 * http://www.ip2location.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contains BIGDIGITS multiple-precision arithmetic code originally
 * written by David Ireland, copyright (c) 2001-6 by D.I. Management
 * Services Pty Limited <www.di-mgt.com.au>, and is used with
 * permission."
 */
#ifndef HAVE_IP2LOCATION_H
#define HAVE_IP2LOCATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h> 
#include <malloc.h>
#ifdef _SUN_
	#include <stdint.h>
#else
	#define uint8_t unsigned char
#endif
#include <bigd.h>

#define API_VERSION   2.0.0
#define MAX_IPV4_RANGE  4294967295U
#define MAX_IPV6_RANGE  "340282366920938463463374607431768211455"
#define IPV4 0
#define IPV6 1

#define  COUNTRYSHORT 0x0001
#define  COUNTRYLONG  0x0002
#define  REGION       0x0004
#define  CITY         0x0008
#define  ISP          0x0010
#define  LATITUDE     0x0020
#define  LONGITUDE    0x0040
#define  DOMAIN       0x0080
#define  ZIPCODE      0x0100
#define  TIMEZONE     0x0200
#define  NETSPEED     0x0400
#define  ALL          COUNTRYSHORT | COUNTRYLONG | REGION | CITY | ISP | LATITUDE | LONGITUDE | DOMAIN | ZIPCODE | TIMEZONE | NETSPEED

#define  DEFAULT	     0x0001
#define  NO_EMPTY_STRING 0x0002
#define  NO_LEADING      0x0004
#define  NO_TRAILING     0x0008

#define INVALID_IPV6_ADDRESS "INVALID IPV6 ADDRESS"
#define INVALID_IPV4_ADDRESS "INVALID IPV4 ADDRESS"
#define  NOT_SUPPORTED "This parameter is unavailable for selected data file. Please upgrade the data file."


typedef struct
{
	FILE *filehandle;
	uint8_t databasetype;
	uint8_t databasecolumn;
	uint8_t databaseday;
	uint8_t databasemonth;
	uint8_t databaseyear;
	uint32_t databasecount;
	uint32_t databaseaddr;
	uint32_t ipversion;
} IP2Location;

typedef struct
{
	char *country_short;
	char *country_long;
	char *region;
	char *city;
	char *isp;
	float latitude;
	float longitude;
	char *domain;
	char *zipcode;
	char *timezone;
	char *netspeed;
} IP2LocationRecord;

struct BIGD
{
	unsigned long *digits;	/* Ptr to array of digits, least sig. first */
	size_t ndigits;		/* No of non-zero significant digits */
	size_t maxdigits;	/* Max size allocated */
};

typedef struct StringList{
	char* data;
	struct StringList* next;
} StringList;


/*##################
# Public Functions
##################*/
IP2Location *IP2Location_open(char *db);
uint32_t IP2Location_close(IP2Location *loc);
IP2LocationRecord *IP2Location_get_country_short(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_country_long(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_region(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_city (IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_isp(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_latitude(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_longitude(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_domain(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_zipcode(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_timezone(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_netspeed(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_all(IP2Location *loc, char *ip);
void IP2Location_free_record(IP2LocationRecord *record);

/*###################
# Private Functions
###################*/

int IP2Location_initialize(IP2Location *loc);
IP2LocationRecord *IP2Location_new_record();
char* IP2Location_read128(FILE *handle, uint32_t position);
uint32_t IP2Location_read32(FILE *handle, uint32_t position);
uint8_t IP2Location_read8(FILE *handle, uint32_t position);
char *IP2Location_readStr(FILE *handle, uint32_t position);
float IP2Location_readFloat(FILE *handle, uint32_t position);
uint32_t IP2Location_ip2no(char* ip);
BIGD IP2Location_ipv6_to_no(char* ip);
int IP2Location_ip_is_ipv4 (char* ipaddr);
int IP2Location_ip_is_ipv6 (char* ipaddr);
IP2LocationRecord *IP2Location_get_record(IP2Location *loc, char *ip, uint32_t mode);
IP2LocationRecord *IP2Location_get_ipv6_record(IP2Location *loc, char *ipstring, uint32_t mode);
char* IP2Location_bd2string (BIGD bd);
StringList* IP2Location_split(char* delimiters, char* targetString, unsigned int flags, int limit);
char* IP2Location_replace(char* substr, char* replace, char* targetString);
unsigned int IP2Location_substr_count(char* substr, char* targetString);
unsigned int StringListCount (StringList* toCount);
void FreeStringList (StringList* toFree);

#ifdef __cplusplus
}
#endif

#endif
