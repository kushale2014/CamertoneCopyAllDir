#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T11:51:53
#
#-------------------------------------------------

QT       += core gui widgets sql

TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        datamodel_cc.cpp \
    copydialog.cpp \
    dialoginfo.cpp \
    selectdir.cpp

HEADERS  += mainwindow.h \
         datamodel_cc.h \
         connection.h \
    copydialog.h \
    qzipreader_p.h \
    qzipwriter_p.h \
    dialoginfo.h \
    selectdir.h


FORMS    += mainwindow.ui \
    copydialog.ui \
    dialoginfo.ui \
    selectdir.ui

RESOURCES += \
    resource.qrc

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build
UI_DIR = build

DESTDIR  = ../CamertoneCopyAllDir
RC_ICONS = src/CanDictophone.ico
