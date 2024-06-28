#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QQuickView>
#include "sailfishapp.h"

class ViewHelper : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "harbour.batteryoverlay")

public:
    explicit ViewHelper(QObject *parent = 0);
    bool eventFilter(QObject *object, QEvent *event);

    Q_INVOKABLE void closeOverlay();
    Q_INVOKABLE void startOverlay();
    Q_INVOKABLE void checkOverlay();
    Q_INVOKABLE void setMouseRegion(int x, int y, int w, int h);

public slots:
    void checkActiveOverlay();
    void showSettings();

    Q_SCRIPTABLE Q_NOREPLY void exit();
    Q_SCRIPTABLE Q_NOREPLY void pingOverlay();

signals:
    Q_SCRIPTABLE void overlayRunning();

private:
    void showOverlay();

    QQuickView *overlayView;

};

#endif // VIEWHELPER_H
