# -----------------------------------------------------
# Define static values
# ----------------------------------------------------- 

# -----------------------------------------------------
# Define requirements for configure, BuildRequire,
# Require and so on (depending on macros)
# -----------------------------------------------------

# -----------------------------------------------------
# RPM-Information
# -----------------------------------------------------
Name:			jhead
Summary:		Tool for handling EXIF metadata in JPEG image files

Version:		2.93
Release:		0

Group:			Libraries
License:		Public Domain
URL:			http://www.sentex.net/~mwandel/jhead/

Source:			http://www.sentex.net/~mwandel/jhead/%{name}-%{version}.tar.gz

BuildRoot:		%{_tmppath}/%{name}-%{version}-root
BuildRequires:		glibc-devel

# -----------------------------------------------------
# Package-Information (for main package)
# -----------------------------------------------------
%description
This package provides a tool for displaying and manipulating non-image
portions of EXIF format JPEG image files, as produced by most digital cameras.

# -----------------------------------------------------
# Prepare-Section
# -----------------------------------------------------
%prep
%setup -q 

# -----------------------------------------------------
# Build-Section
# -----------------------------------------------------
%build
make %{?_smp_mflags}

# -----------------------------------------------------
# Install-Section
# -----------------------------------------------------
%install
mkdir -p %{buildroot}%{_bindir}
cp jhead %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_mandir}/man1/
cp jhead.1.gz %{buildroot}%{_mandir}/man1/

# -----------------------------------------------------
# Clean-Section
# -----------------------------------------------------
%clean
rm -rf %{buildroot}

# -----------------------------------------------------
# Files-Section (for main package)
# -----------------------------------------------------
%files
%defattr(0644,root,root,0755)
%attr(0755,root,root) %{_bindir}/*
%doc readme.txt usage.html
%{_mandir}/man1/jhead.1.gz

# -----------------------------------------------------
# Changelog-Section
# -----------------------------------------------------
%changelog

* Fri Dec 2 2011 - 2.92
- Hanle UTF8 in IPTC
- Fix error in help

* Fri Feb 5 2010 - 2.90
- Improvements to IPTC handling
- Handle mixted-endian-ness files from newer Canon point and shoot cameras.

* Fri Nov 6 2009 - 2.88
- Added ability to move files around with the -n command
- Various minor bugfixes and maintenance changes

* Thu Mar 3 2009 - 2.87
- Added the ability to move files with the -n option
- Fix a bunch of breakages from security fixes asked
  for by debian people

* Sat Feb 14 2009 - 2.86
- Added ability to move files around with the -n command
- Various minor bugfixes and maintenance changes

* Mon Oct 4 2008 - 2.84
- decode more exif tags in -v mode
- Fix gpsinfo altitude bug
- Fix several potential stirng overflows

* Thu Apr 03 2008 - 2.82
- No longer delete XMP section on some operations
- Improve IPTC display
- Minor tweaks and cleanups

* Tue Nov 13 2007 - 2.8
- Limited IPTC handling
- Added -q option
- Fix handling of corrupted GPS directory.
- Extract focus distance from canon makernote.
- Extract subject range (pentax and fuji cameras)

* Thu Jan 11 2007 - 2.7
- Release version 2.7

* Sat  Apr 29 2006 - 2.6
- Release version 2.6

* Sun Jan 08 2006 - 2.5
- Release version 2.5

* Thu Jun 10 2005 - 2.4-2
- Minor fixups.

* Sun May 29 2005 - 2.4
- Display gps info
- Added -da optino for large date adjust
- Fix time reference bug for -ta option
- Fix crashes with some corrupt jpeg files

* Mon Jan 03 2005 - 2.4
- Handle readonly files better
- Handle more strange jpegs
- added 'purejpg' option
- Display digital zoom ratio

* Sun Jun 20 2004 - 2.2-0
- Various bug and spelling fixes.
- added ability to do sequential renaming

* Tue Jan 08 2004 Matthias Wandel <mwandel[at]sentex.net> - 2.1-0
- Bumped version number to 2.1 for new jhead release.
- Added -cl feature
- added -noroot feature

* Tue Jun 03 2003 Oliver Pitzeier <oliver@linux-kernel.at> - 2.0-3
- Specfile cleanup/beautifying
- Use _smp_mflags within make
- Add versions to the changelog entries

* Mon Apr 14 2003 Matthias Wandel <mwandel[at]sentex.net> - 2.0-2
- First jhead 2.0 RPM built by me.
- Finally wrote a nice man page for jhead
- Using jhead 1.9 RPM from connectiva linux as starting point (left in the portugese tags)


