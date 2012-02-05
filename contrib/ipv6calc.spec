# Project    : ipv6calc
# File       : contrib/ipv6calc.spec
# Copyright  : 2001-2008 by Peter Bieringer <pb@bieringer.de>

Summary: IP address format change and calculation utility
Name: ipv6calc
Version: 0.93.1
Release: 1
Group: System Environment/Base
URL: http://www.deepspace6.net/projects/ipv6calc.html
License: GNU GPL version 2

Source: ftp://ftp.bieringer.de/pub/linux/IPv6/ipv6calc/ipv6calc-%{version}.tar.gz

BuildRoot: %{_tmppath}/ipv6calc-root

%define enable_geoip 0
%define enable_ip2location 0

%{?_with_geoip: %{expand: %%define enable_geoip 1}}
%{?_with_ip2location: %{expand: %%define enable_ip2location 1}}

%if %{enable_geoip}
BuildPreReq: GeoIP-devel
Requires: GeoIP
%endif

%if %{enable_ip2location}
BuildPreReq: IP2Location-devel
Requires: IP2Location >= 2.1.3
%endif


%description
ipv6calc is a small utility which formats and calculates IPv6 addresses in
different ways.

Install this package, if you want to extend the existing address detection
on IPv6 initscript setup or make life easier in adding reverse IPv6 zones to DNS
or using in DNS queries like
 dig -t ANY `ipv6calc -r 2001:db8:1234::1/48`
Many more format conversions are supported, see given URL for more.

Available rpmbuild rebuild options:
  --with ip2location
  --with geoip

%{?_with_geoip: %{expand: Built with GeoIP support}}
%{?_with_ip2location: %{expand: Built with IP2Location suppport}}


%package ipv6calcweb
Summary: IP address information web utility
Group: Web/Applications
Requires: perl-URI perl-Digest-SHA1
BuildPreReq: perl-URI perl-Digest-SHA1

%description ipv6calcweb
ipv6calcweb contains a CGI program and a configuration file for
displaying information of IP addresses on a web page.


%prep
%setup -q -n ipv6calc-%{version}


%build
./configure --bindir=%{_bindir} --mandir=%{_mandir} %{?_with_ip2location:--enable-ip2location} %{?_with_geoip:--enable-geoip}
make clean
make
make test-minimal


%install
rm -rf $RPM_BUILD_ROOT

make install DESTDIR=$RPM_BUILD_ROOT

# Move ipv6calc
mkdir -p $RPM_BUILD_ROOT/bin
mv $RPM_BUILD_ROOT%{_bindir}/ipv6calc $RPM_BUILD_ROOT/bin/ipv6calc


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

# Remove all CVS files
find $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version} -type d -name CVS |xargs rm -rf


%clean
rm -rf $RPM_BUILD_ROOT

%post

%preun
  
%files
%defattr(-,root,root)

# binaries
/bin/ipv6calc
%{_bindir}/ipv6logconv
%{_bindir}/ipv6logstats
%{_bindir}/ipv6loganon

# man pages
%{_mandir}/man8/*

# docs, examples and helper
%doc %{_docdir}/%{name}-%{version}/*


%files ipv6calcweb
%defattr(-,root,root)

%{_localstatedir}/www/cgi-bin/ipv6calcweb.cgi
%doc %{_docdir}/%{name}-%{version}/ipv6calcweb/*
%config(noreplace) %{_sysconfdir}/httpd/conf.d/ipv6calcweb.conf


%changelog
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
- adjust code because of use of /usr/bin in Makefile of ivp6calc

* Sat Oct 30 2004 Peter Bieringer <pb@bieringer.de>
- remove openssl(-devel) from requirements, no longer needed

* Fri Nov 22 2003 Peter Bieringer <pb@bieringer.de>
- adjustments

* Fri Nov 21 2003 Peter Bieringer <pb@bieringer.de>
- add ipv6logstats
- add man pages
- add configure options

* Tue Nov 11 2002 Peter Bieringer <pb@bieringer.de>
- change IPv6 address in %description

* Sat Apr 20 2002 Peter Bieringer <pb@bieringer.de>
- Change URL

* Mon Apr 07 2002 Peter Bieringer <pb@bieringer.de>
- add more analog example files

* Mon Apr 05 2002 Peter Bieringer <pb@bieringer.de>
- remove BuildRequires extension, not needed for normal build

* Mon Mar 24 2002 Peter Bieringer <pb@bieringer.de>
- extend BuildRequires for perl /usr/bin/aggregate wget

* Mon Mar 18 2002 Peter Bieringer <pb@bieringer.de>
- add ipv6calcweb.cgi

* Mon Mar 16 2002 Peter Bieringer <pb@bieringer.de>
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
