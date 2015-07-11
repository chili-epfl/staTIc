TEMPLATE = app

QT += qml quick widgets 3dcore 3drenderer 3dinput
CONFIG += c++11
SOURCES += main.cpp \
    structureitemmodel.cpp \
    controller.cpp \
    joint.cpp \
    member.cpp \
    abstractstaticsmodule.cpp \
    twodimensionalstaticsmodule.cpp \
    abstractelement.cpp \
    force.cpp
HEADERS += \
    structureitemmodel.h \
    controller.h \
    joint.h \
    member.h \
    abstractstaticsmodule.h \
    twodimensionalstaticsmodule.h \
    abstractelement.h \
    force.h


RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc
LIBS += -lchilitags

android {
    #INCLUDEPATH += $(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/include
    INCLUDEPATH += /home/chili/opencv/platforms/build-android/install/sdk/native/jni/include
    #INCLUDEPATH += /home/chili/chilitags/build-android/install/include

    #LIBS += -L$(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/lib
    LIBS += -L/home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/
    LIBS += -L/home/chili/chilitags/build-android/install/lib

}
!android{
    INCLUDEPATH += /home/chili/opencv/build-linux/install/include
    #INCLUDEPATH += /home/chili/chilitags/build-linux/install/include
    LIBS += -L/home/chili/opencv/build-linux/install/lib
    LIBS += -L/home/chili/chilitags/build-linux/install/lib
}
