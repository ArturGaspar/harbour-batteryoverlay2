#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include <QObject>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include "sailfishapp.h"
#include <QDBusInterface>
#include <QDBusConnection>

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

    QQuickView *overlayView;

private slots:
    void onPackageStatusChanged(const QString &package, int status);

};

#endif // VIEWHELPER_H
