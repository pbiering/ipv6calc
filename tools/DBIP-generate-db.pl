#!/usr/bin/perl -W
#
# Perl converter for db-ip.com database files into ipv6calc proprietary Berkeley DB file
#
# Project    : ipv6calc/DBIP
# File       : DBIP-generate-db.pl
# Version    : $Id: DBIP-generate-db.pl,v 1.1 2014/08/31 10:27:40 ds6peter Exp $
# Copyright  : 2014-2014 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL version 2
#

use strict;
use warnings;

use Getopt::Std;
use BerkeleyDB;
use POSIX qw(strftime);
use IO::Uncompress::Gunzip qw(gunzip $GunzipError);

my %opts;
getopts ("qdO:I:h?", \%opts);

my $file_input;
my $dir_output = ".";
my $file_type = "cvs";

if (defined $opts{'h'} || defined $opts{'?'}) {
        print qq|
Usage:  PROGNAME -I <input file>

Options:
        -I <input file>        DB-IP.com CSV input file
        -O <output directory>  output directory for DB files
        -d                     debug
        -q                     quiet
|;
        exit 0;
};


if (defined $opts{'I'}){
       $file_input = $opts{'I'};
};

if (defined $opts{'O'}){
       $dir_output = $opts{'O'};
};

if (! defined $file_input) {
	print "ERROR : no input file given (-I <FILENAME>)\n";
	exit 1;
};

if (! -f $file_input) {
	print "ERROR : given input file doesn't exist: $file_input\n";
	exit 1;
};

if (! -d $dir_output) {
	print "ERROR : given output directory doesn't exist: $dir_output\n";
	exit 1;
};

my $type;
my $date;
my $type_string;

if ($file_input !~ /^(.*\/)?dbip-(city|country)-([0-9]{4})-([0-9]{2}).csv(.gz)?$/o) {
	print "ERROR : input file name is not a valid dbip filename: $file_input\n";
	exit 1;
};

if (defined $5) {
	print "INFO  : .gz suffix found on: $file_input\n" if (! defined $opts{'q'});
	$file_type = "cvs.gz";
};

$date = $3 . $4 . "01";

$type_string = $2;

if ($type_string eq "country") {
	$type = 1000;
} elsif ($type_string eq "city") {
	$type = 1010;
};



print "INFO  : input file: $file_input type=$type date=$date\n" if (! defined $opts{'q'});


my $filename_ipv4 = "$dir_output/ipv6calc-dbip-$type_string-ipv4.db";
my $filename_ipv6 = "$dir_output/ipv6calc-dbip-$type_string-ipv6.db";

my @a_ipv4;
my @a_ipv6;

if (-f $filename_ipv4) {
	unlink($filename_ipv4) || die "Can't delete old file: $filename_ipv4";
};
if (-f $filename_ipv6) {
	unlink($filename_ipv6) || die "Can't delete old db file: $filename_ipv6";
};

print "INFO  : create db from input: IPv4=$filename_ipv4 IPv6=$filename_ipv6 INPUT=$file_input\n";

tie @a_ipv4, 'BerkeleyDB::Recno', -Filename => $filename_ipv4, -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $filename_ipv4: $! $BerkeleyDB::Error\n";
tie @a_ipv6, 'BerkeleyDB::Recno', -Filename => $filename_ipv6, -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $filename_ipv6: $! $BerkeleyDB::Error\n";

my $now_string = strftime "%Y%m%d-%H%M%S%z", gmtime;
my $info_ipv4 = "dbusage=ipv6calc;dbformat=1;dbdate=$date;dbtype=" . ($type+4) . ";dbproto=4;dbcreated=$now_string";
my $info_ipv6 = "dbusage=ipv6calc;dbformat=1;dbdate=$date;dbtype=" . ($type+6) . ";dbproto=6;dbcreated=$now_string";

# Add a few key/value pairs to the file
$a_ipv4[0] = $info_ipv4;
$a_ipv6[0] = $info_ipv6;

my $linecounter = 0;
my $counter_ipv4 = 0;
my $counter_ipv6 = 0;

my $FILE;
my $buffer;

if ($file_type eq "cvs.gz") {
	print "INFO : create handle for gzip'ed file: $file_input\n" if (defined $opts{'d'});
	$FILE = new IO::Uncompress::Gunzip $file_input or die "IO::Uncompress::Gunzip failed: $GunzipError\n";
} else {
	open($FILE, "<$file_input") || die "Can't open file: $file_input";
};

while (<$FILE>) {
	my $line = $_;
        chomp $line;
	$linecounter++;

	if ((($linecounter % 100000) == 0) || (defined $opts{'d'})) {
		print "INFO : linecounter=$linecounter: $line\n" if (! defined $opts{'q'});
	};

	my $start;
	my $end;
	my $cc;

	my $city;
	my $region;


	if ($type == 1010) {
		# city
		if ($line !~ /^"([0-9a-fA-F.:]*)","([0-9a-fA-F.:]*)","([A-Z]{0,2})","([^"]*)","([^"]*)"/o) {
			print "ERROR : unexpected line in file (line: $linecounter): $line\n";
			exit 1;	
		};

		$start = $1;
		$end   = $2;
		$cc    = $3;
		$region= $4;
		$city  = $5;

		if ((! defined $city) || ($city eq "")) {
			print "NOTICE: emtpy city found on linecounter=$linecounter\n" if (! defined $opts{'q'});
			$city = "-";
		};
		if ((! defined $region) || ($region eq "")) {
			print "NOTICE: emtpy region found on linecounter=$linecounter\n" if (! defined $opts{'q'});
			$region = "-";
		};
	} else {
		# country
		if ($line !~ /^"([0-9a-fA-F.:]*)","([0-9a-fA-F.:]*)","([A-Z]{0,2})"/o) {
			print "ERROR : unexpected line in file (line: $linecounter): $line\n";
			exit 1;	
		};
		$start = $1;
		$end   = $2;
		$cc    = $3;
	};

	if ((! defined $cc) || ($cc eq "")) {
		print "NOTICE: emtpy CC found on linecounter=$linecounter: $line\n" if (! defined $opts{'q'});
		$cc = "--"; # unknown country
	};

	my $start_value;
	my $end_value;
	my $start_value_0_15;
	my $start_value_16_31;
	my $end_value_0_15;
	my $end_value_16_31;

	if ($start =~ /^([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})$/o) {
		$counter_ipv4++;

		# IPv4, short conversion
		$start_value = ($1 << 24) + ($2 << 16) + ($3 << 8) + $4;

		if ($end =~ /^([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})$/o) {
			# IPv4, short conversion
			$end_value = ($1 << 24) + ($2 << 16) + ($3 << 8) + $4;
		} else {
			print "ERROR : can't parse IPv4 end while IPv4 start (line: $linecounter): $end\n";
			exit 1;	
		};

		#print "INFO : IPv4: $start " . sprintf("(%08x)", $start_value) . " $end " . sprintf("(%08x)", $end_value) . " $cc\n";

		if ($type == 1010) {
			push @a_ipv4, $start_value . ";" . $end_value . ";" . $cc . ";" . $region . ";" . $city;
		} else {
			push @a_ipv4, $start_value . ";" . $end_value . ";" . $cc;
		};
	} else {
		$counter_ipv6++;

		# IPv6
		$start_value = `ipv6calc -q -O hex $start`; 
		$end_value   = `ipv6calc -q -O hex $end`;
		$start_value_0_15  = hex(substr($start_value, 0, 8)); # 1st 8 nibbles = 32 bits
		$start_value_16_31 = hex(substr($start_value, 8, 8)); # 1st 8 nibbles = 32 bits
		$end_value_0_15    = hex(substr($end_value, 0, 8)); # 1st 8 nibbiles = 32 bits
		$end_value_16_31   = hex(substr($end_value, 8, 8)); # 1st 8 nibbiles = 32 bits

		#print "INFO : IPv6: $start " . sprintf("(%08x:%08x)", hex($start_value_0_15), hex($start_value_16_31)) . " $end " . sprintf("(%08x:%08x)", hex($end_value_0_15), hex($end_value_16_31)) . " $cc\n";

		if ($type == 1010) {
			push @a_ipv6, $start_value_0_15 . ";" . $start_value_16_31 . ";" . $end_value_0_15 . ";" . $end_value_16_31 . ";" . $cc . ";" . $region . ";" .$city;
		} else {
			push @a_ipv6, $start_value_0_15 . ";" . $start_value_16_31 . ";" . $end_value_0_15 . ";" . $end_value_16_31 . ";" . $cc;
		};
		#die;
	};

};

if (! defined $opts{'q'}) {
	print "INFO  : IPv4 lines: " . $counter_ipv4 . "\n";
	print "INFO  : IPv6 lines: " . $counter_ipv6 . "\n";

	print "INFO  : IPv4 array elements: " . scalar @a_ipv4 . "\n";
	print "INFO  : IPv6 array elements: " . scalar @a_ipv6 . "\n";

	print "INFO  : IPv4 database info string: " . $a_ipv4[0] . "\n";
	print "INFO  : IPv6 database info string: " . $a_ipv6[0] . "\n";

	print "INFO  : IPv4 database first entry: " . $a_ipv4[1] . "\n";
	print "INFO  : IPv4 database last  entry: " . $a_ipv4[-1] . "\n";

	print "INFO  : IPv6 database first entry: " . $a_ipv6[1] . "\n";
	print "INFO  : IPv6 database last  entry: " . $a_ipv6[-1] . "\n";
};

untie @a_ipv4;
untie @a_ipv6;
