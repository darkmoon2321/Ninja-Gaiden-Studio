#ifndef DUALBUTTON_H
#define DUALBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>

class dualButton : public QPushButton
{
    Q_OBJECT
public:
    explicit dualButton(QWidget *parent = 0);

private slots:
    void mousePressEvent(QMouseEvent *event);
signals:
    void dualClicked(QMouseEvent * event);
public slots:

};

#endif // DUALBUTTON_H
