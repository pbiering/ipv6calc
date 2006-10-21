/* IP2Location.c
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
 
/*#ifdef _SUN_
	#include <stdint.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#else
	#include <winsock2.h>
#endif

#include <stdio.h>
#include <string.h>*/

 
#ifndef _SUN_
#include <stdint.h>
#include <string.h>
#endif
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "bigd.h"
#include "IP2Location.h"

uint8_t COUNTRY_POSITION[15]   = {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
uint8_t REGION_POSITION[15]    = {0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
uint8_t CITY_POSITION[15]      = {0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
uint8_t ISP_POSITION[15]       = {0, 0, 3, 0, 5, 0, 7, 5, 7, 0, 8, 0, 9, 0, 9};
uint8_t LATITUDE_POSITION[15]  = {0, 0, 0, 0, 0, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5};
uint8_t LONGITUDE_POSITION[15] = {0, 0, 0, 0, 0, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6};
uint8_t DOMAIN_POSITION[15]    = {0, 0, 0, 0, 0, 0, 0, 6, 8, 0, 9, 0, 10,0, 10};
uint8_t ZIPCODE_POSITION[15]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 0, 7};
uint8_t TIMEZONE_POSITION[15]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 7, 8};
uint8_t NETSPEED_POSITION[15]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 11};

IP2Location *IP2Location_open(char *db)
{
	FILE *f;
	IP2Location *loc;

	if ((f=fopen(db,"rb")) == NULL)
	{
		printf("IP2Location library error in opening database %s.\n", db);
		return NULL;
	}

	loc = (IP2Location *) malloc(sizeof(IP2Location));
	memset(loc, 0, sizeof(IP2Location));

	loc->filehandle = f;

	IP2Location_initialize(loc);
	
	return loc;
}


uint32_t IP2Location_close(IP2Location *loc)
{
	if (loc->filehandle != NULL) {
		fclose(loc->filehandle);
	}
	if (loc != NULL) {
		free(loc);
	}
	return 0;
}


int IP2Location_initialize(IP2Location *loc)
{
	loc->databasetype   = IP2Location_read8(loc->filehandle, 1);
	loc->databasecolumn = IP2Location_read8(loc->filehandle, 2);
	loc->databaseyear    = IP2Location_read8(loc->filehandle, 3);
	loc->databasemonth  = IP2Location_read8(loc->filehandle, 4);
	loc->databaseday   = IP2Location_read8(loc->filehandle, 5);
	loc->databasecount  = IP2Location_read32(loc->filehandle, 6);
	loc->databaseaddr   = IP2Location_read32(loc->filehandle, 10);
	loc->ipversion      = IP2Location_read32(loc->filehandle, 14);
	return 0;
}


IP2LocationRecord *IP2Location_get_country_short(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, COUNTRYSHORT);
	} else {
		return IP2Location_get_record(loc, ip, COUNTRYSHORT);
	}
}


IP2LocationRecord *IP2Location_get_country_long(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, COUNTRYLONG);
	} else {
		return IP2Location_get_record(loc, ip, COUNTRYLONG);
	}
}


IP2LocationRecord *IP2Location_get_region(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, REGION);
	} else {
		return IP2Location_get_record(loc, ip, REGION);
	}
}


IP2LocationRecord *IP2Location_get_city (IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, CITY);
	} else {
		return IP2Location_get_record(loc, ip, CITY);
	}
}


IP2LocationRecord *IP2Location_get_isp(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, ISP);
	} else {
		return IP2Location_get_record(loc, ip, ISP);
	}
}


IP2LocationRecord *IP2Location_get_latitude(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, LATITUDE);
	} else {
		return IP2Location_get_record(loc, ip, LATITUDE);
	}
}


IP2LocationRecord *IP2Location_get_longitude(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, LONGITUDE);
	} else {
		return IP2Location_get_record(loc, ip, LONGITUDE);
	}
}


IP2LocationRecord *IP2Location_get_domain(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, DOMAIN);
	} else {
		return IP2Location_get_record(loc, ip, DOMAIN);
	}
}


IP2LocationRecord *IP2Location_get_zipcode(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, ZIPCODE);
	} else {
		return IP2Location_get_record(loc, ip, ZIPCODE);
	}
}

IP2LocationRecord *IP2Location_get_timezone(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, TIMEZONE);
	} else {
		return IP2Location_get_record(loc, ip, TIMEZONE);
	}
}

IP2LocationRecord *IP2Location_get_netspeed(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, NETSPEED);
	} else {
		return IP2Location_get_record(loc, ip, NETSPEED);
	}
}

IP2LocationRecord *IP2Location_get_all(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, ALL);
	} else {
		return IP2Location_get_record(loc, ip, ALL);
	}
}

IP2LocationRecord *IP2Location_get_ipv6_record(IP2Location *loc, char *ipstring, uint32_t mode)
{
	uint8_t dbtype = loc->databasetype;
	BIGD ipno = IP2Location_ipv6_to_no(ipstring);
	FILE *handle = loc->filehandle;
	uint32_t baseaddr = loc->databaseaddr;
	uint32_t dbcount = loc->databasecount;
	uint32_t dbcolumn = loc->databasecolumn;

	uint32_t low = 0;
	uint32_t high = dbcount;
	uint32_t mid = 0;
	BIGD ipfrom = bdNew();
	BIGD ipto = bdNew();
	
	IP2LocationRecord *record = IP2Location_new_record();
	if(IP2Location_ip_is_ipv6(ipstring) == 0 ) {
		record->country_short = strdup(INVALID_IPV6_ADDRESS);
		record->country_long = strdup(INVALID_IPV6_ADDRESS);
		record->region = strdup(INVALID_IPV6_ADDRESS);
		record->city = strdup(INVALID_IPV6_ADDRESS);
		record->isp = strdup(INVALID_IPV6_ADDRESS);
		record->latitude = 0;
		record->longitude = 0;
		record->domain = strdup(INVALID_IPV6_ADDRESS);
		record->zipcode = strdup(INVALID_IPV6_ADDRESS);
		record->timezone = strdup(INVALID_IPV6_ADDRESS);
		record->netspeed = strdup(INVALID_IPV6_ADDRESS);
		return record;
	}

	while (low <= high) 
	{
		mid = (uint32_t)((low + high)/2);
		bdConvFromDecimal(ipfrom, IP2Location_read128(handle, baseaddr + mid * (dbcolumn * 4 + 12))); 
		bdConvFromDecimal(ipto, IP2Location_read128(handle, baseaddr + (mid + 1) * (dbcolumn * 4 + 12)));
		
		if( (bdCompare(ipno, ipfrom) >= 0) && (bdCompare(ipno, ipto) < 0))
		{
			if ((mode & COUNTRYSHORT) && (COUNTRY_POSITION[dbtype] != 0)) {
				/* $ip, $baseaddr + $mid *($dbcolumn * 4 + 12) + 12 + 4 * */
				record->country_short = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (COUNTRY_POSITION[dbtype]-1)));
			} else {
				record->country_short = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & COUNTRYLONG) && (COUNTRY_POSITION[dbtype] != 0)) {
				record->country_long = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (COUNTRY_POSITION[dbtype]-1))+3);
			} else {
				record->country_long = strdup(NOT_SUPPORTED);
			}

			if ((mode & REGION) && (REGION_POSITION[dbtype] != 0)) {
				record->region = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (REGION_POSITION[dbtype]-1)));
			} else {
				record->region = strdup(NOT_SUPPORTED);
			}

			if ((mode & CITY) && (CITY_POSITION[dbtype] != 0)) {
				record->city = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (CITY_POSITION[dbtype]-1)));
			} else {
				record->city = strdup(NOT_SUPPORTED);
			}

			if ((mode & ISP) && (ISP_POSITION[dbtype] != 0)) {
				record->isp = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (ISP_POSITION[dbtype]-1)));
			} else {
				record->isp = strdup(NOT_SUPPORTED);
			}

			if ((mode & LATITUDE) && (LATITUDE_POSITION[dbtype] != 0)) {
				record->latitude = IP2Location_readFloat(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (LATITUDE_POSITION[dbtype]-1));
			} else {
				record->latitude = 0.0;
			}

			if ((mode & LONGITUDE) && (LONGITUDE_POSITION[dbtype] != 0)) {
				record->longitude = IP2Location_readFloat(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (LONGITUDE_POSITION[dbtype]-1));
			} else {
				record->longitude = 0.0;
			}

			if ((mode & DOMAIN) && (DOMAIN_POSITION[dbtype] != 0)) {
				record->domain = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (DOMAIN_POSITION[dbtype]-1)));
			} else {
				record->domain = strdup(NOT_SUPPORTED);
			}

			if ((mode & ZIPCODE) && (ZIPCODE_POSITION[dbtype] != 0)) {
				record->zipcode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (ZIPCODE_POSITION[dbtype]-1)));
			} else {
				record->zipcode = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & TIMEZONE) && (TIMEZONE_POSITION[dbtype] != 0)) {
				record->timezone = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (TIMEZONE_POSITION[dbtype]-1)));
			} else {
				record->timezone = strdup(NOT_SUPPORTED);
			}

			if ((mode & NETSPEED) && (NETSPEED_POSITION[dbtype] != 0)) {
				record->netspeed = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (NETSPEED_POSITION[dbtype]-1)));
			} else {
				record->netspeed = strdup(NOT_SUPPORTED);
			}
	
			return record;
		} else {
			if ( bdCompare(ipno, ipfrom) < 0 ) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}
	}
	IP2Location_free_record(record);
	return NULL;
}

IP2LocationRecord *IP2Location_get_record(IP2Location *loc, char *ipstring, uint32_t mode)
{
	uint8_t dbtype = loc->databasetype;
	uint32_t ipno = IP2Location_ip2no(ipstring);
	FILE *handle = loc->filehandle;
	uint32_t baseaddr = loc->databaseaddr;
	uint32_t dbcount = loc->databasecount;
	uint32_t dbcolumn = loc->databasecolumn;

	uint32_t low = 0;
	uint32_t high = dbcount;
	uint32_t mid = 0;
	uint32_t ipfrom = 0;
	uint32_t ipto = 0;
	
	IP2LocationRecord *record = IP2Location_new_record();
	if (ipno == (uint32_t) MAX_IPV4_RANGE) {
		ipno = ipno - 1;
	}
	
	if(IP2Location_ip_is_ipv4(ipstring) == 0 ) {
		record->country_short = strdup(INVALID_IPV4_ADDRESS);
		record->country_long = strdup(INVALID_IPV4_ADDRESS);
		record->region = strdup(INVALID_IPV4_ADDRESS);
		record->city = strdup(INVALID_IPV4_ADDRESS);
		record->isp = strdup(INVALID_IPV4_ADDRESS);
		record->latitude = 0;
		record->longitude = 0;
		record->domain = strdup(INVALID_IPV4_ADDRESS);
		record->zipcode = strdup(INVALID_IPV4_ADDRESS);
		record->timezone = strdup(INVALID_IPV4_ADDRESS);
		record->netspeed = strdup(INVALID_IPV4_ADDRESS);
		return record;
	}

	while (low <= high) 
	{
		mid = (uint32_t)((low + high)/2);
		ipfrom = IP2Location_read32(handle, baseaddr + mid * dbcolumn * 4);
		ipto 	= IP2Location_read32(handle, baseaddr + (mid + 1) * dbcolumn * 4);

		if ((ipno >= ipfrom) && (ipno < ipto)) 
		{
			if ((mode & COUNTRYSHORT) && (COUNTRY_POSITION[dbtype] != 0)) {
				record->country_short = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (COUNTRY_POSITION[dbtype]-1)));
			} else {
				record->country_short = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & COUNTRYLONG) && (COUNTRY_POSITION[dbtype] != 0)) {
				record->country_long = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (COUNTRY_POSITION[dbtype]-1))+3);
			} else {
				record->country_long = strdup(NOT_SUPPORTED);
			}

			if ((mode & REGION) && (REGION_POSITION[dbtype] != 0)) {
				record->region = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (REGION_POSITION[dbtype]-1)));
			} else {
				record->region = strdup(NOT_SUPPORTED);
			}

			if ((mode & CITY) && (CITY_POSITION[dbtype] != 0)) {
				record->city = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (CITY_POSITION[dbtype]-1)));
			} else {
				record->city = strdup(NOT_SUPPORTED);
			}

			if ((mode & ISP) && (ISP_POSITION[dbtype] != 0)) {
				record->isp = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (ISP_POSITION[dbtype]-1)));
			} else {
				record->isp = strdup(NOT_SUPPORTED);
			}

			if ((mode & LATITUDE) && (LATITUDE_POSITION[dbtype] != 0)) {
				record->latitude = IP2Location_readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (LATITUDE_POSITION[dbtype]-1));
			} else {
				record->latitude = 0.0;
			}

			if ((mode & LONGITUDE) && (LONGITUDE_POSITION[dbtype] != 0)) {
				record->longitude = IP2Location_readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (LONGITUDE_POSITION[dbtype]-1));
			} else {
				record->longitude = 0.0;
			}

			if ((mode & DOMAIN) && (DOMAIN_POSITION[dbtype] != 0)) {
				record->domain = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (DOMAIN_POSITION[dbtype]-1)));
			} else {
				record->domain = strdup(NOT_SUPPORTED);
			}

			if ((mode & ZIPCODE) && (ZIPCODE_POSITION[dbtype] != 0)) {
				record->zipcode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (ZIPCODE_POSITION[dbtype]-1)));
			} else {
				record->zipcode = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & TIMEZONE) && (TIMEZONE_POSITION[dbtype] != 0)) {
				record->timezone = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (TIMEZONE_POSITION[dbtype]-1)));
			} else {
				record->timezone = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & NETSPEED) && (NETSPEED_POSITION[dbtype] != 0)) {
				record->netspeed = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (NETSPEED_POSITION[dbtype]-1)));
			} else {
				record->netspeed = strdup(NOT_SUPPORTED);
			}
	
			return record;
		} else {
			if ( ipno < ipfrom ) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}
	}
	IP2Location_free_record(record);
	return NULL;
}


IP2LocationRecord *IP2Location_new_record()
{
	IP2LocationRecord *record = (IP2LocationRecord *) malloc(sizeof(IP2LocationRecord));
	memset(record, 0, sizeof(IP2LocationRecord));
	return record;
}


void IP2Location_free_record(IP2LocationRecord *record)
{
	if (record->city != NULL)
		free(record->city);

	if (record->country_long != NULL)
		free(record->country_long);

	if (record->country_short != NULL)
		free(record->country_short);

	if (record->domain != NULL)
		free(record->domain);

	if (record->isp != NULL)
		free(record->isp);

	if (record->region != NULL)
		free(record->region);

	if (record->zipcode != NULL)
		free(record->zipcode);
		
	if (record->timezone != NULL)
		free(record->timezone);  
		
	if (record->netspeed != NULL)
		free(record->netspeed);  
	
	free(record);
}

char* IP2Location_read128(FILE *handle, uint32_t position) 
{
	uint32_t b96_127 = IP2Location_read32(handle, position);
	uint32_t b64_95 = IP2Location_read32(handle, position + 4); 
	uint32_t b32_63 = IP2Location_read32(handle, position + 8);
	uint32_t b1_31 = IP2Location_read32(handle, position + 12);

	BIGD result = bdNew();
	BIGD multiplier = bdNew();
	BIGD bd96_127 = bdNew();
	BIGD bd64_95 = bdNew();
	BIGD bd32_63 = bdNew();
	BIGD bd1_31 = bdNew();
	
	bdSetShort(multiplier, 65536);
	bdMultiplyEx(multiplier, multiplier, multiplier);
	bdSetShort(bd96_127, b96_127);
	bdSetShort(bd64_95, b64_95);
	bdSetShort(bd32_63, b32_63);
	bdSetShort(bd1_31, b1_31);

	bdMultiplyEx(bd1_31, multiplier, bd1_31);
	bdMultiplyEx(bd1_31, multiplier, bd1_31);
	bdMultiplyEx(bd1_31, multiplier, bd1_31);

    bdMultiplyEx(bd32_63, multiplier, bd32_63);
	bdMultiplyEx(bd32_63, multiplier, bd32_63);

	bdMultiplyEx(bd64_95, multiplier, bd64_95);
	
    bdAddEx(result, bd1_31, bd32_63);
	bdAddEx(result, result, bd64_95);
	bdAddEx(result, result, bd96_127);
	return IP2Location_bd2string(result);
	
}

uint32_t IP2Location_read32(FILE *handle, uint32_t position)
{
	uint8_t byte1 = 0;
	uint8_t byte2 = 0;
	uint8_t byte3 = 0;
	uint8_t byte4 = 0;
	
	if (handle != NULL) {
		fseek(handle, position-1, 0);
		fread(&byte1, 1, 1, handle);
		fread(&byte2, 1, 1, handle);
		fread(&byte3, 1, 1, handle);
		fread(&byte4, 1, 1, handle);
	}
	return ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | (byte1));
}

uint8_t IP2Location_read8(FILE *handle, uint32_t position)
{	
	uint8_t ret = 0;

	if (handle != NULL) {
		fseek(handle, position-1, 0);
		fread(&ret, 1, 1, handle);
	}		
	return ret;
}


char *IP2Location_readStr(FILE *handle, uint32_t position)
{
	uint8_t size = 0;
	char *str = 0;

	if (handle != NULL) {
		fseek(handle, position, 0);
		fread(&size, 1, 1, handle);
		str = (char *)malloc(size+1);
		memset(str, 0, size+1);
		fread(str, size, 1, handle);
	}	
	return str;
}


float IP2Location_readFloat(FILE *handle, uint32_t position)
{
	float ret = 0.0;

#ifdef _SUN_
	char * p = (char *) &ret;
	
	/* for SUN SPARC, have to reverse the byte order */
	if (handle != NULL) {
		fseek(handle, position-1, 0);
		fread(p+3, 1, 1, handle);
		fread(p+2, 1, 1, handle);
		fread(p+1, 1, 1, handle);
		fread(p,   1, 1, handle);
	}
#else
	if (handle != NULL) {
		fseek(handle, position-1, 0);
		fread(&ret, 4, 1, handle);
	}
#endif
	return ret;
}


uint32_t IP2Location_ip2no(char* ipstring)
{
	uint32_t ip = inet_addr(ipstring);
	uint8_t *ptr = (uint8_t *) &ip;
	uint32_t a = 0;
	
	if (ipstring != NULL) {
		a =  (uint8_t)(ptr[3]);
		a += (uint8_t)(ptr[2]) * 256;
		a += (uint8_t)(ptr[1]) * 256 * 256;
		a += (uint8_t)(ptr[0]) * 256 * 256 * 256;
	}
	return a;
}

char* IP2Location_bd2string (BIGD bd) {
	BIGD test = bd;
	char *result = malloc(sizeof(char)*128);
	memset(result, 0, 128);
	bdConvToDecimal(bd, result, 128);
	return result;
}

BIGD IP2Location_ipv6_to_no(char* ipaddr) {
	
	char expanded[8];
	int isExpanded = 0;
	int padCount = 2;
	StringList* array = 0;
	StringList* iter = 0;
	unsigned int n = IP2Location_substr_count(":", ipaddr);
	BIGD ipsub = bdNew();
	BIGD bdResult = bdNew();
	int subLoc = 8;

	memset(&expanded, 0, 8);
	if(n < 7) {
		expanded[0] = ':';
		expanded[1] = ':';
		while(n < 7) {
			expanded[padCount] = ':';
			padCount++;
			n++;
		}
		isExpanded = 1;
		ipaddr = IP2Location_replace("::", expanded, ipaddr);
	}

	array = IP2Location_split(":", ipaddr, DEFAULT, -1);
	iter = array;
	bdSetShort(bdResult, 0);
	for(iter=array; iter!= 0; iter=iter->next ){
		subLoc--;
		if(strcmp(iter->data, "") == 0) 
			continue;

		bdConvFromHex(ipsub, iter->data);
		bdShiftLeft(ipsub, ipsub, (16*subLoc));
		bdAddEx(bdResult, bdResult, ipsub);
	}

	if(isExpanded)
		free(ipaddr);
	bdFree(&ipsub);
	FreeStringList(array);
	return bdResult;
}

int IP2Location_ip_is_ipv4 (char* ipaddr) {
	unsigned int p;
	StringList* iparray = 0;
	StringList* ipsub = 0;
	for(p=0; p<strlen(ipaddr); p++) {
		if( ipaddr[p] >= '0' && ipaddr[p] <= '9' ||
			ipaddr[p] == '.' )
			continue;
		else
			return 0;
	}

	if(ipaddr[0] == '.' || ipaddr[strlen(ipaddr)-1] == '.') {
		return 0;
	}
	
	if( IP2Location_substr_count("::", ipaddr) > 0 ) {
		return 0;
	}

	iparray = IP2Location_split(".", ipaddr, DEFAULT, -1);
	if(StringListCount(iparray) != 4) {
		return 0;
	}
	
	for(ipsub = iparray; ipsub->next!=0; ipsub=ipsub->next) {
		if(atoi(ipsub->data) < 0 || atoi(ipsub->data) > 255) {
			return 0;	
		}
	}		
	FreeStringList(iparray);
	return 1;
}

int IP2Location_ip_is_ipv6 (char* ipaddr) {
	unsigned int n = 0;
	unsigned int k = 0;
	unsigned int m = 0;
	unsigned int p = 0;
	unsigned int checkFlag = 0;
	StringList* ipv6array = 0;
	StringList* ipsub = 0;

	n = IP2Location_substr_count(":", ipaddr); 
	if (n < 1 && n > 7) {
		return 0;
	}
	
	k = 0;
	ipv6array = IP2Location_split(":", ipaddr, DEFAULT, -1);
	for(ipsub = ipv6array; ipsub->next!=0; ipsub=ipsub->next)
	{
		k++;
		if (strcmp(ipsub->data, "") == 0) {
			continue;
		}
		
		if(strlen(ipsub->data) > 4) {
			return 0;
		}

		checkFlag = 1;
		for(p=0; p<strlen(ipsub->data); p++) {
			if( ipsub->data[p] >= '0' && ipsub->data[p] <= '9' ||
				ipsub->data[p] >= 'a' && ipsub->data[p] <= 'f' ||
				ipsub->data[p] >= 'A' && ipsub->data[p] <= 'F' )
				continue;
			else
				checkFlag = 0;
		}

		if(checkFlag)
			continue;

		if (k == n+1) {
			if (IP2Location_ip_is_ipv4(ipsub->data)) {
				// here we know it is embeded ipv4, should retrieve data from ipv4 db, pending...
				// the result of this will not be valid, since all characters are treated and calculated 
				// in hex based.
				// In addition, embeded ipv4 requires 96 '0' bits. We need to check this too.
				continue;	
			}
		}
		return 0;
	}
	
	m = IP2Location_substr_count("::", ipaddr);
	if (m > 1 && n < 7) {
		return 0;
	}
	return 1;
}

StringList* IP2Location_split(char* delimiters, char* targetString, unsigned int flags, int limit)
{
	StringList* tokenHead = 0;
	StringList* prevToken = 0;
	StringList* tokenStore = 0;
	char* targetCopy = malloc(sizeof(char)*(strlen(targetString)+1));
	char* token = 0;	
	unsigned int match, i, j = 0;
	strcpy(targetCopy, targetString);	
	
	token = targetCopy;
	match = 0;
	for(i=0; i<strlen(targetCopy); i++) {
		if(match) {
			match = 0;
			token = targetCopy+i;
		}
		
		for(j=0; j<strlen(delimiters); j++) {
			if(targetCopy[i] == delimiters[j]) {
				match = 1;
				targetCopy[i] = 0;
				break;
			}
		}
		
		if(!match && ((i+1)!=strlen(targetCopy)))
			continue;

		add:
		tokenStore = malloc(sizeof(StringList));
		tokenStore->data = malloc(sizeof(char)*(strlen(token)+1));
		tokenStore->next = 0;
		if(prevToken == 0)
			tokenHead = tokenStore;
		else
			prevToken->next = tokenStore;

		prevToken = tokenStore;
		strcpy(tokenStore->data, token);
		targetCopy[i] = delimiters[j];

		if(match && ((i+1)==strlen(targetCopy)))
		{
			token = targetString+strlen(targetString);
			match = 0;
			goto add;
		}
	}
	
	free(targetCopy);
	return tokenHead;
}

unsigned int IP2Location_substr_count(char* substr, char* targetString) 
{
	unsigned int count = 0;
	char* caret = targetString;
	while( (caret = strstr(caret, substr)) != 0 ) {
		count++;
		caret = caret + strlen(substr);
	}
    return count;	
}

char* IP2Location_replace(char* substr, char* replace, char* targetString)
{
	//estimate max possible replaced string length
	char* buff = malloc(sizeof(char) * ( (int)(strlen(targetString)/strlen(substr) + 0.5) * strlen(replace)));
	char* headCaret = targetString;
	char* endCaret = 0;
	buff[0] = 0;
	
	while( (endCaret = strstr(headCaret, substr)) != 0) {
		strncat(buff, headCaret, endCaret-headCaret);
		strcat(buff, replace);
		headCaret = endCaret + strlen(substr);
	}
	strcat(buff, headCaret);
	return buff;
}

void FreeStringList (StringList* toFree) {
	if(toFree != 0) {
		if(toFree->data != 0)
			free(toFree->data);
		FreeStringList(toFree->next);
		free(toFree);
	}
}

unsigned int StringListCount (StringList* toCount) {
	unsigned int count=0;
	while(toCount!=0) {
		count++;
		toCount = toCount->next;
	}
	return count;
}
