#-------------------------------------------------
#
# Project created by QtCreator 2015-09-27T13:54:02
#
#-------------------------------------------------

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = frontend
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    device/vnadevice.cpp \
    device/vnacommands.cpp \
    qcustomplot/qcustomplot.cpp \
    pugixml/pugixml.cpp \
    device/vnautils.cpp \
    dialogs/dialogselectfrequency.cpp \
    project.cpp \
    device/vnasampling.cpp \
    device/vnasamplingdata.cpp \
    dialogs/dialogcalibration.cpp \
    dialogs/scandock.cpp \
    dialogs/selectcalibration.cpp \
    calibrationsolt.cpp \
    dialogs/selectgraph.cpp \
    mainwindow_graph.cpp \
    dialogs/lcinfowidget.cpp \
    ../shared_code/7bit.c \
    dialogs/updateflashdialog.cpp \
    ../shared_code/md5.c \
    console/consolewriteflash.cpp \
    console/console_utils.cpp \
    device/vnaupdateflash.cpp \
    console/consoleserial.cpp \
    device/vnareceivecalibration.cpp

HEADERS  += mainwindow.h \
    device/vnadevice.h \
    device/vnacommands.h \
    qcustomplot/qcustomplot.h \
    pugixml/pugiconfig.hpp \
    pugixml/pugixml.hpp \
    device/vnautils.h \
    dialogs/dialogselectfrequency.h \
    project.h \
    device/vnasampling.h \
    device/vnasamplingdata.h \
    dialogs/dialogcalibration.h \
    dialogs/scandock.h \
    dialogs/selectcalibration.h \
    calibrationsolt.h \
    dialogs/selectgraph.h \
    dialogs/lcinfowidget.h \
    ../shared_code/7bit.h \
    dialogs/updateflashdialog.h \
    ../shared_code/md5.h \
    console/consolewriteflash.h \
    console/console_utils.h \
    device/vnaupdateflash.h \
    console/consoleserial.h \
    device/vnareceivecalibration.h

FORMS    +=

CONFIG += c++11

RESOURCES += \
    icons.qrc

DISTFILES += \
    icons/file_send.png

INCLUDEPATH += ../shared_code
