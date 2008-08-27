#
# spec file for package qbiff (Version 7.1)
#
# Copyright (c) 2004 SUSE LINUX AG, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
#
Name: qbiff
%if %{suse_version} > 1020
BuildRequires:  fdupes
%endif
%if %{suse_version} > 1010
BuildRequires:  libqt4 libqt4-devel libqt4-x11
BuildRequires:  openssl-devel
%endif
%if %{suse_version} <= 1010
BuildRequires:  qt qt-devel qt-x11
BuildRequires:  openssl-devel gcc-c++
BuildRequires:  libpng-devel freetype2-devel
%endif
Summary:      Yet another biff implementation SSL server/client based
Version:      2.1
Release:      16
Group:        System/X11/Utilities
License:      Other License(s), see package, GPL
Source:       qbiff.tar.bz2
Source1:      qbiff.sysconfig
BuildRoot:    %{_tmppath}/%{name}-%{version}-build
Requires:     qbiff-server

%description
This package contains the qbiff application

%package -n qbiff-server
Summary:  Server part of qbiff
Group:    System/X11/Utilities

%description -n qbiff-server
This package contains the qbiff server

Authors:
--------
    Marcus Sch�fer <ms@suse.de>
#=================================================
# Preparation...
#-------------------------------------------------
%prep
%setup -n qbiff

%build
test -e /.buildenv && . /.buildenv
#=================================================
# build sources
#-------------------------------------------------
./.make && make

#=================================================
# install sources
#-------------------------------------------------
%install
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/sbin
mkdir -p $RPM_BUILD_ROOT/usr/share/qbiff
mkdir -p $RPM_BUILD_ROOT/usr/share/qbiff/cert-server
mkdir -p $RPM_BUILD_ROOT/usr/share/qbiff/cert-client
mkdir -p $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps
mkdir -p $RPM_BUILD_ROOT/etc/init.d

install -m 755 qbiff               $RPM_BUILD_ROOT/usr/bin
install -m 755 qbiff-client        $RPM_BUILD_ROOT/usr/bin
install -m 755 qbiff-server        $RPM_BUILD_ROOT/usr/bin
install -m 755 readmail.local      $RPM_BUILD_ROOT/usr/share/qbiff/readmail
install -m 755 readmail.local      $RPM_BUILD_ROOT/usr/share/qbiff/readmail.private
install -m 644 pixmaps/newmail.png $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps
install -m 644 pixmaps/nomail.png  $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps
install -m 644 pixmaps/private.png $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps
install -m 644 pixmaps/public.png  $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps

install -m 755 init.d/qbiff        $RPM_BUILD_ROOT/etc/init.d

rm -f %{buildroot}%{_sbindir}/rcqbiff
%{__ln_s} ../../etc/init.d/qbiff %{buildroot}%{_sbindir}/rcqbiff
%{__install} -D -m 0644 %{S:1} %{buildroot}/var/adm/fillup-templates/sysconfig.qbiff

install -m 644 cert-server/Makefile \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/rootcert.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/dh1024.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/dh512.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/server.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/root.cnf \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/server.cnf \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/serverCA.cnf \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server

install -m 644 cert-client/Makefile \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-client
install -m 644 cert-client/client.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-client
install -m 644 cert-client/client.cnf \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-client

%if %{suse_version} > 1020
%fdupes $RPM_BUILD_ROOT/usr/share/qbiff
%endif

%clean
%{__rm} -rf %{buildroot}

%preun -n qbiff-server
%stop_on_removal qbiff

%post -n qbiff-server
%{fillup_and_insserv -n -s qbiff}

%postun -n qbiff-server
%restart_on_update qbiff
%{insserv_cleanup}

#=================================================
# qbiff files...      
#-------------------------------------------------
%files
%defattr(-,root,root)
%dir /usr/share/qbiff
/usr/bin/qbiff-client
/usr/share/qbiff/readmail
/usr/share/qbiff/readmail.private
/usr/share/qbiff/pixmaps

#=================================================
# qbiff server files...      
#-------------------------------------------------
%files -n qbiff-server
%defattr(-,root,root)
%dir /usr/share/qbiff
/usr/bin/qbiff
/usr/bin/qbiff-server
/usr/share/qbiff/cert-server
/usr/share/qbiff/cert-client
%{_sbindir}/rcqbiff
/etc/init.d/qbiff
/var/adm/fillup-templates/sysconfig.qbiff
