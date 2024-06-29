#include "settingshelper.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QProcess>
#include <QQmlContext>
#include <QQuickView>
#include <sailfishapp.h>
#include "colorhelper.h"

SettingsHelper::SettingsHelper(QObject *parent) :
    QObject(parent),
    overlay("harbour.batteryoverlay.overlay", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay")
{
    QDBusConnection::sessionBus().connect("", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay",
                                          "visibleChanged", this, SIGNAL(overlayVisibleChanged(bool visible)));

    QQuickView *settingsView = SailfishApp::createView();
    settingsView->setTitle("BatteryOverlay Settings");
    settingsView->rootContext()->setContextProperty("helper", this);
    settingsView->rootContext()->setContextProperty("colorHelper", new ColorHelper(this));
    settingsView->setSource(SailfishApp::pathTo("qml/settings.qml"));
    settingsView->showFullScreen();
    checkOverlay();
}

void SettingsHelper::showOverlay()
{
    startOverlay();
    overlay.call(QDBus::NoBlock, "show");
}

void SettingsHelper::hideOverlay()
{
    overlay.call(QDBus::NoBlock, "hide");
}

void SettingsHelper::checkOverlay()
{
    startOverlay();
    overlay.call(QDBus::NoBlock, "ping");
}

void SettingsHelper::startOverlay()
{
    // Will exit if already running.
    QProcess::startDetached(QCoreApplication::instance()->applicationFilePath(),
                            QStringList() << "-daemon");
}
