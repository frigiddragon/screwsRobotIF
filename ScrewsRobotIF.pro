#-------------------------------------------------
#
# Project created by QtCreator 2018-12-18T12:00:02
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScrewsRobotIF
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    cameraobject.cpp \
    cameraobject.cpp \
    main.cpp \
    mainwindow.cpp \
    Comm.cpp \
    line2Dup.cpp

HEADERS += \
        mainwindow.h \
    cameraobject.h \
    robot/Axis.h \
    robot/DataStruct.h \
    robot/ErrCode.h \
    robot/GeneralRobot.h \
    robot/Link.h \
    robot/LinkObject.h \
    robot/Parameter.h \
    robot/RobotModuleServices.h \
    cameraobject.h \
    mainwindow.h \
    Comm.h \
    line2Dup.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../Development/OpenCV3.3.0/x64/vc14/lib/ -lopencv_world330
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../Development/OpenCV3.3.0/x64/vc14/lib/ -lopencv_world330d
else:unix: LIBS += -L$$PWD/../../../../../../Development/OpenCV3.3.0/x64/vc14/lib/ -lopencv_world330

INCLUDEPATH += $$PWD/../../../../../../Development/OpenCV3.3.0/include
DEPENDPATH += $$PWD/../../../../../../Development/OpenCV3.3.0/include

win32: LIBS += -L$$PWD/'../../../../../../Program Files/Basler/pylon 5/Development/lib/x64/' -lGCBase_MD_VC141_v3_1_Basler_pylon_v5_1

INCLUDEPATH += $$PWD/'../../../../../../Program Files/Basler/pylon 5/Development/include'
DEPENDPATH += $$PWD/'../../../../../../Program Files/Basler/pylon 5/Development/include'

DISTFILES +=
