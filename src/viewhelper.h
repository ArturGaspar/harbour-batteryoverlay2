#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include <QObject>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include "sailfishapp.h"
#include <mlite5/MGConfItem>
#include <QDBusInterface>
#include <QDBusConnection>

#define SERVICENAME "harbour-batteryoverlay.service"
#define SYSTEMDNAME "%1/.config/systemd/user/%2"
#define WANTSNAME "%1/.config/systemd/user/user-session.target.wants/%2"

class ViewHelper : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "harbour.batteryoverlay")

public:
    explicit ViewHelper(QObject *parent = 0);
    bool eventFilter(QObject *object, QEvent *event);

    Q_INVOKABLE void closeOverlay();
    Q_INVOKABLE void startOverlay();
    Q_INVOKABLE void openStore();
    Q_INVOKABLE void checkOverlay();
    Q_INVOKABLE void setMouseRegion(int x, int y, int w, int h);
    Q_INVOKABLE void removeService();

public slots:
    void checkActiveOverlay();
    void showSettings();

    Q_SCRIPTABLE Q_NOREPLY void exit();
    Q_SCRIPTABLE Q_NOREPLY void pingOverlay();

signals:
    Q_SCRIPTABLE void overlayRunning();

    void applicationRemoval();

private:
    void showOverlay();
    void checkService();

    QQuickView *overlayView;

private slots:
    void onPackageStatusChanged(const QString &package, int status);

};

#endif // VIEWHELPER_H
