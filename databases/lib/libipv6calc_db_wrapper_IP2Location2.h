/*
 * Project    : ipv6calc
 * File       : databases/lib/libipv6calc_db_wrapper_IP2Location2.h
 * Copyright  : 2025-2025 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipv6calc_db_wrapper_IP2Location2.c
 */

#include "ipv6calctypes.h"

#ifndef _libipv6calc_db_wrapper_IP2Location2_h

#define _libipv6calc_db_wrapper_IP2Location2_h 1

// database types
/*** Commercial ***/
#define IP2LOCATION2_IPV4_DB1	 1
#define IP2LOCATION2_IPV4_DB2	 2
#define IP2LOCATION2_IPV4_DB3	 3
#define IP2LOCATION2_IPV4_DB4	 4
#define IP2LOCATION2_IPV4_DB5	 5
#define IP2LOCATION2_IPV4_DB6	 6
#define IP2LOCATION2_IPV4_DB7	 7
#define IP2LOCATION2_IPV4_DB8	 8
#define IP2LOCATION2_IPV4_DB9	 9
#define IP2LOCATION2_IPV4_DB10	 10
#define IP2LOCATION2_IPV4_DB11	 11
#define IP2LOCATION2_IPV4_DB12	 12
#define IP2LOCATION2_IPV4_DB13	 13
#define IP2LOCATION2_IPV4_DB14	 14
#define IP2LOCATION2_IPV4_DB15	 15
#define IP2LOCATION2_IPV4_DB16	 16
#define IP2LOCATION2_IPV4_DB17	 17
#define IP2LOCATION2_IPV4_DB18	 18
#define IP2LOCATION2_IPV4_DB19	 19
#define IP2LOCATION2_IPV4_DB20	 20
#define IP2LOCATION2_IPV4_DB21	 21
#define IP2LOCATION2_IPV4_DB22	 22
#define IP2LOCATION2_IPV4_DB23	 23
#define IP2LOCATION2_IPV4_DB24	 24
#define IP2LOCATION2_IPV4_DB25	 25
#define IP2LOCATION2_IPV4_DB26	 26

#define IP2LOCATION2_IPV6_DB1	 101
#define IP2LOCATION2_IPV6_DB2	 102
#define IP2LOCATION2_IPV6_DB3	 103
#define IP2LOCATION2_IPV6_DB4	 104
#define IP2LOCATION2_IPV6_DB5	 105
#define IP2LOCATION2_IPV6_DB6	 106
#define IP2LOCATION2_IPV6_DB7	 107
#define IP2LOCATION2_IPV6_DB8	 108
#define IP2LOCATION2_IPV6_DB9	 109
#define IP2LOCATION2_IPV6_DB10	 110
#define IP2LOCATION2_IPV6_DB11	 111
#define IP2LOCATION2_IPV6_DB12	 112
#define IP2LOCATION2_IPV6_DB13	 113
#define IP2LOCATION2_IPV6_DB14	 114
#define IP2LOCATION2_IPV6_DB15	 115
#define IP2LOCATION2_IPV6_DB16	 116
#define IP2LOCATION2_IPV6_DB17	 117
#define IP2LOCATION2_IPV6_DB18	 118
#define IP2LOCATION2_IPV6_DB19	 119
#define IP2LOCATION2_IPV6_DB20	 120
#define IP2LOCATION2_IPV6_DB21	 121
#define IP2LOCATION2_IPV6_DB22	 122
#define IP2LOCATION2_IPV6_DB23	 123
#define IP2LOCATION2_IPV6_DB24	 124
#define IP2LOCATION2_IPV6_DB25	 125
#define IP2LOCATION2_IPV6_DB26	 126

/* SAMPLE */
#define IP2LOCATION2_IPV4_SAMPLE_DB1	 201
#define IP2LOCATION2_IPV4_SAMPLE_DB2	 202
#define IP2LOCATION2_IPV4_SAMPLE_DB3	 203
#define IP2LOCATION2_IPV4_SAMPLE_DB4	 204
#define IP2LOCATION2_IPV4_SAMPLE_DB5	 205
#define IP2LOCATION2_IPV4_SAMPLE_DB6	 206
#define IP2LOCATION2_IPV4_SAMPLE_DB7	 207
#define IP2LOCATION2_IPV4_SAMPLE_DB8	 208
#define IP2LOCATION2_IPV4_SAMPLE_DB9	 209
#define IP2LOCATION2_IPV4_SAMPLE_DB10	 210
#define IP2LOCATION2_IPV4_SAMPLE_DB11	 211
#define IP2LOCATION2_IPV4_SAMPLE_DB12	 212
#define IP2LOCATION2_IPV4_SAMPLE_DB13	 213
#define IP2LOCATION2_IPV4_SAMPLE_DB14	 214
#define IP2LOCATION2_IPV4_SAMPLE_DB15	 215
#define IP2LOCATION2_IPV4_SAMPLE_DB16	 216
#define IP2LOCATION2_IPV4_SAMPLE_DB17	 217
#define IP2LOCATION2_IPV4_SAMPLE_DB18	 218
#define IP2LOCATION2_IPV4_SAMPLE_DB19	 219
#define IP2LOCATION2_IPV4_SAMPLE_DB20	 220
#define IP2LOCATION2_IPV4_SAMPLE_DB21	 221
#define IP2LOCATION2_IPV4_SAMPLE_DB22	 222
#define IP2LOCATION2_IPV4_SAMPLE_DB23	 223
#define IP2LOCATION2_IPV4_SAMPLE_DB24	 224
#define IP2LOCATION2_IPV4_SAMPLE_DB25	 225
#define IP2LOCATION2_IPV4_SAMPLE_DB26	 226

#define IP2LOCATION2_IPV6_SAMPLE_DB1	 301
#define IP2LOCATION2_IPV6_SAMPLE_DB2	 302
#define IP2LOCATION2_IPV6_SAMPLE_DB3	 303
#define IP2LOCATION2_IPV6_SAMPLE_DB4	 304
#define IP2LOCATION2_IPV6_SAMPLE_DB5	 305
#define IP2LOCATION2_IPV6_SAMPLE_DB6	 306
#define IP2LOCATION2_IPV6_SAMPLE_DB7	 307
#define IP2LOCATION2_IPV6_SAMPLE_DB8	 308
#define IP2LOCATION2_IPV6_SAMPLE_DB9	 309
#define IP2LOCATION2_IPV6_SAMPLE_DB10	 310
#define IP2LOCATION2_IPV6_SAMPLE_DB11	 311
#define IP2LOCATION2_IPV6_SAMPLE_DB12	 312
#define IP2LOCATION2_IPV6_SAMPLE_DB13	 313
#define IP2LOCATION2_IPV6_SAMPLE_DB14	 314
#define IP2LOCATION2_IPV6_SAMPLE_DB15	 315
#define IP2LOCATION2_IPV6_SAMPLE_DB16	 316
#define IP2LOCATION2_IPV6_SAMPLE_DB17	 317
#define IP2LOCATION2_IPV6_SAMPLE_DB18	 318
#define IP2LOCATION2_IPV6_SAMPLE_DB19	 319
#define IP2LOCATION2_IPV6_SAMPLE_DB20	 320
#define IP2LOCATION2_IPV6_SAMPLE_DB21	 321
#define IP2LOCATION2_IPV6_SAMPLE_DB22	 322
#define IP2LOCATION2_IPV6_SAMPLE_DB23	 323
#define IP2LOCATION2_IPV6_SAMPLE_DB24	 324
#define IP2LOCATION2_IPV6_SAMPLE_DB25	 325
#define IP2LOCATION2_IPV6_SAMPLE_DB26	 326

/* LITE */
#define IP2LOCATION2_DB_IP_COUNTRY_LITE				401
#define IP2LOCATION2_DB_IPV6_COUNTRY_LITE			501

#define IP2LOCATION2_DB_IP_COUNTRY_REG_CIT_LITE			403
#define IP2LOCATION2_DB_IPV6_COUNTRY_REG_CIT_LITE		503

#define IP2LOCATION2_DB_IP_COUNTRY_REG_CIT_LL_LITE		405
#define IP2LOCATION2_DB_IPV6_COUNTRY_REG_CIT_LL_LITE		505

#define IP2LOCATION2_DB_IP_COUNTRY_REG_CIT_LL_ZIP_LITE		409
#define IP2LOCATION2_DB_IPV6_COUNTRY_REG_CIT_LL_ZIP_LITE		509

#define IP2LOCATION2_DB_IP_COUNTRY_REG_CIT_LL_ZIP_TZ_LITE	411
#define IP2LOCATION2_DB_IPV6_COUNTRY_REG_CIT_LL_ZIP_TZ_LITE	511

#define IP2LOCATION2_DB_MAX	 IP2LOCATION2_DB_IPV6_COUNTRY_REG_CIT_LL_ZIP_TZ_LITE

// shortcuts
#define IPV6CALC_DB_SC_IP2LOCATION2_IPV4_COUNTRY		(IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV4_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION2_IPV4)
#define IPV6CALC_DB_SC_IP2LOCATION2_IPV4_CITY			(IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV4_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION2_IPV4 | IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV4_TO_CITY)
#define IPV6CALC_DB_SC_IP2LOCATION2_IPV4_ASN			(IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV4_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION2_IPV4 | IPV6CALC_DB_IPV4_TO_REGION | IPV6CALC_DB_IPV4_TO_CITY | IPV6CALC_DB_IPV4_TO_AS)

#define IPV6CALC_DB_SC_IP2LOCATION2_IPV6_COUNTRY		(IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_IPV6_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION2_IPV6)
#define IPV6CALC_DB_SC_IP2LOCATION2_IPV6_CITY			(IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_IPV6_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV6_TO_CITY)
#define IPV6CALC_DB_SC_IP2LOCATION2_IPV6_ASN			(IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_IPV6_TO_COUNTRY | IPV6CALC_DB_IP2LOCATION2_IPV6 | IPV6CALC_DB_IPV6_TO_REGION | IPV6CALC_DB_IPV6_TO_CITY | IPV6CALC_DB_IPV6_TO_AS)

#define IP2LOCATION2_DB1  "Country"
#define IP2LOCATION2_DB2  "Country/ISP"
#define IP2LOCATION2_DB3  "Country/Region/City"
#define IP2LOCATION2_DB4  "Country/Region/City/ISP"
#define IP2LOCATION2_DB5  "Country/Region/City/LongitudeLattitude"
#define IP2LOCATION2_DB6  "Country/Region/City/LongLatt/ISP"
#define IP2LOCATION2_DB7  "Co/Re/Ci/ISP/Domain"
#define IP2LOCATION2_DB8  "Co/Re/Ci/LattLong/ISP/Domain"
#define IP2LOCATION2_DB9  "Co/Re/Ci/LattLong/ZIP"
#define IP2LOCATION2_DB10 "Co/Re/Ci/LattLong/ZIP/ISP/Domain"
#define IP2LOCATION2_DB11 "Co/Re/Ci/LattLong/ZIP/TZ"
#define IP2LOCATION2_DB12 "Co/Re/Ci/LaLo/ZI/TZ/IS/Domain"
#define IP2LOCATION2_DB13 "Co/Re/Ci/LaLo/TZ/NetSpeed"
#define IP2LOCATION2_DB14 "Co/Re/Ci/LaLo/ZI/TZ/ISP/Do/Netspeed"
#define IP2LOCATION2_DB15 "Co/Re/Ci/LaLo/ZIP/TZ/Area"
#define IP2LOCATION2_DB16 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Net/Area"
#define IP2LOCATION2_DB17 "Co/Re/Ci/LaLo/TZ/Net/Weather"
#define IP2LOCATION2_DB18 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/Weather"
#define IP2LOCATION2_DB19 "Co/Re/Ci/LaLo/ISP/Dom/Mobile"
#define IP2LOCATION2_DB20 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo"
#define IP2LOCATION2_DB21 "Co/Re/Ci/LaLo/ZI/TZ/Area/Elevation"
#define IP2LOCATION2_DB22 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo/El"
#define IP2LOCATION2_DB23 "Co/Re/Ci/LaLo/ISP/Dom/Mobile/Usagetype"
#define IP2LOCATION2_DB24 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo/El/Us"
#define IP2LOCATION2_DB25 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo/El/Us/Ad/Ca"
#define IP2LOCATION2_DB26 "Co/Re/Ci/LaLo/ZI/TZ/IS/Do/Ne/Ar/We/Mo/El/Us/Ad/Ca/Di/AS"


#define IPV6CALC_DB_IP2LOCATION2_INTERNAL_SAMPLE		0x00000001
#define IPV6CALC_DB_IP2LOCATION2_INTERNAL_LITE		0x00000002
#define IPV6CALC_DB_IP2LOCATION2_INTERNAL_FREE		0x00000004

#endif


/* prototypes */

extern int         libipv6calc_db_wrapper_IP2Location2_wrapper_init(void);
extern int         libipv6calc_db_wrapper_IP2Location2_wrapper_cleanup(void);
extern uint32_t    libipv6calc_db_wrapper_IP2Location2_wrapper_asn_by_addr(const ipv6calc_ipaddr *ipaddrp, char *as_orgname, const size_t as_orgname_length);
extern uint32_t    libipv6calc_db_wrapper_IP2Location2_wrapper_GeonameID_by_addr(const ipv6calc_ipaddr *ipaddrp, int *source_ptr);
extern void        libipv6calc_db_wrapper_IP2Location2_wrapper_info(char* string, const size_t size);
extern void        libipv6calc_db_wrapper_IP2Location2_wrapper_print_db_info(const int level_verbose, const char *prefix_string);
extern char       *libipv6calc_db_wrapper_IP2Location2_wrapper_db_info_used(void);

extern int         libipv6calc_db_wrapper_IP2Location2_wrapper_country_code_by_addr(const ipv6calc_ipaddr *ipaddrp, char *country, const size_t country_len);

extern int         libipv6calc_db_wrapper_IP2Location2_has_features(uint32_t features);


#ifdef SUPPORT_IP2LOCATION2
#include "libipv6calc_db_wrapper_MMDB.h"

extern char ip2location2_db_dir[PATH_MAX];

extern int         libipv6calc_db_wrapper_IP2Location2_db_avail(const unsigned int type);
extern int         libipv6calc_db_wrapper_IP2Location2_open_type(const unsigned int type);

extern const char *libipv6calc_db_wrapper_IP2Location2_UsageType_description(char *UsageType);

extern int ip2location2_db_lite_to_sample_autoswitch_max_delta_months;
extern int ip2location2_db_comm_to_lite_switch_min_delta_months;
extern int ip2location2_db_only_type;
extern int ip2location2_db_allow_softlinks;

extern int          libipv6calc_db_wrapper_IP2Location2_all_by_addr(const ipv6calc_ipaddr *ipaddrp, libipv6calc_db_wrapper_geolocation_record *recordp);

extern const char *libipv6calc_db_wrapper_IP2Location2_dbdescription(const unsigned int type);
extern char       *libipv6calc_db_wrapper_IP2Location2_database_info(const unsigned int type, const int level_verbose, const int entry, const int flag_copyright);

#endif
