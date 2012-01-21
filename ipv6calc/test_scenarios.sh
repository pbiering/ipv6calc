#!/bin/sh
#
# Project    : ipv6calc
# File       : test_scenarios.sh
# Version    : $Id: test_scenarios.sh,v 1.2 2012/01/21 14:08:54 peter Exp $
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
0.0.0.0			RFC1122
0.255.255.255		RFC1122
10.0.0.0		RFC1918
10.255.255.255		RFC1918
127.0.0.0		RFC1122
127.255.255.255		RFC1122
169.254.0.0		RFC3927
169.254.255.255		RFC3927
172.16.0.0		RFC1918
172.31.255.255		RFC1918
192.0.0.0		RFC5736
192.0.0.255		RFC5736
192.0.2.0		RFC5737
192.0.2.255		RFC5737
192.88.99.0		RFC3068
192.88.99.255		RFC3068
192.168.0.0		RFC1918
192.168.255.255		RFC1918
198.18.0.0		RFC2544
198.19.255.255		RFC2544
198.51.100.0		RFC5737
198.51.100.255		RFC5737
203.0.113.0		RFC5737
203.0.113.255		RFC5737
224.0.0.0		RFC3171
239.255.255.255		RFC3171
240.0.0.0		RFC1112
255.255.255.254		RFC1112
255.255.255.255		RFC919
END
}
