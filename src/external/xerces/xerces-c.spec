Summary:	Xerces-C++ validating XML parser
Name:		xerces-c
Version:	3.2.3
Release:	1
URL:		http://xerces.apache.org/xerces-c/
Source0:	%{name}-%{version}.tar.bz2
License:        Apache
Group:		Libraries
BuildRoot:	%{_tmppath}/%{name}-root
Prefix:		/usr
%{!?_without_curl:BuildRequires: curl-devel}
%{?_with_icu:BuildRequires: libicu-devel}


%description
Xerces-C++ is a validating XML parser written in a portable subset of C++.
Xerces-C++ makes it easy to give your application the ability to read and
write XML data. A shared library is provided for parsing, generating,
manipulating, and validating XML documents.

The parser provides high performance, modularity, and scalability. Source
code, samples and API documentation are provided with the parser. For
portability, care has been taken to make minimal use of templates and
minimal use of #ifdefs.

%package devel
Requires:	%{name} = %{version}
Group:		Development/Libraries
Summary:	Header files for Xerces-C++ validating XML parser

%description devel
Header files you can use to develop XML applications with Xerces-C++.

Xerces-C++ is a validating XML parser written in a portable subset of C++.
Xerces-C++ makes it easy to give your application the ability to read and
write XML data. A shared library is provided for parsing, generating,
manipulating, and validating XML documents.

%prep
%setup -q

%build
%configure %{!?_without_curl:--enable-netaccessor-curl} %{?_with_icu:--enable-transcoder-icu --enable-msgloader-icu} %{?xerces_options}
%{__make}

%install
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf $RPM_BUILD_ROOT
%{__make} install DESTDIR=$RPM_BUILD_ROOT

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf $RPM_BUILD_ROOT

%ifnos solaris2.8 solaris2.9 solaris2.10
%post -p /sbin/ldconfig
%endif

%ifnos solaris2.8 solaris2.9 solaris2.10
%postun -p /sbin/ldconfig
%endif

%files
%defattr(755,root,root)
%{_bindir}/*
%{_libdir}/lib%{name}-*.so
%exclude %{_libdir}/lib%{name}.la


%files devel
%defattr(-,root,root)
%{_includedir}/xercesc
%{_libdir}/lib%{name}.so
%{_libdir}/lib%{name}.a
%{_libdir}/pkgconfig/%{name}.pc

%changelog
* Mon Dec 30 2019 Scott Cantor <cantor.2@osu.edu>
- Upped version

* Wed Jun 21 2017 Scott Cantor <cantor.2@osu.edu>
- Upped version

* Thu Jun 9 2016 Scott Cantor <cantor.2@osu.edu>
- Upped version

* Fri Feb 27 2015 Scott Cantor <cantor.2@osu.edu>
- Upped version and avoided ownership of /usr/include

* Fri Mar  7 2008 Boris Kolpackov <boris@codesynthesis.com>
- Integrated updates for 3.0.0 from Scott Cantor.

* Fri Jun  6 2003 Tuan Hoang <tqhoang@bigfoot.com>
- updated for new Xerces-C filename and directory format
- fixed date format in changelog section

* Fri Mar 14 2003 Tinny Ng <tng@ca.ibm.com>
- changed to 2.3

* Wed Dec 18 2002 Albert Strasheim <albert@stonethree.com>
- added symlink to libxerces-c.so in lib directory

* Fri Dec 13 2002 Albert Strasheim <albert@stonethree.com>
- added seperate doc package
- major cleanups

* Tue Sep 03 2002  <thomas@linux.de>
- fixed missing DESTDIR in Makefile.util.submodule

* Mon Sep 02 2002  <thomas@linux.de>
- Initial build.
