QT += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = TrackGuard
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    network/api_client.cpp

HEADERS += \
    mainwindow.h \
    network/api_client.h