#ifndef SPRITEVIEW_H
#define SPRITEVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>

class spriteView : public QGraphicsView
{
    Q_OBJECT
public:
    spriteView(QWidget *parent);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void tile_pressed(QMouseEvent *);
    void tile_released();
    void hover_event(QMouseEvent *);
    void customContextMenuRequested(const QPoint &);
};

#endif // SPRITEVIEW_H
