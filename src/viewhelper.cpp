#include "viewhelper.h"
#include "colorhelper.h"

#include <wayland-client.h>
#include <qpa/qplatformnativeinterface.h>
#include <QtQml>
#include <QGuiApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include <QQuickView>

static void setWaylandInputRegion(QPlatformNativeInterface *wliface, QWindow *window, const QRegion &region)
{
    if (wl_compositor *wlcompositor = static_cast<wl_compositor *>(
                wliface->nativeResourceForIntegration("compositor"))) {
        if (wl_surface *wlsurface = static_cast<wl_surface *>(
                    wliface->nativeResourceForWindow("surface", window))) {
            wl_region *wlregion = wl_compositor_create_region(wlcompositor);

            for (const QRect &rect : region.rects()) {
                wl_region_add(wlregion, rect.x(), rect.y(), rect.width(), rect.height());
            }

            wl_surface_set_input_region(wlsurface, wlregion);
            wl_region_destroy(wlregion);

            wl_surface_commit(wlsurface);
        }
    }
}

static void setWaylandOpaqueRegion(QPlatformNativeInterface *wliface, QWindow *window, const QRegion &region)
{
    if (wl_compositor *wlcompositor = static_cast<wl_compositor *>(
                wliface->nativeResourceForIntegration("compositor"))) {
        if (wl_surface *wlsurface = static_cast<wl_surface *>(
                    wliface->nativeResourceForWindow("surface", window))) {
            wl_region *wlregion = wl_compositor_create_region(wlcompositor);

            for (const QRect &rect : region.rects()) {
                wl_region_add(wlregion, rect.x(), rect.y(), rect.width(), rect.height());
            }

            wl_surface_set_opaque_region(wlsurface, wlregion);
            wl_region_destroy(wlregion);

            wl_surface_commit(wlsurface);
        }
    }
}

ViewHelper::ViewHelper(QObject *parent) :
    QObject(parent),
    overlayView(NULL)
{
}

bool ViewHelper::eventFilter(QObject *object, QEvent *event)
{
    if (object != overlayView) {
        return false;
    } else switch (event->type()) {
    case QEvent::PlatformSurface: {
        QPlatformSurfaceEvent *platformEvent = static_cast<QPlatformSurfaceEvent *>(event);

        if (QPlatformWindow *handle = platformEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated
                ? overlayView->handle()
                : nullptr) {
            QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

            native->setWindowProperty(handle, QStringLiteral("CATEGORY"), QStringLiteral("notification"));
            setWaylandOpaqueRegion(native, overlayView, QRegion(0, 0, 0, 0));
        }
        return false;
    }
    case QEvent::DynamicPropertyChange: {
        QDynamicPropertyChangeEvent *propertyEvent = static_cast<QDynamicPropertyChangeEvent *>(event);
        if (QPlatformWindow *handle = propertyEvent->propertyName() == "MOUSE_REGION"
                ? overlayView->handle()
                : nullptr) {
            QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

            auto mouseRegion = overlayView->property("MOUSE_REGION");
            native->setWindowProperty(handle, QStringLiteral("MOUSE_REGION"), mouseRegion);
            setWaylandInputRegion(native, overlayView, mouseRegion.value<QRegion>());
        }
        return false;
    }
    default:
        return false;
    }

}

void ViewHelper::closeOverlay()
{
    if (overlayView) {
        QDBusConnection::sessionBus().unregisterObject("/harbour/batteryoverlay/overlay");
        QDBusConnection::sessionBus().unregisterService("harbour.batteryoverlay.overlay");
        overlayView->removeEventFilter(this);
        overlayView->close();
        delete overlayView;
        overlayView = NULL;
    }
    else {
        QDBusInterface iface("harbour.batteryoverlay.overlay", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay");
        iface.call(QDBus::NoBlock, "exit");
    }
}

void ViewHelper::checkOverlay()
{
    QDBusInterface iface("harbour.batteryoverlay.overlay", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay");
    iface.call(QDBus::NoBlock, "pingOverlay");
}

void ViewHelper::setMouseRegion(int x, int y, int w, int h)
{
    if (overlayView) {
        QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
        native->setWindowProperty(overlayView->handle(), QLatin1String("MOUSE_REGION"), QRegion(x, y, w, h));
    }
}

void ViewHelper::startOverlay()
{
    checkActiveOverlay();
}

void ViewHelper::checkActiveOverlay()
{
    bool inactive = QDBusConnection::sessionBus().registerService("harbour.batteryoverlay.overlay");
    if (inactive) {
        showOverlay();
    }
}

void ViewHelper::exit()
{
    QTimer::singleShot(100, qGuiApp, SLOT(quit()));
}

void ViewHelper::pingOverlay()
{
    if (overlayView) {
        Q_EMIT overlayRunning();
    }
}

void ViewHelper::showOverlay()
{
    qDebug() << "show overlay";
    QDBusConnection::sessionBus().registerObject("/harbour/batteryoverlay/overlay", this, QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);

    overlayView = SailfishApp::createView();
    overlayView->installEventFilter(this);
    QObject::connect(overlayView->engine(), SIGNAL(quit()), qGuiApp, SLOT(quit()));
    overlayView->setTitle("BatteryOverlay");
    overlayView->rootContext()->setContextProperty("viewHelper", this);

    QColor color;
    color.setRedF(0.0);
    color.setGreenF(0.0);
    color.setBlueF(0.0);
    color.setAlphaF(0.0);
    overlayView->setColor(color);
    overlayView->setClearBeforeRendering(true);

    overlayView->setSource(SailfishApp::pathTo("qml/overlay.qml"));
    overlayView->create();
    QRegion region(0, 0, 0, 0);
    overlayView->setMask(region);
    overlayView->setProperty("MOUSE_REGION", region);
    overlayView->show();

    QDBusConnection::sessionBus().disconnect("", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay",
                                          "overlayRunning", this, SIGNAL(overlayRunning()));

    Q_EMIT overlayRunning();
}

void ViewHelper::showSettings()
{
    qDebug() << "show settings";

    QQuickView *settingsView = SailfishApp::createView();
    settingsView->setTitle("BatteryOverlay Settings");
    settingsView->rootContext()->setContextProperty("helper", this);
    settingsView->rootContext()->setContextProperty("colorHelper", new ColorHelper(this));
    settingsView->setSource(SailfishApp::pathTo("qml/settings.qml"));
    settingsView->showFullScreen();

    if (!overlayView) {
        QDBusConnection::sessionBus().connect("", "/harbour/batteryoverlay/overlay", "harbour.batteryoverlay",
                                              "overlayRunning", this, SIGNAL(overlayRunning()));
    }
}
