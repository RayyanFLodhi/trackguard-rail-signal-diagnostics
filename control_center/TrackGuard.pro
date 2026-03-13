QT += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrackGuard
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           network/api_client.cpp \
           models/device.cpp \
           widgets/signal_widget.cpp

HEADERS += mainwindow.h \
           network/api_client.h \
           models/device.h \
           widgets/signal_widget.h

FORMS += mainwindow.ui
