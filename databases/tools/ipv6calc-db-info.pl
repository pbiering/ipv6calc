#!/usr/bin/perl -w
#
# Project    : ipv6calc/databases/tools
# File       : ipv6calc-db-info.pl
# Version    : $Id$
# Copyright  : 2017-2017 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL v2
#
# Information:
#  Perl program which retrieves information from an ipv6calc database

use strict;
use warnings;

use IPC::Open2;

use Getopt::Std;
use BerkeleyDB;
use POSIX qw(strftime);

my $progname = $0;

my $debug = 0;

sub help {
	print qq|
Usage: $progname -s <dba-file>
	-s <db-file>	database file
	-h		this online help

|;
	exit 0;
};

# parse options
our ($opt_h, $opt_s);
getopts('s:h') || help();

if (defined $opt_h) {
	help();
};

if (! defined $opt_s) {
	print "WARN  : nothing to do (no database file given -s ...)\n";
	exit 1;
};

## locations
my $file_h;
my $file = $opt_s;

print "INFO  : open db file: $file\n";

## SubDB: info
my %db_h;
my @db_a;
my $subdb;
my $db_type;

$subdb = "info";
$db_type = 'BerkeleyDB::Btree';
tie %db_h, $db_type, -Filename => $file, -Subname => $subdb || die "Cannot open file $file: $! $BerkeleyDB::Error\n";
for my $key (sort keys %db_h) {
	print "INFO  : SubDB=$subdb key=$key value=" . $db_h{$key} . "\n";
};
untie %db_h;

$subdb = "data";
$db_type = 'BerkeleyDB::Recno';
tie @db_a, $db_type, -Filename => $file, -Subname => $subdb || die "Cannot open file $file: $! $BerkeleyDB::Error\n";
print "INFO  : SubDB=$subdb elements=" . scalar(@db_a) . "\n";
untie @db_a;

$subdb = "data-iana";
$db_type = 'BerkeleyDB::Recno';
tie @db_a, $db_type, -Filename => $file, -Subname => $subdb || die "Cannot open file $file: $! $BerkeleyDB::Error\n";
print "INFO  : SubDB=$subdb elements=" . scalar(@db_a) . "\n";
untie @db_a;

$subdb = "data-info";
$db_type = 'BerkeleyDB::Recno';
tie @db_a, $db_type, -Filename => $file, -Subname => $subdb || die "Cannot open file $file: $! $BerkeleyDB::Error\n";
print "INFO  : SubDB=$subdb elements=" . scalar(@db_a) . "\n";
untie @db_a;
