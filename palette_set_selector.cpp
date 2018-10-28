#include "palette_set_selector.h"

palette_set_selector::palette_set_selector(QWidget * parent_window,int base_x,int base_y,palette_set pals) : QWidget(parent_window)
{
    parent = parent_window;
    x = base_x;
    y = base_y;
    p = pals;
    edit_mode = true;
    color_border = NULL;
    color_border_pix.load(QString(":/img/img/palette_select_border.png"));
    current_color = 0;
    current_palette = 0;

    labels[0].setParent(parent);
    labels[0].setText(QString("UBC"));
    labels[1].setParent(parent);
    labels[1].setText(QString("Palette 0"));
    labels[2].setParent(parent);
    labels[2].setText(QString("Palette 1"));
    labels[3].setParent(parent);
    labels[3].setText(QString("Palette 2"));
    labels[4].setParent(parent);
    labels[4].setText(QString("Palette 3"));
    labels[0].setGeometry(x,y,100,25);
    labels[1].setGeometry(x+50,y,70,25);
    labels[2].setGeometry(x+160,y,70,25);
    labels[3].setGeometry(x+270,y,70,25);
    labels[4].setGeometry(x+380,y,70,25);

    labels[0].update();
    labels[1].update();
    labels[2].update();
    labels[3].update();
    labels[4].update();
    unsigned int i,j;
    for(i=0;i<13;i++){
        //connect(&(buttons[i]),SIGNAL(pressed()),this,SLOT (colorPushed()));
        connect(&(buttons[i]),SIGNAL(dualClicked(QMouseEvent*)),this,SLOT (colorPushed(QMouseEvent *)));
        buttons[i].setParent(parent);
    }
    buttons[0].setGeometry(x+5,y + 31,25,25);
    for(i=0;i<4;i++){
        for(j=1;j<4;j++){
            buttons[(i<<1) + i + j].setGeometry(x + 15 + (i*110) + (j*30),y + 31,25,25);
        }
    }
    redraw();

//    if(edit_mode){
    color_border = new QLabel(QString("TESTING"),parent);
    color_border->setPixmap(color_border_pix);
    color_border->setGeometry(x,y + 26,35,35);
    current_color = 0;
    current_palette = 0;
    QPalette pal;
    pal.setColor(QPalette::Window,Qt::transparent);
    pal.setColor(QPalette::WindowText,Qt::black);
    labels[1].setAutoFillBackground(true);
    labels[2].setAutoFillBackground(true);
    labels[3].setAutoFillBackground(true);
    labels[4].setAutoFillBackground(true);
    labels[1].setPalette(pal);
    labels[2].setPalette(pal);
    labels[3].setPalette(pal);
    labels[4].setPalette(pal);
    pal.setColor(QPalette::Window,Qt::yellow);
    labels[current_palette + 1].setPalette(pal);
    color_border->show();
    color_border->setMask(color_border_pix.createMaskFromColor(Qt::transparent,Qt::MaskInColor));
//    }

}

void palette_set_selector::redraw(){
    p.convertColors();

    buttons[0].setText(QString(convertByteToHexString(p.nes_ubg).c_str()));
    buttons[0].setAutoFillBackground(true);
    QPalette pal = buttons[0].palette();
    QColor text_color;
    if(p.ubg_color.lightness() >= 0x80){
        text_color = QColor(Qt::black);
    }
    else{
        text_color = QColor(Qt::white);
    }
    pal.setColor(QPalette::Button,p.ubg_color);
    pal.setColor(QPalette::Text,text_color);
    buttons[0].setPalette(pal);
    buttons[0].update();

    unsigned int i,j;
    for(i=0;i<4;i++){
        for(j=1;j<4;j++){
            buttons[(i<<1) + i + j].setText(QString(convertByteToHexString(p.p[i].nes_colors[j]).c_str()));
            buttons[(i<<1) + i + j].setAutoFillBackground(true);
            pal = buttons[(i<<1) + i + j].palette();
            if(p.p[i].p[j].lightness() >= 0x80){
                text_color = QColor(Qt::black);
            }
            else{
                text_color = QColor(Qt::white);
            }
            pal.setColor(QPalette::Button,p.p[i].p[j]);
            pal.setColor(QPalette::Text,text_color);
            buttons[(i<<1) + i + j].setPalette(pal);
            buttons[(i<<1) + i + j].update();
        }
    }
}

void palette_set_selector::setPalette(palette_set pal){
    p = pal;
    redraw();
}

void palette_set_selector::colorPushed(QMouseEvent * event){
    QPushButton * b = qobject_cast<QPushButton *>(sender());
    if(event->button() == Qt::RightButton){
        edit_mode = false;
    }
    else{
        edit_mode = true;
    }
    unsigned int i;
    for(i=0;i<13;i++){
        if(b == &(buttons[i])) break;
    }
    if(i<13){
        if(edit_mode){
            if(color_border){
                color_border->setPixmap(color_border_pix);
            }
            else{
                color_border = new QLabel(QString("TESTING"),parent);
                color_border->setPixmap(color_border_pix);
            }
            if(!i){
                color_border->setGeometry(x,y + 26,35,35);
                current_color = 0;
            }
            else{
                i--;
                color_border->setGeometry(x + 40 + ((i/3)*110) + ((i%3)*30),y + 26,35,35);
                current_palette = i/3;
                current_color = i%3 + 1;
                QPalette pal;
                pal.setColor(QPalette::Window,Qt::transparent);
                pal.setColor(QPalette::WindowText,Qt::black);
                labels[1].setAutoFillBackground(true);
                labels[2].setAutoFillBackground(true);
                labels[3].setAutoFillBackground(true);
                labels[4].setAutoFillBackground(true);
                labels[1].setPalette(pal);
                labels[2].setPalette(pal);
                labels[3].setPalette(pal);
                labels[4].setPalette(pal);

                pal.setColor(QPalette::Window,Qt::yellow);
                labels[current_palette + 1].setPalette(pal);
            }
            color_border->show();
        }
        else{
            color_picker picker;
            if(!i){
                picker.setDefaultColor(p.nes_ubg);
                p.nes_ubg = picker.exec();
            }
            else{
                i--;
                picker.setDefaultColor(p.p[i/3].nes_colors[(i%3)+1]);
                p.p[i/3].nes_colors[(i%3)+1] = picker.exec();;
            }
            redraw();
        }
    }
    emit color_changed(current_palette,current_color);
}

palette_set palette_set_selector::getPalette(){
    return p;
}

palette_set_selector::~palette_set_selector(){
    delete color_border;
}

void palette_set_selector::setSelectedPalette(uint8_t selection){
    current_palette = selection;
    if(color_border){
        color_border->setPixmap(color_border_pix);
    }
    else{
        color_border = new QLabel(QString("T"),parent);
        color_border->setPixmap(color_border_pix);
    }
    if(!current_color){
        color_border->setGeometry(x,y + 26,35,35);
    }
    else{
        color_border->setGeometry(x + 40 + ((current_palette)*110) + ((current_color-1)*30),y + 26,35,35);
    }
    QPalette pal;
    pal.setColor(QPalette::Window,Qt::transparent);
    pal.setColor(QPalette::WindowText,Qt::black);
    labels[1].setAutoFillBackground(true);
    labels[2].setAutoFillBackground(true);
    labels[3].setAutoFillBackground(true);
    labels[4].setAutoFillBackground(true);
    labels[1].setPalette(pal);
    labels[2].setPalette(pal);
    labels[3].setPalette(pal);
    labels[4].setPalette(pal);

    pal.setColor(QPalette::Window,Qt::yellow);
    labels[current_palette + 1].setPalette(pal);
    color_border->show();
}

void palette_set_selector::moveEvent(QMoveEvent *event){
    //x = event->pos().x();
    //y = event->pos().y();
    int offset_x = x + event->pos().x();
    int offset_y = y + event->pos().y();

    labels[0].move(offset_x,offset_y);
    labels[1].move(offset_x+50,offset_y);
    labels[2].move(offset_x+160,offset_y);
    labels[3].move(offset_x+270,offset_y);
    labels[4].move(offset_x+380,offset_y);

    buttons[0].move(offset_x+5,offset_y + 31);
    unsigned int i,j;
    for(i=0;i<4;i++){
        for(j=1;j<4;j++){
            buttons[(i<<1) + i + j].move(offset_x + 15 + (i*110) + (j*30),offset_y + 31);
        }
    }
    if(!current_color){
        color_border->move(offset_x,offset_y + 26);
    }
    else{
        color_border->move(offset_x + 40 + ((current_palette)*110) + ((current_color-1)*30),offset_y + 26);
    }
}
