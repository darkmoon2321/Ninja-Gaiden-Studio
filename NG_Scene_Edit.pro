#-------------------------------------------------
#
# Project created by QtCreator 2018-04-08T12:11:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NG_Scene_Edit
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    data_handler.cpp \
    script_command.cpp \
    cmd_bgset.cpp \
    cmd_spriteset.cpp \
    cmd_text.cpp \
    cmd_wait.cpp \
    cmd_flash.cpp \
    cmd_sync.cpp \
    cmd_scrollsetup.cpp \
    cmd_fade.cpp \
    cmd_mirroring.cpp \
    cmd_music.cpp \
    cmd_pputransmit.cpp \
    cmd_bggfxpage.cpp \
    cmd_movesprite.cpp \
    cmd_bgcol_mask.cpp \
    cmd_paletteset.cpp \
    cmd_earthquake.cpp \
    cmd_blackout.cpp \
    cmd_setup_ranking_text.cpp \
    cmd_bar_or_reset.cpp \
    cmd_setup_hard_mode.cpp \
    cmd_setup_boss_rush_text.cpp \
    cmd_end_scene_during_popcorn_viewer.cpp \
    cmd_halt_sprite1.cpp \
    cmd_unknown.cpp \
    pixel.cpp \
    data_access_functions.cpp \
    cmd_scrollspeed.cpp \
    importsceneselectdialog.cpp \
    importbgdialog.cpp \
    palette_set_selector.cpp \
    color_picker.cpp \
    chritem.cpp \
    tileedititem.cpp \
    arrangementedititem.cpp \
    dualbutton.cpp \
    resizebgdialog.cpp \
    editspritedialog.cpp \
    spriteedititem.cpp \
    spriteview.cpp \
    selectchrdialog.cpp

HEADERS  += mainwindow.h \
    data_handler.h \
    script_command.h \
    cmd_bgset.h \
    cmd_spriteset.h \
    cmd_text.h \
    cmd_wait.h \
    cmd_flash.h \
    cmd_sync.h \
    cmd_scrollsetup.h \
    cmd_fade.h \
    cmd_mirroring.h \
    cmd_music.h \
    cmd_pputransmit.h \
    cmd_bggfxpage.h \
    cmd_movesprite.h \
    cmd_bgcol_mask.h \
    cmd_paletteset.h \
    cmd_earthquake.h \
    cmd_blackout.h \
    cmd_setup_ranking_text.h \
    cmd_bar_or_reset.h \
    cmd_setup_hard_mode.h \
    cmd_setup_boss_rush_text.h \
    cmd_end_scene_during_popcorn_viewer.h \
    cmd_halt_sprite1.h \
    cmd_unknown.h \
    data_types.h \
    pixel.h \
    cmd_scrollspeed.h \
    importsceneselectdialog.h \
    importbgdialog.h \
    palette_set_selector.h \
    color_picker.h \
    chritem.h \
    tileedititem.h \
    arrangementedititem.h \
    dualbutton.h \
    resizebgdialog.h \
    editspritedialog.h \
    spriteedititem.h \
    spriteview.h \
    selectchrdialog.h

FORMS    += mainwindow.ui \
    importsceneselectdialog.ui \
    importbgdialog.ui \
    resizebgdialog.ui \
    editspritedialog.ui \
    selectchrdialog.ui

RESOURCES += \
    resources.qrc
