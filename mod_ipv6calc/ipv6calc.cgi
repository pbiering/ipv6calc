#!/usr/bin/perl -w -T
#
# Project    : ipv6calc/mod_ipv6calc
# File       : ipv6calc.cgi
# Version    : $Id: f33067600678714c6812091a7232451b9bd7b87d $
# Copyright  : 2019-2019 by Peter Bieringer <pb (at) bieringer.de>
#
# Simple Perl web interface to demonstrate capabilities of mod_ipv6calc

## general
use strict;
use POSIX;
use warnings;

my @envs = (
	'REMOTE_ADDR',
	'IPV6CALC_CLIENT_IP_ANON',
	'IPV6CALC_CLIENT_COUNTRYCODE',
	'IPV6CALC_CLIENT_ASN',
	'IPV6CALC_CLIENT_REGISTRY',
	'IPV6CALC_ANON_METHOD'
);

## Print output
print("Content-type: text/plain\n\n");

for my $env (@envs) {
	if ( defined $ENV{$env} ) {
		print("$env=" . $ENV{$env} . "\n");
	};
};

exit(0);
