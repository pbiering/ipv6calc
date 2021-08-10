#!/usr/bin/env bash
#
# Project    : ipv6calc
# File       : test_ipv6calc.sh
# Version    : $Id$
# Copyright  : 2001-2021 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc conversions

verbose=false
while getopts "Vh\?" opt; do
	case $opt in
	    V)
		verbose=true
		;;
	    *)
		echo "$0 [-V]"
		echo "    -V   verbose"
		exit 1
		;;
	esac
done
	

if [ ! -x ./ipv6calc ]; then
	echo "Binary './ipv6calc' missing or not executable"
	exit 1
fi


# Test Scenarios for autodetection "good case"
source ./test_scenarios.sh

testscenarios() {
# Command								Expected result (no space between "=" and result)
cat <<END | grep -v '^#'
## Machine readable format
-m -F ::1								=IPV6=0000:0000:0000:0000:0000:0000:0000:0001
-m ::1									=IPV6=::1
-m 2.2.3.4								=IPV4=2.2.3.4
## ip6.int.
--addr_to_ip6int 3ffe:ffff:100:f101::1					=1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--in ipv6 --out revnibbles.int 3ffe:ffff:100:f101::1			=1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--addr_to_ip6int 3ffe:ffff:100:f101::1/64				=1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.
--in revnibbles.int --out ipv6 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.	=3ffe:ffff:100:f101::1/128
## ip6.arpa.
--addr_to_ip6arpa 3ffe::1/64						=0.0.0.0.0.0.0.0.0.0.0.0.e.f.f.3.ip6.arpa.
--in ipv6 --out revnibbles.arpa	3ffe::1/64				=0.0.0.0.0.0.0.0.0.0.0.0.e.f.f.3.ip6.arpa.
-a --uppercase 3ffe::1/64						=0.0.0.0.0.0.0.0.0.0.0.0.E.F.F.3.IP6.ARPA.
-a -u 3ffe::1/64							=0.0.0.0.0.0.0.0.0.0.0.0.E.F.F.3.IP6.ARPA.
--out revnibbles.arpa abcd:1234:5678:abcd:1234:1234:1234:1234/125	=0.3.2.1.4.3.2.1.4.3.2.1.4.3.2.1.d.c.b.a.8.7.6.5.4.3.2.1.d.c.b.a.ip6.arpa.
-a --printstart 49 --printend 64 2001:a60:1168:0001:489c:1212:34fd:130f/64 	=1.0.0.0
-a --printprefix 2001:a60:1168:0001:489c:1212:34fd:130f/64 			=1.0.0.0.8.6.1.1.0.6.a.0.1.0.0.2.ip6.arpa.
-a --printsuffix 2001:a60:1168:0001:489c:1212:34fd:130f/64 			=f.0.3.1.d.f.4.3.2.1.2.1.c.9.8.4
## to uncompressed
--addr_to_compressed 3ffe:ffff:0100:f101:0000:0000:0000:0001		=3ffe:ffff:100:f101::1
--in ipv6 --out ipv6 --printcompressed 3ffe:ffff:0100:f101:0000:0000:0000:0001 =3ffe:ffff:100:f101::1
# :0: is not allowed to be compressed according to RFC5952 (https://github.com/pbiering/ipv6calc/issues/28)
--in ipv6 --out ipv6 --printcompressed 2001:db8:0:1:1:1:1:1             =2001:db8:0:1:1:1:1:1 
--in ipv6 --out ipv6 --printcompressed 2001:db8:0:0:1:1:1:1             =2001:db8::1:1:1:1 
--in ipv6 --out ipv6 --printcompressed 0:db8:0:1:1:1:1:1                =0:db8:0:1:1:1:1:1 
--in ipv6 --out ipv6 --printcompressed 0:0:db8:1:1:1:1:1                =::db8:1:1:1:1:1 
--in ipv6 --out ipv6 --printcompressed 2001:db8:0:1:1:1:1:0             =2001:db8:0:1:1:1:1:0 
--in ipv6 --out ipv6 --printcompressed 2001:db8:0:1:1:1:0:0             =2001:db8:0:1:1:1:: 
-I ipv6 -O ipv6 -C 3ffe:ffff:0100:f101:0000:0000:0000:0001 		=3ffe:ffff:100:f101::1
--addr_to_compressed 3ffe:ffff:0100:f101:0000:0000:0000:0001/64		=3ffe:ffff:100:f101::1/64
--addr_to_compressed 0:0:2:2:3:4:0:0					=::2:2:3:4:0:0
--addr_to_compressed 0:0:1:0:0:1:0:0					=::1:0:0:1:0:0
--addr_to_compressed 0:0:0:2:3:0:0:0					=::2:3:0:0:0
--addr_to_compressed 0:0:0:0:3:0:0:0					=::3:0:0:0
--addr_to_compressed 0:0:0:0:0:4:0:0					=::4:0:0
--addr_to_compressed 0:0:1:0:0:1:0:0					=::1:0:0:1:0:0
--addr_to_compressed 0:0:1:2:0:0:0:0					=0:0:1:2::
--addr_to_compressed 0:0:1:2:3:0:0:0					=0:0:1:2:3::
--addr_to_compressed 1080:0:0:0:8:800:200c:417a				=1080::8:800:200c:417a
--addr_to_compressed ff01:0:0:0:0:0:0:43				=ff01::43
--addr_to_compressed 0:0:0:0:0:0:0:1					=::1
--addr_to_compressed 0:0:0:0:0:0:0:0					=::
--addr_to_compressed 0:0:0:0:0:0:13.1.68.3				=::13.1.68.3
--addr_to_compressed 0:0:0:0:0:ffff:129.144.52.38			=::ffff:129.144.52.38
--addr_to_compressed --uppercase 0:0:0:0:0:ffff:129.144.52.38		=::FFFF:129.144.52.38
--addr_to_compressed fd00:1234:5678:0:210:a4ff:fe01:2345		=fd00:1234:5678:0:210:a4ff:fe01:2345
--in ipv6 --out ipv6 --printcompressed --uppercase 0:0:0:0:0:ffff:129.144.52.38		=::FFFF:129.144.52.38
## uncompressed
--addr_to_uncompressed 3ffe:ffff:100:f101::1				=3ffe:ffff:100:f101:0:0:0:1
--in ipv6 --out ipv6 --printuncompressed 3ffe:ffff:100:f101::1		=3ffe:ffff:100:f101:0:0:0:1
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:cd30:1:2:3::	=2001:db8:0:cd30:1:2:3:0
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:cd30:1::2		=2001:db8:0:cd30:1:0:0:2
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:1::2:3		=2001:db8:0:1:0:0:2:3
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:cd30:1:2::		=2001:db8:0:cd30:1:2:0:0
--in ipv6 --out ipv6 --printuncompressed 2001:db8:0:cd30:1:2::4		=2001:db8:0:cd30:1:2:0:4
--in ipv6 --out ipv6 --printuncompressed ::db8:0:cd30:1:2:3:4		=0:db8:0:cd30:1:2:3:4
--in ipv6 --out ipv6 --printuncompressed ::1:cd30:1:2:3:4		=0:0:1:cd30:1:2:3:4
--addr_to_uncompressed 3ffe:ffff:100:f101::1/64				=3ffe:ffff:100:f101:0:0:0:1/64
--addr_to_uncompressed ::13.1.68.3					=0:0:0:0:0:0:13.1.68.3
--addr_to_uncompressed ::013.01.068.0003				=0:0:0:0:0:0:13.1.68.3
--addr_to_uncompressed ::ffff:13.1.68.3					=0:0:0:0:0:ffff:13.1.68.3
--addr_to_uncompressed --maskprefix 3ffe:ffff:100:f101::1/64		=3ffe:ffff:100:f101:0:0:0:0/64
--addr_to_uncompressed --masksuffix 3ffe:ffff:100:f101:c000::1/64	=0:0:0:0:c000:0:0:1/64
--addr_to_uncompressed --uppercase ::ffff:13.1.68.3			=0:0:0:0:0:FFFF:13.1.68.3
--out ipv4addr --no-prefixlength 1.2.3.4				=1.2.3.4
--out ipv4addr --no-prefixlength 1.2.3.4/24				=1.2.3.4
--out ipv4addr --no-prefixlength 1.2.3/24				=1.2.3.0
--out ipv4addr --no-prefixlength 1.2/24					=1.2.0.0
--out ipv4addr --no-prefixlength 1/24					=1.0.0.0
--out ipv4addr 1.2.3.4/0						=1.2.3.4/0
--out ipv4addr 0/0							=0.0.0.0/0
--out ipv4addr --printcompressed 0/0					=0/0
--out ipv4addr --printcompressed 1.2.3.4/24				=1.2.3.4/24
--out ipv4addr --printcompressed 1.2.3.0/24				=1.2.3/24
--out ipv4addr --printcompressed 1.2.3/24				=1.2.3/24
--out ipv4addr --printcompressed 1.2.0.0/24				=1.2/24
--out ipv4addr --printcompressed 1.2/24					=1.2/24
--out ipv4addr --printcompressed 1.0.0.0/24				=1/24
--out ipv4addr --printcompressed 1/24					=1/24
--out ipv4addr --maskprefix 1.2.3.4/24					=1.2.3.0/24
--out ipv4addr --maskprefix 1.2.3./24					=1.2.3.0/24
--out ipv4addr --maskprefix 1.2./24					=1.2.0.0/24
--out ipv4addr --maskprefix 1./24					=1.0.0.0/24
--out ipv4addr --masksuffix 1.2.3.4/24					=0.0.0.4/24
--out ipv4addr --maskprefix 1.2.3.4/22					=1.2.0.0/22
--out ipv4addr --masksuffix 1.2.3.4/22 					=0.0.3.4/22
# selecting suffix/prefix
--addr_to_uncompressed --printsuffix ::ffff:1.2.3.4/64			=0:ffff:1.2.3.4
--in ipv6 --out ipv6 --printsuffix --printuncompressed ::ffff:1.2.3.4/64			=0:ffff:1.2.3.4
--addr_to_uncompressed --printsuffix ::ffff:1.2.3.4/63			=0:0:ffff:1.2.3.4
--addr_to_uncompressed --printsuffix 3ffe:ffff:100:f101::1/64		=0:0:0:1
--addr_to_uncompressed --printprefix 3ffe:ffff:100:f101::1/64		=3ffe:ffff:100:f101
--addr_to_uncompressed --printprefix 3ffe:ffff:100:f101::1/65		=3ffe:ffff:100:f101:0
--in ipv6 --out ipv6 --printprefix --printuncompressed 3ffe:ffff:100:f101::1/65		=3ffe:ffff:100:f101:0
## octal IPv4 (with separator)
--in ipv4 --out ipv4 --print-octal 192.0.2.1				=0300.0000.0002.0001
--in ipv4 --out ipv4 --print-octal 192.0.2.1/24				=0300.0000.0002.0001/24
--in ipv4 --out ipv4 0300.0000.0002.0001				=192.0.2.1
--in ipv4 --out ipv4 0300.0000.0002.0001/24				=192.0.2.1/24
## octal IP
--in ipv4 --out octal 192.0.2.1						=\\\\0300\\\\00\\\\02\\\\01
--in ipv4 --out octal --printfulluncompressed 192.0.2.1			=\\\\0300\\\\0000\\\\0002\\\\0001
--in ipv6 --out octal 3ffe:ffff::1					=\\\\077\\\\0376\\\\0377\\\\0377\\\\00\\\\00\\\\00\\\\00\\\\00\\\\00\\\\00\\\\00\\\\00\\\\00\\\\00\\\\01
--in ipv6 --out octal --printfulluncompressed 3ffe:ffff::1		=\\\\0077\\\\0376\\\\0377\\\\0377\\\\0000\\\\0000\\\\0000\\\\0000\\\\0000\\\\0000\\\\0000\\\\0000\\\\0000\\\\0000\\\\0000\\\\0001
## full uncompressed
--addr_to_fulluncompressed 3ffe:ffff:100:f101::1			=3ffe:ffff:0100:f101:0000:0000:0000:0001
--in ipv6 --out ipv6 --printfulluncompressed 3ffe:ffff:100:f101::1 =3ffe:ffff:0100:f101:0000:0000:0000:0001
--addr_to_fulluncompressed 3ffe:ffff:100:f101::1/64			=3ffe:ffff:0100:f101:0000:0000:0000:0001/64
--addr_to_fulluncompressed ::13.1.68.3					=0000:0000:0000:0000:0000:0000:13.1.68.3
--addr_to_fulluncompressed ::ffff:13.1.68.3				=0000:0000:0000:0000:0000:ffff:13.1.68.3
--addr_to_fulluncompressed --uppercase ::ffff:13.1.68.3			=0000:0000:0000:0000:0000:FFFF:13.1.68.3
## ifinet6
--addr_to_ifinet6 3ffe:ffff:100:f101::1					=3ffeffff0100f1010000000000000001 00
--in ipv6 --out ifinet6 3ffe:ffff:100:f101::1			=3ffeffff0100f1010000000000000001 00
--addr_to_ifinet6 3ffe:ffff:100:f101::1/64				=3ffeffff0100f1010000000000000001 00 40
--in ipv6 --out ifinet6 3ffe:ffff:100:f101::1/64		=3ffeffff0100f1010000000000000001 00 40
## to compressed
--ifinet6_to_compressed 3ffeffff0100f1010000000000000001		=3ffe:ffff:100:f101::1
--in ifinet6 --out ipv6 --printcompressed 3ffeffff0100f1010000000000000001		=3ffe:ffff:100:f101::1
--ifinet6_to_compressed 3ffeffff0100f1010000000000000001 40		=3ffe:ffff:100:f101::1/64
--in ifinet6 --out ipv6 --printcompressed 3ffeffff0100f1010000000000000001 40		=3ffe:ffff:100:f101::1/64
## to base85
--addr_to_base85 1080:0:0:0:8:800:200c:417a				=4)+k&C#VzJ4br>0wv%Yp
--in ipv6 --out base85 1080:0:0:0:8:800:200c:417a		=4)+k&C#VzJ4br>0wv%Yp
--base85_to_addr 4)+k&C#VzJ4br>0wv%Yp					=1080:0:0:0:8:800:200c:417a
## MAC to EUI-64
--mac_to_eui64 00:50:BF:06:B4:F5					=250:bfff:fe06:b4f5
--mac_to_eui64 00:0:F:6:4:5						=200:fff:fe06:405
--in mac --out eui64 00:0:F:6:4:5					=200:fff:fe06:405
--mac_to_eui64 0050BF-06B4F5						=250:bfff:fe06:b4f5
--mac_to_eui64 0050BF06B4F5						=250:bfff:fe06:b4f5
## MAC to EUI-64 (autodetect mode, not supported in pipe mode)
NOPIPETEST--out eui64 00:0:F:6:4:5					=200:fff:fe06:405
## Interface identifier privacy conversion
--eui64_to_privacy 0123:4567:89ab:cdef 0123456789abcdef			=4462:bdea:8654:776d 486072ff7074945e
--in iid+token --out iid+token 0123:4567:89ab:cdef 0123456789abcdef			=4462:bdea:8654:776d 486072ff7074945e
## Bitstring
--addr_to_bitstring 3ffe:ffff::1					=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--in ipv6 --out bitstring 3ffe:ffff::1				=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--addr_to_bitstring 3ffe:ffff::1/64					=\\\\[x3ffeffff000000000000000000000001/128].ip6.arpa.
--addr_to_bitstring --printsuffix 3ffe:ffff::1/64			=\\\\[x0000000000000001/64]
--addr_to_bitstring --printprefix 3ffe:ffff::1/64			=\\\\[x3ffeffff00000000/64].ip6.arpa.
--addr_to_bitstring --printprefix --printstart 17 3ffe:ffff:0100:e100:0123:4567:89ab:cdef/64	=\\\\[xffff0100e100/48]
--addr_to_bitstring --uppercase 3ffe:ffff::1				=\\\\[x3FFEFFFF000000000000000000000001/128].IP6.ARPA.
--in bitstring --out ipv6 \\\\[x3FFEFFFF000000000000000000000001/64].IP6.ARPA.	=3ffe:ffff::1/64
## IPv4 -> IPv6 (6to4)
--ipv4_to_6to4addr 11.12.13.14						=2002:b0c:d0e::
--in ipv4 --out ipv6 --action conv6to4 11.12.13.14			=2002:b0c:d0e::
--action conv6to4 192.0.2.1						=2002:c000:201::
## IPv6 -> IPv4 (6to4)
--in ipv6 --out ipv4 --action conv6to4 2002:102:304::1			=1.2.3.4
--action conv6to4 2002:c000:201::					=192.0.2.1
## IPv4 -> IPv6 (NAT64)
--action convnat64 192.0.2.1						=64:ff9b::c000:201
## IPv6 -> IPv4 (NAT64)
--action convnat64 64:ff9b::c000:201					=192.0.2.1
## Prefix + MAC -> IPv6
--in prefix+mac fec0:0:0:1:: 01:23:45:67:89:01				=fec0::1:323:45ff:fe67:8901
NOPIPETEST--in prefix+mac 1:2:3:4::/56 11:22:33:44:55:aa --action prefixmac2ipv6 --out ipv6addr  --force-prefix 64	=1:2:3:4:1322:33ff:fe44:55aa/64
NOPIPETEST--in prefix+mac 1:2:3:4::/56 11:22:33:44:55:aa --action prefixmac2ipv6 --out ipv6addr --no-prefixlength	=1:2:3:4:1322:33ff:fe44:55aa
## IPv4 -> reverse
--in ipv4addr --out revipv4 1.2.3.4					=4.3.2.1.in-addr.arpa.
## Information
-i fe80::1								=*
-i -m ff02::1								=*
# Anonymization
--action anonymize fe80:0000:0000:0000:0200:5efe:192.0.2.143		=fe80::a909:5214:5c00:21
--action anonymize fe80:0000:0000:0000:0000:5e01:2345:6789		=fe80::a909:5214:e012:3451
--action anonymize fe80:0000:0000:0000:0200:5e01:2345:6789		=fe80::a909:5214:f012:345f
--action anonymize fe80:0000:0000:0000:0000:5eff:fe01:2345		=fe80::a909:5214:c010:1
--action anonymize 2001:0db8:0000:0000:81c0:0f3f:c807:1455		=2001:db8:0:9:a929:4941:0:c
--action anonymize 3ffe:831f:ce49:7601:8000:efff:af4a:86BF		=3ffe:831f:ce49:7601:8000:ffff:af4a:86ff
--action anonymize --mask-ipv4 16 3ffe:831f:ce49:7601:8000:efff:af4a:86BF	=3ffe:831f:ce49:7601:8000:ffff:af4a:ffff
--action anonymize 192.0.2.1						=192.0.2.0
--action anonymize --mask-ipv4 16 192.0.2.1				=192.0.0.0
--action anonymize 2001:1a:392e:a450:2cd3:75e1:6098:8104		=2001:19:a909:a909:a999:4843:0:e
--action anonymize 01:23:45:67:89:ab					=01:23:45:00:00:00
--action anonymize --mask-mac 48 01:23:45:67:89:ab			=01:23:45:67:89:ab
--action anonymize --mask-mac 47 01:23:45:67:89:ab			=01:23:45:67:89:aa
--action anonymize --mask-mac 44 01:23:45:67:89:ab			=01:23:45:67:89:a0
--action anonymize --mask-mac 41 01:23:45:67:89:a8			=01:23:45:67:89:80
--action anonymize --mask-mac 40 01:23:45:67:89:a8			=01:23:45:67:89:00
--action anonymize --mask-mac 8  ff:23:45:67:89:a8			=ff:00:00:00:00:00
--action anonymize --mask-mac 7  ff:23:45:67:89:a8			=fe:00:00:00:00:00
--action anonymize --mask-mac 1  ff:23:45:67:89:a8			=82:00:00:00:00:00
-A anonymize --anonymize-preset zeroize-paranoid   2001:db8:2280:6901:224:21ff:fe01:2345  =2001:db8:2200::
-A anonymize --anonymize-preset zeroize-careful    2001:db8:2280:6901:224:21ff:fe01:2345  =2001:db8:2280:0:224:21ff:fe00:0
-A anonymize --anonymize-preset zeroize-standard   2001:db8:2280:6901:224:21ff:fe01:2345  =2001:db8:2280:6900:224:21ff:fe00:0
-A anonymize --anonymize-preset anonymize-paranoid 2001:db8:2280:6901:224:21ff:fe01:2345  =2001:db8:2209:a909:a969:4291:4022:4213
-A anonymize --anonymize-preset anonymize-careful  2001:db8:2280:6901:224:21ff:fe01:2345  =2001:db8:2280:a909:a949:4291:4022:4219
-A anonymize --anonymize-preset anonymize-standard 2001:db8:2280:6901:224:21ff:fe01:2345  =2001:db8:2280:6909:a929:4291:4022:4217
# RFC 5952 4.2.1
--in ipv6addr --out ipv6addr 2001:db8:0:0:0:0:2:1			=2001:db8::2:1
# RFC 5952 4.2.2
--in ipv6addr --out ipv6addr 2001:db8:0:1:1:1:1:1			=2001:db8:0:1:1:1:1:1
# RFC 5952 4.2.3
--in ipv6addr --out ipv6addr 2001:0:0:1:0:0:0:1			 	=2001:0:0:1::1
--in ipv6addr --out ipv6addr 2001:db8:0:0:1:0:0:1 			=2001:db8::1:0:0:1
# RFC 5952 4.2.4
--in ipv6addr --out ipv6addr 2001:DB8:0:0:1:0:0:1		 	=2001:db8::1:0:0:1
# 6rd
--action 6rd_local_prefix --6rd_prefix 2607:fedc:ff40::/43 --6rd_relay_prefix 6.230.0.0/15 6.231.32.33	=2607:fedc:ff52:210::/60
--action 6rd_local_prefix --6rd_prefix 2001:0b4f::/28 192.0.2.1		=2001:b4c:0:2010::/60
--action 6rd_local_prefix --6rd_prefix 2001:0b4f::/29 192.0.2.1		=2001:b4e:0:1008::/61
--action 6rd_local_prefix --6rd_prefix 2001:0b4f::/30 192.0.2.1		=2001:b4f:0:804::/62
--action 6rd_local_prefix --6rd_prefix 2001:0b4f::/31 192.0.2.1		=2001:b4f:8000:402::/63
--action 6rd_local_prefix --6rd_prefix 2001:0b4f::/32 192.0.2.1		=2001:b4f:c000:201::/64
-A 6rd_extract_ipv4 --6rd_prefixlength 28 2001:b4c:0:2010::/60		=192.0.2.1
# IPv6 literal
--in ipv6literal fe80--1.IPV6-LITERAL.NET				=fe80::1
--in ipv6literal fe80--1.IPV6-liTERal.NET				=fe80::1
--in ipv6literal fe80--218-8bff-fe17-a226s4.ipv6-literal.net		=fe80::218:8bff:fe17:a226%4
--out ipv6literal -u --in ipv6addr 2001:db8::1				=2001-DB8--1.IPV6-LITERAL.NET
--out ipv6literal -U --in ipv6addr 2001:db8::1				=2001-db8-0-0-0-0-0-1.ipv6-literal.net
--out ipv6literal -F --in ipv6addr 2001:db8::1				=2001-0db8-0000-0000-0000-0000-0000-0001.ipv6-literal.net
--out ipv6literal --in ipv6addr fe80::1%0				=fe80--1s0.ipv6-literal.net
--out ipv6literal -F --in ipv6addr fe80::1%0				=fe80-0000-0000-0000-0000-0000-0000-0001s0.ipv6-literal.net
--out ipv6addr -U --in ipv6addr fe80::1%eth0				=fe80:0:0:0:0:0:0:1%eth0
--out ipv6addr -F --in ipv6addr fe80::1%eth0				=fe80:0000:0000:0000:0000:0000:0000:0001%eth0
# hex
-O hex 0123:4567:89ab:cdef:0000:1111:2222:3333					=0123456789abcdef0000111122223333
-O hex -u 0123:4567:89ab:cdef:0000:1111:2222:3333				=0123456789ABCDEF0000111122223333
-O hex --forceprefix 32 --printsuffix 0123:4567:89ab:cdef:0000:1111:2222:3333	=89abcdef0000111122223333
-O hex --forceprefix 32 --printprefix 0123:4567:89ab:cdef:0000:1111:2222:3333	=01234567
-O hex --forceprefix 64 --printsuffix 0123:4567:89ab:cdef:0000:1111:2222:3333	=0000111122223333
-O hex --forceprefix 64 --printprefix 0123:4567:89ab:cdef:0000:1111:2222:3333	=0123456789abcdef
-O hex 1.2.3.4									=01020304
-O hex --forceprefix 8 --printsuffix 1.2.3.4					=020304
-O hex --forceprefix 8 --printprefix 1.2.3.4					=01
-O hex --forceprefix 16 --printsuffix 1.2.3.4					=0304
-O hex --forceprefix 16 --printprefix 1.2.3.4					=0102
-O hex --forceprefix 24 --printsuffix 1.2.3.4					=04
-O hex --forceprefix 24 --printprefix 1.2.3.4					=010203
-O hex -u 192.168.1.1								=C0A80101
# nibble strings
-O ipv6addr 1.ip6.arpa								=1000::/4
-O ipv6addr 1.2.ip6.arpa							=2100::/8
-O ipv6addr 1.2.3.ip6.arpa							=3210::/12
-O ipv6addr 1.2.3.4.ip6.arpa							=4321::/16
# Extraction
NOPIPETEST--in ipv6 2001:db8::a8bb:ccff:fedd:eeff --out eui64                             =aa:bb:cc:ff:fe:dd:ee:ff
NOPIPETEST--in ipv6 2001:db8::a8bb:ccff:fedd:eeff --out mac                               =aa:bb:cc:dd:ee:ff
# Countrycode
NOPIPETEST-A addr2cc 3ffe:831f:ce49:7601:8000:efff:af4a:86bf			=IT;DB_IPV6_CC
NOPIPETEST-A addr2cc 8.8.8.8							=US;DB_IPV4_CC
NOPIPETEST--addr2cc 3ffe:831f:ce49:7601:8000:efff:af4a:86bf			=IT;DB_IPV6_CC
NOPIPETEST--addr2cc 8.8.8.8							=US;DB_IPV4_CC
# revnibble >= 3.1.0
NOPIPETEST9.b.3.2.9.7.f.1.9.a.e.5.3.8.5.f					=::f583:5ea9:1f79:23b9/64
NOPIPETEST9.b.3.2.9.7.f.1.9.a.e.5.3.8.5.f --printsuffix				=f583:5ea9:1f79:23b9
NOPIPETEST0.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f					=::fedc:ba98:7654:3210/64
NOPIPETEST0.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.0.1					=::10:fedc:ba98:7654:3210/56
NOPIPETEST0.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.a.b					=::ba:fedc:ba98:7654:3210/56
NOPIPETEST0.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.a					=::a:fedc:ba98:7654:3210/60
NOPIPETEST0.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.a.b.c					=::cba:fedc:ba98:7654:3210/52
NOPIPETEST0.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.a.b.c.d				=::dcba:fedc:ba98:7654:3210/48
NOPIPETEST0.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.a.b.c.d --printsuffix			=dcba:fedc:ba98:7654:3210
NOPIPETEST0.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.a.b.c --printsuffix			=cba:fedc:ba98:7654:3210
END
}


# Test Scenarios for autodetection "bad case"
testscenarios_auto_bad() {
cat <<END | grep -v '^#'
1.2.3.r4									ipv4addr
2.2.3.4/-1									ipv4addr
2.2.3.4/33									ipv4addr
1.2.3.4/									ipv4addr
1.2.3.4/00									ipv4addr
0300.0000.0002.0001/33								ipv4addr
0300.0000.0002.0001.1								ipv4addr
0500.0000.0002.0001.1								ipv4addr
320.2.3.4									ipv4addr
0/00										ipv4addr
1										ipv4addr
1.										ipv4addr
1.2										ipv4addr
1.2.										ipv4addr
1.2.3										ipv4addr
1.2.3.										ipv4addr
1.2.3.4.5/10									ipv4addr
/10										ipv4addr
./10										ipv4addr
01:23:r5:67:89:01								mac
2002:102:304::r1								ipv6addr
2002:102:304::1/-1								ipv6addr
2002:102:304::1/129								ipv6addr
1.0.0.0.r.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.	revnibbles.int
1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.arpa.arpa.	revnibbles.arpa
0.0.0.0.01.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.arpa.		revnibbles.arpa
123456789									ipv4hex
fe80---1.IPV6-LITERAL.NET							ipv6literal
END
}

# Test Scenarios for pipe handling
testscenarios_pipe() {
	cat <<END
3ffe::1:ff00:1234,--in ipv6addr --out ipv6addr --printuncompressed,3ffe:0:0:0:0:1:ff00:1234
3ffe::1:ff00:1234,--in ipv6addr --out ipv6addr --printuncompressed --printprefix --forceprefix 96,3ffe:0:0:0:0:1
END

}

## main ##

# Test proper option definitions
test="test proper option definition"
echo "INFO  : $test"
if $verbose; then
	./ipv6calc -h
	retval=$?
else
	./ipv6calc -h 2>/dev/null
	retval=$?
fi
if [ $retval -ne 1 ]; then
	echo "ERROR : something wrong in option definition"
	exit 1
fi
echo "INFO  : $test successful"


test="run 'ipv6calc' version test"
echo "INFO  : $test"
if $verbose; then
	./ipv6calc -vvv
	retval=$?
else
	./ipv6calc -vvv 2>/dev/null
	retval=$?
fi
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6calc -vvv'!"
	exit 1
fi
echo "INFO  : $test successful"


test="run 'ipv6calc' version test in debug mode"
echo "INFO  : $test"
if $verbose; then
	./ipv6calc -vvv -d -1
	retval=$?
else
	./ipv6calc -vvv -d -1 >/dev/null 2>&1
	retval=$?
fi
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6calc -vvv -d -1'!"
	exit 1
fi
echo "INFO  : $test successful"


test="run 'ipv6calc' version help test for feature tokens"
echo "INFO  : $test"
if $verbose; then
	./ipv6calc -v -h
	retval=$?
else
	./ipv6calc -v -h 2>/dev/null
	retval=$?
fi
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6calc -v -h'!"
	exit 1
fi
echo "INFO  : $test successful"


test="test './ipv6calc -v'"
echo "INFO  : $test"
output="`./ipv6calc -v 2>&1`"
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing: ipv6calc -v: $output"
	exit 1
fi
echo "INFO  : $test successful"


test="test './ipv6calc -v -v'"
echo "INFO  : $test"
output="`./ipv6calc -v -v 2>&1`"
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing: ipv6calc -v -v: $output"
	exit 1
fi
echo "INFO  : $test successful"


test="run 'ipv6calc' function tests"
echo "INFO  : $test"
testscenarios | sed 's/NOPIPETEST//' | while read line; do
	if [ -z "$line" ]; then
		# end
		continue
	fi

	# extract result
	command="`echo $line | awk -F= '{ print $1 }' | sed 's/ $//g'`"
	result="`echo $line | awk -F= '{ for (i=2; i <= NF; i++) printf "%s%s", $i, (i<NF) ? "=" : ""; }'`"
	# split condition
	condition=`echo "$result" | awk -F';' '{ print $2 }'`
	result=`echo "$result" | awk -F';' '{ print $1 }'`
	if [ -z "$result" -o -z "$command" ]; then
		echo "Something is wrong in line '$line'"
		exit 1
	fi

	info="INFO  : test './ipv6calc $command' for '$result'"
	[ -n "$condition" ] && info="$info (condition=$condition)"
	$verbose && echo "$info"

	# check condition
	if [ -n "$condition" ]; then
		if ./ipv6calc -v 2>&1 | grep -qw "$condition"; then
			true
		else
			echo "NOTICE: condition not satisfied ($condition): ./ipv6calc $command"
			continue
		fi
	fi

	# get result
	output="`./ipv6calc -q $command`"
	retval=$?
	if [ $retval -ne 0 ]; then
		$verbose || echo "$info"
		echo "ERROR: problem executing: ./ipv6calc $command"
		exit 1
	fi
	# Check result
	if [ "$result" != "*" ]; then
		if [ "$output" != "$result" ]; then
			$verbose || echo "$info"
			echo "ERROR: result '$output' doesn't match: $result"
			exit 1
		fi
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' action 'genprivacyiid'"
echo "INFO  : $test"
testscenarios_genprivacyiid | while read in1 in2 out1 out2; do
	info="INFO  : test './ipv6calc -q -A genprivacyiid $in1 $in2'"
	$verbose && echo "$info"

	result="`./ipv6calc -q -A genprivacyiid $in1 $in2`"
	retval=$?

	if [ $retval -ne 0 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi

	if [ "$result" != "$out1 $out2" ]; then
		$verbose || echo "$info"
		echo "Result is not matching!"
		echo "Result is      : $result"
		echo "Result expected: $out1 $out2"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' input validation tests (empty input)"
echo "INFO  : $test"
./ipv6calc -m --in -? | while read inputformat; do
	if echo $inputformat | grep -q '+'; then
		info="INFO  : test './ipv6calc -q --in $inputformat \"\" \"\"'"
		$verbose && echo "$info"
		if $verbose; then
			./ipv6calc -q --in $inputformat "" ""
			retval=$?
		else
			./ipv6calc -q --in $inputformat "" "" 2>/dev/null
			retval=$?
		fi
	else
		info="INFO  : test './ipv6calc -q --in $inputformat \"\"'"
		$verbose && echo "$info"
		if $verbose; then
			./ipv6calc -q --in $inputformat ""
			retval=$?
		else
			./ipv6calc -q --in $inputformat "" 2>/dev/null
			retval=$?
		fi
	fi
	if [ $retval -ne 1 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' input validation tests (too long input)"
echo "INFO  : $test"
line="`perl -e 'print "x" x300'`"
./ipv6calc -m --in -? | while read inputformat; do
	if echo $inputformat | grep -q '+'; then
		info="INFO  : test './ipv6calc -q --in $inputformat \"$line\" \"$line\"'"
		$verbose && echo "$info"
		if $verbose; then
			./ipv6calc -q --in $inputformat "$line" "$line"
			retval=$?
		else
			./ipv6calc -q --in $inputformat "$line" "$line" 2>/dev/null
			retval=$?
		fi
	else
		info="INFO  : test './ipv6calc -q --in $inputformat \"$line\"'"
		$verbose && echo "$info"
		if $verbose; then
			./ipv6calc -q --in $inputformat "$line"
			retval=$?
		else
			./ipv6calc -q --in $inputformat "$line" 2>/dev/null
			retval=$?
		fi
	fi
	if [ $retval -ne 1 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' input validation tests (strange input)"
echo "INFO  : $test"
./ipv6calc -m --in -? | while read inputformat; do
	case $inputformat in
	    hex|ifinet6)
		line="`perl -e 'print "x" x32'`"
		;;
	    *)
		line="`perl -e 'print "x" x11'`"
		;;
	esac
	if echo $inputformat | grep -q '+'; then
		info="INFO  : test './ipv6calc -q --in $inputformat \"$line\" \"$line\"'"
		$verbose && echo "$info"
		if $verbose; then
			./ipv6calc -q --in $inputformat "$line" "$line"
			retval=$?
		else
			./ipv6calc -q --in $inputformat "$line" "$line" 2>/dev/null
			retval=$?
		fi
	else
		info="INFO  : test './ipv6calc -q --in $inputformat \"$line\"'"
		$verbose && echo "$info"
		if $verbose; then
			./ipv6calc -q --in $inputformat "$line"
			retval=$?
		else
			./ipv6calc -q --in $inputformat "$line" 2>/dev/null
			retval=$?
		fi
	fi
	if [ $retval -ne 1 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' input autodetection tests (good cases)"
echo "INFO  : $test"
testscenarios_auto_good | while read input dummy; do
	info="INFO  : test './ipv6calc -q \"$input\"'"
	$verbose && echo "$info"
	./ipv6calc -q "$input" >/dev/null
	retval=$?
	if [ $retval -ne 0 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' input autodetection tests (bad cases)"
echo "INFO  : $test"
testscenarios_auto_bad | while read input dummy; do
	info="INFO  : test './ipv6calc -q \"$input\"'"
	$verbose && echo "$info"
	if $verbose; then
		./ipv6calc -q "$input"
		retval=$?
	else
		./ipv6calc -q "$input" >/dev/null 2>&1
		retval=$?
	fi
	if [ $retval -eq 0 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc' ($retval)!"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"

test="run 'ipv6calc' input tests (good cases)"
echo "INFO  : $test"
testscenarios_auto_good | while read input type; do
	info="INFO  : test './ipv6calc --in $type -q \"$input\"'"
	$verbose && echo "$info"
	./ipv6calc --in $type -q "$input" >/dev/null
	retval=$?
	if [ $retval -ne 0 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' pipe tests (1)"
echo "INFO  : $test"
testscenarios_pipe | while IFS="," read input arguments result; do
	info="INFO  : test 'echo $input | ./ipv6calc $arguments | grep \"^$result\$\"'"
	$verbose && echo "$info"
	output=$(echo -e $input | ./ipv6calc $arguments | grep "^$result\$")
	retval=$?
	$verbose && echo "$output"
	if [ $retval -ne 0 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc' ($retval)!"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' pipe tests (2)"
echo "INFO  : $test"
testscenarios | grep -v "^NOPIPETEST" | while read line; do
	# reuse original test cases
	# extract result
	command="`echo "$line" | awk -F= '{ print $1 }' | sed 's/ $//g'`"
	result="`echo "$line" | awk -F= '{ for (i=2; i <= NF; i++) printf "%s%s", $i, (i<NF) ? "=" : ""; }'`"
	#echo "command=$command" >&2
	stdin="`echo "$command" | awk '{ print $NF }'`"
	options="`echo "$command" | awk '{ for (i=1; i < NF; i++) printf "%s ", $i; }'`"
	case $command in
	    *ifinet*)
		if echo "$stdin" | grep -q "^..$"; then
			stdin="`echo "$command" | awk '{ for (i=NF-1; i <= NF; i++) printf "%s ", $i; }'`"
			options="`echo "$command" | awk '{ for (i=1; i < NF-1; i++) printf "%s ", $i; }'`"
		fi
		;;
	    *eui64_to_privacy*|*iid+token*|*prefix+mac*)
		stdin="`echo "$command" | awk '{ for (i=NF-1; i <= NF; i++) printf "%s ", $i; }'`"
		options="`echo "$command" | awk '{ for (i=1; i < NF-1; i++) printf "%s ", $i; }'`"
		;;
	esac
	info="INFO  : test 'echo $stdin | ./ipv6calc $options | grep \"^$result\$\"'"
	$verbose && echo "$info"
	output="`echo -e $stdin | ./ipv6calc $options`"
	retval=$?
	if [ $retval -ne 0 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc' ($retval)!"
		exit 1
	fi
	# Check result
	if [ "$result" != "*" ]; then
		if [ "$output" != "$result" ]; then
			$verbose || echo "$info"
			echo "Result '$output' doesn't match!"
			exit 1
		fi
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


test="run 'ipv6calc' pipe mode input validation tests (too long input)"
echo "INFO  : $test"
./ipv6calc -m --in -? | while read inputformat; do
	if echo $inputformat | grep -q '+'; then
		info="INFO  : test '8192*x 8192*x | ./ipv6calc -q --in $inputformat"
		$verbose && echo "$info"
		output=$(perl -e 'print "x" x8192 . " " . "y" x8192' | ./ipv6calc -q --in $inputformat 2>&1)
		retval=$?
	else
		info="INFO  : test '8192*x | ./ipv6calc -q --in $inputformat"
		$verbose && echo "$info"
		output=$(perl -e 'print "x" x8192' | ./ipv6calc -q --in $inputformat 2>&1)
		retval=$?
	fi
	$verbose && echo "$output"
	if [ $retval -ne 0 -a $retval -ne 1 ]; then
		$verbose || echo "$info"
		echo "Error executing 'ipv6calc'!"
		exit 1
	fi
	$verbose || echo -n "."
done || exit 1
$verbose || echo
echo "INFO  : $test successful"


retval=$?
if [ $retval -eq 0 ]; then
	echo "INFO  : all tests were successfully done!"
	exit 0
else
	echo "ERROR : tests failed! (code $retval)"
	exit $retval
fi

