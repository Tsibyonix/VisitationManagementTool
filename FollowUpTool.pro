#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T09:18:31
#
#-------------------------------------------------

QT       += core gui sql concurrent network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisitationManagementTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    loaddatabase.cpp \
    aboutdialog.cpp \
    runquerydialog.cpp

HEADERS  += mainwindow.h \
    loaddatabase.h \
    aboutdialog.h \
    runquerydialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    runquerydialog.ui
