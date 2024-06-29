#include "overlayhelper.h"

#include <wayland-client.h>
#include <qpa/qplatformnativeinterface.h>
#include <QDBusConnection>
#include <QGuiApplication>
#include <sailfishapp.h>

static wl_region *makeWlRegion(QPlatformNativeInterface *wliface, const QRegion &region)
{
    auto wlcompositor = static_cast<wl_compositor *>(wliface->nativeResourceForIntegration("compositor"));
    if (!wlcompositor) {
        return nullptr;
    }
    wl_region *wlregion = wl_compositor_create_region(wlcompositor);
    for (const QRect &rect : region.rects()) {
        wl_region_add(wlregion, rect.x(), rect.y(), rect.width(), rect.height());
    }
    return wlregion;
}

static void setWaylandInputRegion(QPlatformNativeInterface *wliface, QWindow *window, const QRegion &region)
{
    wl_region *wlregion = makeWlRegion(wliface, region);
    if (!wlregion) {
        return;
    }
    auto wlsurface = static_cast<wl_surface *>(wliface->nativeResourceForWindow("surface", window));
    if (!wlsurface) {
        return;
    }
    wl_surface_set_input_region(wlsurface, wlregion);
    wl_region_destroy(wlregion);
    wl_surface_commit(wlsurface);
}

static void setWaylandOpaqueRegion(QPlatformNativeInterface *wliface, QWindow *window, const QRegion &region)
{
    wl_region *wlregion = makeWlRegion(wliface, region);
    if (!wlregion) {
        return;
    }
    auto wlsurface = static_cast<wl_surface *>(wliface->nativeResourceForWindow("surface", window));
    if (!wlsurface) {
        return;
    }
    wl_surface_set_opaque_region(wlsurface, wlregion);
    wl_region_destroy(wlregion);
    wl_surface_commit(wlsurface);
}

OverlayHelper::OverlayHelper(QObject *parent) :
    QObject(parent)
{
    QDBusConnection::sessionBus().registerObject("/harbour/batteryoverlay/overlay", this,
                                                 QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);

    overlayView = SailfishApp::createView();
    overlayView->installEventFilter(this);
    overlayView->setTitle("BatteryOverlay");

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

    show();
}

bool OverlayHelper::eventFilter(QObject *object, QEvent *event)
{
    if (object != overlayView) {
        return false;
    }
    switch (event->type()) {
    case QEvent::PlatformSurface: {
        auto platformEvent = static_cast<QPlatformSurfaceEvent *>(event);
        if (platformEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
            QPlatformWindow *handle = overlayView->handle();
            if (handle) {
                QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
                native->setWindowProperty(handle, QStringLiteral("CATEGORY"), QStringLiteral("notification"));
                setWaylandOpaqueRegion(native, overlayView, QRegion(0, 0, 0, 0));
            }
        }
        return false;
    }
    case QEvent::DynamicPropertyChange: {
        auto propertyEvent = static_cast<QDynamicPropertyChangeEvent *>(event);
        if (propertyEvent->propertyName() == "MOUSE_REGION") {
            QPlatformWindow *handle = overlayView->handle();
            if (handle) {
                auto mouseRegion = overlayView->property("MOUSE_REGION");
                QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
                native->setWindowProperty(handle, QStringLiteral("MOUSE_REGION"), mouseRegion);
                setWaylandInputRegion(native, overlayView, mouseRegion.value<QRegion>());
            }
        }
        return false;
    }
    default:
        return false;
    }
}

void OverlayHelper::show()
{
    overlayView->show();
    ping();
}

void OverlayHelper::hide()
{
    overlayView->hide();
    ping();
}

void OverlayHelper::ping()
{
    Q_EMIT visibleChanged(overlayView->isVisible());
}
