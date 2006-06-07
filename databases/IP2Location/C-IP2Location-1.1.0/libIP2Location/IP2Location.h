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
 */
#ifndef HAVE_IP2LOCATION_H
#define HAVE_IP2LOCATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#define API_VERSION   1.1.0
#define MAX_IP_RANGE  4294967295U

#define  COUNTRYSHORT 0x0001
#define  COUNTRYLONG  0x0002
#define  REGION       0x0004
#define  CITY         0x0008
#define  ISP          0x0010
#define  LATITUDE     0x0020
#define  LONGITUDE    0x0040
#define  DOMAIN       0x0080
#define  ZIPCODE      0x0100
#define  ALL          COUNTRYSHORT | COUNTRYLONG | REGION | CITY | ISP | LATITUDE | LONGITUDE | DOMAIN | ZIPCODE
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
} IP2LocationRecord;

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
IP2LocationRecord *IP2Location_get_all(IP2Location *loc, char *ip);
IP2LocationRecord *IP2Location_get_record(IP2Location *loc, char *ip, uint32_t mode);
void IP2Location_free_record(IP2LocationRecord *record);

/*###################
# Private Functions
###################*/
IP2LocationRecord *IP2Location_new_record();
uint32_t IP2Location_read32(FILE *handle, uint32_t position);
uint8_t IP2Location_read8(FILE *handle, uint32_t position);
char *IP2Location_readStr(FILE *handle, uint32_t position);
float IP2Location_readFloat(FILE *handle, uint32_t position);
uint32_t IP2Location_ip2no(char* ip);

#ifdef __cplusplus
}
#endif

#endif
