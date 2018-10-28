#include "spriteview.h"

spriteView::spriteView(QWidget * parent) : QGraphicsView(parent)
{

}

void spriteView::mousePressEvent(QMouseEvent * event){
    emit tile_pressed(event);
    QGraphicsView::mousePressEvent(event);
}

void spriteView::mouseMoveEvent(QMouseEvent * event){
    if(event->buttons() == Qt::LeftButton){
        emit tile_pressed(event);
    }
    else{
        emit hover_event(event);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void spriteView::mouseReleaseEvent(QMouseEvent *event){
    emit tile_released();
    QGraphicsView::mouseReleaseEvent(event);
}
