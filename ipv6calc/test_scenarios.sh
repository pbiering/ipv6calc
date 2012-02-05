#!/bin/sh
#
# Project    : ipv6calc
# File       : test_scenarios.sh
# Version    : $Id: test_scenarios.sh,v 1.3 2012/02/05 09:03:34 peter Exp $
# Copyright  : 2001-2012 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc (functions only)

# Test Scenarios for autodetection "good case"
testscenarios_auto_good() {
	cat <<END | grep -v '^#'
3ffe:831f:ce49:7601:8000:efff:af4a:86BF						ipv6addr
1.2.3.4										ipv4addr
1.2.3.4/0									ipv4addr
1.2.3.4/32									ipv4addr
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

# Test Scenarios for reserved IPv4 addresses (RFC 5735)
testscenarios_ipv4_reserved() {
	cat <<END | grep -v '^#'
0.0.0.0			RFC1122#3.2.1.3
0.255.255.255		RFC1122#3.2.1.3
10.0.0.0		RFC1918#3
10.255.255.255		RFC1918#3
127.0.0.0		RFC1122#3.2.1.3
127.255.255.255		RFC1122#3.2.1.3
169.254.0.0		RFC3927#1
169.254.255.255		RFC3927#1
172.16.0.0		RFC1918#3
172.31.255.255		RFC1918#3
192.0.0.0		RFC5736#1
192.0.0.255		RFC5736#1
192.0.2.0		RFC5737#1
192.0.2.255		RFC5737#1
192.88.99.0		RFC3068#2.3
192.88.99.255		RFC3068#2.3
192.168.0.0		RFC1918#3
192.168.255.255		RFC1918#3
198.18.0.0		RFC2544#C.2.2
198.19.255.255		RFC2544#C.2.2
198.51.100.0		RFC5737#3
198.51.100.255		RFC5737#3
203.0.113.0		RFC5737#3
203.0.113.255		RFC5737#3
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
::0.0.0.2		RFC4291#2.5.5.1
::255.255.255.255	RFC4291#2.5.5.1
::ffff:0.0.0.0		RFC4291#2.5.5.2
::ffff:255.255.255.255	RFC4291#2.5.5.2
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
