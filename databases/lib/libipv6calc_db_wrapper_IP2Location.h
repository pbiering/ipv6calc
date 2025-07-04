/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_IP2Location.h
 * Copyright  : 2013-2025 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_IP2Location.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_IP2Location_h

#define _libipv6calc_db_wrapper_IP2Location_h 1

#ifdef SUPPORT_IP2LOCATION
#include "IP2Location.h"
#endif

#define IP2LOCATION_IPV6_SUPPORT_UNKNOWN	0
#define IP2LOCATION_IPV6_SUPPORT_COMPAT		1
#define IP2LOCATION_IPV6_SUPPORT_FULL		2
#define IP2LOCATION_IPV6_SUPPORT_NOTEXISTS	3

// database types
/*** Commercial ***/
#define IP2LOCATION_IPV4_DB1	 1
#define IP2LOCATION_IPV4_DB2	 2
#define IP2LOCATION_IPV4_DB3	 3
#define IP2LOCATION_IPV4_DB4	 4
#define IP2LOCATION_IPV4_DB5	 5
#define IP2LOCATION_IPV4_DB6	 6
#define IP2LOCATION_IPV4_DB7	 7
#define IP2LOCATION_IPV4_DB8	 8
#define IP2LOCATION_IPV4_DB9	 9
#define IP2LOCATION_IPV4_DB10	 10
#define IP2LOCATION_IPV4_DB11	 11
#define IP2LOCATION_IPV4_DB12	 12
#define IP2LOCATION_IPV4_DB13	 13
#define IP2LOCATION_IPV4_DB14	 14
#define IP2LOCATION_IPV4_DB15	 15
#define IP2LOCATION_IPV4_DB16	 16
#define IP2LOCATION_IPV4_DB17	 17
#define IP2LOCATION_IPV4_DB18	 18
#define IP2LOCATION_IPV4_DB19	 19
#define IP2LOCATION_IPV4_DB20	 20
#define IP2LOCATION_IPV4_DB21	 21
#define IP2LOCATION_IPV4_DB22	 22
#define IP2LOCATION_IPV4_DB23	 23
#define IP2LOCATION_IPV4_DB24	 24
#define IP2LOCATION_IPV4_DB25	 25
#define IP2LOCATION_IPV4_DB26	 26
#define IP2LOCATION_IPV4_ASN	 99

#define IP2LOCATION_IPV6_DB1	 101
#define IP2LOCATION_IPV6_DB2	 102
#define IP2LOCATION_IPV6_DB3	 103
#define IP2LOCATION_IPV6_DB4	 104
#define IP2LOCATION_IPV6_DB5	 105
#define IP2LOCATION_IPV6_DB6	 106
#define IP2LOCATION_IPV6_DB7	 107
#define IP2LOCATION_IPV6_DB8	 108
#define IP2LOCATION_IPV6_DB9	 109
#define IP2LOCATION_IPV6_DB10	 110
#define IP2LOCATION_IPV6_DB11	 111
#define IP2LOCATION_IPV6_DB12	 112
#define IP2LOCATION_IPV6_DB13	 113
#define IP2LOCATION_IPV6_DB14	 114
#define IP2LOCATION_IPV6_DB15	 115
#define IP2LOCATION_IPV6_DB16	 116
#define IP2LOCATION_IPV6_DB17	 117
#define IP2LOCATION_IPV6_DB18	 118
#define IP2LOCATION_IPV6_DB19	 119
#define IP2LOCATION_IPV6_DB20	 120
#define IP2LOCATION_IPV6_DB21	 121
#define IP2LOCATION_IPV6_DB22	 122
#define IP2LOCATION_IPV6_DB23	 123
#define IP2LOCATION_IPV6_DB24	 124
#define IP2LOCATION_IPV6_DB25	 125
#define IP2LOCATION_IPV6_DB26	 126
#define IP2LOCATION_IPV6_ASN	 199

/* SAMPLE */
#define IP2LOCATION_IPV4_SAMPLE_DB1	 201
#define IP2LOCATION_IPV4_SAMPLE_DB2	 202
#define IP2LOCATION_IPV4_SAMPLE_DB3	 203
#define IP2LOCATION_IPV4_SAMPLE_DB4	 204
#define IP2LOCATION_IPV4_SAMPLE_DB5	 205
#define IP2LOCATION_IPV4_SAMPLE_DB6	 206
#define IP2LOCATION_IPV4_SAMPLE_DB7	 207
#define IP2LOCATION_IPV4_SAMPLE_DB8	 208
#define IP2LOCATION_IPV4_SAMPLE_DB9	 209
#define IP2LOCATION_IPV4_SAMPLE_DB10	 210
#define IP2LOCATION_IPV4_SAMPLE_DB11	 211
#define IP2LOCATION_IPV4_SAMPLE_DB12	 212
#define IP2LOCATION_IPV4_SAMPLE_DB13	 213
#define IP2LOCATION_IPV4_SAMPLE_DB14	 214
#define IP2LOCATION_IPV4_SAMPLE_DB15	 215
#define IP2LOCATION_IPV4_SAMPLE_DB16	 216
#define IP2LOCATION_IPV4_SAMPLE_DB17	 217
#define IP2LOCATION_IPV4_SAMPLE_DB18	 218
#define IP2LOCATION_IPV4_SAMPLE_DB19	 219
#define IP2LOCATION_IPV4_SAMPLE_DB20	 220
#define IP2LOCATION_IPV4_SAMPLE_DB21	 221
#define IP2LOCATION_IPV4_SAMPLE_DB22	 222
#define IP2LOCATION_IPV4_SAMPLE_DB23	 223
#define IP2LOCATION_IPV4_SAMPLE_DB24	 224
#define IP2LOCATION_IPV4_SAMPLE_DB25	 225
#define IP2LOCATION_IPV4_SAMPLE_DB26	 226
#define IP2LOCATION_IPV4_SAMPLE_ASN	 299

#define IP2LOCATION_IPV6_SAMPLE_DB1	 301
#define IP2LOCATION_IPV6_SAMPLE_DB2	 302
#define IP2LOCATION_IPV6_SAMPLE_DB3	 303
#define IP2LOCATION_IPV6_SAMPLE_DB4	 304
#define IP2LOCATION_IPV6_SAMPLE_DB5	 305
#define IP2LOCATION_IPV6_SAMPLE_DB6	 306
#define IP2LOCATION_IPV6_SAMPLE_DB7	 307
#define IP2LOCATION_IPV6_SAMPLE_DB8	 308
#define IP2LOCATION_IPV6_SAMPLE_DB9	 309
#define IP2LOCATION_IPV6_SAMPLE_DB10	 310
#define IP2LOCATION_IPV6_SAMPLE_DB11	 311
#define IP2LOCATION_IPV6_SAMPLE_DB12	 312
#define IP2LOCATION_IPV6_SAMPLE_DB13	 313
#define IP2LOCATION_IPV6_SAMPLE_DB14	 314
#define IP2LOCATION_IPV6_SAMPLE_DB15	 315
#define IP2LOCATION_IPV6_SAMPLE_DB16	 316
#define IP2LOCATION_IPV6_SAMPLE_DB17	 317
#define IP2LOCATION_IPV6_SAMPLE_DB18	 318
#define IP2LOCATION_IPV6_SAMPLE_DB19	 319
#define IP2LOCATION_IPV6_SAMPLE_DB20	 320
#define IP2LOCATION_IPV6_SAMPLE_DB21	 321
#define IP2LOCATION_IPV6_SAMPLE_DB22	 322
#define IP2LOCATION_IPV6_SAMPLE_DB23	 323
#define IP2LOCATION_IPV6_SAMPLE_DB24	 324
#define IP2LOCATION_IPV6_SAMPLE_DB25	 325
#define IP2LOCATION_IPV6_SAMPLE_DB26	 326
#define IP2LOCATION_IPV6_SAMPLE_ASN	 399

/* LITE */
#define IP2LOCATION_DB_IP_COUNTRY_LITE				401
#define IP2LOCATION_DB_IPV6_COUNTRY_LITE			501

#define IP2LOCATION_DB_IP_COUNTRY_REG_CIT_LITE			403
#define IP2LOCATION_DB_IPV6_COUNTRY_REG_CIT_LITE		503

#define IP2LOCATION_DB_IP_COUNTRY_REG_CIT_LL_LITE		405
#define IP2LOCATION_DB_IPV6_COUNTRY_REG_CIT_LL_LITE		505

#define IP2LOCATION_DB_IP_COUNTRY_REG_CIT_LL_ZIP_LITE		409
#define IP2LOCATION_DB_IPV6_COUNTRY_REG_CIT_LL_ZIP_LITE		509

#define IP2LOCATION_DB_IP_COUNTRY_REG_CIT_LL_ZIP_TZ_LITE	411
#define IP2LOCATION_DB_IPV6_COUNTRY_REG_CIT_LL_ZIP_TZ_LITE	511

#define IP2LOCATION_DB_IP_ASN_LITE				499
#define IP2LOCATION_DB_IPV6_ASN_LITE				599

#define IP2LOCATION_DB_MAX	 IP2LOCATION_DB_IPV6_ASN_LITE

#define IP2LOCATION_DB_YEAR_MIN		2020	// older databases are ignored

// shortcuts
#define IPV6CALC_DB_SC_IP2LOCATION_IPV4_COUNTRY			(IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV4_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION_IPV4)
#define IPV6CALC_DB_SC_IP2LOCATION_IPV4_CITY			(IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV4_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION_IPV4 | IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV4_TO_CITY)
#define IPV6CALC_DB_SC_IP2LOCATION_IPV4_ASN			(IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV4_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION_IPV4 | IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV4_TO_CITY | IPV6CALC_DB_IPV4_TO_AS)

#define IPV6CALC_DB_SC_IP2LOCATION_IPV6_COUNTRY			(IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_IPV6_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION_IPV6)
#define IPV6CALC_DB_SC_IP2LOCATION_IPV6_CITY			(IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_IPV6_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION_IPV6 | IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV6_TO_CITY)
#define IPV6CALC_DB_SC_IP2LOCATION_IPV6_ASN			(IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_IPV6_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION_IPV6 | IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV6_TO_CITY | IPV6CALC_DB_IPV6_TO_AS)

#define IPV6CALC_DB_SC_IP2LOCATION_IPVx_COUNTRY			(IPV6CALC_DB_SC_IP2LOCATION_IPV4_COUNTRY | IPV6CALC_DB_SC_IP2LOCATION_IPV6_COUNTRY)
#define IPV6CALC_DB_SC_IP2LOCATION_IPVx_CITY			(IPV6CALC_DB_SC_IP2LOCATION_IPV4_CITY | IPV6CALC_DB_SC_IP2LOCATION_IPV6_CITY)
#define IPV6CALC_DB_SC_IP2LOCATION_IPVx_ASN			(IPV6CALC_DB_SC_IP2LOCATION_IPV4_ASN | IPV6CALC_DB_SC_IP2LOCATION_IPV6_ASN)

#define IP2LOCATION_DB1  "Country"
#define IP2LOCATION_DB2  "Country/ISP"
#define IP2LOCATION_DB3  "Country/Region/City"
#define IP2LOCATION_DB4  "Country/Region/City/ISP"
#define IP2LOCATION_DB5  "Country/Region/City/LongitudeLattitude"
#define IP2LOCATION_DB6  "Country/Region/City/LongLatt/ISP"
#define IP2LOCATION_DB7  "Co/Re/Ci/ISP/Domain"
#define IP2LOCATION_DB8  "Co/Re/Ci/LattLong/ISP/Domain"
#define IP2LOCATION_DB9  "Co/Re/Ci/LattLong/ZIP"
#define IP2LOCATION_DB10 "Co/Re/Ci/LattLong/ZIP/ISP/Domain"
#define IP2LOCATION_DB11 "Co/Re/Ci/LattLong/ZIP/TZ"
#define IP2LOCATION_DB12 "Co/Re/Ci/LaLo/ZI/TZ/IS/Domain"
#define IP2LOCATION_DB13 "Co/Re/Ci/LaLo/TZ/NetSpeed"
#define IP2LOCATION_DB14 "Co/Re/Ci/LaLo/ZI/TZ/ISP/Do/Netspeed"
#define IP2LOCATION_DB15 "Co/Re/Ci/LaLo/ZIP/TZ/Area"
#define IP2LOCATION_DB16 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Net/Area"
#define IP2LOCATION_DB17 "Co/Re/Ci/LaLo/TZ/Net/Weather"
#define IP2LOCATION_DB18 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/Weather"
#define IP2LOCATION_DB19 "Co/Re/Ci/LaLo/ISP/Dom/Mobile"
#define IP2LOCATION_DB20 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo"
#define IP2LOCATION_DB21 "Co/Re/Ci/LaLo/ZI/TZ/Area/Elevation"
#define IP2LOCATION_DB22 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo/El"
#define IP2LOCATION_DB23 "Co/Re/Ci/LaLo/ISP/Dom/Mobile/Usagetype"
#define IP2LOCATION_DB24 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo/El/Us"
#define IP2LOCATION_DB25 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo/El/Us/Ad/Ca"
#define IP2LOCATION_DB26 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo/El/Us/Ad/Ca/Di/AS"
#define IP2LOCATION_ASN  "ASN"


#define IPV6CALC_DB_IP2LOCATION_INTERNAL_SAMPLE		0x00000001
#define IPV6CALC_DB_IP2LOCATION_INTERNAL_LITE		0x00000002
#define IPV6CALC_DB_IP2LOCATION_INTERNAL_FREE		0x00000004
#define IPV6CALC_DB_IP2LOCATION_INTERNAL_LIB_VERSION_6	0x00000008
#define IPV6CALC_DB_IP2LOCATION_INTERNAL_LIB_VERSION_8_4 0x00000010
#define IPV6CALC_DB_IP2LOCATION_INTERNAL_LIB_VERSION_8_6 0x00000020

#endif


/* prototypes */

extern int         libipv6calc_db_wrapper_IP2Location_wrapper_init(void);
extern int         libipv6calc_db_wrapper_IP2Location_wrapper_cleanup(void);
extern uint32_t    libipv6calc_db_wrapper_IP2Location_wrapper_asn_by_addr(const ipv6calc_ipaddr *ipaddrp, char *as_orgname, const size_t as_orgname_length);
extern void        libipv6calc_db_wrapper_IP2Location_wrapper_info(char* string, const size_t size);
extern void        libipv6calc_db_wrapper_IP2Location_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern char       *libipv6calc_db_wrapper_IP2Location_wrapper_db_info_used(void);

extern int         libipv6calc_db_wrapper_IP2Location_wrapper_country_code_by_addr(char *addr, const int proto, char *country, const size_t country_len);

extern int         libipv6calc_db_wrapper_IP2Location_has_features(uint32_t features);

extern int         libipv6calc_db_wrapper_IP2Location_library_version_major(void);
extern int         libipv6calc_db_wrapper_IP2Location_library_version_majorminor(void);


#ifdef SUPPORT_IP2LOCATION
extern char ip2location_lib_file[PATH_MAX];
extern char ip2location_db_dir[PATH_MAX];

extern int                libipv6calc_db_wrapper_IP2Location_db_avail(const unsigned int type);
extern char              *libipv6calc_db_wrapper_IP2Location_lib_version(void);
extern IP2Location       *libipv6calc_db_wrapper_IP2Location_open(char *db);
extern IP2Location       *libipv6calc_db_wrapper_IP2Location_open_type(const unsigned int type);
extern int                libipv6calc_db_wrapper_IP2Location_close(IP2Location *loc);
extern IP2LocationRecord *libipv6calc_db_wrapper_IP2Location_get_all(IP2Location *loc, char *ip);
void                      libipv6calc_db_wrapper_IP2Location_free_record(IP2LocationRecord *record);

extern const char  *libipv6calc_db_wrapper_IP2Location_dbdescription(const unsigned int type);
extern char        *libipv6calc_db_wrapper_IP2Location_database_info(IP2Location *loc, const int level_verbose, const int entry, const int flag_copyright);

extern const char *libipv6calc_db_wrapper_IP2Location_UsageType_description(char *UsageType);

extern int ip2location_db_lite_to_sample_autoswitch_max_delta_months;
extern int ip2location_db_comm_to_lite_switch_min_delta_months;
extern int ip2location_db_only_type;
extern int ip2location_db_allow_softlinks;

extern int          libipv6calc_db_wrapper_IP2Location_all_by_addr(const ipv6calc_ipaddr *ipaddrp, libipv6calc_db_wrapper_geolocation_record *recordp);

#ifndef IP2LOCATION_API_VERSION
/*
 * API_VERSION in old versions was defined as a bareword in IP2Location.h,
 *  we need this trick to stringify it. Blah.
 */
#define makestr(x) #x
#define xmakestr(x) makestr(x)
#define IP2LOCATION_API_VERSION xmakestr(API_VERSION)
#endif /* IP2LOCATION_API_VERSION */

#endif
