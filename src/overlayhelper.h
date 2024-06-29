#ifndef OVERLAYHELPER_H
#define OVERLAYHELPER_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QQuickView>

class OverlayHelper : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "harbour.batteryoverlay")

public:
    explicit OverlayHelper(QObject *parent = 0);
    bool eventFilter(QObject *object, QEvent *event);

public slots:
    Q_SCRIPTABLE Q_NOREPLY void show();
    Q_SCRIPTABLE Q_NOREPLY void hide();
    Q_SCRIPTABLE Q_NOREPLY void ping();

signals:
    Q_SCRIPTABLE void visibleChanged(bool visible);

private:
    QQuickView *overlayView;

};

#endif // OVERLAYHELPER_H
