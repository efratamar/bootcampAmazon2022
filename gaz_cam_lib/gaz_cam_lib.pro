QT -= gui

TEMPLATE = lib
DEFINES += GAZ_CAM_LIB_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += -L /usr/lib/x86_64-linux-gnu  -lavcodec -lavformat -lavutil -lavcodec -ljpeg
SOURCES += \
    encode.c \
    gaz_cam_lib.c \
    queue.c

HEADERS += \
    encode.h \
    gaz_cam_lib_global.h \
    gaz_cam_lib.h \
    queue.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
