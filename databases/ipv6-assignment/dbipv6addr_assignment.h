/*
 * Project       : ipv6calc
 * File          : dbipv6_assignment.h
 * Version       : $Id: dbipv6addr_assignment.h,v 1.65 2015/02/11 07:35:35 ds6peter Exp $
 * Generated     : 20150211-080511+0100 CET
 * Data copyright: IANA ARIN RIPENCC APNIC LACNIC AFRINIC
 *
 * Information:
 *  Additional header file for databases/lib/libipv6calc_db_wrapper_BuiltIn.c
 */

#include "databases/lib/libipv6calc_db_wrapper_BuiltIn.h"

/*@unused@*/ static const char* dbipv6addr_registry_status __attribute__ ((__unused__)) = "AFRINIC/20150211 APNIC/20150210 ARIN/20150210 IANA/20140520 LACNIC/20150210 RIPENCC/20150210";
/*@unused@*/ static const time_t dbipv6addr_registry_unixtime __attribute__ ((__unused__)) = 1423638311;

static const s_ipv6addr_assignment dbipv6addr_assignment[] = {
	//ipv6_00_31, ipv6_32_63, mask_00_31, mask_32_63, mask_length, registry  
	{ 0x20010000, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_IANA       },
	{ 0x20010200, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_APNIC      },
	{ 0x20010400, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_ARIN       },
	{ 0x20010600, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x200107fa, 0x00000001, 0xffffffff, 0xffffffff,  64, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00000002, 0xffffffff, 0xffffffff,  64, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00000003, 0xffffffff, 0xffffffff,  64, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00010000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00020000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00030000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00040000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00050000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00060000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00070000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00080000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00090000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x000a0000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x000b0000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x000c0000, 0xffffffff, 0xfffe0000,  47, REGISTRY_APNIC      },
	{ 0x200107fa, 0x000e0000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x000f0000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00100000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x200107fa, 0x00110000, 0xffffffff, 0xffff0000,  48, REGISTRY_APNIC      },
	{ 0x20010800, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20010a00, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20010c00, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_APNIC      },
	{ 0x20010e00, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_APNIC      },
	{ 0x20011200, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_LACNIC     },
	{ 0x20011400, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20011600, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20011800, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_ARIN       },
	{ 0x20011a00, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20011c00, 0x00000000, 0xfffffc00, 0x00000000,  22, REGISTRY_RIPENCC    },
	{ 0x20012000, 0x00000000, 0xfffff000, 0x00000000,  20, REGISTRY_RIPENCC    },
	{ 0x20013000, 0x00000000, 0xfffff800, 0x00000000,  21, REGISTRY_RIPENCC    },
	{ 0x20013800, 0x00000000, 0xfffffc00, 0x00000000,  22, REGISTRY_RIPENCC    },
	{ 0x20013c00, 0x00000000, 0xfffffc00, 0x00000000,  22, REGISTRY_IANA       },
	{ 0x20014000, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20014200, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_AFRINIC    },
	{ 0x20014400, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_APNIC      },
	{ 0x20014600, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20014800, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_ARIN       },
	{ 0x20014a00, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20014c00, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_RIPENCC    },
	{ 0x20015000, 0x00000000, 0xfffff000, 0x00000000,  20, REGISTRY_RIPENCC    },
	{ 0x20018000, 0x00000000, 0xffffe000, 0x00000000,  19, REGISTRY_APNIC      },
	{ 0x2001a000, 0x00000000, 0xfffff000, 0x00000000,  20, REGISTRY_APNIC      },
	{ 0x2001b000, 0x00000000, 0xfffff000, 0x00000000,  20, REGISTRY_APNIC      },
	{ 0x20020000, 0x00000000, 0xffff0000, 0x00000000,  16, REGISTRY_6TO4       },
	{ 0x20030000, 0x00000000, 0xffffc000, 0x00000000,  18, REGISTRY_RIPENCC    },
	{ 0x24000000, 0x00000000, 0xfff00000, 0x00000000,  12, REGISTRY_APNIC      },
	{ 0x26000000, 0x00000000, 0xfff00000, 0x00000000,  12, REGISTRY_ARIN       },
	{ 0x26100000, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_ARIN       },
	{ 0x26200000, 0x00000000, 0xfffffe00, 0x00000000,  23, REGISTRY_ARIN       },
	{ 0x28000000, 0x00000000, 0xfff00000, 0x00000000,  12, REGISTRY_LACNIC     },
	{ 0x2a000000, 0x00000000, 0xfff00000, 0x00000000,  12, REGISTRY_RIPENCC    },
	{ 0x2c000000, 0x00000000, 0xfff00000, 0x00000000,  12, REGISTRY_AFRINIC    },
	{ 0x2d000000, 0x00000000, 0xff000000, 0x00000000,   8, REGISTRY_IANA       },
	{ 0x2e000000, 0x00000000, 0xfe000000, 0x00000000,   7, REGISTRY_IANA       },
	{ 0x30000000, 0x00000000, 0xf0000000, 0x00000000,   4, REGISTRY_IANA       },
	{ 0x3ffe0000, 0x00000000, 0xffff0000, 0x00000000,  16, REGISTRY_IANA       },
	{ 0x5f000000, 0x00000000, 0xff000000, 0x00000000,   8, REGISTRY_IANA       },
};
	