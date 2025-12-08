QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cricket-app
TEMPLATE = app

# C++11 or later required
CONFIG += c++11

# Your Firebase Project Configuration
# Project name: cricket-app
# Project ID: cricket-app-1a79e
# Project number: 417569085744
# Web API key: AIzaSyAkWEZyrIT_xpw5dA1fmAaNBnaXO-Dnpog

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    loginwidget.cpp \
    firebaseauth.cpp \
    dashboard.cpp \
    matchcard.cpp \
    matchdetails.cpp \
    recent.cpp \
    upcoming.cpp

HEADERS += \
    mainwindow.h \
    loginwidget.h \
    firebaseauth.h \
    dashboard.h \
    matchcard.h \
    matchdetails.h \
    matchinfo.h \
    recent.h \
    upcoming.h

# Optional: For better organization
INCLUDEPATH += .

# Application version
VERSION = 1.0.0

# Application icon (optional)
# win32:RC_ICONS = app.ico
# macx:ICON = app.icns
