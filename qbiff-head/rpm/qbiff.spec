#
# spec file for package qbiff (Version 7.1)
#
# Copyright (c) 2004 SUSE LINUX AG, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://www.suse.de/feedback/
#

%if %{suse_version} > 1010
BuildRequires:  doxygen ghostscript-fonts-std graphviz hal-devel java-1_4_2-gcj-compat-devel python-devel qt3-devel readline-devel swig sysfsutils update-desktop-files xorg-x11-server-sdk antlr
gettext-devel openssl-devel
%else
BuildRequires:  doxygen ghostscript-fonts-std graphviz hal-devel java2-devel-packages python-devel qt3-devel readline-devel swig sysfsutils update-desktop-files openssl-devel
%endif

#=================================================
# Description qbiff
#-------------------------------------------------
Name:         qbiff
Summary:      Yet another biff implementation SSL server/client based
Version:      7.1
Release:      2
Group:        System/X11/Utilities
License:      Other License(s), see package, GPL
Source:       qbiff.tar.bz2
%if %{suse_version} <= 1000
Patch:        qbiff.dif
%endif
BuildRoot:    %{_tmppath}/%{name}-%{version}-build

%description
This package contains the qbiff application

Authors:
--------
    Marcus Sch�fer <ms@suse.de>

#=================================================
# Preparation...
#-------------------------------------------------
%prep
%setup -n qbiff
%if %{suse_version} <= 1000
%patch
%endif

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
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/sbin
mkdir -p $RPM_BUILD_ROOT/usr/share/qbiff
mkdir -p $RPM_BUILD_ROOT/usr/share/qbiff/cert-server
mkdir -p $RPM_BUILD_ROOT/usr/share/qbiff/cert-client
mkdir -p $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps

install -m 755 qbiff               $RPM_BUILD_ROOT/usr/sbin
install -m 755 qbiff-client        $RPM_BUILD_ROOT/usr/sbin
install -m 755 readmail.local      $RPM_BUILD_ROOT/usr/share/qbiff/readmail
install -m 644 pixmaps/tooltip.xpm $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps
install -m 644 pixmaps/private.png $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps
install -m 644 pixmaps/public.png  $RPM_BUILD_ROOT/usr/share/qbiff/pixmaps

install -m 644 cert-server/rootcert.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/dh1024.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/dh512.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server
install -m 644 cert-server/server.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-server

install -m 644 cert-client/client.pem \
	$RPM_BUILD_ROOT/usr/share/qbiff/cert-client


#=================================================
# qbiff files...      
#-------------------------------------------------
%files
%defattr(-,root,root)
%dir /usr/share/qbiff
/usr/sbin/qbiff
/usr/sbin/qbiff-client
/usr/share/qbiff/readmail
/usr/share/qbiff/pixmaps
/usr/share/qbiff/cert-server
/usr/share/qbiff/cert-client

