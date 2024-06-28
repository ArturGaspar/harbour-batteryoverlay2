Name:       harbour-batteryoverlay2

%define __provides_exclude_from ^%{_datadir}/.*$
%define __requires_exclude ^libQt5.*$

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary:    Battery overlay
Version:    0.3.0
Release:    0
Group:      Qt/Qt
License:    WTFPL
URL:        https://github.com/ArturGaspar/%{name}
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(wayland-client)
BuildRequires:  desktop-file-utils
BuildRequires:  systemd

%description
Simple application for displaying battery charging percentage as overlay line
on top of all windows

%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5 \
    VERSION=%{version}

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%if "%{_userunitdir}" != "/usr/lib/systemd/user"
mv %{buildroot}/usr/lib/systemd/user/%{name}.service %{buildroot}/%{_userunitdir}/%{name}.service
%endif

desktop-file-install --delete-original \
    --dir %{buildroot}%{_datadir}/applications \
    %{buildroot}%{_datadir}/applications/*.desktop

%post
%systemd_user_post %{name}.service

%preun
%systemd_user_preun %{name}.service

%postun
%systemd_user_postun %{name}.service

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/86x86/apps/%{name}.png
%{_userunitdir}/%{name}.service
