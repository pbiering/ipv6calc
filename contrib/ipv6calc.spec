# Project    : ipv6calc
# File       : contrib/ipv6calc.spec
# Version    : $Id: ipv6calc.spec,v 1.6 2002/04/20 18:19:34 peter Exp $
# Copyright  : 2001-2002 by Peter Bieringer <pb@bieringer.de>

Summary: IPv6 address format change and calculation utility
Name: ipv6calc
Version: 0.40rc2
Release: 1
Group: System Environment/Base
URL: ftp://ftp.bieringer.de/pub/linux/IPv6/ipv6calc/
License: GNU GPL version 2

Source: ftp://ftp.bieringer.de/pub/linux/IPv6/ipv6calc/ipv6calc-%{version}.tar.gz

BuildRoot: %{_tmppath}/ipv6calc-root
Requires: openssl
BuildRequires: openssl-devel 
#BuildRequires: openssl-devel perl /usr/bin/aggregate wget

%description
ipv6calc is a small utility which formats and calculates IPv6 addresses in
different ways.

Install this package, if you want to extend the existing address detection
on IPv6 initscript setup or make life easier in adding reverse IPv6 zones to DNS
or using in DNS queries like
 nslookup -q=ANY `ipv6calc -r 3ffe:400:100:f101::1/48`
See also here for more details: http://www.bieringer.de/linux/IPv6/

%prep
%setup -q -n ipv6calc-%{version}

%build
./configure
make clean
make
make test
	
%install
rm -rf $RPM_BUILD_ROOT
make installonly root=$RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%_defaultdocdir/%{name}-%{version}/
install -m644 $RPM_BUILD_DIR/%{name}-%{version}/ChangeLog $RPM_BUILD_ROOT%_defaultdocdir/%{name}-%{version}/
install -m644 $RPM_BUILD_DIR/%{name}-%{version}/README    $RPM_BUILD_ROOT%_defaultdocdir/%{name}-%{version}/
install -m644 $RPM_BUILD_DIR/%{name}-%{version}/CREDITS   $RPM_BUILD_ROOT%_defaultdocdir/%{name}-%{version}/
install -m644 $RPM_BUILD_DIR/%{name}-%{version}/TODO      $RPM_BUILD_ROOT%_defaultdocdir/%{name}-%{version}/
install -m644 $RPM_BUILD_DIR/%{name}-%{version}/COPYING   $RPM_BUILD_ROOT%_defaultdocdir/%{name}-%{version}/
install -m644 $RPM_BUILD_DIR/%{name}-%{version}/LICENSE   $RPM_BUILD_ROOT%_defaultdocdir/%{name}-%{version}/

%clean
rm -rf $RPM_BUILD_ROOT

%post

%preun
  
%files
%defattr(-,root,root)
/bin/ipv6calc
/usr/bin/ipv6logconv
%doc ChangeLog README CREDITS TODO COPYING LICENSE
%doc examples/analog/analog-dist.cfg %doc examples/analog/run_analog.sh examples/analog/ipv6calc.tab ipv6calcweb/ipv6calcweb.cgi examples/analog/analog-ipv6calc-descriptions.txt examples/analog/analog-dist-combined.cfg

%changelog
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
- change "make install" to "make installonly" (make test should be only executed once" 

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
