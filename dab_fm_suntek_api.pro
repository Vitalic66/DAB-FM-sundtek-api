#-------------------------------------------------
#
# Project created by QtCreator 2020-05-10T08:35:08
#
#-------------------------------------------------

QT       += core gui concurrent quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DAB_FM_sundtek
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
        dialog.cpp \
    tune.cpp \
    scan.cpp \
    mute.cpp \
    fm_rds.cpp \
    file.cpp \
    globals.cpp \
    dabdata.cpp \
    dabstrength.cpp \
    charsets.cpp \
    tools.cpp \
    mot_manager.cpp \
    mot_image_provider.cpp \
    pad_decoder.cpp \
    dabplus_decoder.cpp \
    dab_mot.cpp

HEADERS += \
    dialog.h\
    tune.h \
    scan.h \
    mute.h \
    fm_rds.h \
    file.h \
    globals.h \
    dabdata.h \
    dabstrength.h \
    charsets.h \
    tools.h \
    mot_manager.h \
    mot_image_provider.h \
    pad_decoder.h \
    dabplus_decoder.h \
    subchannel_sink.h \
    dab_mot.h

FORMS += \
        dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../opt/lib/release/ -lmcsimple
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../opt/lib/debug/ -lmcsimple
else:unix: LIBS += -L$$PWD/../../../../../../opt/lib/ -lmcsimple

INCLUDEPATH += $$PWD/../../../../../../opt/include
DEPENDPATH += $$PWD/../../../../../../opt/include
