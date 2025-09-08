#!/usr/bin/env bash
#
# Project    : ipv6calc
# File       : test_scenarios.sh
# Copyright  : 2001-2025 by Peter Bieringer <pb (at) bieringer.de>
# License    : GPLv2
#
# Test patterns for ipv6calc (functions only)

# Test Scenarios for autodetection "good case"
testscenarios_auto_good() {
	cat <<END | grep -v '^#'
3ffe:831f:ce49:7601:8000:efff:af4a:86BF						ipv6addr
8.8.8.8										ipv4addr
8.8.8.8/0									ipv4addr
8.8.8.8/32									ipv4addr
01:23:45:67:89:01								mac
01-23-45-67-89-01								mac
012345678901									mac
012345-678901									mac
2002:102:304::1/0								ipv6addr
2002:102:304::1/128								ipv6addr
\\\\[x3FFEFFFF000000000000000000000001/64].IP6.ARPA.				bitstring
1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.	revnibbles.int
1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.arpa.	revnibbles.arpa
0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.arpa.		revnibbles.int
12345678									ipv4hex
4)+k&C#VzJ4br>0wv%Yp								base85
# RFC 5952 2.x
2001:db8:0:0:1:0:0:1								ipv6addr
2001:0db8:0:0:1:0:0:1								ipv6addr
2001:db8::1:0:0:1								ipv6addr
2001:db8::0:1:0:0:1								ipv6addr
2001:0db8::1:0:0:1								ipv6addr
2001:db8:0:0:1::1								ipv6addr
2001:db8:0000:0:1::1								ipv6addr
2001:DB8:0:0:1::1								ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:0001						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:001						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:01						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:1						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd::1							ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:0:1						ipv6addr
2001:db8:0:0:0::1								ipv6addr
2001:db8:0:0::1									ipv6addr
2001:db8:0::1									ipv6addr
2001:db8::1									ipv6addr
2001:db8::aaaa:0:0:1								ipv6addr
2001:db8:0:0:aaaa::1								ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:aaaa						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:AAAA						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:AaAa						ipv6addr
64:ff9b::192.0.2.33								ipv6addr
fe80::1%eth0									ipv6addr
fe80--1.IPV6-LITERAL.NET							ipv6literal
2001-db8-0-0-0-0-0-1.ipv6-literal.net						ipv6literal
2001-0db8-0000-0000-0000-0000-0000-0001.ipv6-literal.net			ipv6literal
2001-db8--1.ipv6-literal.net							ipv6literal
fe80--218-8bff-fe17-a226s4.ipv6-literal.net					ipv6literal
END
}

# Test Scenarios for reserved IPv4 addresses (RFC 5735) https://en.wikipedia.org/wiki/Reserved_IP_addresses
testscenarios_ipv4_reserved() {
	cat <<END | grep -v '^#'
0.0.0.0			RFC1122#3.2.1.3
0.255.255.255		RFC1122#3.2.1.3
9.255.255.255		noRFC		skip-anon-test
10.0.0.0		RFC1918#3
10.255.255.255		RFC1918#3
11.0.0.0		noRFC		skip-anon-test
100.63.255.255		noRFC
100.64.0.0		RFC6598
100.127.255.255		RFC6598
100.128.0.0		noRFC
126.255.255.255		noRFC
127.0.0.0		RFC1122#3.2.1.3
127.255.255.255		RFC1122#3.2.1.3
128.0.0.0		noRFC		skip-anon-test
169.253.255.255		noRFC
169.254.0.0		RFC3927#1
169.254.255.255		RFC3927#1
169.255.0.0		noRFC		skip-anon-test
172.15.255.255		noRFC
172.16.0.0		RFC1918#3
172.31.255.255		RFC1918#3
172.32.0.0		noRFC
191.255.255.255		noRFC
192.0.0.0		RFC5736#1
192.0.0.255		RFC5736#1
192.0.1.0		noRFC		skip-anon-test
192.0.1.255		noRFC		skip-anon-test
192.0.2.0		RFC5737#1
192.0.2.255		RFC5737#1
192.0.3.0		noRFC
192.88.98.255		noRFC		skip-anon-test
192.88.99.0		RFC3068#2.3
192.88.99.255		RFC3068#2.3
192.88.100.0		noRFC		skip-anon-test
192.167.255.255		noRFC
192.168.0.0		RFC1918#3
192.168.255.255		RFC1918#3
192.169.0.0		noRFC		skip-anon-test
198.17.255.255		noRFC
198.18.0.0		RFC2544#C.2.2
198.19.255.255		RFC2544#C.2.2
198.20.0.0		noRFC
198.51.99.255		noRFC		skip-anon-test
198.51.100.0		RFC5737#3
198.51.100.255		RFC5737#3
198.51.101.0		noRFC		skip-anon-test
203.0.112.255		noRFC
203.0.113.0		RFC5737#3
203.0.113.255		RFC5737#3
203.0.114.0		noRFC
223.255.255.255		noRFC
224.0.0.0		RFC3171#2
239.255.255.255		RFC3171#2
240.0.0.0		RFC1112#4
255.255.255.254		RFC1112#4
255.255.255.255		RFC919#7
END
}

# Test Scenarios for reserved IPv6 addresses (RFC 5156)
testscenarios_ipv6_reserved() {
	cat <<END | grep -v '^#'
::			RFC4291#2.5.2
::1			RFC4291#2.5.3
::0.0.0.2		RFC4291#2.5.5.1			skip-anon-test
::255.255.255.255	RFC4291#2.5.5.1
::ffff:0.0.0.0		RFC4291#2.5.5.2
::ffff:255.255.255.255	RFC4291#2.5.5.2
0100::					RFC6666
0100:0000:0000:0000:ffff:ffff:ffff:ffff	RFC6666
0100:0000:0000:0001::			noRFC
2001:0000::				RFC4380#6
2001:0000:ffff:ffff:ffff:ffff:ffff:ffff	RFC4380#6
2001:0010::				RFC4843#2
2001:001f:ffff:ffff:ffff:ffff:ffff:ffff	RFC4843#2
2001:0db8::				RFC3849#4
2001:0db8:ffff:ffff:ffff:ffff:ffff:ffff	RFC3849#4
2002::					RFC3056#2
2002:ffff:ffff:ffff:ffff:ffff:ffff:ffff	RFC3056#2
fc00::					RFC4193#3.1
fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff	RFC4193#3.1
fe80::					RFC4291#2.5.6
fe9f:ffff:ffff:ffff:ffff:ffff:ffff:ffff	RFC4291#2.5.6
fec0::					RFC4291#2.5.7
fed0:ffff:ffff:ffff:ffff:ffff:ffff:ffff	RFC4291#2.5.7
ff00::					RFC4291#2.7
ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff	RFC4291#2.7
END
}

# Test Scenarios for filter
testscenarios_filter() {
	cat <<END | grep -v '^#'
3ffe:1a05:510:200:200:5efe:140.173.129.8	iid-isatap
3ffe:831f:ce49:7601:8000:efff:af4a:86BF		teredo
2001:db8:1:4135:7536:e4f:5513:4dd2		iid-random
2001:db8:1:4135:208:54ff:fe00:1			iid-global
2001:db8::a909:4291:c02d:5d1d			anonymized-iid
2001:db8::a909:4291:c02d:5d1d			iid-local
2001:db8::a909:4291:4002:4217			anonymized-iid
2001:db8::a909:4291:4002:4217			iid-eui48
2001:db8::a909:4941:0:7				anonymized-iid
2001:db8::a909:4941:0:7				iid-random
2001:a69:a909:a909:a999:4941::1			anonymized-iid
fe80::200:5efe:192.0.2.143			iid-isatap
fe80::200:5efe:192.0.2.143			iid-global
fe80::200:5efe:192.0.2.143			link-local
fe80::200:5efe:192.0.2.143			iid
fe80::5efe:192.168.1.1				iid-isatap
fe80::5efe:192.168.1.1				iid-local
fe80::5efe:192.168.1.1				link-local
fe80::5efe:192.168.1.1				iid
fe80::5eff:fe01:2345				iid
fe80::5eff:fe01:2345				iid-local
fe80::5eff:fe01:2345				iid-isatap
fe80::5e01:2345:6789				iid
fe80::5e01:2345:6789				iid-local
fe80::5e01:2345:6789				iid-isatap
2001:1a:392e:a450:2cd3:75e1:6098:8104		orchid
2001:2a:392e:a450:2cd3:75e1:6098:8104		orchid
2001:1a:392e:a450:2cd3:75e1:6098:8104		^iid-local
2001:1a:392e:a450:2cd3:75e1:6098:8104		^iid
2001:1a:392e:a450:2cd3:75e1:6098:8104		^iid-random
2001:19:a909:a909:a999:4843::e			orchid
2001:19:a909:a909:a999:4843::e			anonymized-iid
2001:19:a909:a909:a999:4843::e			^iid-random
3ffe:1a05:510:a909:a949:5214:48ca:d81d		anonymized-iid
3ffe:1a05:510:a909:a949:5214:48ca:d81d		iid-isatap
3ffe:1a05:510:a909:a949:5214:48ca:d81d		^iid-random
2001:4860:4801:2:3800:6006:1300:b075		^iid-random
3ffe:ffff::a4ff:fe01:2345			iid-local,iid-eui48
3ffe:ffff::0200:a4ff:fe01:2345			iid-global,iid-eui48
fe80:0000:0000:0000:0200:5efe:192.0.2.143	^anonymized-iid
fe80:0000:0000:0000:0000:5e01:2345:6789		^anonymized-iid
fe80:0000:0000:0000:0000:5eff:fe01:2345		^anonymized-iid
2001:0db8:0000:0000:81c0:0f3f:c807:1455		^anonymized-iid
3ffe:831f:ce49:7601:8000:efff:af4a:86BF		^anonymized-iid
2001:1a:392e:a450:2cd3:75e1:6098:8104		^anonymized-iid
3ffe:ffff::000:a4ff:fe01:2345			^anonymized-iid
2002:c0a8:f900:9:a929:4291:4021:132d		anonymized-iid
2002:c0a8:f900:9:a929:4291:4021:132d		6to4
2001::d91f:cca0:3875:ffff:263c:ffff		anonymized-iid
2001::d91f:cca0:3875:ffff:263c:ffff		teredo
2002:c000:0251::c000:0251			6to4-microsoft
2002:c000:200:9:a929:4291:8c00:28		6to4-microsoft
2002:c000:200:9:a929:4291:8c00:28		anonymized-iid
a909:16fa:9092:23ff:a909:4941::7		anonymized-iid
a909:16fa:9092:23ff:a909:4941::7		anonymized-prefix
242.222.34.63					anonymized
64:ff9b::f618:3b41				anonymized-iid
2002:f618:3b41:0009:a929:4291:8f61:83b0		anonymized-iid
2002:f618:3b41:0009:a929:4291:8f61:83b0		anonymized-prefix
3ffe:831f:ce49:7601:8000:ffff:0a0b:f33a		anonymized-iid
3ffe:831f:ce49:7601:8000:ffff:0a0b:f33a		^anonymized-prefix
8.8.8.8						ipv4
8.8.8.8						ipv4.db.reg=ARIN#DB_IPV4_REG
8.8.8.8						ipv4.db.cc=US#DB_IPV4_CC
8.8.8.8						ipv4.db.asn=15169#DB_IPV4_AS
8.8.8.8						ipv4.addr=le=8.8.8.8
8.8.8.8						ipv4.addr=lt=8.8.8.9
8.8.8.8						ipv4.addr=ge=8.8.8.8
8.8.8.8						ipv4.addr=gt=8.8.8.7
2001:db8::2					ipv6.addr=le=2001:db8::2
2001:db8::2					ipv6.addr=lt=2001:db8::3
2001:db8::2					ipv6.addr=ge=2001:db8::2
2001:db8::2					ipv6.addr=gt=2001:db8::1
2001:db8::2					ipv6.addr=gt=2001:db8::1,ipv6.addr=lt=2001:db8::3
8.8.8.8						ipv4.addr=gt=8.8.8.7,ipv4.addr=lt=8.8.8.9
8.8.8.8						ipv4.addr=8.8.8.0/24
8.8.8.8						^ipv4.addr=8.8.4.0/24
2001:db8::2					ipv6.addr=2001:db8::/32
2001:db8::2					^ipv6.addr=2001:db9::/32
2001:db8::2					^ipv6.addr=ge=2001:db8::3
2001:db8::2					^ipv6.addr=lt=2001:db8::1
8.8.8.8						^ipv4.addr=gt=8.8.8.9
8.8.8.8						^ipv4.addr=lt=8.8.8.7
2610:00d0::1					lisp
2001:67c:198::1					lisp-proxyegresstunnelrouter-anycast,anycast,^unicast
2001:67c:28::1					lisp-mapresolver-anycast,anycast,^unicast
153.16.0.1					lisp,unicast
193.105.183.1					lisp,lisp-proxyegresstunnelrouter-anycast,anycast,^unicast
91.220.201.1					lisp,lisp-mapresolver-anycast,anycast,^unicast
192.88.99.1					anycast,^unicast,6to4relay
2001:0db8:0000:0000:81c0:0f3f:c807:1455 	iid-random,iid-local
2001:0db8:0000:0000::1			 	iid-local,^iid-random
3ffe::1						6bone
5f00::1						srv6
END
}

# Test scenarios for gienprivacyiid
testscenarios_genprivacyiid() {
	cat <<END | grep -v '^#'
0123:4567:89ab:cdef	0123456789abcdef	4462:bdea:8654:776d	486072ff7074945e
4462:bdea:8654:776d	486072ff7074945e	69ad:92b8:b11d:be18	d74e03fb77f2f561
END
}

# Test scenarios for anonymization options (ipv6calc & ipv6loganon)
testscenarios_anonymization_options() {
	cat <<END | grep -v '^#'
--mask-eui64  64 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef:123:4567
--mask-eui64  60 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef:123:4560
--mask-eui64  56 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef:123:4500
--mask-eui64  48 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef:123:0
--mask-eui64  40 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef:100:0
--mask-eui64  36 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef::
--mask-eui64  32 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef::
--mask-eui64  28 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cde0::
--mask-eui64  12 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89a0::
--mask-eui64   8 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:8900::
--mask-eui64   0 --anonymize-method zeroize	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500::
--anonymize-standard			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4509:a929:4291:c02d:5d15
--anonymize-careful			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:a909:a949:4291:c02d:5d13
--anonymize-paranoid			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:109:a909:a969:4291:c02d:5d1a
--anonymize-preset anonymize-standard	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4509:a929:4291:c02d:5d15
--anonymize-preset as			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4509:a929:4291:c02d:5d15
--anonymize-preset anonymize-careful	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:a909:a949:4291:c02d:5d13
--anonymize-preset ac			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:a909:a949:4291:c02d:5d13
--anonymize-preset anonymize-paranoid	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:109:a909:a969:4291:c02d:5d1a
--anonymize-preset ap			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:109:a909:a969:4291:c02d:5d1a
--anonymize-preset zeroize-standard	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef:100:0
--anonymize-preset zs			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4500:89ab:cdef:100:0
--anonymize-preset zeroize-careful	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:0:89ab:cd00::
--anonymize-preset zc			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:0:89ab:cd00::
--anonymize-preset zeroize-paranoid	2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:100::
--anonymize-preset zp			2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:100::
--mask-ipv4 32				12.34.56.78=12.34.56.78
--mask-ipv4 28				12.34.56.78=12.34.56.64
--mask-ipv4 24				12.34.56.78=12.34.56.0
--mask-ipv4 20				12.34.56.78=12.34.48.0
--mask-ipv4 16				12.34.56.78=12.34.0.0
--mask-ipv4 12				12.34.56.78=12.32.0.0
--mask-ipv4  8				12.34.56.78=12.0.0.0
--mask-ipv4  4				12.34.56.78=0.0.0.0
--mask-ipv4  0				12.34.56.78=0.0.0.0
--mask-ipv6 64				2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4567:a909:4291:c02d:5d1d
--mask-ipv6 60				2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4569:a919:4291:c02d:5d1f
--mask-ipv6 56				2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:4509:a929:4291:c02d:5d15
--mask-ipv6 48				2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db8:123:a909:a949:4291:c02d:5d13
--mask-ipv6 28				2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:db9:a909:a909:a999:4291:c02d:5d1f
--mask-ipv6 24				2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:d09:a909:a909:a9a9:4291:c02d:5d16
--mask-ipv6 20 				2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:909:a909:a909:a9b9:4291:c02d:5d14
--mask-ipv6 16				2001:0db8:0123:4567:89ab:cdef:0123:4567=2001:a909:a909:a909:a9c9:4291:c02d:5d14
--mask-mac  48				12:34:56:78:9a:bc=12:34:56:78:9a:bc
--mask-mac  40				12:34:56:78:9a:bc=12:34:56:78:9a:00
--mask-mac  28				12:34:56:78:9a:bc=12:34:56:70:00:00
--mask-mac  24				12:34:56:78:9a:bc=12:34:56:00:00:00
--mask-mac  20				12:34:56:78:9a:bc=12:34:50:00:00:00
--mask-mac  16				12:34:56:78:9a:bc=12:34:00:00:00:00
--mask-mac   8				12:34:56:78:9a:bc=12:00:00:00:00:00
--mask-mac   0 				12:34:56:78:9a:bc=02:00:00:00:00:00
--mask-ipv4 24				2002:0808:0808::211:32ff:fe13:956f=2002:808:800:9:a929:4291:4021:132d
--mask-ipv4 24				2002:c000:0251::c000:0251=2002:c000:200:9:a929:4291:8c00:28
--mask-ipv4 24 --anonymize-method zeroize	2002:c000:0251::c000:0251=2002:c000:200::192.0.2.0
--anonymize-standard			2001:db8:0123:4567:0250:c2ff:fe86:2345=2001:db8:123:4509:a929:4291:5021:8621
--anonymize-standard			2001:db8:0123:4567:0250:c286:2345:4567=2001:db8:123:4509:a929:4291:7021:8626
# EUI-64: local
--anonymize-preset zs --mask-eui64 0			12:34:56:78:9a:ff:ff:ff=02:00:00:00:00:00:00:00
--anonymize-preset zs --mask-eui64 31			12:34:56:ff:ff:ff:ff:ff=12:34:56:fe:00:00:00:00
--anonymize-preset zs --mask-eui64 32			12:34:56:ff:ff:ff:ff:ff=12:34:56:ff:00:00:00:00
--anonymize-preset zs --mask-eui64 33			12:34:56:ff:ff:ff:ff:ff=12:34:56:ff:80:00:00:00
--anonymize-preset zs --mask-eui64 55			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:fe:00
--anonymize-preset zs --mask-eui64 56			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:00
--anonymize-preset zs --mask-eui64 57			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:80
--anonymize-preset zs --mask-eui64 58			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:c0
--anonymize-preset zs --mask-eui64 59			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:e0
--anonymize-preset zs --mask-eui64 60			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:f0
--anonymize-preset zs --mask-eui64 61			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:f8
--anonymize-preset zs --mask-eui64 62			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:fc
--anonymize-preset zs --mask-eui64 63			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:fe
--anonymize-preset zs --mask-eui64 64			12:34:56:78:9a:ff:ff:ff=12:34:56:78:9a:ff:ff:ff
# EUI-48: local
--anonymize-preset zs --mask-mac 0			12:34:56:78:9a:ff=02:00:00:00:00:00
--anonymize-preset zs --mask-mac 31			12:34:56:ff:ff:ff=12:34:56:fe:00:00
--anonymize-preset zs --mask-mac 32			12:34:56:ff:ff:ff=12:34:56:ff:00:00
--anonymize-preset zs --mask-mac 33			12:34:56:ff:ff:ff=12:34:56:ff:80:00
# EUI-64: global (OUI-24)
--anonymize-preset zc		00:02:b3:ff:ff:ff:ff:ff=00:02:b3:00:00:00:00:00
# EUI-64: global (OUI-24) expanded EUI-48
--anonymize-preset zc		00:02:b3:ff:fe:ff:ff:ff=00:02:b3:ff:fe:00:00:00
# EUI-64: global (OUI-36)
--anonymize-preset zc		00:1b:c5:03:df:ff:ff:ff=00:1b:c5:03:d0:00:00:00
--anonymize-preset zc		70:b3:d5:f7:19:87:65:43=70:b3:d5:f7:10:00:00:00
# EUI-64: global (OUI-36) expanded EUI-48
--anonymize-preset zc		00:1b:c5:ff:fe:03:df:ff=00:1b:c5:ff:fe:03:d0:00
--anonymize-preset zc --mask-eui64 16 --mask-autoadjust no	00:1b:c5:ff:fe:03:df:ff=00:1b:00:00:00:00:00:00
# EUI-64: global (OUI-28)
--anonymize-preset zc		a0:3e:6b:23:67:01:43:21=a0:3e:6b:20:00:00:00:00
--anonymize-preset zc		b4:a2:eb:b9:87:65:43:21=b4:a2:eb:b0:00:00:00:00
# EUI-48: global (OUI-24)
--anonymize-preset zc		00:02:b3:ff:ff:ff=00:02:b3:00:00:00
--anonymize-preset zc		00:50:56:01:23:45=00:50:56:00:00:00
# EUI-48: global (OUI-36)
--anonymize-preset zc		00:1b:c5:03:df:ff=00:1b:c5:03:d0:00
--anonymize-preset zc		70:b3:d5:f7:19:87=70:b3:d5:f7:10:00
--anonymize-preset zc --mask-mac 16 --mask-autoadjust no	00:1b:c5:03:df:ff=00:1b:00:00:00:00
# EUI-48: global (OUI-28)
--anonymize-preset zc		a0:3e:6b:23:67:01=a0:3e:6b:20:00:00
--anonymize-preset zc		b4:a2:eb:b9:87:65=b4:a2:eb:b0:00:00
# LISP Anycast
--mask-ipv6 16				2001:67c:198:f123:0123:4567:89ab:cdef=2001:67c:198:a909:a949:4291:c02d:5d13
--mask-ipv6 24				2001:67c:198:f123:0123:4567:89ab:cdef=2001:67c:198:a909:a949:4291:c02d:5d13
--mask-ipv6 32				2001:67c:198:f123:0123:4567:89ab:cdef=2001:67c:198:a909:a949:4291:c02d:5d13
--mask-ipv6 40				2001:67c:198:f123:0123:4567:89ab:cdef=2001:67c:198:a909:a949:4291:c02d:5d13
--mask-ipv6 48				2001:67c:198:f123:0123:4567:89ab:cdef=2001:67c:198:a909:a949:4291:c02d:5d13
--mask-ipv6 56				2001:67c:198:f123:0123:4567:89ab:cdef=2001:67c:198:f109:a929:4291:c02d:5d15
--mask-ipv6 16				2001:67c:28:f123:0123:4567:89ab:cdef=2001:67c:28:a909:a949:4291:c02d:5d13
--mask-ipv6 24				2001:67c:28:f123:0123:4567:89ab:cdef=2001:67c:28:a909:a949:4291:c02d:5d13
--mask-ipv6 32				2001:67c:28:f123:0123:4567:89ab:cdef=2001:67c:28:a909:a949:4291:c02d:5d13
--mask-ipv6 40				2001:67c:28:f123:0123:4567:89ab:cdef=2001:67c:28:a909:a949:4291:c02d:5d13
--mask-ipv6 48				2001:67c:28:f123:0123:4567:89ab:cdef=2001:67c:28:a909:a949:4291:c02d:5d13
--mask-ipv6 56				2001:67c:28:f123:0123:4567:89ab:cdef=2001:67c:28:f109:a929:4291:c02d:5d15
END
}

testscenarios_anonymization_options_kp() {
	cat <<END | grep -v '^#'
--anonymize-preset kp			46.244.223.233=242.222.34.63
--anonymize-preset kp			2a04:8000:11e1:9501:e876:aee4:0721:e8ac=a909:118a:90a0:d40f:a909:4941:0:7
--anonymize-preset kp			::8.8.8.8=::245.216.59.65
--anonymize-preset kp			::ffff:8.8.8.8=::ffff:245.216.59.65
--anonymize-preset kp			::1=::1
--anonymize-preset kp			3ffe:831f:ce49:7601:8000:efff:af4a:86bf=3ffe:831f:ce49:7601:8000:ffff:a0b:f33a
--anonymize-preset kp			64:ff9b::0808:0808=64:ff9b::f5d8:3b41
--anonymize-preset kp			2001:0db8:0000:0000:81c0:0f3f:c807:1455=2001:db8::a909:4941:0:7
# EUI-48 OUI-24/28/36
--anonymize-preset kp			fd00:bbbb:bbbb:1:265e:beff:fe12:3456=fd00:bbbb:bbbb:9:a929:4291:4265:ebee
--anonymize-preset kp			fd00:bbbb:bbbb:1:0250:c2ff:fe12:3456=fd00:bbbb:bbbb:9:a929:4291:5021:1234
--anonymize-preset kp			fd00:bbbb:bbbb:1:0255:daff:fe12:3456=fd00:bbbb:bbbb:9:a929:4291:5030:1001
# EUI-64 OUI-24/28/36
--anonymize-preset kp			fd00:bbbb:bbbb:1:265e:be12:3456:789a=fd00:bbbb:bbbb:9:a929:4291:6265:ebeb
--anonymize-preset kp			fd00:bbbb:bbbb:1:0250:c212:3456:789a=fd00:bbbb:bbbb:9:a929:4291:7021:123c
--anonymize-preset kp			fd00:bbbb:bbbb:1:0255:da12:3456:789a=fd00:bbbb:bbbb:9:a929:4291:7030:1001
# do not anonymize LISP anycast prefix
--anonymize-preset kp			2001:67c:198::1=2001:67c:198:0:a909:4291:c02d:5d1d
END
}

testscenarios_showinfo() {
	cat <<END | grep -v '^#'
00:02:b3:01:23:45	OUI="Intel Corporation"
00:02:b3:01:23:45	EUI48_SCOPE=global
00:1b:c5:06:a3:45	OUI="IST GmbH"
00:1b:c5:06:a3:45	EUI48_SCOPE=global
00:50:c2:86:23:45	OUI="Elsys AG"
00:50:c2:86:23:45	EUI48_SCOPE=global
52:54:00:01:23:45	OUI="possible QEMU VM"
00:50:56:01:23:45	OUI="VMware, Inc. (possible VMware VM)"
00:02:b3:01:23:45:67:89	OUI="Intel Corporation"
00:02:b3:01:23:45:67:89	EUI64_SCOPE=global
00:1b:c5:06:a3:45:67:89	OUI="IST GmbH"
00:1b:c5:06:a3:45:67:89	EUI64_SCOPE=global
00:50:c2:86:23:45:67:89	OUI="Elsys AG"
00:50:c2:86:23:45:67:89	EUI64_SCOPE=global
2001:db8:123:4509:a929:4291:5021:8621	OUI="Elsys AG"
2001:db8:123:4509:a929:4291:5021:8621	EUI48_SCOPE=global
2001:db8:123:4509:a929:4291:7021:8626	OUI="Elsys AG"
2001:db8:123:4509:a929:4291:7021:8626	EUI64_SCOPE=global
2001:db8:11e0:6201:224:21ff:fe01:2345	EUI48_SCOPE=global
0123.4567.89ab				EUI48=01:23:45:67:89:ab
01-23-45-67-89-ab			EUI48=01:23:45:67:89:ab
012345-6789ab				EUI48=01:23:45:67:89:ab
0123456789ab				EUI48=01:23:45:67:89:ab
c0a80102				IPV4=192.168.1.2
0a010203				IPV4=10.1.2.3
a010203					IPV4=10.1.2.3
# non-anonymized IPv6 prefix
2001:a60:113a:0123:0123:4567:89ab:cdef	IPV6_REGISTRY=RIPENCC
# anonymized IPv6 prefix
a909:16fa:9092:23ff:a909:4941::7	IPV6_COUNTRYCODE=DE
a909:16fa:9092:23ff:a909:4941::7	IPV6_AS_NUM=8767
a909:16fa:9092:23ff:a909:4941::7	IPV6_REGISTRY=RIPENCC
# non-anonymized IPv4 address
46.244.223.233				IPV4_REGISTRY=RIPENCC
# anonymized IPv4 address
242.222.34.63				IPV4_COUNTRYCODE=DE
242.222.34.63				IPV4_AS_NUM=8767
246.24.59.65				IPV4_COUNTRYCODE=AU
::246.24.59.65				IPV4_COUNTRYCODE[246.24.59.65]=AU
::ffff:246.24.59.65			IPV4_COUNTRYCODE[246.24.59.65]=AU
2001:07fa:0:1::1			IPV6_REGISTRY=RIPENCC
2001:07fa:0:0::1			IPV6_REGISTRY=RIPENCC
# output variations
1.2.3.4					1.2.3.4|--mrtvo IPV4
1.2.3.4					"1.2.3.4"|--mrtvo IPV4 --mrqva
1.2.3.4					IPV4=1.2.3.4|--mrst IPV4
1.2.3.4					IPV4="1.2.3.4"|--mrst IPV4 --mrqva
2a01:238:423d:8800:85b3:9e6b:3019:8909	IPV4=""|--mrstpa IPV4 --mrqva
2a01:238:423d:8800:85b3:9e6b:3019:8909	"ipv6,unicast,global-unicast,productive,iid-random,iid,iid-local"|--mrqva --mrtvo IPV6_TYPE
2a01:238:423d:8800:85b3:9e6b:3019:8909	ipv6,unicast,global-unicast,productive,iid-random,iid,iid-local|--mrqvn --mrtvo IPV6_TYPE
2a01:238:423d:8800:85b3:9e6b:3019:8909	IPV6_TYPE=ipv6,unicast,global-unicast,productive,iid-random,iid,iid-local|--mrst IPV6_TYPE
1.2.3.4					ipv4,unicast,global|--mrtvo IPV._TYPE
2a01::1					ipv6,unicast,global-unicast,productive,iid,iid-local|--mrtvo IPV._TYPE
# 6rd
2001:b4c:0:2010::/60			IPV4[192.0.2.1]=192.0.2.1|--6rd_prefixlength 28
2001:b4e:0:1008::/61			IPV4[192.0.2.1]=192.0.2.1|--6rd_prefixlength 29
2001:b4f:0:804::/62			IPV4[192.0.2.1]=192.0.2.1|--6rd_prefixlength 30
2001:b4f:8000:402::/63			IPV4[192.0.2.1]=192.0.2.1|--6rd_prefixlength 31
2001:b4f:c000:201::/64			IPV4[192.0.2.1]=192.0.2.1|--6rd_prefixlength 32
# LISP
153.16.13.192				IPV4_REGISTRY=ARIN(LISP#aaronm-xtr)
2610:00d0:1100::			IPV6_REGISTRY=ARIN(LISP#asp-xtr)
END
}

testscenarios_showinfo_geoip() {
	cat <<END | grep -v '^#'
# non-anonymized IPv4 address
46.244.223.233				GEOIP_COUNTRY_SHORT=DE
46.244.223.233				IPV4_AS_NUM=8767		DB_IPV4_AS
46.244.223.233				IPV4_REGISTRY=RIPENCC
8.8.8.8					IPV4_COUNTRYCODE=US
::8.8.8.8				IPV4_COUNTRYCODE[8.8.8.8]=US
::ffff:8.8.8.8				IPV4_COUNTRYCODE[8.8.8.8]=US
# non-anonymized IPv6 prefix
2001:a60:113a:0123:0123:4567:89ab:cdef	IPV6_COUNTRYCODE=DE		DB_IPV6_CC
2001:a60:113a:0123:0123:4567:89ab:cdef	IPV6_AS_NUM=8767		DB_IPV6_AS
END
}

testscenarios_showinfo_ip2location() {
	cat <<END | grep -v '^#'
# non-anonymized IPv4 address
46.244.223.233				IPV4_COUNTRYCODE=DE
46.244.223.233				IP2LOCATION_COUNTRY_SHORT=DE
END
}

testscenarios_showinfo_ip2location6() {
	cat <<END | grep -v '^#'
# non-anonymized IPv6 prefix
2a04::1					IPV6_COUNTRYCODE=DE
2a04::1					IP2LOCATION_COUNTRY_SHORT=DE
END
}

testscenarios_showinfo_DBIPv4() {
	cat <<END | grep -v '^#'
# match first valid entry in database
1.0.0.0					IPV4_COUNTRYCODE=AU
# match first entry in database
223.255.255.255				IPV4_COUNTRYCODE=AU
# non-anonymized IPv4 address
46.244.223.233				IPV4_COUNTRYCODE=DE
46.244.223.233				DBIP_COUNTRY_SHORT=DE
END
}

testscenarios_showinfo_DBIPv6() {
	cat <<END | grep -v '^#'
# match first entry in database
2001:4860:4860::8888			IPV6_COUNTRYCODE=US
# non-anonymized IPv6 prefix
2001:a60:113a:0123:0123:4567:89ab:cdef	IPV6_COUNTRYCODE=DE
2001:a60:113a:0123:0123:4567:89ab:cdef	DBIP_COUNTRY_SHORT=DE
END
}

testscenarios_showinfo_anonymized_info() {
	cat <<END | grep -v '^#'
# IPv4 address
8.8.8.8;;match;IPV4_ANON=245.216.59.65
8.8.8.8;;key-word;IPV6CALC_FEATURES=ANON_KEEP-TYPE-ASN-CC
8.8.8.8;--db-ip2location-disable --db-geoip2-disable --db-dbip2-disable --db-ip2location2-disable;match;IPV4_ANON=8.8.8.0
8.8.8.8;--db-ip2location-disable --db-geoip2-disable --db-dbip2-disable --db-ip2location2-disable;key-no-word;IPV6CALC_FEATURES=ANON_KEEP-TYPE-ASN-CC
END
}

testscenarios_kp() {
	testscenarios_anonymization_options_kp | while IFS="=" read input result; do
		echo "$input" | awk '{ print $NF }'
	done | sort | uniq

	testscenarios_anonymization_options | while IFS="=" read input result; do
		echo "$input" | awk '{ print $NF }'
	done | sort | uniq

	test_list | while read input filter; do
		echo "$input"
	done | sort | uniq

	testscenarios_filter | while read input filter; do
		echo "$input"
	done | sort | uniq

	testscenarios_ipv4_reserved | grep -vw "skip-anon-test" | while read input filter rest; do
		echo "$input"
	done | sort | uniq

	testscenarios_ipv6_reserved | grep -vw "skip-anon-test" | while read input filter rest; do
		echo "$input"
	done | sort | uniq

	testscenarios_auto_good | while read input filter rest; do
		echo "$input"
	done | sort | uniq
}

testscenario_hugelist() {
	case $1 in
	    ipv4)
		perl -e '{ for ($i = 0; $i < 256; $i++) { for ($j = 0; $j < 256; $j++) { print "$i.$j.$j.$i\n" } } }';
		;;
	esac
}

testscenario_action_test() {
	# result|options|description
	cat <<END | grep -v ^#
0|--test_ge 2001:db8:: --test_le 2001:db8:ffff:ffff:ffff:ffff:ffff:ffff 2001:db8::1
1|--test_ge 2001:db9:: --test_le 2001:db9:ffff:ffff:ffff:ffff:ffff:ffff 2001:db8::1
1|--test_ge 2001:db9:: --test_lt 2001:dba:: 2001:db8::1
0|--test_ge 2001:db8:: --test_lt 2001:db9:: 2001:db8::1
1|--test_gt 2001:db8::1 --test_lt 2001:db9:: 2001:db8::1
0|--test_ge 2001:db8::1 --test_lt 2001:db9:: 2001:db8::1
1|--test_gt 2001:db8::1 --test_lt 2001:db9::1 2001:db9::1
0|--test_ge 2001:db8::1 --test_le 2001:db9::1 2001:db9::1
0|--test_ge 1.2.3.0 --test_lt 1.2.3.5 1.2.3.0
1|--test_ge 1.2.3.0 --test_lt 1.2.3.5 1.2.3.5
0|--test_ge 1.2.3.0 --test_le 1.2.3.5 1.2.3.5
1|--test_ge 1.2.3.1 --test_le 1.2.3.5 1.2.3.0
END
}
