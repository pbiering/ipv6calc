#!/usr/bin/perl -w

use IPC::Open2;
#use strict;


my $OUTFILE = "dbipv4addr_assignment.h";

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time - 24*60*60);

$year = 1900 + $year;
$mon = sprintf "%02d", $mon + 1;
$mday = sprintf "%02d", $mday;

my @files = ( "arin/arin." . $year . $mon . "01", "ripencc/ripencc." . $year . $mon . $mday, "apnic/apnic-" . $year . "-" . $mon . "-01" );
#my @files = ( "ripencc/ripencc.20020320" );

my (@arin, @apnic, @ripencc, @iana);

my (@arin_agg, @apnic_agg, @ripencc_agg, @iana_agg);

my $global_file = "iana/ipv4-address-space";

my %assignments;

my $max_prefixlength_not_arin = 0;


# Generate subnet powers
my %subnet_powers;

print STDERR "Generate subnet powers\n";
for (my $i = 32; $i >= 1; $i--) {
	my $power = 1 << (32 - $i);

	my $dec;
	if ($i == 32) {
		$dec = 0xffffffff;
	} else {
		$dec = ((2 << $i) - 1) << (32 - $i);
	};

	#print STDERR "Power " . $i . ":". $power;
	#printf STDERR "   subnet mask: %8x\n", $dec;
	%subnet_powers->{$i}->{'numbers'} = $power;
	%subnet_powers->{$i}->{'mask'} = $dec;
};



sub ipv4_to_dec($) {
	my $ipv4 = shift || die "Missing IPv4 address";

	my ($t1, $t2, $t3, $t4) = split /\./, $ipv4;

	my $dec = $t4 + 256 * ( $t3 + 256 * ( $t2 + 256 * $t1 ));

	return ($dec);
};

sub dec_to_ipv4($) {
	my $dec = shift || die "Missing decimal";

 	my $t1 = ($dec & 0xff000000) >> 24;
 	my $t2 = ($dec & 0x00ff0000) >> 16;
 	my $t3 = ($dec & 0x0000ff00) >> 8;
 	my $t4 = ($dec & 0x000000ff);

	my $ipv4 = sprintf "%d.%d.%d.%d", $t1, $t2, $t3, $t4;

	return ($ipv4);
};

sub length_to_dec($) {
	my $length = shift || die "Missing prefix length";

	my $dec = ((2 << $length) - 1) << (32 - $length);

	return ($dec);
};


sub check_in_list($) {
	my $num = shift || die "Missing IPv4 address number";

	foreach my $ipv4num (keys %assignments ) {
		if ( ( $num & %assignments->{$ipv4num}->{'mask'} ) == $ipv4num ) {
			return( %assignments->{$ipv4num}->{'registry'} );
		};
	};
	return;
};

# Should't be used, a little bit obsolete
sub proceed_global() {
	# Proceed first global IANA file
	print "Proceed file: " . $global_file . "\n";

	open(FILE, "<$global_file") || die "Cannot open file: $global_file";

	my $line;
	my $ipv4; my $length;
	my ($ipv4_start, $ipv4_end);
	while (<FILE>) {
		$line = $_;
		chomp $line;
		my $reg;

		if ( $line =~ /^\d{3}\// ) {
			# single entry
			(my $block, $reg, my $dummy) = split /\s+/, $line;

			($block, $length) = split /\//, $block;

			$ipv4_start = int($block);
			$ipv4_end = int($block);

		} elsif ( $line =~ /^\d{3}\-\d{3}\// ) {
			# range entry
			(my $rangeblock, $reg, my $dummy) = split /\s+/, $line;

			($rangeblock, $length) = split /\//, $rangeblock;

			my ($start, $end) = split /\-/, $rangeblock;
			$ipv4_start = int($start);
			$ipv4_end = int($end);
		} else {
			# skip not proper lines
			next;
		};

		$reg = uc($reg);
		$reg =~ s/RIPE/RIPENCC/g;

		if ( ($reg ne "ARIN") && ($reg ne "APNIC") && ($reg ne "RIPENCC") && ($reg ne "IANA") ) {
			#print "Unsupported registry: " . $reg . "\n";
			next;
		};

		for ($ipv4 = $ipv4_start; $ipv4 <= $ipv4_end; $ipv4++) {
			$ipv4 = $ipv4 . ".0.0.0";
	
		#printf "%s/%d=%s", $ipv4, $length, $reg . "\n";

			if ($reg eq "ARIN" ) {
				#print "Push ARIN: " . $ipv4 . "/" . $length . "\n";
				push @arin, $ipv4 . "/" . $length;
			} elsif ($reg eq "APNIC" ) {
				#print "Push APNIC: " . $ipv4 . "/" . $length . "\n";
				push @apnic, $ipv4 . "/" . $length;
			} elsif ($reg eq "RIPENCC" ) {
				#print "Push RIPENCC: " . $ipv4 . "/" . $length . "\n";
				push @ripencc, $ipv4 . "/" . $length;
			} elsif ($reg eq "IANA" ) {
				#print "Push IANA: " . $ipv4 . "/" . $length . "\n";
				push @iana, $ipv4 . "/" . $length;
			} else {
				die "Unsupported registry";	
			};
		};
	};
	close(FILE);
};

&proceed_global();

foreach my $file (@files) {
	print "Proceed file: " . $file . "\n";

	open(FILE, "<$file") || die "Cannot open file: $file";

	my $line;
	my $ipv4;
	my $length;
	my $flag_proceeded;
	while (<FILE>) {
		$line = $_;
		chomp $line;

		# skip not proper lines
		if ( ! ( $line =~ /\|ipv4\|/ ) ) { next; };
		if ( $line =~ /\|\*\|/ ) { next; };

		#print $line . "\n";

		my ($reg, $tld, $token, $ipv4, $numbers, $date, $status) = split /\|/, $line;

		if ( $token ne "ipv4" ) { next; };

		$reg = uc($reg);
		$reg =~ s/\wRIPE\w/RIPENCC/g;

		if ( $reg ne "ARIN" && $reg ne "APNIC" && $reg ne "RIPENCC" && $reg ne "IANA" ) {
			print "Unsupported registry: " . $reg . "\n";
			next;
		};

		# get registry array
		my $parray;

		if ($reg eq "ARIN" ) {
			$parray = \@arin;
		} elsif ($reg eq "APNIC" ) {
			$parray = \@apnic;
		} elsif ($reg eq "RIPENCC" ) {
			$parray = \@ripencc;
		} elsif ($reg eq "IANA" ) {
			$parray = \@iana;
		} else {
			die "Unsupported registry: " . $reg;
		};


		# convert IPv4 address to decimal
		my $ipv4_dec = &ipv4_to_dec($ipv4);

		my $check_length;
		$flag_proceeded = 0;
		# check numbers maching
		for ($check_length = 1; $check_length <= 32; $check_length++) {
			if ( %subnet_powers->{$check_length}->{'numbers'} == $numbers ) {
				# case 1: numbers = 2^x
				
				if ( ( $ipv4_dec & %subnet_powers->{$check_length}->{'mask'} ) == $ipv4_dec ) {
					# case 1a: easy, subnet(numbers) matches given network
					push @$parray, $ipv4 . "/" . $check_length;
					#printf "%s/%d=%s (case 1a)\n", $ipv4, $check_length, $reg;
					$flag_proceeded = 1;
					
					last;
				} else {
					#printf "%s/%d=%s (case 1b)\n", $ipv4, $check_length, $reg;
					$check_length++;
					$flag_proceeded = 2;
					last;
				};
			} elsif ( %subnet_powers->{$check_length}->{'numbers'} < $numbers ) {
				# case 2: numbers != 2^x
				#printf "%s=%s (case 2: %d)\n", $ipv4, $reg, $numbers;
				$flag_proceeded = 3;
				last;
			};
		};

		if ($flag_proceeded == 1) {
			# next one...
			next;
		} elsif ($flag_proceeded == 0) {
			die "Shouldn't happen";
		};

		# now the harder work...
		my $newnumbers = $numbers;
		while ($newnumbers > 0) {
			#printf "Newnumbers: %d   Length: %d\n", $newnumbers, $check_length;

			while ( $newnumbers < %subnet_powers->{$check_length}->{'numbers'} ) {
				$check_length++;
			};

			if ( ( $ipv4_dec & (~ %subnet_powers->{$check_length}->{'mask'}) ) == 0 ) {
				push @$parray, $ipv4 . "/" . $check_length;
				#printf "%s/%d=%s (partially catch case 1b or 2: %d)\n", &dec_to_ipv4($ipv4_dec), $check_length, $reg, %subnet_powers->{$check_length}->{'numbers'};
				$newnumbers -= %subnet_powers->{$check_length}->{'numbers'};
				$ipv4_dec += %subnet_powers->{$check_length}->{'numbers'};

				next;
			} else {
				$check_length++;
				if ($check_length > 32) {
					die "Shouldn't happen";
				};
			};
		};
	};

	close(FILE);
};

sub proceed_array($$) {
	my $parray = shift || die "missing array pointer";
	my $parray_agg = shift || die "missing array pointer";

	print "Start proceeding array with 'aggregate'\n";

	my $pid = open2(AGGREGATE_READ, AGGREGATE_WRITE, "aggregate -t") || die "cannot for: $!";
	
	foreach my $entry (@$parray) {
		# filter out longer prefix length



		print AGGREGATE_WRITE $entry . "\n";
	};
	close(AGGREGATE_WRITE);

	while (<AGGREGATE_READ>) {
		my $line = $_;
		chomp $line;
		push @$parray_agg, $line;
	};

	close(AGGREGATE_READ);

	print "End proceeding array with 'aggregate'\n";
};



print "Aggregate RIPENCC\n";
&proceed_array(\@ripencc, \@ripencc_agg);

print "Aggregate APNIC\n";
&proceed_array(\@apnic, \@apnic_agg);

print "Aggregate IANA\n";
&proceed_array(\@iana, \@iana_agg);

# Look for maximum used prefix length
my ($net, $length);
for my $entry (@ripencc_agg, @apnic_agg, @iana_agg) {
	my ($net, $length) = split /\//, $entry;
	if ($length > $max_prefixlength_not_arin) {
		$max_prefixlength_not_arin = $length;
	};
};

print "Maximum used prefix length by not ARIN: " . $max_prefixlength_not_arin . "\n";

## Run filter of ARIN entries
print "Run filter on ARIN entries\n";
# 1. overwrite prefix length and network
for (my $i = 0; $i < $#arin; $i++) {
	my ($net, $length) = split /\//, $arin[$i];
	if ($length > $max_prefixlength_not_arin) {
		$arin[$i] = &dec_to_ipv4(&ipv4_to_dec($net) & %subnet_powers->{$max_prefixlength_not_arin}->{'mask'}) . "\/" . $max_prefixlength_not_arin;
	};
};
# 2. remove duplicates
my @arin_new;
push @arin_new, $arin[0];	
for (my $i = 1; $i < $#arin; $i++) {
	if ($arin[$i] eq $arin[$i - 1]) {
		next;
	} else {
		push @arin_new, $arin[$i];	
	};
};
print "End of filter on ARIN entries\n";

print "Aggregate ARIN (this can need some time...)\n";
&proceed_array(\@arin, \@arin_agg);


# Create header file


print "Create outfile now: " . $OUTFILE . "\n";
open(OUT, ">$OUTFILE") || die "Cannot open outfile: $OUTFILE";

# Header
my $now_string = localtime;
print OUT qq|
/*
 * Project       : ipv6calc
 * File          : dbipv4_assignment.h
 * Version       : Generated $now_string
 * Data copyright: RIPE NCC, APNIC, ARIN
 *
 * Information:
 *  Additional header file for libipv4addr.c
 */

|;

# Structure
print OUT qq|
static const s_ipv4addr_assignment dbipv4addr_assignment[] = {
|;


sub print_header_array($$) {
	my $parray = shift || die "missing array pointer";
	my $reg = shift || die "missing registry";

	foreach my $entry (@$parray) {
		my ($ipv4, $length) = split /\//, $entry;
		
		printf OUT "\t{ 0x%08x, 0x%08x, \"%s\" },\n", &ipv4_to_dec($ipv4), &length_to_dec($length), $reg;
	};
};

&print_header_array(\@apnic_agg, "APNIC");
&print_header_array(\@ripencc_agg, "RIPENCC");
&print_header_array(\@arin_agg, "ARIN");
&print_header_array(\@iana_agg, "IANA");

print OUT qq|
};
|;

print "Finished\n";
