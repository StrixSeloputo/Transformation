#-------------------------------------------------
#
# Project created by QtCreator 2015-11-29T13:34:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Transformation
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        matrixoftransformation.cpp \
        picture.cpp \
        point.cpp \
        image.cpp \
        dialogaboutdot.cpp \
        dialogabouttestmatrix.cpp \
    helpfunction.cpp

HEADERS  += mainwindow.h \
        matrixoftransformation.h \
        picture.h \
        point.h \
        image.h \
        dialogaboutdot.h \
        dialogabouttestmatrix.h \
    helpfunction.h

FORMS    += mainwindow.ui \
        image.ui \
        dialogaboutdot.ui \
        dialogabouttestmatrix.ui
