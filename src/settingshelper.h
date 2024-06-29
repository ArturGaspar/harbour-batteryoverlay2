#ifndef SETTINGSHELPER_H
#define SETTINGSHELPER_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusInterface>

class SettingsHelper : public QObject
{
    Q_OBJECT

public:
    explicit SettingsHelper(QObject *parent = 0);

    Q_INVOKABLE void showOverlay();
    Q_INVOKABLE void hideOverlay();

signals:
    Q_SCRIPTABLE void overlayVisibleChanged(bool visible);

private:
    void checkOverlay();
    void startOverlay();

    QDBusInterface overlay;

};

#endif // SETTINGSHELPER_H
