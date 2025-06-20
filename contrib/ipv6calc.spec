# Project    : ipv6calc
# File       : contrib/ipv6calc.spec
# Copyright  : 2001-2025 by Peter Bieringer <pb@bieringer.de>

### supports following defines during RPM build:
#
### specific git commit on upstream (EXAMPLE)
## build SRPMS+RPM
# rpmbuild --undefine=_disable_source_fetch -ba -D "gitcommit b32d47bf915d38e08b53904501764452773d62ca" ipv6calc.spec
#
## rebuild SRPMS on a different system using
# rpmbuild --rebuild -D "gitcommit b32d47bf915d38e08b53904501764452773d62ca" ipv6calc-<VERSION>-<RELEASE>.YYYYMMDDgitSHORTHASH.DIST.src.rpm


%if 0%{?gitcommit:1}
%global shortcommit %(c=%{gitcommit}; echo ${c:0:7})
%define build_timestamp %(date +"%Y%m%d")
%global gittag .%{build_timestamp}git%{shortcommit}
%endif


# shared library support (deselectable)
%if "%{?_without_shared:0}%{?!_without_shared:1}" == "1"
%define enable_shared 1
%endif

Summary:	IPv6/IPv4 address information, format change, filter and calculation utility
Name:		ipv6calc
Version:	4.3.2
Release:	81%{?gittag}%{?dist}
URL:		http://www.deepspace6.net/projects/%{name}.html
License:	GPLv2
%if 0%{?gitcommit:1}
Source:		https://github.com/pbiering/%{name}/archive/%{gitcommit}/%{name}-%{gitcommit}.tar.gz
%else
Source:		https://github.com/pbiering/%{name}/archive/%{version}/%{name}-%{version}.tar.gz
%endif
BuildRequires:	automake make
BuildRequires:	gcc
BuildRequires:	openssl-devel
BuildRequires:	perl-generators
BuildRequires:	perl(Digest::MD5), perl(Digest::SHA), perl(URI::Escape)
BuildRequires:	perl(strict), perl(warnings)
BuildRequires:	procps-ng
Requires:	perl(:MODULE_COMPAT_%(eval "`%{__perl} -V:version`"; echo $version))
Requires:	unzip
Requires:	perl-BerkeleyDB perl-Net-IP
%if %{enable_shared}
Provides:	ipv6calc-libs = %{version}-%{release}
%else
Conflicts:	ipv6calc-libs
%endif


# mod_ipv6calc related
%{!?_httpd_apxs:    %{expand: %%global _httpd_apxs    %%{_sbindir}/apxs}}
%{!?_httpd_moddir:  %{expand: %%global _httpd_moddir  %%{_libdir}/httpd/modules}}
%{!?_httpd_confdir: %{expand: %%global _httpd_confdir %%{_sysconfdir}/httpd/conf.d}}

# database support (deselectable)
%if "%{?_without_ip2location:0}%{?!_without_ip2location:1}" == "1"
%define enable_ip2location 1
%endif

%if "%{?_without_mmdb:0}%{?!_without_mmdb:1}" == "1"
%define enable_mmdb 1
%endif

%if "%{?_without_external:0}%{?!_without_external:1}" == "1"
%define enable_external 1
%endif

%if "%{?_without_mod_ipv6calc:0}%{?!_without_mod_ipv6calc:1}" == "1"
%define enable_mod_ipv6calc 1
%endif


# database locations
%define ip2location_db	%{_datadir}/IP2Location
%define geoip_db	%{_datadir}/GeoIP
%define dbip_db		%{_datadir}/DBIP
%define external_db	%{_datadir}/%{name}/db


# Berkeley DB selector
%define require_db4 %(echo "%{dist}" | grep -Eq '^\.el(5|6)$' && echo 1 || echo 0)
%if %{require_db4}
BuildRequires: db4-devel
Requires:      db4
%else
BuildRequires: libdb-devel
Requires:      libdb
%endif

%if %{enable_mmdb}
BuildRequires: libmaxminddb-devel
Recommends:    libmaxminddb

%if 0%{?fedora} >= 39
BuildRequires: geolite2-country
BuildRequires: geolite2-city
BuildRequires: geolite2-asn
Recommends:    geolite2-country
Recommends:    geolite2-city
Recommends:    geolite2-asn
%endif

%if 0%{?rhel} >= 8
BuildRequires: geolite2-country
BuildRequires: geolite2-city
Recommends:    geolite2-country
Recommends:    geolite2-city
%endif

%endif

%if %{enable_ip2location}
BuildRequires: IP2Location-devel >= 8.6.0
Recommends:    IP2Location       >= 8.6.0
%endif

# RPM license macro detector
%define rpm_license_extra %(echo "%{_defaultlicensedir}" | grep -q defaultlicensedir && echo 0 || echo 1)


%description
ipv6calc is a small utility which formats and calculates IPv4/IPv6 addresses
in different ways.

Install this package, if you want to retrieve information about a particular
IPv4/IPv6/MAC address (-i ADDRESS) or make life easier in adding entries to
reverse IPv6 DNS zones (e.g. -a 2001:db8:1234::1/48).

In addition many format and type conversions are supported, see online help
and/or given URL for more.

Also this package contains additional programs
 - ipv6loganon: anonymize Apache web server logs
 - ipv6logconv: special Apache web server log converter
    (examples included for use with analog)
 - ipv6logstats: create statistics from list of IPv4/IPv6 addresses
    (examples included for use with gnu-plot)
 - mod_ipv6calc: Apache module for anonymization/information logging on-the-fly

Support for following databases
 - IP2Location(BIN)  %{?enable_ip2location:ENABLED}%{?!enable_ip2location:DISABLED}
		     default directory for downloaded db files: %{ip2location_db}
		     (requires also IP2Location library on system)

 - IP2Location(MMDB) %{?enable_mmdb:ENABLED}%{?!enable_mmdb:DISABLED}
		     default directory for downloaded db files: %{ip2location_db}
		     (requires also MMDB library on system)

 - GeoIP(MMDB)	     %{?enable_mmdb:ENABLED}%{?!enable_mmdb:DISABLED}
		     default directory for downloaded db files: %{geoip_db}
		     (requires also MMDB library on system)

 - db-ip.com(MMDB)   %{?enable_mmdb:ENABLED}%{?!enable_mmdb:DISABLED}
		     (once generated database files are found on system)
		     default directory for generated db files: %{dbip_db}
		     (requires also MMDB library on system)

 - External	     %{?enable_external:ENABLED}%{?!enable_external:DISABLED}
		     default directory for generated db files: %{external_db}

Built %{?enable_shared:WITH}%{?!enable_shared:WITHOUT} shared-library

Available rpmbuild rebuild options:
  --without ip2location : disables IP2Location(BIN)
  --without mmdb : disables GeoIP, db-ip.com, IP2Location(MMDB)
  --without external
  --without shared
  --without mod_ipv6calc


%package ipv6calcweb
Summary:	IP address information web utility
Requires:	ipv6calc httpd
Requires:	perl(URI) perl(Digest::SHA) perl(Digest::MD5) perl(HTML::Entities)
BuildRequires:	perl(URI) perl(Digest::SHA) perl(Digest::MD5) perl(HTML::Entities)

%description ipv6calcweb
ipv6calcweb contains a CGI program and a configuration file for
displaying information of IP addresses on a web page using ipv6calc.

Check/adjust %{_sysconfdir}/httpd/conf.d/ipv6calcweb.conf
Default restricts access to localhost


%if %{enable_mod_ipv6calc}
%package mod_ipv6calc
Summary:	Apache module for ipv6calc
BuildRequires:	httpd-devel psmisc curl
Requires:	httpd >= 2.4.0
Requires:	httpd <= 2.4.99999
Requires:	ipv6calc = %{version}-%{release}
%if %{enable_shared}
Requires:	ipv6calc-libs = %{version}-%{release}
%endif


%description mod_ipv6calc
mod_ipv6calc contains an Apache module and a default configuration
file.

Features:
 - store anonymized IPv4/v6 address in environment variable
 - store CountryCode of IPv4/v6 address in environment variable
(environment variables can be used for custom log format)

Check/adjust %{_sysconfdir}/httpd/conf.d/ipv6calc.conf
By default the module is disabled.
%endif


%prep
%if 0%{?gitcommit:1}
%setup -q -n %{name}-%{gitcommit}
%else
%setup -q
%endif

autoreconf


%build
%configure \
	%{?enable_ip2location:--enable-ip2location} \
	%{?enable_ip2location:--with-ip2location-dynamic} \
	--with-ip2location-db=%{ip2location_db} \
	--with-geoip-db=%{geoip_db} \
	--with-dbip-db=%{dbip_db} \
	%{?enable_mmdb:--enable-mmdb --with-mmdb-dynamic} \
	%{?enable_external:--enable-external} \
	--with-external-db=%{external_db} \
	%{?enable_shared:--enable-shared} \
	%{?enable_mod_ipv6calc:--enable-mod_ipv6calc}

make clean
make %{?_smp_mflags} COPTS="$RPM_OPT_FLAGS"


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

## Install examples and helper files
install -d -p %{buildroot}%{_docdir}/%{name}-%{version}

## examples
install -d %{buildroot}%{_datadir}/%{name}/examples

# ipv6logconv
install -d %{buildroot}%{_datadir}/%{name}/examples/ipv6logconv
for file in examples/analog/*.{cfg,txt,tab,sh}; do
	install $file %{buildroot}%{_datadir}/%{name}/examples/ipv6logconv/
done

# ipv6loganon
install -d %{buildroot}%{_datadir}/%{name}/examples/ipv6loganon
for file in ipv6loganon/README; do
	install $file %{buildroot}%{_datadir}/%{name}/examples/ipv6loganon/
done

# ipv6logstats
install -d %{buildroot}%{_datadir}/%{name}/examples/ipv6logstats
for file in ipv6logstats/README ipv6logstats/example_* ipv6logstats/collect_ipv6logstats.pl; do
	install $file %{buildroot}%{_datadir}/%{name}/examples/ipv6logstats/
done


# db directory
install -d %{buildroot}%{external_db}
install -d %{buildroot}%{external_db}/lisp
install -m 644 databases/registries/lisp/site-db %{buildroot}%{external_db}/lisp/

# selinux
install -d %{buildroot}%{_datadir}/%{name}/selinux


# ipv6calcweb
install -d %{buildroot}%{_sysconfdir}/httpd/conf.d
install -d %{buildroot}%{_localstatedir}/www/ipv6calcweb/cgi-bin

install ipv6calcweb/ipv6calcweb.conf %{buildroot}%{_sysconfdir}/httpd/conf.d/
install -m 755 ipv6calcweb/ipv6calcweb.cgi %{buildroot}%{_localstatedir}/www/ipv6calcweb/cgi-bin/
install -m 644 ipv6calcweb/ipv6calcweb-databases-in-var.te %{buildroot}%{_datadir}/%{name}/selinux/

%if %{enable_mod_ipv6calc}
# mod_ipv6calc
install -d %{buildroot}%{_sysconfdir}/httpd/conf.d
install -d %{buildroot}%{_localstatedir}/www/ipv6calc/cgi-bin
install mod_ipv6calc/ipv6calc.conf %{buildroot}%{_sysconfdir}/httpd/conf.d/
install -m 755 mod_ipv6calc/ipv6calc.cgi %{buildroot}%{_localstatedir}/www/ipv6calc/cgi-bin/
%endif

%clean
rm -rf %{buildroot}


%check
%ifnarch ppc64
	make test
%endif


%files
%if %{rpm_license_extra}
%doc ChangeLog README README.* CREDITS TODO USAGE doc/ipv6calc.lyx doc/ipv6calc.html doc/ipv6calc.xml
%license COPYING LICENSE
%else
%doc ChangeLog README README.* CREDITS TODO USAGE doc/ipv6calc.lyx doc/ipv6calc.html doc/ipv6calc.xml COPYING LICENSE
%endif

%defattr(644,root,root,755)

# binaries
%attr(755,-,-) %{_bindir}/*

# man pages
%{_mandir}/man8/*

# tools
%attr(755,-,-) %{_datadir}/%{name}/tools/*

# selinux
%attr(644,-,-) %{_datadir}/%{name}/selinux/*

# shared library
%{?enable_shared:%attr(755,-,-) %{_libdir}/libipv6calc*}

# database directory
%{external_db}

# examples
%attr(755,-,-) %{_datadir}/%{name}/examples/*/*.pl
%attr(755,-,-) %{_datadir}/%{name}/examples/*/*.sh
%{_datadir}/%{name}/examples/ipv6loganon/
%{_datadir}/%{name}/examples/ipv6logconv/
%{_datadir}/%{name}/examples/ipv6logstats/


%files ipv6calcweb
%if %{rpm_license_extra}
%doc ipv6calcweb/README ipv6calcweb/USAGE
%license COPYING LICENSE
%else
%doc ipv6calcweb/README ipv6calcweb/USAGE COPYING LICENSE
%endif

%defattr(644,root,root,755)

%attr(755,-,-) %{_localstatedir}/www/ipv6calcweb/cgi-bin/ipv6calcweb.cgi
%config(noreplace) %{_sysconfdir}/httpd/conf.d/ipv6calcweb.conf


%files mod_ipv6calc
%if %{rpm_license_extra}
%doc mod_ipv6calc/README.md
%license COPYING LICENSE
%else
%doc mod_ipv6calc/README.md COPYING LICENSE
%endif

%defattr(644,root,root,755)

%config(noreplace) %{_httpd_confdir}/ipv6calc.conf
%attr(755,-,-) %{_httpd_moddir}/mod_ipv6calc.so

%attr(755,-,-) %{_localstatedir}/www/ipv6calc/cgi-bin/ipv6calc.cgi


%post
if [ -x /usr/sbin/ldconfig ]; then
	/usr/sbin/ldconfig
elif [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi


%postun
if [ -x /usr/sbin/ldconfig ]; then
	/usr/sbin/ldconfig
elif [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi


%changelog
* Wed Jan 29 2025 Peter Bieringer <pb@bieringer.de>
- remove ipv6calc.sgml from doc
- use now only releases from GitHub

* Tue Jan 14 2025 Peter Bieringer <pb@bieringer.de>
- include databases/registries/lisp/site-db as no longer reachable for download

* Mon Jan 06 2025 Peter Bieringer <pb@bieringer.de>
- add additional Perl requirements

* Sat Aug 23 2023 Peter Bieringer <pb@bieringer.de>
- add BuildRequires+Recommends for geolite in case MMDB is enabled

* Fri Jan 26 2023 Peter Bieringer <pb@bieringer.de>
- cosmetics, minor updates and alignment with EPEL9 variant
- change locations of CGI scripts

* Wed Dec 06 2023 Peter Bieringer <pb@bieringer.de>
- call autoreconf before configure

* Wed Jun 07 2023 Peter Bieringer <pb@bieringer.de>
- increase minimum IP2Location version to 8.6.0 (support of DB-26)

* Tue Oct 13 2020 Peter Bieringer <pb@bieringer.de>
- remove obsolete BuildRoot and Group, fix requirement of unzip

* Mon Oct 12 2020 Peter Bieringer <pb@bieringer.de>
- remove support for GeoIP(legacy) and force minimum IP2Location version, add some recommendations and remove legacy support

* Sun Jun 21 2020 Peter Bieringer <pb@bieringer.de>
- cover 2 potential ldconfig locations

* Sat Sep 07 2019 Peter Bieringer <pb@bieringer.de>
- add build requirement: automake make

* Tue Jan 29 2019 Peter Bieringer <pb@bieringer.de>
- subpackage ipv6calcweb: remove dependency Perl(Proc::ProcessTable) because no longer used

* Sat Jan 26 2019 Peter Bieringer <pb@bieringer.de>
- add option for libmaxminddb supporting GeoIP v2 and db-ip.com v2

* Sun Sep 23 2018 Peter Bieringer <pb@bieringer.de>
- subpackage ipv6calcweb: add dependency Perl(Proc::ProcessTable)

* Wed Dec 28 2016 Peter Bieringer <pb@bieringer.de>
- add support for git commit hash
- align RPM build requires with fedora-scm

* Thu Jul 30 2015 Peter Bieringer <pb@bieringer.de>
- add automatic RPM license feature detection

* Sat Jul 25 2015 Peter Bieringer <pb@bieringer.de>
- replace ipv6calc.{lyx,sgml,html,xml} by dedicated file.suffix

* Mon Jun 01 2015 Peter Bieringer <pb@bieringer.de>
- add subpackage mod_ipv6calc

* Sat Mar 07 2015 Peter Bieringer <pb@bieringer.de>
- take use of license macro

* Sun Mar 01 2015 Peter Bieringer <pb@bieringer.de>
- remove not necessary x-bits for some files by proper definition in %%files section

* Wed Feb 18 2015 Peter Bieringer <pb@bieringer.de>
- delete subpackage db, major spec file alignment with Fedora-SCM version

* Sun Feb 15 2015 Peter Bieringer <pb@bieringer.de>
- extend sub-package db

* Tue Dec 09 2014 Peter Bieringer <pb@bieringer.de>
- create new sub-package: db with all related tools
- configure: --enable-external

* Sat Oct 25 2014 Peter Bieringer <pb@bieringer.de>
- add /usr/share/ipv6calc/db directory

* Fri Oct 10 2014 Peter Bieringer <pb@bieringer.de>
- add DBIP-update.sh DBIP-generate-db.pl

* Mon Jul 14 2014 Peter Bieringer <pb@bieringer.de>
- review description

* Sun Feb 02 2014 Peter Bieringer <pb@bieringer.de>
- add support for optional shared-library build

* Tue Nov 12 2013 Peter Bieringer <pb@bieringer.de>
- add datadir with tools

* Thu Sep 26 2013 Peter Bieringer <pb@bieringer.de>
- add support for --with geoip-dyn and --with ip2location-dyn

* Wed Sep 12 2012 Peter Bieringer <pb@bieringer.de>
- change requirements for ip2location to Mandrake Linux compatible names
- change location of "ipv6calc" from bin to %%{_bindir}
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
- change IPv6 address in description

* Sat Apr 20 2002 Peter Bieringer <pb@bieringer.de>
- Change URL

* Sun Apr 07 2002 Peter Bieringer <pb@bieringer.de>
- add more analog example files

* Fri Apr 05 2002 Peter Bieringer <pb@bieringer.de>
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
- Add doc directory also to files to make sure the directory will be removed on update or deinstall
- change install permissions for entries in doc directory
- change "make install" to "make installonly" (make test should be only executed once)

* Wed Mar 14 2001 Peter Bieringer <pb@bieringer.de>
- Add "make clean" and "make test" on build

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
