TARGET = harbour-batteryoverlay2

QT += dbus gui-private
CONFIG += sailfishapp
PKGCONFIG += \
  wayland-client \
# PKGCONFIG end

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    src/main.cpp \
    src/settingshelper.cpp \
    src/overlayhelper.cpp \
    src/colorhelper.cpp

HEADERS += \
    src/settingshelper.h \
    src/overlayhelper.h \
    src/colorhelper.h

OTHER_FILES += \
    rpm/harbour-batteryoverlay2.spec \
    harbour-batteryoverlay2.desktop \
    harbour-batteryoverlay2.png \
    qml/overlay.qml \
    qml/settings.qml \
    qml/pages/MainPage.qml \
    qml/cover/CoverPage.qml \
    qml/pages/ColorDialog.qml \
    qml/components/ColorSelector.qml \
    qml/components/ColorItem.qml \
    qml/pages/AboutPage.qml

service.path = /usr/lib/systemd/user
service.files = harbour-batteryoverlay2.service
INSTALLS += service
