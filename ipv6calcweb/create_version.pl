#!/usr/bin/perl -w
#
# Project    : ipv6calc/ipv6calcweb
# File       : create_version.pl
# Version    : $Id: create_version.pl,v 1.1 2002/03/21 08:23:42 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Put version information into ipv6calcweb.cgi
#

my $FILE_VERSION="../VERSION";
my $FILE_IN="ipv6calcweb.cgi";


## Variables
my $IPV6CALC_VERSION;

## Get version
open(VERSION, "<$FILE_VERSION") || die "Cannot open file: $FILE_VERSION";

while (<VERSION>) {
	if ($_ =~ /VERSION/) {
		if ( $_ =~ /^#/ ) {
			next;
		};
		chomp $_;
		#print "Line: '" . $_ . "'\n";
		my ($token, $version) = split /\s/, $_, 2;
		if (! defined $token || ! defined $version) {
			next;
		};
		#print "Split: '" . $token . "'  =   '" . $version . "'\n";
		if ( $token eq "VERSION" || $version ne "" ) {
			$IPV6CALC_VERSION = $version;
		};
	};
};
close(VERSION);

if (! defined $IPV6CALC_VERSION) {
	die "Cannot retrieve version information";
} else {
	#print "Version: " . $IPV6CALC_VERSION . "\n";
};


## Open file
open(FILE, "<$FILE_IN") || die "Cannot open file $FILE_IN";

while (<FILE>) {
	my $line = $_;

	if ($line =~ /\@\@VERSION\@\@/) {
		$line = "my \$program_version = \"" . $IPV6CALC_VERSION . "\";       # automatic generated line, don't remove this token: \@\@VERSION\@\@\n";
	};

	print $line;
};
close(FILE);
