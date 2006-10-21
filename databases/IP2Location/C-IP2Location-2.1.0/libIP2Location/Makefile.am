AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I. -pipe -D _GNU_SOURCE -D_SUN_
AM_CXXFLAGS = -Wall -Werror

pkglib_LTLIBRARIES = libIP2Location.la

libIP2Location_la_SOURCES = IP2Location.c \
														bigd.c \
														bigdigits.c \
														spBigdigits.c

libIP2Location_la_LDFLAGS = -module -no-undefined -avoid-version
