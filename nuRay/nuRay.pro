QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 
LIBS += -fopenmp


QMAKE_CXXFLAGS -= -O
QMAKE_CXXFLAGS -= -O1
QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS -= -O3
QMAKE_CXXFLAGS += -O3

QMAKE_CXXFLAGS += -fopenmp

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bvh.cpp \
    bvhnode.cpp \
    camera.cpp \
    glwidget.cpp \
    lightsampler.cpp \
    loader.cpp \
    main.cpp \
    matblinnphong.cpp \
    matggx.cpp \
    matggxrefl.cpp \
    matglass.cpp \
    matlambert.cpp \
    matlight.cpp \
    renderer.cpp \
    texture.cpp \
    triangle.cpp \
    utils.cpp \
    vec3.cpp \
    widget.cpp

HEADERS += \
    bvh.h \
    bvhnode.h \
    camera.h \
    glwidget.h \
    lightsampler.h \
    loader.h \
    matblinnphong.h \
    material.h \
    matggx.h \
    matggxrefl.h \
    matglass.h \
    matlambert.h \
    matlight.h \
    renderer.h \
    texture.h \
    triangle.h \
    utils.h \
    vec3.h \
    widget.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
