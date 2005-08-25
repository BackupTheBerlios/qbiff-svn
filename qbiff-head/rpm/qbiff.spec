#
# spec file for package qbiff (Version 7.1)
#
# Copyright (c) 2004 SUSE LINUX AG, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://www.suse.de/feedback/
#

# neededforbuild  flex hwinfo hwinfo-devel qt3-devel-packages swig udev update-desktop-files x-devel-packages xorg-x11-driver-options xorg-x11-server yacc fbset mono-devel-packages glib2 java2-devel-packages python-devel python wireless-tools graphviz doxygen xorg-x11-fonts-100dpi xorg-x11-fonts-75dpi ghostscript-fonts-std sysfsutils hal dbus-1

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
%setup -n qbiff -a 1
# %patch

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

#=================================================
# qbiff files...      
#-------------------------------------------------
%files
%defattr(-,root,root)
