#ifndef OVERLAYHELPER_H
#define OVERLAYHELPER_H

#include <QObject>
#include <QQuickView>

class OverlayHelper : public QObject
{
    Q_OBJECT

public:
    explicit OverlayHelper(QObject *parent = 0);
    bool eventFilter(QObject *object, QEvent *event);

private:
    QQuickView *overlayView;

};

#endif // OVERLAYHELPER_H
