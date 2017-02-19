#!/usr/bin/perl -W
#
# Perl converter for db-ip.com database files into ipv6calc proprietary Berkeley DB file
#
# Project    : ipv6calc/DBIP
# File       : DBIP-generate-db.pl
# Version    : $Id$
# Copyright  : 2014-2017 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL version 2
#
# Supported formats (https://db-ip.com/db/)
#  IP to Country	(1000)
#  IP to City		(1010)
#  IP to Location	(1020) - UNTESTED
#  IP to ISP		(1030) - UNTESTED
#  IP to Location + ISP (1040)

use strict;
use warnings;

use Getopt::Std;
use BerkeleyDB;
use POSIX qw(strftime);
use IO::Uncompress::Gunzip qw(gunzip $GunzipError);
use Net::IP;
use Text::CSV_PP;

my %opts;
getopts ("qdAS:O:I:h?", \%opts);

my $file_input;
my $dir_output = ".";
my $suffix = "";
my $suffix_orig;
my $file_type = "cvs";
my $atomic = 0;

if (defined $opts{'h'} || defined $opts{'?'}) {
        print qq|
Usage:  PROGNAME -I <input file> [-O <output directory>] [-S <suffix>] [-A]

Options:
        -I <input file>        DB-IP.com CSV input file
        -O <output directory>  optional output directory for DB files, default: .
        -S <suffix>            optional suffix
        -A                     atomic operation (generate .new and move on success)
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

if (defined $opts{'S'}){
	$suffix = $opts{'S'};
};

$suffix_orig = $suffix;

if (defined $opts{'A'}){
	$suffix = $suffix . ".new";
	$atomic = 1;
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

if ($file_input !~ /^(.*\/)?dbip-(city|country|full)-([0-9]{4})-([0-9]{2}).csv(.gz)?$/o) {
	print "ERROR : input file name is not a valid dbip filename: $file_input\n";
	exit 1;
};

if (defined $5) {
	print "INFO  : .gz suffix found on: $file_input\n" if (! defined $opts{'q'});
	$file_type = "cvs.gz";
};

$date = $3 . $4 . "01";

$type_string = $2;

my $csv_fields_required = 0;

if ($type_string eq "country") {
	$type = 1000;
	$csv_fields_required = 3;
} elsif ($type_string eq "city") {
	$type = 1010;
	$csv_fields_required = 5;
} elsif ($type_string eq "location") {	# TODO verify string
	$type = 1020;
	$csv_fields_required = 12;
} elsif ($type_string eq "isp") {	# TODO verify string
	$type = 1030;
	$csv_fields_required = 6;
} elsif ($type_string eq "full") {
	$type = 1040;
	$csv_fields_required = 15;
} else {
	print "ERROR : unselected type (FIX CODE) for type_string: $type_string\n";
	exit 1;
};



print "INFO  : input file: $file_input type=$type date=$date\n" if (! defined $opts{'q'});


my $filename_ipv4 = "$dir_output/ipv6calc-dbip-ipv4-$type_string.db" . $suffix;
my $filename_ipv6 = "$dir_output/ipv6calc-dbip-ipv6-$type_string.db" . $suffix;

my $filename_ipv4_orig = "$dir_output/ipv6calc-dbip-ipv4-$type_string.db" . $suffix_orig;
my $filename_ipv6_orig = "$dir_output/ipv6calc-dbip-ipv6-$type_string.db" . $suffix_orig;

my @a_ipv4;
my @a_ipv6;

if (-f $filename_ipv4) {
	unlink($filename_ipv4) || die "Can't delete old file: $filename_ipv4";
};
if (-f $filename_ipv6) {
	unlink($filename_ipv6) || die "Can't delete old db file: $filename_ipv6";
};

print "INFO  : create db from input: IPv4=$filename_ipv4 IPv6=$filename_ipv6 INPUT=$file_input\n";

my $now_string = strftime "%Y%m%d-%H%M%S%z", gmtime;

my %h_info4;
my %h_info6;

tie %h_info4, 'BerkeleyDB::Btree', -Filename => $filename_ipv4, -Subname => 'info', -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $filename_ipv4: $! $BerkeleyDB::Error\n";
tie %h_info6, 'BerkeleyDB::Btree', -Filename => $filename_ipv6, -Subname => 'info', -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $filename_ipv6: $! $BerkeleyDB::Error\n";

$h_info4{'dbusage'} = "ipv6calc";
$h_info4{'dbformat'} = "1"; # ';' separated values
$h_info4{'dbdate'} = $date;
$h_info4{'dbtype'} = $type + 4;
$h_info4{'dbproto'} = '4';
$h_info4{'dbcreated'} = $now_string;
$h_info4{'dbcreated_unixtime'} = time;

$h_info6{'dbusage'} = "ipv6calc";
$h_info6{'dbformat'} = "1"; # ';' separated values
$h_info6{'dbdate'} = $date;
$h_info6{'dbtype'} = $type + 6;
$h_info6{'dbproto'} = '6';
$h_info6{'dbcreated'} = $now_string;
$h_info6{'dbcreated_unixtime'} = time;

untie %h_info4;
untie %h_info6;

tie @a_ipv4, 'BerkeleyDB::Recno', -Filename => $filename_ipv4, -Subname => 'data', -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $filename_ipv4: $! $BerkeleyDB::Error\n";
tie @a_ipv6, 'BerkeleyDB::Recno', -Filename => $filename_ipv6, -Subname => 'data', -Flags => DB_CREATE, -Mode => 0644 || die "Cannot open file $filename_ipv6: $! $BerkeleyDB::Error\n";

my $linecounter = 0;
my $counter_ipv4 = 0;
my $counter_ipv6 = 0;

my $FILE;
my $buffer;

if ($file_type eq "cvs.gz") {
	print "INFO  : create handle for gzip'ed file: $file_input\n" if (defined $opts{'d'});
	$FILE = new IO::Uncompress::Gunzip $file_input or die "IO::Uncompress::Gunzip failed: $GunzipError\n";
} else {
	open($FILE, "<$file_input") || die "Can't open file: $file_input";
};

my %stats_city;

my $csv = Text::CSV_PP->new();

while (<$FILE>) {
	my $line = $_;
        chomp $line;
	$linecounter++;

	if ((($linecounter % 100000) == 0) || (defined $opts{'d'})) {
		print "INFO  : linecounter=$linecounter: $line\n" if (! defined $opts{'q'});
	};

	my $start;
	my $end;
	my $cc;

	my $city;
	my $region;
	my $district;
	my $zipcode;
	my $latitude;
	my $longitude;
	my $geoname_id;
	my $tz_offset;
	my $tz_name;
	my $isp_name;
	my $conn_type;
	my $orgname;

	my $status;
	my @entries;

	if ($type == 1000) {
		# country
		if ($line !~ /^"([0-9a-fA-F.:]*)","([0-9a-fA-F.:]*)","([A-Z]{0,2})"/o) {
			print "ERROR : unexpected line in file (line: $linecounter): $line\n";
			exit 1;	
		};
		$start = $1;
		$end   = $2;
		$cc    = $3;
	} elsif ($type == 1010) {
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
			print "DEBUG: empty city found on linecounter=$linecounter\n" if (defined $opts{'d'});
			$city = "-";
			$stats_city{'empty-city'}++;
		};
		if ((! defined $region) || ($region eq "")) {
			print "DEBUG: empty region found on linecounter=$linecounter\n" if (defined $opts{'d'});
			$region = "-";
			$stats_city{'empty-region'}++;
		};
	} elsif ($type > 1010) {
		# full/isp/location
		if ($line !~ /^([0-9a-fA-F.:]*),([0-9a-fA-F.:]*),([A-Z]{0,2}),/o) {
			print "ERROR : unexpected line in file (line: $linecounter): $line\n";
			exit 1;	
		};

		# convert line into array
		$status  = $csv->parse($line);
		if ($status != 1) {
			print "ERROR : can't parse line in file (line: $linecounter): $line\n";
		};

		@entries = $csv->fields();

		if ($linecounter == 1) {
			print "NOTICE: found number of entries per line: " . scalar(@entries) . "\n";
		};

		if (scalar(@entries) != $csv_fields_required) {
			print "ERROR : unexpected line in file, number of entries " . scalar(@entries) . " are not expected, not equal to $csv_fields_required (line: $linecounter): $line\n";
			exit 1;
		};

		my $f = 0;
		$start      = $entries[$f++];
		$end        = $entries[$f++];
		$cc         = $entries[$f++];

		# replace any semicolon in data with / to prevent separation issues
		for ($f = 0; $f < scalar(@entries); $f++) {
			$entries[$f] =~ s/;/\//g;
		};
	} else {
		print "ERROR : unsupported type (FIX CODE) for parsing\n";
		exit 1;
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

	my $data;
	if ($type == 1000) {
		$data = $cc;
	} elsif ($type == 1010) {
		$data = $cc . ";" . $region . ";" . $city;
	} elsif ($type > 1010) {
		shift @entries;
		shift @entries;
		$data = join(";", @entries);
	} else {
		print "ERROR : unsupported type (FIX CODE) for creating data string\n";
		exit 1;
	};
	
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

		print "INFO : IPv4: $start " . sprintf("(%08x)", $start_value) . " $end " . sprintf("(%08x)", $end_value) . " $data\n" if (defined $opts{'d'});

		push @a_ipv4, sprintf("%08x", $start_value) . ";" . sprintf("%08x", $end_value) . ";" . $data;
	} else {
		$counter_ipv6++;

		# IPv6
		$start_value = Net::IP::ip_expand_address($start, 6);
		$end_value   = Net::IP::ip_expand_address($end  , 6);

		$start_value =~ s/://go;
		$end_value =~ s/://go;

		$start_value_0_15  = substr($start_value, 0, 8); # 1st 8 nibbles = 32 bits
		$start_value_16_31 = substr($start_value, 8, 8); # 1st 8 nibbles = 32 bits
		$end_value_0_15    = substr($end_value, 0, 8); # 1st 8 nibbiles = 32 bits
		$end_value_16_31   = substr($end_value, 8, 8); # 1st 8 nibbiles = 32 bits

		print "INFO : IPv6: $start " . sprintf("(%08x:%08x)", hex($start_value_0_15), hex($start_value_16_31)) . " $end " . sprintf("(%08x:%08x)", hex($end_value_0_15), hex($end_value_16_31)) . " $data\n" if (defined $opts{'d'});

		push @a_ipv6, $start_value_0_15 . ";" . $start_value_16_31 . ";" . $end_value_0_15 . ";" . $end_value_16_31 . ";" . $data;
	};

};

if (! defined $opts{'q'}) {
	print "INFO  : IPv4 lines: " . $counter_ipv4 . "\n";
	print "INFO  : IPv6 lines: " . $counter_ipv6 . "\n";

	print "INFO  : IPv4 array elements: " . scalar @a_ipv4 . "\n";
	print "INFO  : IPv6 array elements: " . scalar @a_ipv6 . "\n";

	print "INFO  : IPv4 database first entry: " . $a_ipv4[0] . "\n";
	print "INFO  : IPv4 database last  entry: " . $a_ipv4[-1] . "\n";

	print "INFO  : IPv6 database first entry: " . $a_ipv6[0] . "\n";
	print "INFO  : IPv6 database last  entry: " . $a_ipv6[-1] . "\n";
};

untie @a_ipv4;
untie @a_ipv6;

if ($atomic == "1") {
	rename $filename_ipv4, $filename_ipv4_orig;
	if ($? != 0) {
		print "ERROR : can't rename file to: $filename_ipv4_orig ($!) - delete: $filename_ipv4\n";
		unlink $filename_ipv4;
	};

	rename $filename_ipv6, $filename_ipv6_orig;
	if ($? != 0) {
		print "ERROR : can't rename file to: $filename_ipv6_orig ($!) - delete: $filename_ipv6\n";
		unlink $filename_ipv6;
	};
};
