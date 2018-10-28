#include "dualbutton.h"

dualButton::dualButton(QWidget *parent) : QPushButton(parent)
{

}

void dualButton::mousePressEvent(QMouseEvent *event){
    emit dualClicked(event);
    QPushButton::mousePressEvent(event);
}
