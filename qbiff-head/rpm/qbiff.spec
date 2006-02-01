#
# spec file for package qbiff (Version 7.1)
#
# Copyright (c) 2004 SUSE LINUX AG, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://www.suse.de/feedback/
#

# neededforbuild  qt3-devel-packages x-devel-packages glib2 openssl openssl-devel

BuildRequires: aaa_base acl attr bash bind-utils bison bzip2 coreutils cpio cpp cracklib cvs cyrus-sasl db devs diffutils e2fsprogs file filesystem fillup findutils flex gawk gdbm-devel glibc glibc-devel glibc-locale gpm grep groff gzip info insserv klogd less libacl libattr libgcc libnscd libselinux libstdc++ libxcrypt libzio m4 make man mktemp module-init-tools ncurses ncurses-devel net-tools netcfg openldap2-client openssl pam pam-modules patch permissions popt procinfo procps psmisc pwdutils rcs readline sed strace syslogd sysvinit tar tcpd texinfo timezone unzip util-linux vim zlib zlib-devel autoconf automake binutils expat fbset fontconfig fontconfig-devel freeglut freeglut-devel freetype2 freetype2-devel gcc gcc-c++ gdbm gettext hwinfo hwinfo-devel libjpeg libjpeg-devel liblcms liblcms-devel libmng libmng-devel libpng libpng-devel libstdc++-devel libtool perl qt3 qt3-devel rpm swig udev update-desktop-files wireless-tools xorg-x11-Mesa xorg-x11-Mesa-devel xorg-x11-devel xorg-x11-driver-options xorg-x11-libs xorg-x11-server

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
    Marcus Schäfer <ms@suse.de>

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

