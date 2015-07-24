#-------------------------------------------------
#
# Project created by QtCreator 2015-07-24T16:48:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-jetson-zed-opencv-nosdk

TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += \
    -lopencv_core \
    -lopencv_video \
    -lopencv_gpu \
    -lopencv_imgproc \
    -lopencv_highgui \
    -lopencv_calib3d
