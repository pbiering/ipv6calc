# Project    : ipv6calc
# File       : contrib/ipv6calc.spec
# Copyright  : 2001-2014 by Peter Bieringer <pb@bieringer.de>
# $Id: ipv6calc.spec,v 1.174 2014/07/13 10:59:18 ds6peter Exp $

Summary: IP address format change and calculation utility
Name: ipv6calc
Version: 0.97.3
Release: 1%dist
Group: System Environment/Base
URL: http://www.deepspace6.net/projects/ipv6calc.html
License: GNU GPL version 2

Source: ftp://ftp.bieringer.de/pub/linux/IPv6/ipv6calc/ipv6calc-%{version}.tar.gz

BuildRoot: %{_tmppath}/ipv6calc-root

%define require_geoip 0
%define require_ip2location 0
%define require_geoip_dyn 0
%define require_ip2location_dyn 0

%{?_with_geoip: %{expand: %%define require_geoip 1}}
%{?_with_ip2location: %{expand: %%define require_ip2location 1}}

%{?_with_geoip_dyn: %{expand: %%define require_geoip_dyn 1}}
%{?_with_ip2location_dyn: %{expand: %%define require_ip2location_dyn 1}}

%{?_with_geoip: %{expand: %%define enable_geoip 1}}
%{?_with_ip2location: %{expand: %%define enable_ip2location 1}}

%{?_with_geoip_dyn: %{expand: %%define enable_geoip_dyn 1}}
%{?_with_ip2location_dyn: %{expand: %%define enable_ip2location_dyn 1}}
%{?_with_geoip_dyn: %{expand: %%define enable_geoip 1}}
%{?_with_ip2location_dyn: %{expand: %%define enable_ip2location 1}}

%{?_with_shared: %{expand: %%define enable_shared 1}}


%if %{require_geoip}
BuildRequires: GeoIP-devel
%if %{require_geoip_dyn}
#nothing required
%else
Requires: GeoIP
%endif
%endif

%if %{require_ip2location}
BuildRequires: ip2location-devel
%if %{require_ip2location_dyn}
#nothing required
%else
Requires: ip2Location >= 2.1.3
%endif
%endif


%description
ipv6calc is a small utility which formats and calculates IPv6 addresses in
different ways.

Install this package, if you want to extend the existing address detection
on IPv6 initscript setup or make life easier in adding reverse IPv6 zones to DNS
or using in DNS queries like
 dig -t ANY `ipv6calc -r 2001:db8:1234::1/48`
Many more format conversions are supported, see given URL for more.

Also this package contains additional binaries (and some examples)
 - ipv6loganon: anonymizes Apache webserver logs
 - ipv6logstats: create statistics from list of IPv4/IPv6 addresses (can be used by analog)

Available rpmbuild rebuild options:
  --with ip2location
  --with ip2location-dyn
  --with geoip
  --with geoip-dyn
  --with shared

%{?enable_geoip: %{expand: Built with GeoIP support}}
%{?enable_geoip_dyn: %{expand: Built with GeoIP dynamic-library-load support}}
%{?enable_ip2location: %{expand: Built with IP2Location suppport}}
%{?enable_ip2location_dyn: %{expand: Built with IP2Location dynamic-library-load suppport}}
%{?enable_shared: %{expand: Built with shared-library}}


%package ipv6calcweb
Summary: IP address information web utility
Group: Web/Applications
Requires: perl(URI) perl(Digest::SHA1) perl(Digest::MD5)
BuildRequires: perl(URI) perl(Digest::SHA1) perl(Digest::MD5)

%description ipv6calcweb
ipv6calcweb contains a CGI program and a configuration file for
displaying information of IP addresses on a web page.


%prep
%setup -q -n ipv6calc-%{version}


%build
./configure --bindir=%{_bindir} --mandir=%{_mandir} --libdir=%{_libdir} %{?enable_ip2location:--enable-ip2location} %{?enable_geoip:--enable-geoip} %{?enable_ip2location_dyn:--with-ip2location-dynamic} %{?enable_geoip_dyn:--with-geoip-dynamic} %{?enable_shared:--enable-shared}
make clean
make
make test-minimal


%install
rm -rf $RPM_BUILD_ROOT

make install DESTDIR=$RPM_BUILD_ROOT


## Install examples and helper files

mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/

# ipv6logconv
mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logconv
cp -r examples/analog/* $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logconv

# ipv6loganon
mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6loganon
pushd ipv6loganon
cp README $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6loganon/
popd

# ipv6logstats
mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logstats
pushd ipv6logstats
cp example_* collect_ipv6logstats.pl README $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logstats/
for dir in examples-data examples-gri; do
	cp -r $dir $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logstats/
done
popd

# tools
install -d $RPM_BUILD_ROOT%{_datadir}/%{name}/tools
for tool in GeoIP-update.sh IP2Location-update.sh; do
	install -m 755 tools/$tool $RPM_BUILD_ROOT%{_datadir}/%{name}/tools
done

# ipv6calcweb
install -d $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6calcweb
install -d $RPM_BUILD_ROOT%{_sysconfdir}/httpd/conf.d
install -d $RPM_BUILD_ROOT%{_localstatedir}/www/cgi-bin

install -m 644 ipv6calcweb/USAGE            $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6calcweb
install -m 644 ipv6calcweb/ipv6calcweb.conf $RPM_BUILD_ROOT%{_sysconfdir}/httpd/conf.d
install -m 555 ipv6calcweb/ipv6calcweb.cgi  $RPM_BUILD_ROOT%{_localstatedir}/www/cgi-bin

# Docs
for f in ChangeLog README CREDITS TODO COPYING LICENSE USAGE doc/ipv6calc.lyx doc/ipv6calc.sgml doc/ipv6calc.html; do
	cp $f $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/
done

for f in tools/GeoIP-update.sh tools/IP2Location-update.sh; do
	cp $f $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/
done

# Remove all CVS files
find $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version} -type d -name CVS |xargs rm -rf


%clean
rm -rf $RPM_BUILD_ROOT

%post

%preun
  
%files
%defattr(-,root,root)

# binaries
%{_bindir}/ipv6calc
%{_bindir}/ipv6logconv
%{_bindir}/ipv6logstats
%{_bindir}/ipv6loganon

# man pages
%{_mandir}/man8/*

# tools
%{_datadir}/%{name}/tools/*

# docs, examples and helper
%doc %{_docdir}/%{name}-%{version}/*

%if "%{enable_shared}" == "1"
%{_libdir}/*
%endif

%files ipv6calcweb
%defattr(-,root,root)

%{_localstatedir}/www/cgi-bin/ipv6calcweb.cgi
%doc %{_docdir}/%{name}-%{version}/ipv6calcweb/*
%config(noreplace) %{_sysconfdir}/httpd/conf.d/ipv6calcweb.conf


%changelog
* Sun Feb 02 2014 Peter Bieringer <pb@bieringer.de>
- add support for optional shared-library build

* Tue Nov 12 2013 Peter Bieringer <pb@bieringer.de>
- add datadir with tools

* Thu Sep 26 2013 Peter Bieringer <pb@bieringer.de>
- add support for --with geoip-dyn and --with ip2location-dyn

* Wed Sep 12 2012 Peter Bieringer <pb@bieringer.de>
- change requirements for ip2location to Mandrake Linux compatible names
- change location of "ipv6calc" from bin to %{_bindir}
- install tools/GeoIP-update.sh tools/IP2Location-update.sh to doc dir

* Sun Jan 01 2012 Peter Bieringer <pb@bieringer.de>
- create subpackage for ipv6calcweb
- minor review

* Sat Oct 08 2011 Peter Bieringer <pb@bieringer.de>
- reduce "make test" to "make test-minimal"

* Wed Nov 19 2008 Peter Bieringer <pb@bieringer.de>
- switch from "make installonly" to "make install"

* Wed Feb 14 2007 Peter Bieringer <pb@bieringer.de>
- remove support for build option --with-geoip-system
- add support for 'ipv6loganon'

* Sat Oct 28 2006 Peter Bieringer <pb@bieringer.de>
- add support for build option --with-geoip-system

* Sun Aug 06 2006 Peter Bieringer <pb@bieringer.de>
- add support for build option --with-geoip

* Wed Jun 07 2006 Peter Bieringer <pb@bieringer.de>
- add support for build option --with-ip2location

* Wed Jul 20 2005 Peter Bieringer <pb@bieringer.de>
- adjust code because of use of "bindir" in Makefile of ivp6calc

* Sat Oct 30 2004 Peter Bieringer <pb@bieringer.de>
- remove openssl(-devel) from requirements, no longer needed

* Sat Nov 22 2003 Peter Bieringer <pb@bieringer.de>
- adjustments

* Fri Nov 21 2003 Peter Bieringer <pb@bieringer.de>
- add ipv6logstats
- add man pages
- add configure options

* Mon Nov 11 2002 Peter Bieringer <pb@bieringer.de>
- change IPv6 address in %description

* Sat Apr 20 2002 Peter Bieringer <pb@bieringer.de>
- Change URL

* Sun Apr 07 2002 Peter Bieringer <pb@bieringer.de>
- add more analog example files

* Sat Apr 06 2002 Peter Bieringer <pb@bieringer.de>
- remove BuildRequires extension, not needed for normal build

* Sun Mar 24 2002 Peter Bieringer <pb@bieringer.de>
- extend BuildRequires for perl /usr/bin/aggregate wget

* Mon Mar 18 2002 Peter Bieringer <pb@bieringer.de>
- add ipv6calcweb.cgi

* Sat Mar 16 2002 Peter Bieringer <pb@bieringer.de>
- add ipv6logconv, analog examples

* Mon Mar 11 2002 Peter Bieringer <pb@bieringer.de>
- Add perl to buildrequire and openssl to require

* Mon Jan 21 2002 Peter Bieringer <pb@bieringer.de>
- Add LICENSE + COPYING file

* Thu Dec 27 2001 Peter Bieringer <pb@bieringer.de>
- Add comment header
- Add call to configure on build

* Tue Dec 18 2001 Peter Bieringer <pb@bieringer.de>
- Replace hardwired version number with autoconf/configure variable

* Wed Apr 25 2001 Peter Bieringer <pb@bieringer.de>
- Fix permissions of doc files

* Thu Mar 15 2001 Peter Bieringer <pb@bieringer.de>
- Add doc directory also to %files to make sure the directory will be removed
   on update or deinstall
- change install permissions for entries in doc directory
- change "make install" to "make installonly" (make test should be only executed once)" 

* Wed Mar 14 2001 Peter Bieringer <pb@bieringer.de>
- Add "make clean" and "make test" on %build

* Tue Mar 13 2001 Peter Bieringer <pb@bieringer.de>
- add CREDITS and TODO for install

* Sat Mar 10 2001 Peter Bieringer <pb@bieringer.de>
- enable "URL"

* Sun Mar 04 2001 Peter Bieringer <pb@bieringer.de>
- change install location to /bin

* Tue Feb 27 2001 Peter Bieringer <pb@bieringer.de>
- review for new release, now named "ipv6calc"
- review install section for RedHat 7.0.91

* Sun Feb 25 2001 Peter Bieringer <pb@bieringer.de>
- initial build
