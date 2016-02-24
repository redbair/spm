#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T11:43:33
#
#-------------------------------------------------

QT       += core gui webkitwidgets widgets webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$PWD

TARGET = SPM
TEMPLATE = app


SOURCES += main.cpp\
        menu.cpp

HEADERS  += menu.h

FORMS    += menu.ui \
    settings.ui \
    info.ui

CONFIG += c++11

RESOURCES += \
    spmres.qrc

TRANSLATIONS += SPM_ru.ts

DISTFILES += \
    SPM_ru.ts \
    SPM_ru.qm
