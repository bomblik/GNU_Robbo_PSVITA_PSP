%define name gnurobbo
%define version 0.65
%define release %mkrel 1
 
Summary: Logic game ported from ATARI XE/XL
Name:      %{name}
Version:   %{version}
Release:   %{release}
License: GPLv2+
Group: Games/Arcade
Source: http://prdownloads.sourceforge.net/gnurobbo/%{name}-%{version}-source.tar.gz
Source10: %{name}.16.png.bz2
Source11: %{name}.32.png.bz2
Source12: %{name}.48.png.bz2

URL: http://gnurobbo.sourceforge.net
BuildRoot: %{_tmppath}/%{name}-buildroot

BuildRequires: libSDL-devel
BuildRequires: libSDL_ttf-devel
BuildRequires: libSDL_image-devel
BuildRequires: libSDL_mixer-devel
#Requires: libSDL1.2 >= 1.2.5
#Requires: libSDL_ttf2.0_0 >= 2.0.5

%description
GNU Robbo is a very addictive logic game. You must help
little robot to get out of an unfriendly planet, collecting
parts of an emergency capsule.

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build
%make PACKAGE_DATA_DIR=%{_datadir}/%{name}

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall BINDIR=$RPM_BUILD_ROOT/%{_bindir} \
PACKAGE_DATA_DIR=$RPM_BUILD_ROOT/%{_datadir}/%{name} \
DOCDIR=$RPM_BUILD_ROOT/%{_docdir}/%{name}

mkdir -p $RPM_BUILD_ROOT%{_datadir}/applications
cat > $RPM_BUILD_ROOT%{_datadir}/applications/mandriva-%{name}.desktop << EOF
[Desktop Entry]
Name=GNU Robbo
Comment=%{summary}
Exec=%{_bindir}/%{name}
Icon=%{name}
Terminal=false
Type=Application
StartupNotify=true
Categories=Game;ArcadeGame;
EOF

mkdir -p $RPM_BUILD_ROOT/%{_miconsdir}
mkdir -p $RPM_BUILD_ROOT/%{_liconsdir}
bzcat %{SOURCE10} > $RPM_BUILD_ROOT%{_miconsdir}/%{name}.png
bzcat %{SOURCE11} > $RPM_BUILD_ROOT%{_iconsdir}/%{name}.png
bzcat %{SOURCE12} > $RPM_BUILD_ROOT%{_liconsdir}/%{name}.png

%if %mdkversion < 200900
%post
%{update_menus}
%endif

%if %mdkversion < 200900
%postun
%{clean_menus}
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{_bindir}/*
%{_datadir}/%{name}
%{_datadir}/applications/mandriva-%{name}.desktop
%defattr(644,root,root,755)
%doc README ChangeLog AUTHORS
%{_miconsdir}/%{name}.png
%{_iconsdir}/%{name}.png
%{_liconsdir}/%{name}.png


%changelog
* Wed Dec 30 2009 Crispin Boylan <crisb@mandriva.org> 0.65-1mdv2010.1
+ Revision: 483836
- BuildRequires SDL_mixer
- New release

* Fri May 15 2009 Samuel Verschelde <stormi@mandriva.org> 0.62-1mdv2010.0
+ Revision: 376151
- new version 0.62

* Tue Mar 03 2009 Crispin Boylan <crisb@mandriva.org> 0.61-1mdv2009.1
+ Revision: 347659
- New version, does not use configure
 
* Thu Jul 24 2008 Thierry Vignaud <tvignaud@mandriva.com> 0.57-13mdv2009.0
+ Revision: 246501
- rebuild

+ Pixel <pixel@mandriva.com>
- rpm filetriggers deprecates update_menus/update_scrollkeeper/update_mime_database/update_icon_cache/update_desktop_database/post_install_gconf_schemas

* Wed Mar 19 2008 Emmanuel Andry <eandry@mandriva.org> 0.57-11mdv2008.1
+ Revision: 189023
  - Drop useless requires

* Mon Feb 18 2008 Thierry Vignaud <tvignaud@mandriva.com> 0.57-10mdv2008.1
+ Revision: 170871
  - rebuild
  - fix "foobar is blabla" summary (=> "blabla") so that it looks nice in rpmdrake

* Tue Jan 22 2008 Crispin Boylan <crisb@mandriva.org> 0.57-9mdv2008.1
+ Revision: 156329
  - Fix requires

* Mon Jan 21 2008 Crispin Boylan <crisb@mandriva.org> 0.57-8mdv2008.1
+ Revision: 155922
  - Rebuild

+ Olivier Blin <oblin@mandriva.com>
  - restore BuildRoot
 
+ Thierry Vignaud <tvignaud@mandriva.com>
  - kill re-definition of %%buildroot on Pixel's request

* Fri Dec 14 2007 Funda Wang <fwang@mandriva.org> 0.57-7mdv2008.1
+ Revision: 119618
  - rebuild to fix menu

+ Thierry Vignaud <tvignaud@mandriva.com>
  - kill desktop-file-validate's error: string list key "Categories" in group "Desktop Entry" does not have a semicolon (";") as trailing character
  - kill desktop-file-validate's 'warning: key "Encoding" in group "Desktop Entry" is deprecated'

* Mon Apr 30 2007 Crispin Boylan <crisb@mandriva.org> 0.57-6mdv2008.0
+ Revision: 19463
- XDG menu
- Import gnurobbo



* Mon Apr 28 2005 Giuseppe Ghibò <ghibo@mandrakesoft.com> 0.57-5mdk
- Fixed dep for x86-64.

* Thu Sep 02 2004 Lenny Cartier <lenny@mandrakesoft.com> 0.57-4mdk
- fix menu

* Thu Jun 03 2004 Lenny Cartier <lenny@mandrakesoft.com 0.57-3mdk
- rebuild

* Sat Feb 01 2003 Lenny Cartier <lenny@mandrakesoft.com 0.57-2mdk
- rebuild

* Sun Nov 17 2002 Lenny Cartier <lenny@mandrakesoft.com> 0.57-1mdk 
- 0.57
 
* Fri Nov 08 2002 Lenny Cartier <lenny@mandrakesoft.com> 0.56-1mdk
- add menu
- from Arkadiusz Lipiec <arkadiusz.lipiec@gmail.com> :
- Created
