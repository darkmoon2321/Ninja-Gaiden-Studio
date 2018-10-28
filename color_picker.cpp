#include "color_picker.h"

color_picker::color_picker(QWidget *parent) : QDialog(parent)
{
    unsigned int i,j;
    setWindowTitle(QString("Available Colors"));
    QPalette pal;
    QColor text_color;
    setGeometry(0,0,400,100);
    for(i=0;i<0x40;i++){
        connect(&(buttons[i]),SIGNAL (released()),this,SLOT (buttonPushed()));
        buttons[i].setParent(this);
        buttons[i].setText(QString(convertByteToHexString(i).c_str()));
    }
    for(i=0;i<4;i++){
        for(j=0;j<0x10;j++){
            buttons[(i<<4) + j].setGeometry(j*25,i*25,25,25);

            buttons[(i<<4) + j].setAutoFillBackground(true);
            pal = buttons[(i<<4) + j].palette();
            if(getColor((i<<4) + j).lightness() >= 0x80){
                text_color = QColor(Qt::black);
            }
            else{
                text_color = QColor(Qt::white);
            }
            pal.setColor(QPalette::Button,getColor((i<<4) + j));
            pal.setColor(QPalette::Text,text_color);
            buttons[(i<<4) + j].setPalette(pal);
            buttons[(i<<4) + j].update();
        }
    }
}

void color_picker::buttonPushed(){
    QPushButton * b = qobject_cast<QPushButton *>(sender());
    unsigned int i;
    for(i=0;i<0x40;i++){
        if(b == &buttons[i])break;
    }
    selection = (i<0x40)? i : 0;
    this->done(selection);
}

void color_picker::setDefaultColor(uint8_t init_value){
    selection = init_value;
}

void color_picker::closeEvent(QCloseEvent * event){
    this->done(selection);
}
