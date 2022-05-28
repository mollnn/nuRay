QT       += core gui opengl 

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console
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
    ../src/cmd/cmd.cpp \
    ../src/main.cpp \
    ../src/accel/bvh.cpp \
    ../src/accel/bvhnode.cpp \
    ../src/accel/kdtree.cpp \
    ../src/material/matblinnphong.cpp \
    ../src/material/material.cpp \
    ../src/material/matggx.cpp \
    ../src/material/matggxrefl.cpp \
    ../src/material/matglass.cpp \
    ../src/material/matlambert.cpp \
    ../src/material/matlight.cpp \
    ../src/material/matmirror.cpp \
    ../src/math/mat4.cpp \
    ../src/math/vec3.cpp \
    ../src/math/vec4.cpp \
    ../src/renderer/renderer.cpp \
    ../src/renderer/rendererpt.cpp \
    ../src/sampler/envmapsampler.cpp \
    ../src/sampler/lightsampler.cpp \
    ../src/sampler/sampler.cpp \
    ../src/sampler/samplerstd.cpp \
    ../src/scene/camera.cpp \
    ../src/scene/envmap.cpp \
    ../src/scene/loader.cpp \
    ../src/scene/texture.cpp \
    ../src/scene/triangle.cpp \
    ../src/utils/config.cpp \
    ../src/utils/utils.cpp


HEADERS += \
    ../src/cmd/cmd.h \
    ../src/accel/bvh.h \
    ../src/accel/bvhnode.h \
    ../src/accel/kdtree.h \
    ../src/material/matblinnphong.h \
    ../src/material/material.h \
    ../src/material/matggx.h \
    ../src/material/matggxrefl.h \
    ../src/material/matglass.h \
    ../src/material/matlambert.h \
    ../src/material/matlight.h \
    ../src/material/matmirror.h \
    ../src/math/mat4.h \
    ../src/math/vec3.h \
    ../src/math/vec4.h \
    ../src/renderer/renderer.h \
    ../src/renderer/rendererpt.h \
    ../src/sampler/envmapsampler.h \
    ../src/sampler/lightsampler.h \
    ../src/sampler/sampler.h \
    ../src/sampler/samplerstd.h \
    ../src/scene/camera.h \
    ../src/scene/envmap.h \
    ../src/scene/loader.h \
    ../src/scene/texture.h \
    ../src/scene/triangle.h \
    ../src/utils/config.h \
    ../src/utils/utils.h


FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../src/gui/shader.frag \
    ../src/gui/shader.vert
