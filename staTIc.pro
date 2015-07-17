TEMPLATE = app

QT += qml quick widgets 3dcore 3drenderer 3dinput multimedia

CONFIG += c++11
SOURCES += main.cpp \
    structureitemmodel.cpp \
    statics/twodimensionalstaticsmodule.cpp \
    statics/joint.cpp \
    statics/force.cpp \
    statics/abstractstaticsmodule.cpp \
    statics/abstractelement.cpp \
    controller.cpp \
    statics/beam.cpp
HEADERS += \
    structureitemmodel.h \
    statics/twodimensionalstaticsmodule.h \
    statics/joint.h \
    statics/force.h \
    statics/abstractstaticsmodule.h \
    statics/abstractelement.h \
    controller.h \
    statics/beam.h


RESOURCES += qml.qrc

INCLUDEPATH+= /home/chili/QTProjects/qml-chilitags/src



# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

LIBS += -lopencv_core
#-lopencv_highgui -lopencv_imgproc
#-lopencv_video -lopencv_calib3d


android {
    #INCLUDEPATH += $(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/include
    INCLUDEPATH += /home/chili/opencv/platforms/build-android/install/sdk/native/jni/include
    INCLUDEPATH += /home/chili/chilitags/build-android/install/include

    #LIBS += -L$(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/lib
    LIBS += -L/home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/
    LIBS += -L/home/chili/chilitags/build-android/install/lib

}
!android{
    INCLUDEPATH += /home/chili/opencv/build-linux/install/include
    INCLUDEPATH += /home/chili/chilitags/build-linux/install/include
    LIBS += -L/home/chili/opencv/build-linux/install/lib
    LIBS += -L/home/chili/chilitags/build-linux/install/lib
}
