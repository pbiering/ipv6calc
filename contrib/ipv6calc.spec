# Project    : ipv6calc
# File       : contrib/ipv6calc.spec
# Version    : $Id: ipv6calc.spec,v 1.43 2005/09/15 17:28:51 peter Exp $
# Copyright  : 2001-2005 by Peter Bieringer <pb@bieringer.de>

Summary: IPv6 address format change and calculation utility
Name: ipv6calc
Version: 0.50
Release: 1
Group: System Environment/Base
URL: http://www.deepspace6.net/projects/ipv6calc.html
License: GNU GPL version 2

Source: ftp://ftp.bieringer.de/pub/linux/IPv6/ipv6calc/ipv6calc-%{version}.tar.gz

BuildRoot: %{_tmppath}/ipv6calc-root

%description
ipv6calc is a small utility which formats and calculates IPv6 addresses in
different ways.

Install this package, if you want to extend the existing address detection
on IPv6 initscript setup or make life easier in adding reverse IPv6 zones to DNS
or using in DNS queries like
 dig -t ANY `ipv6calc -r 3ffe:ffff:100::1/48`
Many more format conversions are supported, see given URL for more.

%prep
%setup -q -n ipv6calc-%{version}

%build
./configure --bindir=%{_bindir} --mandir=%{_mandir}
make clean
make
make test
	
%install
rm -rf $RPM_BUILD_ROOT

make installonly root=$RPM_BUILD_ROOT

# Move ipv6calc
mkdir -p $RPM_BUILD_ROOT/bin
mv $RPM_BUILD_ROOT%{_bindir}/ipv6calc $RPM_BUILD_ROOT/bin/ipv6calc


## Install examples and helper files

mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/

# ipv6logconv
mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logconv
cp -r examples/analog/* $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logconv

# ipv6logstats
mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logstats
pushd ipv6logstats
cp example_* collect_ipv6logstats.pl README $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logstats/
for dir in examples-data examples-gri; do
	cp -r $dir $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6logstats/
done
popd

# ipv6calcweb
mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6calcweb
pushd ipv6calcweb
cp USAGE ipv6calcweb.cgi $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/ipv6calcweb
popd

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

# man pages
%{_mandir}/man8/*

# docs, examples and helper
%{_docdir}/%{name}-%{version}/*


%changelog
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
