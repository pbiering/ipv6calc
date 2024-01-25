# ipv6calc's module for Apache web server
 - developed since 2015
 - tested on Apache versions
   - 2.2 (EL6)
   - 2.4 (EL7, EL8, EL9, Fedora 30-37)


## Features

Depending on module options and available "offline" databases following environment variables are set
 - IPV6CALC_CLIENT_COUNTRYCODE
   - country code of IP address
   - provided by
     - GeoIP
     - IP2Location
     - db-ip.com
 - IPV6CALC_CLIENT_ASN
   - ASN of IP address
   - provided by
     - GeoIP
     - db-ip.com
 - IPV6CALC_CLIENT_REGISTRY
   - Registry of IP address
     - provided by
       - ipv6calc's "built-in" or "external" database
 - IPV6CALC_CLIENT_GEONAMEID
   - GeonameID of IP address
     - provided by
       - GeoIP
       - db-ip.com
 - IPV6CALC_CLIENT_IP_ANON
   - anonymized IP address
 - IPV6CALC_ANON_METHOD
   - used anonymization method


## Installation
 - copy compiled module to Apache module directory
	Fedora/Enterprise Linux
```
cp .libs/mod_ipv6calc.so /etc/httpd/modules/
```

 - copy default configuration to Apache configuration directory
	Fedora/Enterprise Linux
```
cp ipv6calc.conf /etc/httpd/conf.d/
```


 - copy CGI script to dedicated folder (OPTIONAL)
	Fedora/Enterprise Linux
```
mkdir /var/www/ipv6calc
cp ipv6calc.cgi /var/www/ipv6calc
```

## Configuration

 - see defaults/examples in `ipv6calc.conf`


## Activation

 - restart Apache


## Verification

### Startup log

See summary after Apache startup in log (e.g. /var/log/httpd/error_log) for default/configured parameters, e.g.

```
[Tue Aug 02 05:51:41.909367 2022] [ipv6calc:notice] [pid ...:tid ...] module is active by default
[Tue Aug 02 05:51:41.909414 2022] [ipv6calc:notice] [pid ...:tid ...] internal main     library version: 4.0.1  API: 4.0.0  (shared)
[Tue Aug 02 05:51:41.909424 2022] [ipv6calc:notice] [pid ...:tid ...] internal database library version: 4.0.1  API: 4.0.0  (shared)
[Tue Aug 02 05:51:41.910225 2022] [ipv6calc:notice] [pid ...:tid ...] configured module actions: anonymize=ON countrycode=ON asn=ON registry=ON geonameid=ON
[Tue Aug 02 05:51:41.910233 2022] [ipv6calc:notice] [pid ...:tid ...] default module debug level: 0x00000000 (0)
[Tue Aug 02 05:51:41.910237 2022] [ipv6calc:notice] [pid ...:tid ...] module cache: ON (default)  limit=20 (default/minimum)  statistics_interval=0 (default)
[Tue Aug 02 05:51:41.934758 2022] [ipv6calc:notice] [pid ...:tid ...] features: GeoIP GeoIPv6 IP2Location IP2Location6 DBIPv4 DBIPv6 DB_AS_REG DB_IPV4_REG DB_IPV6_REG DB_IPV4_AS DB_IPV6_AS DB_IPV4_CC DB_IPV6_CC DB_IPV4_COUNTRY DB_IPV6_COUNTRY DB_IPV4_CITY DB_IPV6_CITY DB_IPV4_REGION DB_IPV6_REGION DB_CC_REG DB_IEEE DB_IPV4_INFO DB_IPV6_INFO DB_IPV4_GEONAMEID DB_IPV6_GEONAMEID
[Tue Aug 02 05:51:41.934810 2022] [ipv6calc:notice] [pid ...:tid ...] capabilities: GeoIP2(dyn-load/MaxMindDB) IP2Location(dyn-load) DBIP2(dyn-load/MaxMindDB) DB_AS_REG(BuiltIn) DB_CC_REG(BuiltIn) DB_IPV4_REG(BuiltIn) DB_IPV6_REG(BuiltIn) DB_IEEE(BuiltIn)
[Tue Aug 02 05:51:41.934819 2022] [ipv6calc:notice] [pid ...:tid ...] supported anonymization methods: ANON_ZEROISE ANON_ANONYMIZE ANON_KEEP-TYPE-ASN-CC ANON_KEEP-TYPE-GEONAMEID
[Tue Aug 02 05:51:41.934827 2022] [ipv6calc:notice] [pid ...:tid ...] configured anonymization method: keep-type-asn-cc
[Tue Aug 02 05:51:42.289411 2022] [ipv6calc:notice] [pid ...:tid ...] module is active by default
[Tue Aug 02 05:51:42.289498 2022] [ipv6calc:notice] [pid ...:tid ...] internal main     library version: 4.0.1  API: 4.0.0  (shared)
[Tue Aug 02 05:51:42.289515 2022] [ipv6calc:notice] [pid ...:tid ...] internal database library version: 4.0.1  API: 4.0.0  (shared)
[Tue Aug 02 05:51:42.289529 2022] [ipv6calc:notice] [pid ...:tid ...] configured module actions: anonymize=ON countrycode=ON asn=ON registry=ON geonameid=ON
[Tue Aug 02 05:51:42.289536 2022] [ipv6calc:notice] [pid ...:tid ...] default module debug level: 0x00000000 (0)
[Tue Aug 02 05:51:42.289548 2022] [ipv6calc:notice] [pid ...:tid ...] module cache: ON (default)  limit=20 (default/minimum)  statistics_interval=0 (default)
[Tue Aug 02 05:51:42.293067 2022] [ipv6calc:notice] [pid ...:tid ...] features: GeoIP GeoIPv6 IP2Location IP2Location6 DBIPv4 DBIPv6 DB_AS_REG DB_IPV4_REG DB_IPV6_REG DB_IPV4_AS DB_IPV6_AS DB_IPV4_CC DB_IPV6_CC DB_IPV4_COUNTRY DB_IPV6_COUNTRY DB_IPV4_CITY DB_IPV6_CITY DB_IPV4_REGION DB_IPV6_REGION DB_CC_REG DB_IEEE DB_IPV4_INFO DB_IPV6_INFO DB_IPV4_GEONAMEID DB_IPV6_GEONAMEID
[Tue Aug 02 05:51:42.293165 2022] [ipv6calc:notice] [pid ...:tid ...] capabilities: GeoIP2(dyn-load/MaxMindDB) IP2Location(dyn-load) DBIP2(dyn-load/MaxMindDB) DB_AS_REG(BuiltIn) DB_CC_REG(BuiltIn) DB_IPV4_REG(BuiltIn) DB_IPV6_REG(BuiltIn) DB_IEEE(BuiltIn)
[Tue Aug 02 05:51:42.293176 2022] [ipv6calc:notice] [pid ...:tid ...] supported anonymization methods: ANON_ZEROISE ANON_ANONYMIZE ANON
```

### run test with provided CGI

Check ACL configuration in `ipv6calc.conf` in advance

```
curl https://<FQDN>/cgi-bin/ipv6calc.cgi
REMOTE_ADDR=2001:a61:a8a:0123:4567:89ab:cdef:c87f
IPV6CALC_CLIENT_IP_ANON=a909:16fa:9092:23ff:a909:4291:c02d:5d1d
IPV6CALC_CLIENT_COUNTRYCODE=DE
IPV6CALC_CLIENT_ASN=8767
IPV6CALC_CLIENT_REGISTRY=RIPENCC
IPV6CALC_CLIENT_GEONAMEID=2867714
IPV6CALC_ANON_METHOD=keep-type-asn-cc
```

### Access log

### Anonymized log file according to example LogFormat looks then like:

```
a909:16fa:9092:23ff:a909:4941::7 "DE" - [30/May/2015:18:14:59 +0200] "GET / HTTP/1.1" 403 4609 "-" "curl/7.40.0"
a909:16fa:9092:23ff:a909:4941::7 "DE" - [30/May/2015:18:15:01 +0200] "GET / HTTP/1.1" 403 4609 "-" "curl/7.40.0"
127.0.0.0 "-" - [30/May/2015:18:15:02 +0200] "HEAD / HTTP/1.0" 403 - "-" "-"
```


### Log file with anonymized IP address and enriched with geolocation information looks then like:

```
LogFormat "%{IPV6CALC_CLIENT_IP_ANON}e %{IPV6CALC_ANON_METHOD}e %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-Agent}i\" %{local}p \"%{Host}i\" \"%{X-Forwarded-For}i\" \"%{IPV6CALC_CLIENT_COUNTRYCODE}e/%{IPV6CALC_CLIENT_ASN}e/%{IPV6CALC_CLIENT_REGISTRY}e/%{IPV6CALC_CLIENT_GEONAMEID}e\""`
```

```
245.216.31.139 - - [31/Jul/2022:23:52:17 +0200] "GET / HTTP/1.1" 200 4514 "http://ip.bieringer.net/" "Mozilla/5.0 (compatible; DuckDuckGo-Favicons-Bot/1.0; +http://duckduckgo.com)" 80 "ip.bieringer.net" "-" "US/8075/ARIN/4788822"
```

## Troubleshooting

### Database access/status
`ipv6calc -vv` shows proper database setup while Apache has issues
	=> check/adjust SELinux (temporary set to permissive)

### Module debugging

use `ipv6calcDebuglevel` to enable built-in debug information (take care on heavy loaded webservers, can create huge amount of lines)
