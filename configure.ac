dnl Project  : ipv6calc
dnl File     : configure.ac
dnl Copyright: 2001-2025 by Peter Bieringer <pb (at) bieringer.de>

dnl Process this file with autoconf to produce a configure script.

AC_INIT([ipv6calc],[4.2.2],[ipv6calc@deepspace6.net],[ipv6calc],[https://www.deepspace6.net/projects/ipv6calc.html])

COPYRIGHT_YEAR=2025

dnl Program/library version (align with version above)
PACKAGE_VERSION_MAJOR=4
PACKAGE_VERSION_MINOR=2
PACKAGE_VERSION_BUGFIX=2

dnl API version
API_VERSION_MAJOR=4
API_VERSION_MINOR=2
API_VERSION_BUGFIX=0


dnl Generic data definitions
AC_SUBST(COPYRIGHT_YEAR)
AC_DEFINE_UNQUOTED(COPYRIGHT_YEAR, "$COPYRIGHT_YEAR", [Copyright year])

dnl Program/library version variants
PACKAGE_VERSION_NUMERIC=$(echo "$PACKAGE_VERSION_MAJOR $PACKAGE_VERSION_MINOR $PACKAGE_VERSION_BUGFIX" | awk '{ print ( $1 * 1000 + $2 ) * 1000 + $3 }')
PACKAGE_VERSION_STRING=$(echo "$PACKAGE_VERSION_MAJOR.$PACKAGE_VERSION_MINOR.$PACKAGE_VERSION_BUGFIX")

AC_DEFINE_UNQUOTED(IPV6CALC_PACKAGE_VERSION_MAJOR,   $PACKAGE_VERSION_MAJOR   , "Package version Major")
AC_DEFINE_UNQUOTED(IPV6CALC_PACKAGE_VERSION_MINOR,   $PACKAGE_VERSION_MINOR   , "Package version Minor")
AC_DEFINE_UNQUOTED(IPV6CALC_PACKAGE_VERSION_BUGFIX,  $PACKAGE_VERSION_BUGFIX  , "Package version Bugfix")
AC_DEFINE_UNQUOTED(IPV6CALC_PACKAGE_VERSION_STRING,  "$PACKAGE_VERSION_STRING", "Package version as string")
AC_DEFINE_UNQUOTED(IPV6CALC_PACKAGE_VERSION_NUMERIC, $PACKAGE_VERSION_NUMERIC , "Package version numeric")

dnl API version variants
API_VERSION_NUMERIC=$(echo "$API_VERSION_MAJOR $API_VERSION_MINOR $API_VERSION_BUGFIX" | awk '{ print ( $1 * 1000 + $2 ) * 1000 + $3 }')
API_VERSION_STRING=$(echo "$API_VERSION_MAJOR.$API_VERSION_MINOR.$API_VERSION_BUGFIX")

AC_DEFINE_UNQUOTED(IPV6CALC_API_VERSION_MAJOR,   $API_VERSION_MAJOR   , "API version Major")
AC_DEFINE_UNQUOTED(IPV6CALC_API_VERSION_MINOR,   $API_VERSION_MINOR   , "API version Minor")
AC_DEFINE_UNQUOTED(IPV6CALC_API_VERSION_BUGFIX,  $API_VERSION_BUGFIX  , "API version Bugfix")
AC_DEFINE_UNQUOTED(IPV6CALC_API_VERSION_STRING,  "$API_VERSION_STRING", "API version as string")
AC_DEFINE_UNQUOTED(IPV6CALC_API_VERSION_NUMERIC, $API_VERSION_NUMERIC , "API version numeric")

dnl Define version header
AC_CONFIG_HEADERS([config.h])

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CC
dnl AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_MAKE_SET

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_C_BIGENDIAN

dnl Default extra CFLAGS
dnl  -D__EXTENSIONS__ is for Solaris & strtok_r
dnl  -fPIC is for shared library support
CFLAGS_EXTRA="-fPIC -D__EXTENSIONS__"

dnl Default CFLAGS
if test "$CFLAGS" = ""; then
CFLAGS="-O2 -Wall -g"
fi

dnl *************************************************
dnl 32-bit mode
dnl *************************************************
AC_ARG_ENABLE(
	[m32],
	AS_HELP_STRING([--enable-m32],
		[Enable 32-bit compilation]),
	[
		if test "$enable_m32" = "yes"; then
			AC_MSG_RESULT([*** switch to 32-bit compilation])
			CFLAGS="${CFLAGS:+$CFLAGS }-m32 -DMMDB_UINT128_IS_BYTE_ARRAY=1"
			LDFLAGS="${LDFLAGS:+$LDFLAGS }-m32"
		fi
	])


dnl *************************************************
dnl check for compiler options for several -W*
dnl *************************************************
AC_ARG_ENABLE([compiler-warning-to-error], 
	AS_HELP_STRING([--disable-compiler-warning-to-error],
		[Disable extra CFLAGS for mapping warnings to errors during compilation]),
	[
	],[
		AC_MSG_RESULT([**** analyze compiler options: CC=$CC])

		AC_MSG_RESULT([*** check for CFLAGS: -Werror=format-security])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Wformat -Werror -Werror=format-security"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Werror=format-security])
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Wformat -Werror -Werror=format-security"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Werror=format-security])
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Wformat"
			])
		CFLAGS="$CFLAGS_SAVE"

		AC_MSG_RESULT([*** check for CFLAGS: -Wsizeof-array-argument])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Wsizeof-array-argument"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Wsizeof-array-argument])
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Wsizeof-array-argument"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Wsizeof-array-argument])
			])
		CFLAGS="$CFLAGS_SAVE"

		AC_MSG_RESULT([*** check for CFLAGS: -Wparentheses-equality])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Wparentheses-equality"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Wparentheses-equality])
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Wparentheses-equality"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Wparentheses-equality])
			])
		CFLAGS="$CFLAGS_SAVE"

		AC_MSG_RESULT([*** check for CFLAGS: -Wextra])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Wextra"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Wextra])
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Wextra"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Wextra])
			])
		CFLAGS="$CFLAGS_SAVE"

		AC_MSG_RESULT([*** check for CFLAGS: -Werror=format-truncation=1])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Werror -Werror=format-truncation=1"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Werror=format-truncation=1])
				# enable errors related to format-truncation
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Werror=format-truncation=1"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Werror=format-truncation=1])
			])
		CFLAGS="$CFLAGS_SAVE"

		AC_MSG_RESULT([*** check for CFLAGS: -Werror=char-subscripts])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Werror -Werror=char-subscripts"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Werror=char-subscripts])
				# disable errors related to format-truncation
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Werror=char-subscripts"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Werror=char-subscripts])
			])
		CFLAGS="$CFLAGS_SAVE"

		AC_MSG_RESULT([*** check for CFLAGS: -Wno-deprecated-declarations])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Werror -Wno-deprecated-declarations"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Wno-deprecated-declarations])
				# disable errors related to deprecated-declarations
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Wno-deprecated-declarations"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Wno-deprecated-declarations])
			])
		CFLAGS="$CFLAGS_SAVE"

		AC_MSG_RESULT([*** check for CFLAGS: -Wdate-time])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Werror -Wdate-time"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Wdate-time])
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Wdate-time"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Wdate-time])
			])
		CFLAGS="$CFLAGS_SAVE"

		AC_MSG_RESULT([*** check for CFLAGS: -Os])
		CFLAGS_SAVE="$CFLAGS"
		CFLAGS="$CFLAGS -Werror -Os"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
			     [[int a=1;]])
			],
			[
				AC_MSG_RESULT([*** CFLAGS supported: -Os])
				CFLAGS_EXTRA="${CFLAGS_EXTRA:+$CFLAGS_EXTRA }-Os"
			],
			[
				AC_MSG_WARN([*** missing CFLAGS support: -Os])
			])
		CFLAGS="$CFLAGS_SAVE"
	])

dnl *************************************************
dnl add -Wno-unused-const-variable if supported
dnl *************************************************
dnl AC_MSG_RESULT([*** check for CFLAGS: -Wno-unused-const-variable])
dnl CFLAGS_SAVE="$CFLAGS"
dnl CFLAGS="$CFLAGS -Wno-unused-const-variable"
dnl AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
dnl 	     [[int a=1;]])
dnl 	],
dnl 	[
dnl 		AC_MSG_RESULT([*** CFLAGS supported: -Wno-unused-const-variable])
dnl 		CFLAGS_EXTRA="-Wno-unused-const-variable"
dnl 	],
dnl 	[
dnl 		AC_MSG_WARN([*** missing CFLAGS support: -Wno-unused-const-variable])
dnl 		CFLAGS_EXTRA="-Wformat"
dnl 	])
dnl CFLAGS="$CFLAGS_SAVE"


dnl *************************************************
dnl getopt library
dnl *************************************************
ENABLE_BUNDLED_GETOPT=0
AC_ARG_ENABLE(
	[bundled-getopt],
	AS_HELP_STRING([--enable-bundled-getopt],
		[Enable bundled getopt library (default: autoselected)]),
	[
		if test "$enable_bundled_getopt" = "yes"; then
			AC_MSG_RESULT([*** use of bundled getopt library forced])
			ENABLE_BUNDLED_GETOPT=1
		else
			AC_MSG_RESULT([*** use of system getopt library forced])
		fi
	],
	[
		AC_CHECK_FUNC(getopt_long,
			[
				AC_MSG_RESULT([*** use of system getopt library autoselected])
			],
			[
				AC_MSG_RESULT([*** use of bundled getopt library autoselected])
				ENABLE_BUNDLED_GETOPT=1
			])
	])

if test "$ENABLE_BUNDLED_GETOPT" = "1"; then
	AC_LIBOBJ(../getopt/getopt)
	AC_LIBOBJ(../getopt/getopt1)
	GETOPT_INCLUDE=-I../getopt
	AC_DEFINE(ENABLE_BUNDLED_GETOPT, 1, Define if bundled GETOPT should be used)
fi

AC_SUBST(GETOPT_INCLUDE)
AC_SUBST(ENABLE_BUNDLED_GETOPT)


dnl *************************************************
dnl MD5 library
dnl *************************************************
ENABLE_BUNDLED_MD5=0
HAVE_OPENSSL_EVP_MD5=0
HAVE_OPENSSL_MD5=0
HAVE_LIBMD_MD5=0
ENABLE_OPENSSL_EVP_MD5=0
ENABLE_OPENSSL_MD5=0
ENABLE_LIBMD_MD5=0

dnl Check capabilities
AC_MSG_RESULT([*** check for libmd MD5])
AC_CHECK_HEADER(md5.h, [
	AC_MSG_RESULT([libmd/md5.h header file found])
	AC_CHECK_LIB(md, MD5Init,
	[
		LDFLAGS_ORIG="$LDFLAGS"
		LDFLAGS="-lmd $LDFLAGS"
		HAVE_LIBMD_MD5=1
		AC_CHECK_FUNCS(MD5Init MD5Update MD5Final,
		[
		],[
			AC_MSG_RESULT([libmd crypto library supports MD5])
			HAVE_OPENSSL_MD5=0
		])
		LDFLAGS="$LDFLAGS_ORIG"

		if test "$HAVE_LIBMD_MD5" = "1"; then
			AC_MSG_RESULT([*** libmd crypto library supports MD5])
		fi
	],[
		AC_MSG_WARN([libmd crypto library DOES NOT support MD5])
	])
],[
	AC_MSG_WARN([libmd/md5.h header file not found])
])

AC_MSG_RESULT([*** check for OpenSSL EVP MD5])
AC_CHECK_HEADER(openssl/evp.h, [
	AC_MSG_RESULT([openssl/evp.h header file found])
	AC_CHECK_LIB(crypto, EVP_DigestInit_ex,
	[
		LDFLAGS_ORIG="$LDFLAGS"
		LDFLAGS="-lcrypto $LDFLAGS"
		HAVE_OPENSSL_EVP_MD5=1
		HAVE_OPENSSL_EVP_MD5_LEGACY=1

		dnl OpenSSL 3.x
		AC_CHECK_FUNCS(EVP_MD_CTX_new EVP_md5 EVP_DigestInit_ex EVP_DigestUpdate EVP_DigestFinal_ex EVP_MD_CTX_free,
		[
		],[
			AC_MSG_WARN([OpenSSL crypto library DOES NOT support MD5 via EVP])
			HAVE_OPENSSL_EVP_MD5=0
		])

		dnl OpenSSL 1.x
		AC_CHECK_FUNCS(EVP_MD_CTX_init EVP_md5 EVP_DigestInit_ex EVP_DigestUpdate EVP_DigestFinal_ex,
		[
		],[
			AC_MSG_WARN([OpenSSL crypto library DOES NOT support MD5 via EVP LEGACY])
			HAVE_OPENSSL_EVP_MD5_LEGACY=0
		])

		LDFLAGS="$LDFLAGS_ORIG"

		if test "$HAVE_OPENSSL_EVP_MD5" = "1"; then
			AC_MSG_RESULT([*** OpenSSL crypto library supports MD5 via EVP implementation])
		fi
	],[
		AC_MSG_WARN([OpenSSL crypto library DOES NOT support MD5 via EVP])
	])
],[
	AC_MSG_WARN([openssl/evp.h header file not found])
])

AC_MSG_RESULT([*** check for OpenSSL legacy MD5])
AC_CHECK_HEADER(openssl/md5.h, [
	AC_MSG_RESULT([openssl/md5.h header file found])
	AC_CHECK_LIB(crypto, MD5_Init,
	[
		LDFLAGS_ORIG="$LDFLAGS"
		LDFLAGS="-lcrypto $LDFLAGS"
		HAVE_OPENSSL_MD5=1
		AC_CHECK_FUNCS(MD5_Init MD5_Update MD5_Final,
		[
		],[
			AC_MSG_RESULT([OpenSSL crypto library supports MD5 via legacy implementation])
			HAVE_OPENSSL_MD5=0
		])
		LDFLAGS="$LDFLAGS_ORIG"

		if test "$HAVE_OPENSSL_MD5" = "1"; then
			AC_MSG_RESULT([*** OpenSSL crypto library supports MD5 via legacy implementation])
		fi
	],[
		AC_MSG_WARN([OpenSSL crypto library DOES NOT support MD5 via legacy implementation])
	])
],[
	AC_MSG_WARN([openssl/md5.h header file not found])
])

AC_ARG_ENABLE(
	[bundled-md5],
	AS_HELP_STRING([--enable-bundled-md5],
		[Enable bundled MD5 library (default: autoselected)]),
	[
		if test "$enable_bundled_md5" = "yes"; then
			AC_MSG_RESULT([*** use of bundled MD5 library forced])
			ENABLE_BUNDLED_MD5=1
		else
			AC_MSG_RESULT([*** use of OpenSSL MD5 library forced])
		fi
	],[
		if test "$HAVE_OPENSSL_EVP_MD5" = "0"; then
			if test "$HAVE_OPENSSL_MD5" = "0"; then
				AC_MSG_RESULT([*** use of bundled MD5 library autoselected because no OpenSSL support found])
				ENABLE_BUNDLED_MD5=1
			fi
		fi
	]
)

if test "$HAVE_OPENSSL_EVP_MD5" = "1"; then
	# precedence
	ENABLE_OPENSSL_EVP_MD5=1
elif test "$HAVE_OPENSSL_EVP_MD5_LEGACY" = "1"; then
	# precedence
	ENABLE_OPENSSL_EVP_MD5=1
else
	if test "$HAVE_OPENSSL_MD5" = "1"; then
		# fallback
		ENABLE_OPENSSL_MD5=1
	fi
fi

AC_ARG_ENABLE(
	[libmd-md5],
	AS_HELP_STRING([--enable-libmd-md5],
		[Enforce use of libmd MD5 implementation (default: autoselected)]),
	[
		if test "$enable_libmd_md5" = "yes"; then
			AC_MSG_RESULT([*** use of libmd MD5 implementation forced])
			ENABLE_LIBMD_MD5=1
			ENABLE_OPENSSL_MD5=0
			ENABLE_OPENSSL_EVP_MD5=0
		fi
	]
)

AC_ARG_ENABLE(
	[openssl-evp-md5],
	AS_HELP_STRING([--enable-openssl-evp-md5],
		[Enforce use of OpenSSL EVP MD5 implementation (default: autoselected)]),
	[
		if test "$enable_openssl_evp_md5" = "yes"; then
			AC_MSG_RESULT([*** use of OpenSSL EVP MD5 implementation forced])
			ENABLE_LIBMD_MD5=0
			ENABLE_OPENSSL_EVP_MD5=1
			ENABLE_OPENSSL_MD5=0
		fi
	]
)

AC_ARG_ENABLE(
	[openssl-md5],
	AS_HELP_STRING([--enable-openssl-md5],
		[Enforce use of OpenSSL legacy MD5 implementation (default: autoselected)]),
	[
		if test "$enable_openssl_md5" = "yes"; then
			AC_MSG_RESULT([*** use of OpenSSL legacy MD5 implementation forced])
			ENABLE_LIBMD_MD5=0
			ENABLE_OPENSSL_EVP_MD5=0
			ENABLE_OPENSSL_MD5=1
		fi
	]
)

if test "$ENABLE_BUNDLED_MD5" = "1"; then
	AC_LIBOBJ(../md5/md5)
	MD5_INCLUDE=-I../md5
	AC_DEFINE(ENABLE_BUNDLED_MD5, 1, Define if bundled MD5 should be used)
else
	if test "$ENABLE_LIBMD_MD5" = "1"; then
		LDFLAGS_EXTRA="${LDFLAGS_EXTRA:+$LDFLAGS_EXTRA }-lmd"
		LDFLAGS_EXTRA_MOD_IPV6CALC="-lmd $LDFLAGS_EXTRA_MOD_IPV6CALC"
		LDFLAGS_EXTRA_STATIC="${LDFLAGS_EXTRA_STATIC:+$LDFLAGS_EXTRA_STATIC }-lmd -lz -ldl -lpthread -lc"

		AC_DEFINE(ENABLE_LIBMD_MD5, 1, Define if libmd MD5 should be used)
		AC_MSG_RESULT([*** use of libmd MD5 implementation])
	fi

	if test "$ENABLE_OPENSSL_EVP_MD5" = "1"; then
		LDFLAGS_EXTRA="${LDFLAGS_EXTRA:+$LDFLAGS_EXTRA }-lcrypto"
		LDFLAGS_EXTRA_MOD_IPV6CALC="-lcrypto $LDFLAGS_EXTRA_MOD_IPV6CALC"
		LDFLAGS_EXTRA_STATIC="${LDFLAGS_EXTRA_STATIC:+$LDFLAGS_EXTRA_STATIC }-lcrypto -lz -ldl -lpthread -lc"

		AC_DEFINE(ENABLE_OPENSSL_EVP_MD5, 1, Define if OpenSSL EVP MD5 should be used)
		AC_MSG_RESULT([*** use of OpenSSL EVP MD5 implementation])
	fi

	if test "$ENABLE_OPENSSL_MD5" = "1"; then
		LDFLAGS_EXTRA="${LDFLAGS_EXTRA:+$LDFLAGS_EXTRA }-lcrypto"
		LDFLAGS_EXTRA_MOD_IPV6CALC="-lcrypto $LDFLAGS_EXTRA_MOD_IPV6CALC"
		LDFLAGS_EXTRA_STATIC="${LDFLAGS_EXTRA_STATIC:+$LDFLAGS_EXTRA_STATIC }-lcrypto -lz -ldl -lpthread -lc"

		AC_DEFINE(ENABLE_OPENSSL_MD5, 1, Define if OpenSSL legacy MD5 should be used)
		AC_MSG_RESULT([*** use of OpenSSL legacy MD5 implementation])
	fi
fi

AC_SUBST(MD5_INCLUDE)
AC_SUBST(ENABLE_BUNDLED_MD5)
AC_SUBST(ENABLE_LIBMD_MD5)
AC_SUBST(ENABLE_OPENSSL_EVP_MD5)
AC_SUBST(ENABLE_OPENSSL_MD5)


dnl *************************************************
dnl defaults
dnl *************************************************
geoip_db_default="/usr/share/GeoIP"
ip2location_db_default="/usr/share/IP2Location"
dbip_db_default="/usr/share/DBIP"
external_db_default="/usr/share/ipv6calc/db"

mmdb_dyn_lib_default="libmaxminddb.so.0"
ip2location_dyn_lib_default="libIP2Location.so.3"


dnl *************************************************
dnl Create binaries ushing shared library
dnl *************************************************
AC_ARG_ENABLE(
	[shared],
	AS_HELP_STRING([--enable-shared],
		[Enable shared library build (default: disabled)])
	)

if test "$enable_shared" = "yes"; then
	AC_MSG_RESULT([*** Shared library build enabled])
	SHARED_LIBRARY="yes"
	AC_DEFINE(SHARED_LIBRARY, 1, Define if you want ipv6calc binaries compiled with shared library libipv6calc)
	LDFLAGS_EXTRA_MOD_IPV6CALC="-Wl,../lib/libipv6calc.so.$PACKAGE_VERSION -Wl,../databases/lib/libipv6calc_db_wrapper.so.$PACKAGE_VERSION $LDFLAGS_EXTRA"
	LDFLAGS_EXTRA="../lib/libipv6calc.so.$PACKAGE_VERSION ../databases/lib/libipv6calc_db_wrapper.so.$PACKAGE_VERSION $LDFLAGS_EXTRA"
	LD_LIBRARY_PATH=../lib:../databases/lib:$LD_LIBRARY_PATH
	LD_LIBRARY_PATH_BASIC=../lib:../databases/lib:$LD_LIBRARY_PATH_BASIC
else
	SHARED_LIBRARY="no"
	IPV6CALC_LIB="-L../lib/ -L../databases/lib/ -lipv6calc_db_wrapper -lipv6calc"
fi

AC_SUBST(SHARED_LIBRARY)
AC_SUBST(IPV6CALC_LIB)


dnl *************************************************
dnl Check for dynamic loading of libraries capability
dnl *************************************************
AC_CHECK_HEADER(dlfcn.h,
	[
		AC_MSG_RESULT([*** Dynamic loading of libraries is SUPPORTED])
		DYNAMIC_LOAD_SUPPORT="yes"
	],
	[
		DYNAMIC_LOAD_SUPPORT="no"
		AC_MSG_WARN(["Dynamic loading of libraries is not supported, no header file found"])
	])

AC_ARG_ENABLE(
	[dynamic-load],
	AS_HELP_STRING([--disable-dynamic-load],
	               [Disable dynamic load of libraries (default: enabled)])
	)

if test "$enable_dynamic_load" = "yes"; then
	DYNAMIC_LOAD="no"
else
	DYNAMIC_LOAD="yes"
fi

if test "$DYNAMIC_LOAD_SUPPORT" = "yes"; then
	if test "$DYNAMIC_LOAD" = "no"; then
		AC_MSG_WARN(["Dynamic loading of libraries forced to be disabled"])
	fi
fi

dnl *************************************************
dnl internal databases default
dnl *************************************************
BUILTIN="no"

dnl *************************************************
dnl disable built-in database IEEE
dnl *************************************************
AC_ARG_ENABLE([db-ieee], 
	AS_HELP_STRING([--disable-db-ieee],
		[Disable built-in IEEE database (default: enabled)]),
	[
		DB_IEEE="$enable_db_ieee"
	],[
		DB_IEEE="yes"
		BUILTIN="yes"
	])

if test "$DB_IEEE" = "yes"; then
	AC_DEFINE(SUPPORT_DB_IEEE, 1, Define if you want IEEE database included.)
fi


dnl *************************************************
dnl disable built-in database IPv4
dnl *************************************************
AC_ARG_ENABLE([db-ipv4], 
	AS_HELP_STRING([--disable-db-ipv4],
		[Disable built-in IPv4-to-Registry database (default: enabled)]),
	[
		DB_IPV4="$enable_db_ipv4"
	],[
		DB_IPV4="yes"
		BUILTIN="yes"
	])

if test "$DB_IPV4" = "yes"; then
	AC_DEFINE(SUPPORT_DB_IPV4_REG, 1, Define if you want IPv4-to-Registry database included.)
fi


dnl *************************************************
dnl disable built-in database IPv6
dnl *************************************************
AC_ARG_ENABLE([db-ipv6], 
	AS_HELP_STRING([--disable-db-ipv6],
	               [Disable built-in IPv6-to-Registry database (default: enabled)]),
	[
		DB_IPV6="$enable_db_ipv6"
	],
	[
		DB_IPV6="yes"
		BUILTIN="yes"
	])

if test "$DB_IPV6" = "yes"; then
	AC_DEFINE(SUPPORT_DB_IPV6_REG, 1, Define if you want IPv6-to-Registry database included.)
fi


dnl *************************************************
dnl disable built-in database AS->Registry
dnl *************************************************
AC_ARG_ENABLE([db-as-registry], 
	AS_HELP_STRING([--disable-db-as-registry],
	               [Disable built-in AS-to-Registry database (default: enabled)]),
	[
		DB_AS_REG="$enable_db_as_registry"
	],
	[
		DB_AS_REG="yes"
		BUILTIN="yes"
	])

if test "$DB_AS_REG" = "yes"; then
	AC_DEFINE(SUPPORT_DB_AS_REG, 1, Define if you want AS-to-Registry database included.)
fi


dnl *************************************************
dnl disable built-in database CC->Registry
dnl *************************************************
AC_ARG_ENABLE([db-cc-registry], 
	AS_HELP_STRING([--disable-db-cc-registry],
	               [Disable built-in CountryCode-to-Registry database (default: enabled)]),
	[
		DB_CC_REG="$enable_db_cc_registry"
	],
	[
		DB_CC_REG="yes"
		BUILTIN="yes"
	])

if test "$DB_CC_REG" = "yes"; then
	AC_DEFINE(SUPPORT_DB_CC_REG, 1, Define if you want CC-to-Registry database included.)
fi


dnl *************************************************
dnl Built-in database support (catch-all)
dnl *************************************************
AC_ARG_ENABLE([builtin],
	AS_HELP_STRING([--disable-builtin],
	               [Disable all built-in databases (default: enabled)]),
	[
		DB_CC_REG="$enable_builtin"
		DB_AS_REG="$enable_builtin"
		DB_IEEE="$enable_builtin"
		DB_IPV4="$enable_builtin"
		DB_IPV6="$enable_builtin"
	]
)



dnl *************************************************
dnl enable built-in database support in general
dnl *************************************************
if test "$BUILTIN" = "yes"; then
	AC_DEFINE(SUPPORT_BUILTIN, 1, Define if you want general BuiltIn database included.)
fi


dnl *************************************************
dnl External ipv6calc db support
dnl *************************************************
AC_ARG_ENABLE([external],
	AS_HELP_STRING([--enable-external],
	               [Enable external db support (default: disabled)]),
	[
		EXTERNAL="$enable_external"
	],
	[
		EXTERNAL="no"
	])

dnl defaults for database directories
AC_ARG_WITH([external-db],
	AS_HELP_STRING([--with-external-db=DIR],
               [Use specified external database directory, default: /usr/share/ipv6calc/db]),
	[
		external_db="$with_external_db"
	],
	[
		external_db=$external_db_default
	])

if test "$EXTERNAL" = "yes"; then
	require_libdb="yes"

	AC_DEFINE(SUPPORT_EXTERNAL, 1, Define if you want external support included.)
	AC_MSG_RESULT([*** external db dir: $external_db])
	AC_DEFINE_UNQUOTED(EXTERNAL_DB, "$external_db", Define external database directory.)
fi

EXTERNAL_DB=$external_db
AC_SUBST(EXTERNAL_DB)


dnl *************************************************
dnl libdb required
dnl *************************************************
if test "$require_libdb" = "yes"; then
	AC_CHECK_HEADER(db.h, [
		AC_CHECK_LIB(db, db_create,
		[
			AC_MSG_RESULT([Berkeley DB library found and usable: db_create])
		],[
			AC_MSG_ERROR([Berkeley DB library file misses: db_create])
		])
	],[
		AC_MSG_ERROR([Berkeley DB library header files not found - missing db4-devel or libdb-devel?])
	])

	AC_CHECK_FILE(/sbin/ldconfig, [
		dnl try to retrieve installed version: '#?'/'#\?' -> ugly workaround to avoid that autoconf parser removes '[' and ']'
		DB_VERSION=`/sbin/ldconfig -p | grep -E '#?\s*libdb-[0-9]+.[0-9]+' | sed -e 's/^#\?\s*libdb-\([0-9]*\)\.\([0-9]*\).*/\1.\2/' | sort -n | tail -1`
		if test -n "$DB_VERSION"; then
			AC_MSG_RESULT([Berkeley DB library version found: libdb-$DB_VERSION])
			LDFLAGS_EXTRA_STATIC="-ldb-$DB_VERSION${LDFLAGS_EXTRA_STATIC:+ $LDFLAGS_EXTRA_STATIC}"
		fi
	],[
		AC_MSG_WARN([Missing /sbin/ldconfig, cannot detect Berkeley DB library version])
	])

	LDFLAGS_EXTRA="${LDFLAGS_EXTRA:+$LDFLAGS_EXTRA }-ldb"
	LDFLAGS_EXTRA_MOD_IPV6CALC="-ldb $LDFLAGS_EXTRA_MOD_IPV6CALC"
	HAVE_BERKELEY_DB_SUPPORT=1
	AC_DEFINE(HAVE_BERKELEY_DB_SUPPORT, 1, Define if Berkeley DB support is required.)
	AC_MSG_RESULT([*** Berkeley DB library found and usable (required)])
fi


dnl *************************************************
dnl MaxMindDB support (GeoIP2, DB-IP, IP2Location)
dnl *************************************************
MMDB_LIB_NAME="maxminddb"
MMDB_INCLUDE_VERSION=""

AC_ARG_ENABLE([mmdb],
	AS_HELP_STRING([--enable-mmdb],
	               [Enable MaxMindDB support (GeoIP/db-ip.com/IP2Location) (default: disabled)]),
	[
		if test "$enable_mmdb" != "no"; then
			MMDB="$enable_mmdb"
		fi
	],[
		MMDB="no"
	])

AC_ARG_WITH([mmdb-dynamic],
	AS_HELP_STRING([--with-mmdb-dynamic],
                   [Enable use of dynamic loading of MaxMindDB library (default=no)]),
	[
		if test "$with_mmdb_dynamic" != "no"; then
			MMDB_DYN="yes"
		fi
	])

AC_ARG_WITH([mmdb-headers],
	AS_HELP_STRING([--with-mmdb-headers=DIR],
		[MaxMindDB include files location]),
	[
		MMDB_INCLUDE_DIR="$with_mmdb_headers"
	])

if test -n "$MMDB_INCLUDE_DIR"; then
	MMDB_INCLUDE="-I$MMDB_INCLUDE_DIR"
	if echo "$MMDB_INCLUDE_DIR" | grep -q "^/"; then
		# absolute path
		MMDB_INCLUDE_L1="-I$MMDB_INCLUDE_DIR"
		MMDB_INCLUDE_L2="-I$MMDB_INCLUDE_DIR"
	else
		MMDB_INCLUDE_L1="-I../$MMDB_INCLUDE_DIR"
		MMDB_INCLUDE_L2="-I../../$MMDB_INCLUDE_DIR"
	fi
fi

AC_ARG_WITH([mmdb-lib],
	AS_HELP_STRING([--with-mmdb-lib=DIR],
		[MaxMindDB library location]),
	[
		if test -d "$with_mmdb_lib"; then
			MMDB_LIB_DIR="$with_mmdb_lib"
			MMDB_LIB="-L$MMDB_LIB_DIR $MMDB_LIB"
			AC_MSG_RESULT([MaxMindDB library directory given: $MMDB_LIB_DIR])
		else
			AC_MSG_ERROR([MaxMindDB library directory not found: $with_mmdb_lib])
		fi

		if echo "$MMDB_LIB_DIR" | grep -q "^/"; then
			# absolute path
			MMDB_LIB_L1="-L$MMDB_LIB_DIR"
			MMDB_LIB_L2="-L$MMDB_LIB_DIR"
			LD_LIBRARY_PATH=$MMDB_LIB_DIR:$LD_LIBRARY_PATH
		else
			MMDB_LIB_L1="-L../$MMDB_LIB_DIR"
			MMDB_LIB_L2="-L../../$MMDB_LIB_DIR"
			LD_LIBRARY_PATH=../$MMDB_LIB_DIR:$LD_LIBRARY_PATH
		fi
	])

AC_ARG_WITH([mmdb-dyn-lib],
	AS_HELP_STRING([--with-mmdb-dyn-lib=NAME],
		[Use specified MaxMindDB dynamic library, default: libmaxminddb.so.0]),
	[
		mmdb_dyn_lib="$with_mmdb_dyn_lib"
	],
	[
		mmdb_dyn_lib=$mmdb_dyn_lib_default
	])

if test "$MMDB" = "yes"; then
	if test "$MMDB_DYN" != "yes"; then
		MMDB_LIB="-l$MMDB_LIB_NAME $MMDB_LIB"
		MMDB_LIB_L1="-l$MMDB_LIB_NAME $MMDB_LIB_L1"
		MMDB_LIB_L2="-l$MMDB_LIB_NAME $MMDB_LIB_L2"
	else
		AC_MSG_RESULT([*** MaxMindDB dynamic load support enabled, disable MaxMindDB linker options])
	fi

	CPPFLAGS="$CPPFLAGS $MMDB_INCLUDE"

	AC_MSG_RESULT([*** MaxMindDB support requested])

	AC_MSG_RESULT([***** CPPFLAGS=$CPPFLAGS])
	AC_MSG_RESULT([***** CFLAGS  =$CFLAGS])
	AC_MSG_RESULT([***** LDFLAGS =$LDFLAGS])

	AC_CHECK_HEADER(maxminddb.h, [
		if test -n "$MMDB_INCLUDE_DIR"; then
			if test -e "$MMDB_INCLUDE_DIR/maxminddb.h"; then
				CPPFLAGS="$CPPFLAGS -include $MMDB_INCLUDE_DIR/maxminddb.h"
			else
				AC_MSG_ERROR([MaxMindDB library header file not found: $MMDB_INCLUDE_DIR/maxminddb.h])
			fi
		else
			CPPFLAGS="$CPPFLAGS -include maxminddb.h"
		fi

		AC_CHECK_DECL([MMDB_open],,
			AC_MSG_ERROR([MaxMindDB library header file misses MMDB_open]))

		AC_CHECK_DECLS([MMDB_lib_version])

	],
	[
		AC_MSG_ERROR([MaxMindDB library header files not found - missing libmaxminddb-devel?])
	])

	AC_MSG_RESULT([*** MaxMindDB database directory: $mmdb_db])
	AC_DEFINE_UNQUOTED(MMDB_DB, "$mmdb_db", Define MaxMindDB database directory.)

	if test "$MMDB_DYN" != "yes"; then
		CPPFLAGS="$CFLAGS"

		if test -n "$MMDB_LIB_DIR"; then
			if test -e "$MMDB_LIB_DIR/lib$MMDB_LIB_NAME.so"; then
				LIBS="-L$MMDB_LIB_DIR"
			else
				AC_MSG_ERROR([MaxMindDB library file not found: $MMDB_INCLUDE_DIR/lib$MMDB_LIB_NAME.so])
			fi
		fi

		AC_MSG_RESULT([***** CPPFLAGS=$CPPFLAGS])
		AC_MSG_RESULT([***** CFLAGS  =$CFLAGS])
		AC_MSG_RESULT([***** LDFLAGS =$LDFLAGS])

		AC_CHECK_LIB($MMDB_LIB_NAME, MMDB_open,
		[
			AC_DEFINE(SUPPORT_MMDB, 1, Define if you want MaxMindDB support.)
			AC_MSG_RESULT([*** MaxMindDB support enabled])
		],
		[
			AC_MSG_ERROR([MaxMindDB library header files given but the library was not found, LDFLAGS=$LDFLAGS])
		])
	else
		# wrapper detects by itself the real support
		AC_MSG_RESULT([*** GeoIP dyn lib: $mmdb_dyn_lib])
		AC_DEFINE_UNQUOTED(MMDB_DYN_LIB, "$mmdb_dyn_lib", Define MaxMindDB dynamic library.)

		AC_MSG_RESULT([*** MaxMindDB support will be implemented by using dynamic load of library])
		AC_DEFINE(SUPPORT_MMDB, 1, Define if you want MaxMindDB support.)
		AC_DEFINE(SUPPORT_MMDB_DYN, 1, Define if you want MaxMindDB support with dynamic loading support - requires also additional linker options.)
	fi

	# GeoIPv2, DBIP(MMDB), IP2Location(MMDB) support
	AC_ARG_ENABLE([geoip2],
		AS_HELP_STRING([--disable-geoip2],
			       [Disable MaxMindDB support for GeoIP (default: enabled)]),
		[
			GEOIP2="no"
		],[
			GEOIP2="yes"
		])

	AC_ARG_ENABLE([dbip2],
		AS_HELP_STRING([--disable-dbip2],
			       [Disable MaxMindDB support for db-ip.com (default: enabled)]),
		[
			DBIP2="no"
		],[
			DBIP2="yes"
		])

	AC_ARG_ENABLE([ip2location2],
		AS_HELP_STRING([--disable-ip2location2],
			       [Disable MaxMindDB support for IP2Location (default: enabled)]),
		[
			IP2LOCATION2="no"
		],[
			IP2LOCATION2="yes"
		])
else
	GEOIP2=no
	DBIP2=no
	IP2LOCATION2=no
fi

AC_SUBST(MMDB_INCLUDE)
AC_SUBST(MMDB_INCLUDE_VERSION)
AC_SUBST(MMDB_INCLUDE_L2)
AC_SUBST(MMDB_INCLUDE_L1)
AC_SUBST(MMDB_LIB)
AC_SUBST(MMDB_LIB_L1)
AC_SUBST(MMDB_LIB_L2)
AC_SUBST(MMDB_DYN_LIB)

if test -n "$MMDB_INCLUDE_VERSION"; then
	AC_DEFINE_UNQUOTED(MMDB_INCLUDE_VERSION, "$MMDB_INCLUDE_VERSION", MaxMindDB include file version.)
fi


dnl *************************************************
dnl DB-IP.com support
dnl *************************************************
AC_ARG_ENABLE([dbip], 
	AS_HELP_STRING([--enable-dbip],
	               [Compatibility Dummy (no longer supported)]),
	[
	])

dnl defaults for database directories
AC_ARG_WITH([dbip-db],
	AS_HELP_STRING([--with-dbip-db=DIR],
               [Use specified db-ip.com database directory, default: /usr/share/DBIP]),
	[
		dbip_db="$with_dbip_db"
	],
	[
		dbip_db=$dbip_db_default
	])

if test "$DBIP2" = "yes"; then
	AC_MSG_RESULT([*** db-ip.com (MaxMindDB) support auotenabled"])
	AC_DEFINE(SUPPORT_DBIP2, 1, Define if you want db-ip.com (MaxMindDB) support included.)
	AC_MSG_RESULT([*** db-ip.com (MaxMindDB) database directory: $dbip_db])
	AC_DEFINE_UNQUOTED(DBIP2_DB, "$dbip_db", Define db-ip.com (MaxMindDB) database directory.)
fi

DBIP2_DB=$dbip_db
AC_SUBST(DBIP2_DB)


dnl *************************************************
dnl IP2Location support
dnl *************************************************
IP2LOCATION_LIB_NAME="IP2Location"
IP2LOCATION_INCLUDE_VERSION=""

AC_ARG_ENABLE([ip2location], 
	AS_HELP_STRING([--enable-ip2location],
		[Enable IP2Location support (default: disabled)]),
	[
		IP2LOCATION="$enable_ip2location"
	],[
		IP2LOCATION="no"
	])

AC_ARG_WITH([ip2location-dynamic],
	AS_HELP_STRING([--with-ip2location-dynamic],
                [Enable use of dynamic loading of IP2Location library (default=no)]),
	[
		if test "$enable_ip2location_dynamic" != "no"; then
			IP2LOCATION_DYN="yes"
		fi
	])

AC_ARG_WITH([ip2location-headers],
	AS_HELP_STRING([--with-ip2location-headers=DIR],
		[IP2Location include files location]),
	[
		IP2LOCATION_INCLUDE_DIR="$with_ip2location_headers"
	])

AC_ARG_WITH([ip2location-headers-fallback],
	AS_HELP_STRING([--with-ip2location-headers-fallback],
		[Compatibility Dummy (no longer supported)]),
	[
	])

if test -n "$IP2LOCATION_INCLUDE_DIR"; then
	IP2LOCATION_INCLUDE="-I$IP2LOCATION_INCLUDE_DIR"
	if test -d "$IP2LOCATION_INCLUDE_DIR/iMath"; then
		# include iMath where existing (API < 7.0.0)
		IP2LOCATION_INCLUDE="$IP2LOCATION_INCLUDE -I$IP2LOCATION_INCLUDE_DIR/iMath"
	fi
	if echo "$IP2LOCATION_INCLUDE_DIR" | grep -q "^/"; then
		# absolute path
		IP2LOCATION_INCLUDE_L1="-I$IP2LOCATION_INCLUDE_DIR"
		IP2LOCATION_INCLUDE_L2="-I$IP2LOCATION_INCLUDE_DIR"
		if test -d "$IP2LOCATION_INCLUDE_DIR/iMath"; then
			# include iMath where existing (API < 7.0.0)
			IP2LOCATION_INCLUDE_L1="$IP2LOCATION_INCLUDE_L1 -I$IP2LOCATION_INCLUDE_DIR/iMath"
			IP2LOCATION_INCLUDE_L2="$IP2LOCATION_INCLUDE_L2 -I$IP2LOCATION_INCLUDE_DIR/iMath"
		fi
	else
		IP2LOCATION_INCLUDE_L1="-I../$IP2LOCATION_INCLUDE_DIR"
		IP2LOCATION_INCLUDE_L2="-I../../$IP2LOCATION_INCLUDE_DIR"
		if test -d "$IP2LOCATION_INCLUDE_DIR/iMath"; then
			# include iMath where existing (API < 7.0.0)
			IP2LOCATION_INCLUDE_L1="$IP2LOCATION_INCLUDE_L1 -I../$IP2LOCATION_INCLUDE_DIR/iMath"
			IP2LOCATION_INCLUDE_L2="$IP2LOCATION_INCLUDE_L2 -I../../$IP2LOCATION_INCLUDE_DIR/iMath"
		fi
	fi
fi

AC_ARG_WITH([ip2location-lib],
	AS_HELP_STRING([--with-ip2location-lib=FILE],
		[IP2Location library location]),
	[
		if test -d "$with_ip2location_lib"; then 
			IP2LOCATION_LIB_DIR="$with_ip2location_lib"
			IP2LOCATION_LIB="-L$IP2LOCATION_LIB_DIR $IP2LOCATION_LIB"
			AC_MSG_RESULT([IP2Location library directory given: $IP2LOCATION_LIB_DIR])
		else
			AC_MSG_ERROR([IP2Location library directory not found: $with_ip2location_lib])
		fi

		if echo "$IP2LOCATION_LIB_DIR" | grep -q "^/"; then
			# absolute path
			IP2LOCATION_LIB_L1="-L$IP2LOCATION_LIB_DIR"
			IP2LOCATION_LIB_L2="-L$IP2LOCATION_LIB_DIR"
			if test -n "$LD_LIBRARY_PATH"; then
				LD_LIBRARY_PATH=$IP2LOCATION_LIB_DIR:$LD_LIBRARY_PATH
			else
				LD_LIBRARY_PATH=$IP2LOCATION_LIB_DIR
			fi
		else
			IP2LOCATION_LIB_L1="-L../$IP2LOCATION_LIB_DIR"
			IP2LOCATION_LIB_L2="-L../../$IP2LOCATION_LIB_DIR"
			if test -n "$LD_LIBRARY_PATH"; then
				LD_LIBRARY_PATH=../$IP2LOCATION_LIB_DIR:$LD_LIBRARY_PATH
			else
				LD_LIBRARY_PATH=$IP2LOCATION_LIB_DIR
			fi
		fi
	])

# must be tested
#AC_ARG_WITH([ip2location-lib-name],
#	AS_HELP_STRING([--with-ip2location-lib-name=NAME],
#		[IP2Location library name]),
#	[
#		IP2LOCATION_LIB_NAME="$with_ip2location_lib_name"
#		IP2LOCATION_LIB="-l$IP2LOCATION_LIB_NAME $IP2LOCATION_LIB"
#		AC_MSG_RESULT([IP2Location library name given: $IP2LOCATION_LIB_NAME])
#        ],[
#	])

AC_ARG_WITH([ip2location-static],
	AS_HELP_STRING([--with-ip2location-static],
		[Explicitly link IP2Location statically (default=no)]),
	[
		if test "$with_ip2location_static" != "no"; then 
			IP2LOCATION_LIB="-Wl,-Bstatic $IP2LOCATION_LIB -Wl,-Bdynamic"
		fi
	])

dnl defaults for database directories
AC_ARG_WITH([ip2location-db],
	AS_HELP_STRING([--with-ip2location-db=DIR],
               [Use specified IP2Location database directory, default: /usr/share/IP2Location]),
	[
		ip2location_db="$with_ip2location_db"
	],
	[
		ip2location_db=$ip2location_db_default
	])

AC_ARG_WITH([ip2location-dyn-lib],
	AS_HELP_STRING([--with-ip2location-dyn-lib=NAME],
		[Use specified IP2Location dynamic library, default: libIP2Location.so]),
	[
		ip2location_dyn_lib="$with_ip2location_dyn_lib"
	],
	[
		ip2location_dyn_lib=$ip2location_dyn_lib_default
	])

if test "$IP2LOCATION" = "yes"; then
	if test "$IP2LOCATION_DYN" != "yes"; then
		IP2LOCATION_LIB="-l$IP2LOCATION_LIB_NAME $IP2LOCATION_LIB"
		IP2LOCATION_LIB_L1="-l$IP2LOCATION_LIB_NAME $IP2LOCATION_LIB_L1"
		IP2LOCATION_LIB_L2="-l$IP2LOCATION_LIB_NAME $IP2LOCATION_LIB_L2"
	else
		AC_MSG_RESULT([**** IP2Location dynamic load support enabled, disable IP2Location linker options])
	fi

	CPPFLAGS="$CPPFLAGS $IP2LOCATION_INCLUDE"

	AC_MSG_RESULT([*** IP2Location support requested])

	AC_MSG_RESULT([***** CPPFLAGS=$CPPFLAGS])
	AC_MSG_RESULT([***** CFLAGS  =$CFLAGS])
	AC_MSG_RESULT([***** LDFLAGS =$LDFLAGS])

	AC_CHECK_HEADER(IP2Location.h, [
		if test -n "$IP2LOCATION_INCLUDE_DIR"; then
			if test -e "$IP2LOCATION_INCLUDE_DIR/IP2Location.h"; then
				CPPFLAGS="$CPPFLAGS -include $IP2LOCATION_INCLUDE_DIR/IP2Location.h"
			else
				AC_MSG_ERROR([IP2Location library header file not found: $IP2LOCATION_INCLUDE_DIR/IP2Location.h])
			fi
		else
			CPPFLAGS="$CPPFLAGS -include IP2Location.h"
		fi

		AC_CHECK_DECL([IP2Location_open],,
			AC_MSG_ERROR([IP2Location library header file misses IP2Location_open]))

		AC_CHECK_DECLS([IP2Location_close,
			IP2Location_get_country_short,
			IP2Location_get_country_long])

		AC_CHECK_DECLS([API_VERSION])

		AC_CHECK_DECLS([IP2Location_api_version_string], IP2LOCATION_HAS_API_STRING="yes")
		AC_CHECK_DECL([IP2Location_lib_version_string], IP2LOCATION_HAS_LIB_STRING="yes")

		AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
		     [[
				#include <IP2Location.h>
				IP2Location loc;
				void test() {
					loc.ipv6_database_address = 1;
				};
		     ]])],[
				AC_MSG_RESULT([IP2Location has full (and updated) IPv6 support (API >= 8.2.0)])
			],[
				AC_MSG_ERROR([IP2Location has only compat IPv6 support (API < 7.0.0) or old (API < 8.2.0) which is no longer supported])
			]
		)
	],
	[
		AC_MSG_ERROR([IP2Location library header files not found - missing IP2Location-devel?])
	])

	AC_MSG_RESULT([*** IP2location db dir: $ip2location_db])
	AC_DEFINE_UNQUOTED(IP2LOCATION_DB, "$ip2location_db", Define IP2Location database directory.)

	if test "$IP2LOCATION_DYN" != "yes"; then
		if test "$IP2LOCATION_STATIC" = "yes"; then
			AC_DEFINE(SUPPORT_IP2LOCATION_STATIC, 1, Define if you want IP2Location support statically linked - requires also additional linker options.)
		fi

		CPPFLAGS="$CFLAGS"

		if test -n "$IP2LOCATION_LIB_DIR"; then
			if test -e "$IP2LOCATION_LIB_DIR/lib$IP2LOCATION_LIB_NAME.so"; then
				LIBS="-L$IP2LOCATION_LIB_DIR"
			else
				AC_MSG_ERROR([IP2Location library file not found: $IP2LOCATION_INCLUDE_DIR/lib$IP2LOCATION_LIB_NAME.so])
			fi
		fi

		AC_CHECK_LIB($IP2LOCATION_LIB_NAME, IP2Location_api_version_string,
		[
			AC_MSG_RESULT([*** IP2Location library supports API version string])
		],[
			AC_MSG_ERROR([IP2Location library file misses IP2Location_api_version_string - no longer supported])
		])

		AC_CHECK_LIB($IP2LOCATION_LIB_NAME, IP2Location_lib_version_string,
		[
			if test "$IP2LOCATION_HAS_LIB_STRING" != "yes"; then
				AC_MSG_ERROR([IP2Location library header file misses IP2Location_lib_version_string while library contains])
			else
				AC_MSG_RESULT([*** IP2Location library supports LIB version string])
			fi
		])

		AC_MSG_RESULT([***** CPPFLAGS=$CPPFLAGS])
		AC_MSG_RESULT([***** CFLAGS  =$CFLAGS])
		AC_MSG_RESULT([***** LDFLAGS =$LDFLAGS])

		AC_CHECK_LIB($IP2LOCATION_LIB_NAME, IP2Location_open,
		[
			AC_DEFINE(SUPPORT_IP2LOCATION, 1, Define if you want IP2Location support.)
			AC_MSG_RESULT([*** IP2Location support enabled])
		],
		[
			AC_MSG_ERROR([IP2Location library header files were found but the library was not found, LDFLAGS=$LDFLAGS, CFLAGS=$CFLAGS])
		])
	else
		AC_CHECK_DECLS([IP2Location_api_version_string],
		[
			AC_MSG_RESULT([*** IP2Location header supports API version string])
		],[
			AC_MSG_ERROR([IP2Location library header file misses IP2Location_api_version_string - no longer supported])
		])

		# wrapper detects by itself the real support
		AC_MSG_RESULT([*** IP2Location dyn lib: $ip2location_dyn_lib])
		AC_DEFINE_UNQUOTED(IP2LOCATION_DYN_LIB, "$ip2location_dyn_lib", Define IP2Location dynamic library.)

		AC_MSG_RESULT([*** IP2Location support will be implemented by using dynamic load of library])
		AC_DEFINE(SUPPORT_IP2LOCATION, 1, Define if you want IP2Location support.)
		AC_DEFINE(SUPPORT_IP2LOCATION_DYN, 1, Define if you want IP2Location support with dynamic loading support - requires also additional linker options.)
	fi
fi

IP2LOCATION_DB=$ip2location_db
AC_SUBST(IP2LOCATION_INCLUDE)
AC_SUBST(IP2LOCATION_INCLUDE_L1)
AC_SUBST(IP2LOCATION_INCLUDE_L2)
AC_SUBST(IP2LOCATION_LIB)
AC_SUBST(IP2LOCATION_LIB_L1)
AC_SUBST(IP2LOCATION_LIB_L2)
AC_SUBST(IP2LOCATION_DB)
AC_SUBST(IP2LOCATION_DYN_LIB)

if test -n "$IP2LOCATION_INCLUDE_VERSION"; then
	AC_DEFINE_UNQUOTED(IP2LOCATION_INCLUDE_VERSION, "$IP2LOCATION_INCLUDE_VERSION", IP2Location include file version.)
fi

if test "$IP2LOCATION2" = "yes"; then
	AC_MSG_RESULT([*** IP2Location(MaxMindDB) support autoenabled"])
	AC_DEFINE(SUPPORT_IP2LOCATION2, 1, Define if you want IP2Location(MaxMindDB) support included.)
	AC_MSG_RESULT([*** IP2Location(MaxMindDB) database directory: $ip2location_db])
	AC_DEFINE_UNQUOTED(IP2LOCATION2_DB, "$dbip_db", Define IP2Location(MaxMindDB) database directory.)
fi
IP2LOCATION2_DB=$ip2location_db
AC_SUBST(IP2LOCATION2_DB)


dnl *************************************************
dnl IP2Location v2 support powered by MaxMindDB
dnl *************************************************
if test "$IP2LOCATION2" = "yes"; then
	AC_MSG_RESULT([*** IP2Location(MaxMindDB) autoenabled"])
fi


dnl *************************************************
dnl GeoIP v1 support
dnl *************************************************
GEOIP_LIB_NAME="GeoIP"
GEOIP_INCLUDE_VERSION=""

AC_ARG_ENABLE([geoip], 
	AS_HELP_STRING([--enable-geoip],
	               [Compatibility Dummy (no longer supported)]),
	[
	])

AC_ARG_WITH([geoip-dynamic],
	AS_HELP_STRING([--with-geoip-dynamic],
                   [Compatibility Dummy (no longer supported)]),
	[
	])

AC_ARG_WITH([geoip-headers],
	AS_HELP_STRING([--with-geoip-headers=DIR],
		[Compatibility Dummy (no longer supported)]),
	[
	])

AC_ARG_WITH([geoip-headers-fallback],
	AS_HELP_STRING([--with-geoip-headers-fallback],
		[Compatibility Dummy (no longer supported)]),
	[
	])

AC_ARG_WITH([geoip-lib],
	AS_HELP_STRING([--with-geoip-lib=DIR],
		[Compatibility Dummy (no longer supported)]),
	[
	])

AC_ARG_WITH([geoip-static],
	AS_HELP_STRING([--with-geoip-static],
                   [Compatibility Dummy (no longer supported)]),
	[
	])

AC_ARG_WITH([geoip-dyn-lib],
	AS_HELP_STRING([--with-geoip-dyn-lib=NAME],
		[Compatibility Dummy (no longer supported)]),
	[
	])

AC_ARG_WITH([geoip-ipv6-compat],
	AS_HELP_STRING([--with-geoip-ipv6-compat],
		[Compatibility Dummy (no longer supported)]),
	[
	])


dnl *************************************************
dnl GeoIP v2 support powered by MaxMindDB
dnl *************************************************
if test "$GEOIP2" = "yes"; then
	AC_MSG_RESULT([*** GeoIP (MaxMindDB) auotenabled"])
fi


dnl *************************************************
dnl GeoIP common support powered by MaxMindDB
dnl *************************************************
AC_ARG_WITH([geoip-db],
	AS_HELP_STRING([--with-geoip-db=DIR],
		[Use specified GeoIP database directory, default: /usr/share/GeoIP]),
	[
		geoip_db="$with_geoip_db"
	],
	[
		geoip_db=$geoip_db_default
	])


if test "$GEOIP2" = "yes"; then
	AC_DEFINE(SUPPORT_GEOIP2, 1, Define if you want GeoIP (MaxMindDB) support.)
	AC_MSG_RESULT([*** GeoIP (MaxMindDB) support enabled])

	AC_MSG_RESULT([*** GeoIP (MaxMindDB) database directory: $geoip_db])
	AC_DEFINE_UNQUOTED(GEOIP2_DB, "$geoip_db", Define GeoIP (MaxMindDB) database directory.)
fi

GEOIP2_DB=$geoip_db
AC_SUBST(GEOIP2_DB)


dnl *************************************************
dnl Dynamic load used?
dnl *************************************************
if test "$DYNAMIC_LOAD" = "yes"; then
	if test "$GEOIP_DYN" = "yes" -o "$IP2LOCATION_DYN" = "yes" -o "$MMDB_DYN" = "yes"; then
		DYNAMIC_LOAD="yes"
		AC_MSG_RESULT([*** Dynamic loading of libraries is ENABLED])
		DYNLOAD_LIB="-rdynamic -ldl"
	fi
fi

AC_SUBST(DYNLOAD_LIB)


dnl *************************************************
dnl mod_ipv6calc / apxs - APache eXtension tool
dnl *************************************************
AC_ARG_ENABLE([mod_ipv6calc], 
	AS_HELP_STRING([--enable-mod_ipv6calc],
		[Enable build of mod_ipv6calc for Apache (default: disabled)])
	)

if test "$enable_mod_ipv6calc" = "yes"; then
    ENABLE_MOD_IPV6CALC="1"
else
    ENABLE_MOD_IPV6CALC="0"
fi

AC_SUBST(ENABLE_MOD_IPV6CALC)

AC_ARG_WITH(apxs, AS_HELP_STRING([--with-apxs=NAME],[name of the apxs executable (apxs)]),
            [APXS="$with_apxs"])

if test "$ENABLE_MOD_IPV6CALC" = "1"; then
	AC_MSG_RESULT([Build of mod_ipv6calc enabled])

	if test -z "$APXS"; then
		AC_PATH_PROGS(APXS, apxs2 apxs, [], [$PATH:/usr/local/apache/bin:/usr/sbin])
	fi

	if test -z "$APXS"; then
		AC_MSG_ERROR([No APXS binary found: cannot create Apache module - missing httpd-devel?])
	else
		AC_MSG_RESULT([APXS binary found: $APXS])

		# partially taken from configure.ac from mod_wsgi
		AC_MSG_CHECKING(Apache version)
		HTTPD="`${APXS} -q SBINDIR`/`${APXS} -q TARGET`"
		HTTPD_INCLUDEDIR="`${APXS} -q INCLUDEDIR`"
		if test -x ${HTTPD}; then
		    HTTPD_VERSION=`${HTTPD} -v | awk '/version/ {print $3}' \
		     | awk -F/ '{print $2}'`
		else
		    if test -f ${HTTPD_INCLUDEDIR}/ap_release.h; then
			HTTPD_VERSION=`grep '^#define AP_SERVER_MAJORVERSION_NUMBER ' \
			 ${HTTPD_INCLUDEDIR}/ap_release.h | sed -e \
			 's/^#define AP_SERVER_MAJORVERSION_NUMBER \([0-9]\).*/\1.X/'`
		    else
			if test -f ${HTTPD_INCLUDEDIR}/httpd.h; then
			    HTTPD_VERSION=`grep '^#define APACHE_RELEASE ' \
			     ${HTTPD_INCLUDEDIR}/httpd.h | sed -e \
			     's/^#define APACHE_RELEASE \([0-9]\).*/\1.X/'`
			else
			    HTTPD_VERSION="2.?"
			fi
		    fi
		fi

		HTTPD_LIBEXECDIR="`${APXS} -q libexecdir`"
		if test -z "$HTTPD_LIBEXECDIR"; then
			AC_MSG_ERROR([${APXS} cannot retrieve libexecdir])
		fi

		HTTPD_SYSCONFDIR="`${APXS} -q sysconfdir`"
		if test -z "$HTTPD_SYSCONFDIR"; then
			AC_MSG_ERROR([${APXS} cannot retrieve sysconfdir])
		fi

		if test -n "$HTTPD_SYSCONFDIR"; then
			HTTPD_SYSCONFDIR="$HTTPD_SYSCONFDIR.d"
		fi
		if test -n "$HTTPD_VERSION"; then
			HTTPD_VERSION_MAJOR_MINOR=`echo "$HTTPD_VERSION" | awk -F. '{ print $1 "." $2 }'`
		fi
	fi

	AC_SUBST(HTTPD_LIBEXECDIR)
	AC_SUBST(HTTPD_SYSCONFDIR)
	AC_SUBST(HTTPD_VERSION_MAJOR_MINOR)
	AC_SUBST(APXS)
else
	AC_MSG_WARN([Build of mod_ipv6calc NOT enabled])
fi

dnl *************************************************
dnl Files to create from <file>.in
dnl *************************************************
AC_SUBST(CFLAGS_EXTRA)
AC_SUBST(LDFLAGS_EXTRA)
AC_SUBST(LDFLAGS_EXTRA_MOD_IPV6CALC)
AC_SUBST(LDFLAGS_EXTRA_STATIC)
AC_SUBST(LD_LIBRARY_PATH)
AC_SUBST(LD_LIBRARY_PATH_BASIC)

AC_CONFIG_FILES([ tools/ipv6calc-db-update.sh ], [ chmod a+x tools/ipv6calc-db-update.sh ])
AC_CONFIG_FILES([ tools/DBIP-update.sh        ], [ chmod a+x tools/DBIP-update.sh        ])
AC_CONFIG_FILES([ tools/IP2Location-update.sh ], [ chmod a+x tools/IP2Location-update.sh ])
AC_CONFIG_FILES([ tools/GeoIP-update.sh       ], [ chmod a+x tools/GeoIP-update.sh       ])

AC_CONFIG_FILES([
		Makefile
		md5/Makefile
		tools/Makefile
		doc/Makefile
		getopt/Makefile
		ipv6calc/Makefile
		lib/Makefile
		man/Makefile
		databases/lib/Makefile
		ipv6logconv/Makefile
		ipv6loganon/Makefile
		ipv6logstats/Makefile
		ipv6calcweb/Makefile
		contrib/ipv6calc.spec
		mod_ipv6calc/Makefile
		VERSION
])

AC_OUTPUT

AC_MSG_RESULT([***** Summary])

AC_MSG_RESULT([*** SHARED_LIBRARY             =$SHARED_LIBRARY])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** MMDB                       =$MMDB])
AC_MSG_RESULT([*** MMDB_DYN                   =$MMDB_DYN])
AC_MSG_RESULT([*** MMDB_DYN_LIB               =$mmdb_dyn_lib])
AC_MSG_RESULT([*** MMDB_INCLUDE               =$MMDB_INCLUDE])
AC_MSG_RESULT([*** MMDB_INCLUDE_VERSION       =$MMDB_INCLUDE_VERSION])
AC_MSG_RESULT([*** MMDB_LIB                   =$MMDB_LIB])
AC_MSG_RESULT([*** MMDB_LIB_NAME              =$MMDB_LIB_NAME])
AC_MSG_RESULT([*** MMDB_LIB_DIR               =$MMDB_LIB_DIR])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** IP2LOCATION                =$IP2LOCATION])
AC_MSG_RESULT([*** IP2LOCATION_DB             =$IP2LOCATION_DB])
AC_MSG_RESULT([*** IP2LOCATION_DYN            =$IP2LOCATION_DYN])
AC_MSG_RESULT([*** IP2LOCATION_DYN_LIB        =$ip2location_dyn_lib])
AC_MSG_RESULT([*** IP2LOCATION_INCLUDE        =$IP2LOCATION_INCLUDE])
AC_MSG_RESULT([*** IP2LOCATION_INCLUDE_VERSION=$IP2LOCATION_INCLUDE_VERSION])
AC_MSG_RESULT([*** IP2LOCATION_LIB            =$IP2LOCATION_LIB])
AC_MSG_RESULT([*** IP2LOCATION_LIB_NAME       =$IP2LOCATION_LIB_NAME])
AC_MSG_RESULT([*** IP2LOCATION_LIB_DIR        =$IP2LOCATION_LIB_DIR])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** IP2OCATION2                =$IP2LOCATION2])
AC_MSG_RESULT([*** IP2LOCATION2_DB            =$IP2LOCATION2_DB])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** GEOIP2                     =$GEOIP2])
AC_MSG_RESULT([*** GEOIP2_DB                  =$GEOIP2_DB])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** DBIP2                      =$DBIP2])
AC_MSG_RESULT([*** DBIP2_DB                   =$DBIP2_DB])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** EXTERNAL                   =$EXTERNAL])
AC_MSG_RESULT([*** EXTERNAL_DB                =$EXTERNAL_DB])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** BUILTIN                    =$BUILTIN])
AC_MSG_RESULT([*** DB_IPV4                    =$DB_IPV4])
AC_MSG_RESULT([*** DB_IPV6                    =$DB_IPV6])
AC_MSG_RESULT([*** DB_IEEE                    =$DB_IEEE])
AC_MSG_RESULT([*** DB_AS_REG                  =$DB_AS_REG])
AC_MSG_RESULT([*** DB_CC_REG                  =$DB_CC_REG])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** CC                         =$CC])
AC_MSG_RESULT([*** CFLAGS_EXTRA               =$CFLAGS_EXTRA])
AC_MSG_RESULT([*** LDFLAGS_EXTRA              =$LDFLAGS_EXTRA])
AC_MSG_RESULT([*** LDFLAGS_EXTRA_STATIC       =$LDFLAGS_EXTRA_STATIC])
AC_MSG_RESULT([*** LD_LIBRARY_PATH            =$LD_LIBRARY_PATH])
AC_MSG_RESULT([*** LD_LIBRARY_PATH_BASIC      =$LD_LIBRARY_PATH_BASIC])

AC_MSG_RESULT([*** CPPFLAGS                   =$CPPFLAGS])
AC_MSG_RESULT([*** CFLAGS                     =$CFLAGS])
AC_MSG_RESULT([*** LDFLAGS                    =$LDFLAGS])
AC_MSG_RESULT([*** LIBS                       =$LIBS])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** HAVE_LIBMD_MD5             =$HAVE_LIBMD_MD5])
AC_MSG_RESULT([*** HAVE_OPENSSL_EVP_MD5       =$HAVE_OPENSSL_EVP_MD5])
AC_MSG_RESULT([*** HAVE_OPENSSL_EVP_MD5_LEGACY=$HAVE_OPENSSL_EVP_MD5_LEGACY])
AC_MSG_RESULT([*** HAVE_OPENSSL_MD5           =$HAVE_OPENSSL_MD5])
AC_MSG_RESULT([*** ENABLE_BUNDLED_MD5         =$ENABLE_BUNDLED_MD5])
AC_MSG_RESULT([*** ENABLE_LIBMD_MD5           =$ENABLE_LIBMD_MD5])
AC_MSG_RESULT([*** ENABLE_OPENSSL_EVP_MD5     =$ENABLE_OPENSSL_EVP_MD5])
AC_MSG_RESULT([*** ENABLE_OPENSSL_MD5         =$ENABLE_OPENSSL_MD5])

AC_MSG_RESULT([])
AC_MSG_RESULT([*** HAVE_BERKELEY_DB_SUPPORT   =$HAVE_BERKELEY_DB_SUPPORT])
AC_MSG_RESULT([*** DB_VERSION                 =$DB_VERSION])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** PACKAGE_VERSION_NUMERIC    =$PACKAGE_VERSION_NUMERIC])
AC_MSG_RESULT([*** PACKAGE_VERSION_STRING     =$PACKAGE_VERSION_STRING])
AC_MSG_RESULT([*** API_VERSION_NUMERIC        =$API_VERSION_NUMERIC])
AC_MSG_RESULT([*** API_VERSION_STRING         =$API_VERSION_STRING])

AC_MSG_RESULT([])

AC_MSG_RESULT([*** ENABLE_MOD_IPV6CALC        =$ENABLE_MOD_IPV6CALC])
if test "$ENABLE_MOD_IPV6CALC" = "1"; then
	AC_MSG_RESULT([*** APXS                       =$APXS])
	AC_MSG_RESULT([*** HTTPD_SYSCONFDIR           =$HTTPD_SYSCONFDIR])
	AC_MSG_RESULT([*** HTTPD_LIBEXECDIR           =$HTTPD_LIBEXECDIR])
	AC_MSG_RESULT([*** HTTPD_VERSION              =$HTTPD_VERSION])
	AC_MSG_RESULT([*** HTTPD_VERSION_MAJOR_MINOR  =$HTTPD_VERSION_MAJOR_MINOR])
	AC_MSG_RESULT([*** LDFLAGS_EXTRA_MOD_IPV6CALC =$LDFLAGS_EXTRA_MOD_IPV6CALC])
fi
