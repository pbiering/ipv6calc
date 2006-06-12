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
 */
 
#ifndef _SUN_
#include <stdint.h>
#include <string.h>
#endif
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "IP2Location.h"

uint8_t COUNTRY_POSITION[11]   = {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
uint8_t REGION_POSITION[11]    = {0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3};
uint8_t CITY_POSITION[11]      = {0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4};
uint8_t ISP_POSITION[11]       = {0, 0, 3, 0, 5, 0, 7, 5, 7, 0, 8};
uint8_t LATITUDE_POSITION[11]  = {0, 0, 0, 0, 0, 5, 5, 0, 5, 5, 5};
uint8_t LONGITUDE_POSITION[11] = {0, 0, 0, 0, 0, 6, 6, 0, 6, 6, 6};
uint8_t DOMAIN_POSITION[11]    = {0, 0, 0, 0, 0, 0, 0, 6, 8, 0, 9};
uint8_t ZIPCODE_POSITION[11]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7};


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
	loc->databaseyear   = IP2Location_read8(loc->filehandle, 3);
	loc->databasemonth  = IP2Location_read8(loc->filehandle, 4);
	loc->databaseday    = IP2Location_read8(loc->filehandle, 5);
	loc->databasecount  = IP2Location_read32(loc->filehandle, 6);
	loc->databaseaddr   = IP2Location_read32(loc->filehandle, 10);
	return 0;
}


IP2LocationRecord *IP2Location_get_country_short(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, COUNTRYSHORT);
}


IP2LocationRecord *IP2Location_get_country_long(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, COUNTRYLONG);
}


IP2LocationRecord *IP2Location_get_region(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, REGION);
}


IP2LocationRecord *IP2Location_get_city (IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, CITY);
}


IP2LocationRecord *IP2Location_get_isp(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, ISP);
}


IP2LocationRecord *IP2Location_get_latitude(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, LATITUDE);
}


IP2LocationRecord *IP2Location_get_longitude(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, LONGITUDE);
}


IP2LocationRecord *IP2Location_get_domain(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, DOMAIN);
}


IP2LocationRecord *IP2Location_get_zipcode(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, ZIPCODE);
}


IP2LocationRecord *IP2Location_get_all(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, ALL);
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
	
	if (ipno == (uint32_t) MAX_IP_RANGE) {
		ipno = ipno - 1;
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
		
	free(record);
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
