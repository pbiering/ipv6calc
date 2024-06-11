/*
 * Project    : ipv6calc
 * File       : libieee.c
 * Version    : $Id$
 * Copyright  : 2002-2022 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for IEEE information
 */

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "libieee.h"
#include "libmac.h"
#include "libeui64.h"

#include "libipv6calc_db_wrapper.h"


// internal map for IAB / OUI-28 / OUI-36 lists
// mapping value used for pack/unpack - do not change per entry, max 0xfff
static const s_ieee_mapping ieee_mapping[] = {
	// IAB
	{ 0x0050C2,	IEEE_IAB, 0x1 },
	{ 0x40D855,	IEEE_IAB, 0x2 },

	// OUI36
	{ 0x001BC5,	IEEE_OUI36, 0x9 },
	{ 0x70B3D5,	IEEE_OUI36, 0xa },
	{ 0x001BC5,	IEEE_OUI36, 0xb },
	{ 0x70B3D5,	IEEE_OUI36, 0xc },
	{ 0x8C1F64,	IEEE_OUI36, 0xd },

	// OUI28 (2019-10-08)
	{ 0x0055DA,	IEEE_OUI28, 0x0010 },
	{ 0x006967,	IEEE_OUI28, 0x0011 },
	{ 0x04714B,	IEEE_OUI28, 0x0012 },
	{ 0x04C3E6,	IEEE_OUI28, 0x0013 },
	{ 0x04D16E,	IEEE_OUI28, 0x0014 },
	{ 0x08ED02,	IEEE_OUI28, 0x0015 },
	{ 0x0C73EB,	IEEE_OUI28, 0x0016 },
	{ 0x0CEFAF,	IEEE_OUI28, 0x0017 },
	{ 0x0CFE5D,	IEEE_OUI28, 0x0018 },
	{ 0x100723,	IEEE_OUI28, 0x0019 },
	{ 0x10DCB6,	IEEE_OUI28, 0x001a },
	{ 0x141FBA,	IEEE_OUI28, 0x001b },
	{ 0x144FD7,	IEEE_OUI28, 0x001c },
	{ 0x189BA5,	IEEE_OUI28, 0x001d },
	{ 0x1C21D1,	IEEE_OUI28, 0x001e },
	{ 0x1C8259,	IEEE_OUI28, 0x001f },
	{ 0x1C8774,	IEEE_OUI28, 0x0020 },
	{ 0x1C8776,	IEEE_OUI28, 0x0021 },
	{ 0x1C8779,	IEEE_OUI28, 0x0022 },
	{ 0x1C8879,	IEEE_OUI28, 0x0023 },
	{ 0x1CA0D3,	IEEE_OUI28, 0x0024 },
	{ 0x1CC0E1,	IEEE_OUI28, 0x0025 },
	{ 0x1CCAE3,	IEEE_OUI28, 0x0026 },
	{ 0x1CFD08,	IEEE_OUI28, 0x0027 },
	{ 0x208593,	IEEE_OUI28, 0x0028 },
	{ 0x241510,	IEEE_OUI28, 0x0029 },
	{ 0x244E7B,	IEEE_OUI28, 0x002a },
	{ 0x282C02,	IEEE_OUI28, 0x002b },
	{ 0x283638,	IEEE_OUI28, 0x002c },
	{ 0x28F537,	IEEE_OUI28, 0x002d },
	{ 0x28FD80,	IEEE_OUI28, 0x002e },
	{ 0x2C16BD,	IEEE_OUI28, 0x002f },
	{ 0x2C265F,	IEEE_OUI28, 0x0030 },
	{ 0x2C279E,	IEEE_OUI28, 0x0031 },
	{ 0x2C4835,	IEEE_OUI28, 0x0032 },
	{ 0x2C6A6F,	IEEE_OUI28, 0x0033 },
	{ 0x2CD141,	IEEE_OUI28, 0x0034 },
	{ 0x3009F9,	IEEE_OUI28, 0x0035 },
	{ 0x300A60,	IEEE_OUI28, 0x0036 },
	{ 0x301F9A,	IEEE_OUI28, 0x0037 },
	{ 0x34008A,	IEEE_OUI28, 0x0038 },
	{ 0x34049E,	IEEE_OUI28, 0x0039 },
	{ 0x34298F,	IEEE_OUI28, 0x003a },
	{ 0x34D0B8,	IEEE_OUI28, 0x003b },
	{ 0x34E1D1,	IEEE_OUI28, 0x003c },
	{ 0x383A21,	IEEE_OUI28, 0x003d },
	{ 0x3873EA,	IEEE_OUI28, 0x003e },
	{ 0x38B19E,	IEEE_OUI28, 0x003f },
	{ 0x38B8EB,	IEEE_OUI28, 0x0040 },
	{ 0x38FDFE,	IEEE_OUI28, 0x0041 },
	{ 0x3C24F0,	IEEE_OUI28, 0x0042 },
	{ 0x3C39E7,	IEEE_OUI28, 0x0043 },
	{ 0x3C427E,	IEEE_OUI28, 0x0044 },
	{ 0x3C6A2C,	IEEE_OUI28, 0x0045 },
	{ 0x401175,	IEEE_OUI28, 0x0046 },
	{ 0x4048FD,	IEEE_OUI28, 0x0047 },
	{ 0x40A36B,	IEEE_OUI28, 0x0048 },
	{ 0x40ED98,	IEEE_OUI28, 0x0049 },
	{ 0x40F385,	IEEE_OUI28, 0x004a },
	{ 0x440377,	IEEE_OUI28, 0x004b },
	{ 0x44D5F2,	IEEE_OUI28, 0x004c },
	{ 0x480BB2,	IEEE_OUI28, 0x004d },
	{ 0x4865EE,	IEEE_OUI28, 0x004e },
	{ 0x4C4BF9,	IEEE_OUI28, 0x004f },
	{ 0x4C65A8,	IEEE_OUI28, 0x0050 },
	{ 0x4C917A,	IEEE_OUI28, 0x0051 },
	{ 0x4CBC98,	IEEE_OUI28, 0x0052 },
	{ 0x4CE173,	IEEE_OUI28, 0x0053 },
	{ 0x500B91,	IEEE_OUI28, 0x0054 },
	{ 0x506255,	IEEE_OUI28, 0x0055 },
	{ 0x50A4D0,	IEEE_OUI28, 0x0056 },
	{ 0x50FF99,	IEEE_OUI28, 0x0057 },
	{ 0x549A11,	IEEE_OUI28, 0x0058 },
	{ 0x54A493,	IEEE_OUI28, 0x0059 },
	{ 0x58E876,	IEEE_OUI28, 0x005a },
	{ 0x58FCDB,	IEEE_OUI28, 0x005b },
	{ 0x5CF286,	IEEE_OUI28, 0x005c },
	{ 0x6095CE,	IEEE_OUI28, 0x005d },
	{ 0x60D7E3,	IEEE_OUI28, 0x005e },
	{ 0x643139,	IEEE_OUI28, 0x005f },
	{ 0x64FB81,	IEEE_OUI28, 0x0060 },
	{ 0x6891D0,	IEEE_OUI28, 0x0061 },
	{ 0x6C5C3D,	IEEE_OUI28, 0x0062 },
	{ 0x6CDFFB,	IEEE_OUI28, 0x0063 },
	{ 0x70886B,	IEEE_OUI28, 0x0064 },
	{ 0x70F8E7,	IEEE_OUI28, 0x0065 },
	{ 0x7419F8,	IEEE_OUI28, 0x0066 },
	{ 0x741AE0,	IEEE_OUI28, 0x0067 },
	{ 0x745BC5,	IEEE_OUI28, 0x0068 },
	{ 0x74E14A,	IEEE_OUI28, 0x0069 },
	{ 0x74F8DB,	IEEE_OUI28, 0x006a },
	{ 0x78C2C0,	IEEE_OUI28, 0x006b },
	{ 0x78CA83,	IEEE_OUI28, 0x006c },
	{ 0x78D800,	IEEE_OUI28, 0x006d },
	{ 0x7C477C,	IEEE_OUI28, 0x006e },
	{ 0x7C70BC,	IEEE_OUI28, 0x006f },
	{ 0x7CBACC,	IEEE_OUI28, 0x0070 },
	{ 0x7CBC84,	IEEE_OUI28, 0x0071 },
	{ 0x7CCBE2,	IEEE_OUI28, 0x0072 },
	{ 0x800A80,	IEEE_OUI28, 0x0073 },
	{ 0x807B85,	IEEE_OUI28, 0x0074 },
	{ 0x80E4DA,	IEEE_OUI28, 0x0075 },
	{ 0x8439BE,	IEEE_OUI28, 0x0076 },
	{ 0x8489EC,	IEEE_OUI28, 0x0077 },
	{ 0x848BCD,	IEEE_OUI28, 0x0078 },
	{ 0x84E0F4,	IEEE_OUI28, 0x0079 },
	{ 0x885D90,	IEEE_OUI28, 0x007a },
	{ 0x885FE8,	IEEE_OUI28, 0x007b },
	{ 0x88A9A7,	IEEE_OUI28, 0x007c },
	{ 0x8C147D,	IEEE_OUI28, 0x007d },
	{ 0x8C192D,	IEEE_OUI28, 0x007e },
	{ 0x8C1CDA,	IEEE_OUI28, 0x007f },
	{ 0x8C593C,	IEEE_OUI28, 0x0080 },
	{ 0x8CC8F4,	IEEE_OUI28, 0x0081 },
	{ 0x904E91,	IEEE_OUI28, 0x0082 },
	{ 0x90C682,	IEEE_OUI28, 0x0083 },
	{ 0x9802D8,	IEEE_OUI28, 0x0084 },
	{ 0x980637,	IEEE_OUI28, 0x0085 },
	{ 0x986D35,	IEEE_OUI28, 0x0086 },
	{ 0x98AAFC,	IEEE_OUI28, 0x0087 },
	{ 0x98F9C7,	IEEE_OUI28, 0x0088 },
	{ 0x9C431E,	IEEE_OUI28, 0x0089 },
	{ 0x9C69B4,	IEEE_OUI28, 0x008a },
	{ 0x9CF6DD,	IEEE_OUI28, 0x008b },
	{ 0xA019B2,	IEEE_OUI28, 0x008c },
	{ 0xA0224E,	IEEE_OUI28, 0x008d },
	{ 0xA02833,	IEEE_OUI28, 0x008e },
	{ 0xA03E6B,	IEEE_OUI28, 0x008f },
	{ 0xA0BB3E,	IEEE_OUI28, 0x0090 },
	{ 0xA0C5F2,	IEEE_OUI28, 0x0091 },
	{ 0xA41163,	IEEE_OUI28, 0x0092 },
	{ 0xA43BFA,	IEEE_OUI28, 0x0093 },
	{ 0xA44F29,	IEEE_OUI28, 0x0094 },
	{ 0xA4580F,	IEEE_OUI28, 0x0095 },
	{ 0xA4DA22,	IEEE_OUI28, 0x0096 },
	{ 0xA4ED43,	IEEE_OUI28, 0x0097 },
	{ 0xA83FA1,	IEEE_OUI28, 0x0098 },
	{ 0xAC1DDF,	IEEE_OUI28, 0x0099 },
	{ 0xAC64DD,	IEEE_OUI28, 0x009a },
	{ 0xB01F81,	IEEE_OUI28, 0x009b },
	{ 0xB0C5CA,	IEEE_OUI28, 0x009c },
	{ 0xB0FD0B,	IEEE_OUI28, 0x009d },
	{ 0xB437D1,	IEEE_OUI28, 0x009e },
	{ 0xB44BD6,	IEEE_OUI28, 0x009f },
	{ 0xB4A2EB,	IEEE_OUI28, 0x00a0 },
	{ 0xB8D812,	IEEE_OUI28, 0x00a1 },
	{ 0xBC3400,	IEEE_OUI28, 0x00a2 },
	{ 0xBC6641,	IEEE_OUI28, 0x00a3 },
	{ 0xBC9740,	IEEE_OUI28, 0x00a4 },
	{ 0xC08359,	IEEE_OUI28, 0x00a5 },
	{ 0xC0D391,	IEEE_OUI28, 0x00a6 },
	{ 0xC47C8D,	IEEE_OUI28, 0x00a7 },
	{ 0xC4954D,	IEEE_OUI28, 0x00a8 },
	{ 0xC4FFBC,	IEEE_OUI28, 0x00a9 },
	{ 0xC82C2B,	IEEE_OUI28, 0x00aa },
	{ 0xC86314,	IEEE_OUI28, 0x00ab },
	{ 0xC88ED1,	IEEE_OUI28, 0x00ac },
	{ 0xCC1BE0,	IEEE_OUI28, 0x00ad },
	{ 0xCC2237,	IEEE_OUI28, 0x00ae },
	{ 0xCCD31E,	IEEE_OUI28, 0x00af },
	{ 0xCCD39D,	IEEE_OUI28, 0x00b0 },
	{ 0xD02212,	IEEE_OUI28, 0x00b1 },
	{ 0xD05F64,	IEEE_OUI28, 0x00b2 },
	{ 0xD07650,	IEEE_OUI28, 0x00b3 },
	{ 0xD0C857,	IEEE_OUI28, 0x00b4 },
	{ 0xD0D94F,	IEEE_OUI28, 0x00b5 },
	{ 0xD425CC,	IEEE_OUI28, 0x00b6 },
	{ 0xD47C44,	IEEE_OUI28, 0x00b7 },
	{ 0xD8860B,	IEEE_OUI28, 0x00b8 },
	{ 0xDC4427,	IEEE_OUI28, 0x00b9 },
	{ 0xDCE533,	IEEE_OUI28, 0x00ba },
	{ 0xE05A9F,	IEEE_OUI28, 0x00bb },
	{ 0xE0B6F5,	IEEE_OUI28, 0x00bc },
	{ 0xE41E0A,	IEEE_OUI28, 0x00bd },
	{ 0xE44CC7,	IEEE_OUI28, 0x00be },
	{ 0xE4956E,	IEEE_OUI28, 0x00bf },
	{ 0xE81863,	IEEE_OUI28, 0x00c0 },
	{ 0xEC9F0D,	IEEE_OUI28, 0x00c1 },
	{ 0xF023B9,	IEEE_OUI28, 0x00c2 },
	{ 0xF041C8,	IEEE_OUI28, 0x00c3 },
	{ 0xF0ACD7,	IEEE_OUI28, 0x00c4 },
	{ 0xF40E11,	IEEE_OUI28, 0x00c5 },
	{ 0xF80278,	IEEE_OUI28, 0x00c6 },
	{ 0xF81D78,	IEEE_OUI28, 0x00c7 },
	{ 0xF88A3C,	IEEE_OUI28, 0x00c8 },
	{ 0xF8B568,	IEEE_OUI28, 0x00c9 },
	{ 0xFCA47A,	IEEE_OUI28, 0x00ca },
	{ 0xFCD2B6,	IEEE_OUI28, 0x00cb },

	{ 0x14AE85,	IEEE_OUI28, 0x00cc },
	{ 0x200A0D,	IEEE_OUI28, 0x00cd },
	{ 0x28B77C,	IEEE_OUI28, 0x00ce },
	{ 0x304950,	IEEE_OUI28, 0x00cd },
	{ 0x38F7CD,	IEEE_OUI28, 0x00ce },
	{ 0x3CFAD3,	IEEE_OUI28, 0x00cf },
	{ 0x402C76,	IEEE_OUI28, 0x00d0 },
	{ 0x4C93A6,	IEEE_OUI28, 0x00d1 },
	{ 0x50DE19,	IEEE_OUI28, 0x00d2 },
	{ 0x5C857E,	IEEE_OUI28, 0x00d3 },
	{ 0x646266,	IEEE_OUI28, 0x00d4 },
	{ 0x706979,	IEEE_OUI28, 0x00d5 },
	{ 0x90E2FC,	IEEE_OUI28, 0x00d6 },
	{ 0x9405BB,	IEEE_OUI28, 0x00d7 },
	{ 0x94CC04,	IEEE_OUI28, 0x00d8 },
	{ 0x94FBA7,	IEEE_OUI28, 0x00d9 },
	{ 0xB0B353,	IEEE_OUI28, 0x00da },
	{ 0xC09BF4,	IEEE_OUI28, 0x00db },
	{ 0xCCC261,	IEEE_OUI28, 0x00dc },
	{ 0xD01411,	IEEE_OUI28, 0x00de },
	{ 0xE8B470,	IEEE_OUI28, 0x00df },
	{ 0xF0D7AF,	IEEE_OUI28, 0x00e0 },
	{ 0xF469D5,	IEEE_OUI28, 0x00e1 },
	{ 0xF490CB,	IEEE_OUI28, 0x00e2 },

	{ 0x18FDCB,	IEEE_OUI28, 0x00e3 },
	{ 0x98FC84,	IEEE_OUI28, 0x00e4 },
	{ 0xC0619A,	IEEE_OUI28, 0x00e5 },

	{ 0x58208A,	IEEE_OUI28, 0x00e5 },
	{ 0x5895D8,	IEEE_OUI28, 0x00e6 },
	{ 0x687912,	IEEE_OUI28, 0x00e7 },
	{ 0x8411C2,	IEEE_OUI28, 0x00e8 },
	{ 0x8C476E,	IEEE_OUI28, 0x00e9 },
	{ 0x8CAE49,	IEEE_OUI28, 0x00ea },
	{ 0xCC4F5C,	IEEE_OUI28, 0x00eb },
	{ 0xDC4A9E,	IEEE_OUI28, 0x00ec },
	{ 0xFCCD2F,	IEEE_OUI28, 0x00ed },

	// OUI28 (2020-11-07)
	{ 0xA0024A,	IEEE_OUI28, 0x00ee },
	{ 0xA453EE,	IEEE_OUI28, 0x00ef },

	// OUI28 (2021-01-13)
	{ 0x041119,	IEEE_OUI28, 0x00f0 },
	{ 0x245DFC,	IEEE_OUI28, 0x00f1 },
	{ 0x446FD8,	IEEE_OUI28, 0x00f2 },
	{ 0x78D4F1,	IEEE_OUI28, 0x00f3 },
	{ 0x982782,	IEEE_OUI28, 0x00f4 },
	{ 0xE86CC7,	IEEE_OUI28, 0x00f5 },

	// OUI28 (2021-05-16)
	{ 0x0C5CB5,	IEEE_OUI28, 0x00f6 },
	{ 0x1874E2,	IEEE_OUI28, 0x00f7 },
	{ 0x1CA0EF,	IEEE_OUI28, 0x00f8 },
	{ 0x20CE2A,	IEEE_OUI28, 0x00f9 },
	{ 0x44A92C,	IEEE_OUI28, 0x00fa },
	{ 0x601592,	IEEE_OUI28, 0x00fb },
	{ 0x781305,	IEEE_OUI28, 0x00fc },
	{ 0x88C9B3,	IEEE_OUI28, 0x00fd },
	{ 0xA85B36,	IEEE_OUI28, 0x00fe },
	{ 0xC0FBF9,	IEEE_OUI28, 0x00ff },
	{ 0xC8F5D6,	IEEE_OUI28, 0x0100 },
	{ 0xD09FD9,	IEEE_OUI28, 0x0101 },
	{ 0xE87829,	IEEE_OUI28, 0x0102 },
	{ 0xF02A2B,	IEEE_OUI28, 0x0103 },

	// OUI28 (2021-07-12)
	{ 0x283613,	IEEE_OUI28, 0x0104 },
	{ 0x38A8CD,	IEEE_OUI28, 0x0105 },
	{ 0x584849,	IEEE_OUI28, 0x0106 },
	{ 0x6433B5,	IEEE_OUI28, 0x0107 },
	{ 0x785EE8,	IEEE_OUI28, 0x0108 },

	// OUI28 (2021-09-19)
	{ 0x1CAE3E,	IEEE_OUI28, 0x0109 },
	{ 0x787264,	IEEE_OUI28, 0x010a },
	{ 0x7C8334,	IEEE_OUI28, 0x010b },
	{ 0x986EE8,	IEEE_OUI28, 0x010c },
	{ 0x9880BB,	IEEE_OUI28, 0x010d },
	{ 0xF4700C,	IEEE_OUI28, 0x010e },
	{ 0xF4A454,	IEEE_OUI28, 0x010f },

	// OUI28 (2021-10-19)
	{ 0x08F80D,	IEEE_OUI28, 0x0110 },
	{ 0x1845B3,	IEEE_OUI28, 0x0111 },
	{ 0x94C9B7,	IEEE_OUI28, 0x0112 },

	// OUI28 (2022-01-25)
	{ 0x04EEE8,	IEEE_OUI28, 0x0113 },
	{ 0x0826AE,	IEEE_OUI28, 0x0114 },
	{ 0x0C8629,	IEEE_OUI28, 0x0115 },
	{ 0x1054D2,	IEEE_OUI28, 0x0116 },
	{ 0x18D793,	IEEE_OUI28, 0x0117 },
	{ 0x3043D7,	IEEE_OUI28, 0x0118 },
	{ 0x50A030,	IEEE_OUI28, 0x0119 },
	{ 0x6C9308,	IEEE_OUI28, 0x011a },
	{ 0x988FE0,	IEEE_OUI28, 0x011b },
	{ 0xDC3643,	IEEE_OUI28, 0x011c },

	// OUI28 (2022-07-30)
	{ 0x0C7FED,	IEEE_OUI28, 0x011d },
	{ 0x18A59C,	IEEE_OUI28, 0x011e },
	{ 0x18C3F4,	IEEE_OUI28, 0x011f },
	{ 0x1C5974,	IEEE_OUI28, 0x0120 },
	{ 0x2C691D,	IEEE_OUI28, 0x0121 },
	{ 0x303D51,	IEEE_OUI28, 0x0122 },
	{ 0x381F26,	IEEE_OUI28, 0x0123 },
	{ 0x5847CA,	IEEE_OUI28, 0x0124 },
	{ 0x6C1524,	IEEE_OUI28, 0x0125 },
	{ 0x7050E7,	IEEE_OUI28, 0x0126 },
	{ 0x8002F4,	IEEE_OUI28, 0x0127 },
	{ 0x883CC5,	IEEE_OUI28, 0x0128 },
	{ 0x8C5109,	IEEE_OUI28, 0x0129 },
	{ 0xC48372,	IEEE_OUI28, 0x012a },
	{ 0xC49894,	IEEE_OUI28, 0x012b },
	{ 0xC4A10E,	IEEE_OUI28, 0x012c },
	{ 0xD09686,	IEEE_OUI28, 0x012d },
	{ 0xD42000,	IEEE_OUI28, 0x012e },
	{ 0xD4BABA,	IEEE_OUI28, 0x012f },
	{ 0xFC6179,	IEEE_OUI28, 0x0130 },

	// OUI28 (2022-12-04)
	{ 0x0CCC47,	IEEE_OUI28, 0x0131 },
	{ 0x4C74A7,	IEEE_OUI28, 0x0132 },
	{ 0x705A6F,	IEEE_OUI28, 0x0133 },
	{ 0x7C45F9,	IEEE_OUI28, 0x0134 },
	{ 0x84B386,	IEEE_OUI28, 0x0135 },
	{ 0x8C5DB2,	IEEE_OUI28, 0x0136 },
	{ 0xC0EAC3,	IEEE_OUI28, 0x0137 },
	{ 0xC4A559,	IEEE_OUI28, 0x0138 },
	{ 0xD46137,	IEEE_OUI28, 0x0139 },
	{ 0xE0382D,	IEEE_OUI28, 0x013a },
	{ 0xF0221D,	IEEE_OUI28, 0x013b },

	// OUI28 (2023-06-08)
	{ 0x50482C,	IEEE_OUI28, 0x013c },
	{ 0x54083B,	IEEE_OUI28, 0x013d },
	{ 0x5491AF,	IEEE_OUI28, 0x013e },
	{ 0x58C41E,	IEEE_OUI28, 0x013f },
	{ 0x5C6AEC,	IEEE_OUI28, 0x0140 },
	{ 0x68DA73,	IEEE_OUI28, 0x0141 },
	{ 0x6C2ADF,	IEEE_OUI28, 0x0142 },
	{ 0x700692,	IEEE_OUI28, 0x0143 },
	{ 0x88A6EF,	IEEE_OUI28, 0x0144 },
	{ 0x90F421,	IEEE_OUI28, 0x0145 },
	{ 0xB0FF72,	IEEE_OUI28, 0x0146 },
	{ 0xB44D43,	IEEE_OUI28, 0x0147 },
	{ 0xBC3198,	IEEE_OUI28, 0x0148 },
	{ 0xC86BBC,	IEEE_OUI28, 0x0149 },
	{ 0xD015BB,	IEEE_OUI28, 0x014a },
	{ 0xD016F0,	IEEE_OUI28, 0x014b },
	{ 0xD09395,	IEEE_OUI28, 0x014c },
	{ 0xE03C1C,	IEEE_OUI28, 0x014d },

	// OUI28 (2023-12-07)
	{ 0x008DF4,	IEEE_OUI28, 0x014e },
	{ 0x100648,	IEEE_OUI28, 0x014f },
	{ 0x28F8C6,	IEEE_OUI28, 0x0150 },
	{ 0x2CC44F,	IEEE_OUI28, 0x0151 },
	{ 0x48DA35,	IEEE_OUI28, 0x0152 },
	{ 0x4CEA41,	IEEE_OUI28, 0x0153 },
	{ 0x5CF838,	IEEE_OUI28, 0x0154 },
	{ 0x78392D,	IEEE_OUI28, 0x0155 },
	{ 0x8C5570,	IEEE_OUI28, 0x0156 },
	{ 0x8CA682,	IEEE_OUI28, 0x0157 },
	{ 0x90A9F7,	IEEE_OUI28, 0x0158 },
	{ 0xB84C87,	IEEE_OUI28, 0x0159 },
	{ 0xC022F1,	IEEE_OUI28, 0x015a },
	{ 0xC85CE2,	IEEE_OUI28, 0x015b },
	{ 0xC898DB,	IEEE_OUI28, 0x015c },
	{ 0xE8FF1E,	IEEE_OUI28, 0x015d },
	{ 0xEC9A0C,	IEEE_OUI28, 0x015e },
	{ 0xF41A79,	IEEE_OUI28, 0x015f },

	// OUI28 (2023-12-18)
	{ 0xD0A011,	IEEE_OUI28, 0x0160 },

	// OUI28 (2024-01-20)
	{ 0x34C8D6,	IEEE_OUI28, 0x0161 },
	{ 0x78E996,	IEEE_OUI28, 0x0162 },
	{ 0x80A579,	IEEE_OUI28, 0x0163 },

	// OUI28 (2024-02-14)
	{ 0x086332,	IEEE_OUI28, 0x0164 },
	{ 0x901564,	IEEE_OUI28, 0x0165 },

	// OUI28 (2024-06-05)
	{ 0x08DA33,	IEEE_OUI28, 0x0166 },
	{ 0x1063A3,	IEEE_OUI28, 0x0167 },
	{ 0x344663,	IEEE_OUI28, 0x0168 },
	{ 0x48E663,	IEEE_OUI28, 0x0169 },
	{ 0x48E6C6,	IEEE_OUI28, 0x016a },
	{ 0x5C5A4C,	IEEE_OUI28, 0x016b },
	{ 0x5C87D8,	IEEE_OUI28, 0x016c },
	{ 0x60A434,	IEEE_OUI28, 0x016d },
	{ 0xC4CC37,	IEEE_OUI28, 0x016e },
	{ 0xC8FFBF,	IEEE_OUI28, 0x016f },
	{ 0xEC5BCD,	IEEE_OUI28, 0x0170 },
	{ 0xF87A39,	IEEE_OUI28, 0x0171 },
};


/*
 * check for OUI-36/IAB
 * in:  bits_00_23
 * out: 0=not OUI-36/IAB, 1=is OUI-36/IAB
 */
int libieee_check_oui36_iab(const uint32_t bits_00_23) {
	int r = 0, i;

	DEBUGPRINT_WA(DEBUG_libieee, "called with bits_00_23=%06x", bits_00_23);

	/* run through map */
	for (i = 0; i < MAXENTRIES_ARRAY(ieee_mapping); i++) {
		DEBUGPRINT_WA(DEBUG_libieee, "check against: %06x", ieee_mapping[i].bits_00_23);

		if (ieee_mapping[i].bits_00_23 == bits_00_23) {
			if ((ieee_mapping[i].type == IEEE_IAB) || (ieee_mapping[i].type == IEEE_OUI36)) {
				DEBUGPRINT_WA(DEBUG_libieee, "found entry in map: %06x", bits_00_23);
				r = 1;
				break;
			};
		};
	};

	return(r);
};


/*
 * check for OUI-28
 * in:  bits_00_23
 * out: 0=not OUI-28, 1=is OUI-28
 */
int libieee_check_oui28(const uint32_t bits_00_23) {
	int r = 0, i;

	DEBUGPRINT_WA(DEBUG_libieee, "called with bits_00_23=%06x", bits_00_23);

	/* run through map */
	for (i = 0; i < MAXENTRIES_ARRAY(ieee_mapping); i++) {
		DEBUGPRINT_WA(DEBUG_libieee, "check against: %06x", ieee_mapping[i].bits_00_23);

		if (ieee_mapping[i].bits_00_23 == bits_00_23) {
			if (ieee_mapping[i].type == IEEE_OUI28) {
				DEBUGPRINT_WA(DEBUG_libieee, "found entry in map: %06x", bits_00_23);
				r = 1;
				break;
			};
		};
	};

	return(r);
};


/*
 * map OUI-36/IAB and OUI-28
 * in:  bits_00_23, bits_24_36
 * out: mapping value (0x1nnnnnn)
 */
uint32_t libieee_map_oui28_oui36_iab(const uint32_t bits_00_23, const uint32_t bits_24_36) {
	int i;
	uint32_t map_value = bits_00_23; // default is OUI-24

	DEBUGPRINT_WA(DEBUG_libieee, "called with bits_00_23=%06x bits_24_36=%03x", bits_00_23, bits_24_36);

	/* run through map */
	for (i = 0; i < MAXENTRIES_ARRAY(ieee_mapping); i++) {
		DEBUGPRINT_WA(DEBUG_libieee, "check against: %06x", ieee_mapping[i].bits_00_23);

		if (ieee_mapping[i].bits_00_23 == bits_00_23) {
			if ((ieee_mapping[i].type == IEEE_IAB) || (ieee_mapping[i].type == IEEE_OUI36)) {
				// hit, set flag (0x1mmmvvv), mapping number (mmm) and 12 bit vendor code (vvv)
				map_value = 0x1000000 | (ieee_mapping[i].mapping << 12) | bits_24_36;
				DEBUGPRINT_WA(DEBUG_libieee, "found entry in map (OUI-36/IAB): %06x -> %08x", bits_00_23, map_value);
				break;
			} else if (ieee_mapping[i].type == IEEE_OUI28) {
				// hit, set flag (0x1mmmv00), mapping number (mmm) and 4 bit vendor code (v)
				map_value = 0x1000000 | (ieee_mapping[i].mapping << 12) | (bits_24_36 & 0xf00);
				DEBUGPRINT_WA(DEBUG_libieee, "found entry in map (OUI-28): %06x -> %08x", bits_00_23, map_value);
				break;
			};
		};
	};

	return (map_value);
};

/*
 * unmap OUI-36/IAB and OUI-28
 * in : mapping value
 * out: bits_00_23, bits_24_36
 */
int libieee_unmap_oui28_oui36_iab(const uint32_t map_value, uint32_t *bits_00_23_p, uint32_t *bits_24_36_p) {
	int i;
	uint32_t map_index = (map_value & 0xfff000) >> 12;

	*bits_00_23_p = 0;
	*bits_24_36_p = 0;

	DEBUGPRINT_WA(DEBUG_libieee, "called with map_value=%08x", map_value);

	/* run through map */
	for (i = 0; i < MAXENTRIES_ARRAY(ieee_mapping); i++) {
		if (ieee_mapping[i].mapping == map_index) {
			DEBUGPRINT_WA(DEBUG_libieee, "found entry in map: %06x", map_index);

			*bits_00_23_p = ieee_mapping[i].bits_00_23;
			*bits_24_36_p = map_value & 0xfff;

			DEBUGPRINT_WA(DEBUG_libieee, "mapped to bits_00_23=%06x bits_24_36=%03x", *bits_00_23_p, *bits_24_36_p);

			return (0);
		};
	};

	fprintf(stderr, "%s/%s: missing entry in map: %06x\n", __FILE__, __func__, map_index);
	return (1);
};

/*
 * map IAB/OUI-36/OUI-28
 * in:  macaddrp
 * out: OUI (0x0......) or mapped IAB/OUI-36/OUI-28 (0x1mmm...)
 *	mmm = map index
 */
uint32_t libieee_map_oui_macaddr(const ipv6calc_macaddr *macaddrp) {
	uint32_t oui, ven;

	oui = (macaddrp->addr[0] << 16) | (macaddrp->addr[1] << 8) | macaddrp->addr[2];
	ven = (macaddrp->addr[3] << 4)  | (macaddrp->addr[4] >> 4); // 12 bits

	DEBUGPRINT_WA(DEBUG_libieee, "called with OUI/VEN: %06x %03x", oui, ven);

	return (libieee_map_oui28_oui36_iab(oui, ven));
};

/*
 * map IAB/OUI-36/OUI-28
 * in:  eui64addrp
 * out: OUI (0x0......) or mapped IAB/OUI-36/OUI-28 (0x1mmm...)
 *	mmm = map index
 */
uint32_t libieee_map_oui_eui64addr(const ipv6calc_eui64addr *eui64addrp) {
	uint32_t oui, ven;

	oui = (eui64addrp->addr[0] << 16) | (eui64addrp->addr[1] << 8) | eui64addrp->addr[2];
	ven = (eui64addrp->addr[3] << 4)  | (eui64addrp->addr[4] >> 4);

	DEBUGPRINT_WA(DEBUG_libieee, "called with OUI/VEN: %06x %03x", oui, ven);

	return (libieee_map_oui28_oui36_iab(oui, ven));
};

/*
 * unmap IAB/OUI-36/OUI-28
 * in : OUI (0x0......) or mapped IAB/OUI-36/OUI-28 (0x1mmm...)
 *	mmm = map index
 * mod: macaddrp (last bits zeroized)
 * out: result
 */
int libieee_unmap_oui_macaddr(ipv6calc_macaddr *macaddrp, uint32_t map_value) {
	uint32_t bits_00_23, bits_24_36;

	mac_clearall(macaddrp);

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	libieee_unmap_oui28_oui36_iab(map_value, &bits_00_23, &bits_24_36);

	macaddrp->addr[0] = (bits_00_23 & 0xff0000) >> 16;
	macaddrp->addr[1] = (bits_00_23 & 0x00ff00) >>  8;
	macaddrp->addr[2] = (bits_00_23 & 0x0000ff);
	macaddrp->addr[3] = (bits_24_36 & 0x000ff0) >>  4;
	macaddrp->addr[4] = (bits_24_36 & 0x00000f) <<  4;
	macaddrp->flag_valid = 1;

	return (0);
};


/*
 * unmap IAB/OUI-36/OUI-28
 * in : OUI (0x0......) or mapped IAB/OUI-36/OUI-28 (0x1mmm...)
 *	mmm = map index
 * mod: eui64addrp (last bits zeroized)
 * out: result
 */
int libieee_unmap_oui_eui64addr(ipv6calc_eui64addr *eui64addrp, uint32_t map_value) {
	uint32_t bits_00_23, bits_24_36;

	libeui64_clearall(eui64addrp);

	DEBUGPRINT_NA(DEBUG_libieee, "called");

	libieee_unmap_oui28_oui36_iab(map_value, &bits_00_23, &bits_24_36);

	eui64addrp->addr[0] = (bits_00_23 & 0xff0000) >> 16;
	eui64addrp->addr[1] = (bits_00_23 & 0x00ff00) >>  8;
	eui64addrp->addr[2] = (bits_00_23 & 0x0000ff);
	eui64addrp->addr[3] = (bits_24_36 & 0x000ff0) >>  4;
	eui64addrp->addr[4] = (bits_24_36 & 0x00000f) <<  4;
	eui64addrp->flag_valid = 1;

	return (0);
};
