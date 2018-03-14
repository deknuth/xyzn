TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += \
    curvature.c \
    decision.c \
    follow.c \
    public.c \
    gps.c \
    hal.c \
    core.c \
    print.c \
    kmp.c \
    coordinates.c \
    track.c \
    control.c \
    lidar.c \
    src/control.c \
    src/coordinates.c \
    src/core.c \
    src/curvature.c \
    src/decision.c \
    src/follow.c \
    src/gps.c \
    src/hal.c \
    src/kmp.c \
    src/lidar.c \
    src/log.c \
    src/print.c \
    src/public.c \
    src/track.c \
    src/socket.c \
    src/pool.c \
    src/crc.c \
    src/rtk.c

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    gps.h \
    core.h \
    hal.h \
    public.h \
    decision.h \
    kmp.h \
    print.h \
    coordinates.h \
    can.h \
    follow.h \
    control.h \
    lidar.h \
    inc/control.h \
    inc/coordinates.h \
    inc/core.h \
    inc/decision.h \
    inc/follow.h \
    inc/gps.h \
    inc/hal.h \
    inc/kmp.h \
    inc/lidar.h \
    inc/log.h \
    inc/print.h \
    inc/public.h \
    inc/can.h \
    inc/socket.h \
    inc/pool.h \
    inc/crc.h
