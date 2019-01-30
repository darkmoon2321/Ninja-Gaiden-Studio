#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <stdint.h>
#include "data_handler.h"
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_save_button_clicked();

    void on_importROM_button_clicked();

    void on_importScene_button_clicked();

    void on_importBG_button_clicked();

    void on_importSprite_button_clicked();

    void on_export_button_clicked();

    void on_centralWidget_destroyed();

private:
    uint8_t * data;
    uint32_t file_size;
    Ui::MainWindow *ui;
    data_handler d_handler;
    std::string file_location;
    QLabel status_message;

    void updateTextTable(QString);
};

#endif // MAINWINDOW_H
