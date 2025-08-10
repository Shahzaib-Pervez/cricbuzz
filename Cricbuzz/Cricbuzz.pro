QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = cricket-app
TEMPLATE = app
CONFIG += c++11

SOURCES += \
    loginwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    firebaseauth.cpp

HEADERS += \
    loginwidget.h \
    mainwindow.h \
    firebaseauth.h

INCLUDEPATH += .
VERSION = 1.0.0
