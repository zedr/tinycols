Name:           tinycols
Version:        0.8.4
Release:        1%{?dist}
Summary:        A jewel-matching puzzle game for the Unix terminal

License:        BSD-3-Clause
URL:            https://github.com/zedr/tinycols
Source0:        %{url}/archive/v%{version}/%{name}-%{version}.tar.gz

%global debug_package %{nil}

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  ncurses-devel

%description
TinyCols is a jewel-matching puzzle game for the Unix terminal, inspired by
classic arcade and console games. Players guide clusters of three colored 
jewels as they fall into a grid; when three or more jewels of the same color align
vertically, horizontally, or diagonally, they are removed and points are
scored. The game offers three difficulty classes (Novice, Amateur, Pro) with
an increasing number of jewel colors, and supports a customizable grid size.

%prep
%autosetup

%build
%make_build

%install
install -Dpm 0755 build/%{name} %{buildroot}%{_bindir}/%{name}
install -Dpm 0644 doc/man/%{name}.6 %{buildroot}%{_mandir}/man6/%{name}.6

%files
%license LICENSE
%{_bindir}/%{name}
%{_mandir}/man6/%{name}.6*

%changelog
* Sun Mar 15 2026 Rigel Di Scala <zedr@zedr.com> - 0.8.4-1
- Initial package
