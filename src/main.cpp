#include <QDBusConnection>
#include <QGuiApplication>
#include <QScopedPointer>
#include <sailfishapp.h>
#include "overlayhelper.h"
#include "settingshelper.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(SailfishApp::application(argc, argv));
    application->setApplicationVersion(QString(APP_VERSION));

    if (application->arguments().contains("-daemon")) {
        bool registered = QDBusConnection::sessionBus().registerService("harbour.batteryoverlay.overlay");
        if (!registered) {
            return 1;
        }
        QScopedPointer<OverlayHelper> overlayHelper(new OverlayHelper(application.data()));
    } else {
        QScopedPointer<SettingsHelper> settingsHelper(new SettingsHelper(application.data()));
    }
    return application->exec();
}
