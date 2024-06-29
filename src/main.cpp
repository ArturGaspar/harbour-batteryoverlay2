#include <QDir>
#include <QGuiApplication>
#include <QLockFile>
#include <QProcess>
#include <QQmlContext>
#include <QQuickView>
#include <QScopedPointer>
#include <QStandardPaths>
#include <sailfishapp.h>
#include "colorhelper.h"
#include "overlayhelper.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(SailfishApp::application(argc, argv));
    application->setApplicationVersion(QString(APP_VERSION));

    QScopedPointer<OverlayHelper> overlayHelper;
    QScopedPointer<QLockFile> lockFile;
    QScopedPointer<QQuickView> settingsView;

    if (application->arguments().contains("-daemon")) {
        QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        lockFile.reset(new QLockFile(QDir(appData).filePath("daemon-lock")));
        lockFile->setStaleLockTime(0);
        if (!lockFile->tryLock()) {
            return 1;
        }
        overlayHelper.reset(new OverlayHelper(application.data()));
    } else {
        // Will exit if already running.
        QProcess::startDetached(QCoreApplication::instance()->applicationFilePath(),
                                QStringList() << "-daemon");

        settingsView.reset(SailfishApp::createView());

        settingsView->setTitle("BatteryOverlay Settings");
        settingsView->rootContext()->setContextProperty("colorHelper", new ColorHelper(settingsView.data()));
        settingsView->setSource(SailfishApp::pathTo("qml/settings.qml"));
        settingsView->showFullScreen();
    }
    return application->exec();
}
