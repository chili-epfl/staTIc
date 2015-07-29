TEMPLATE = app

QT += qml quick widgets 3dcore 3drenderer 3dinput multimedia

CONFIG += c++11
SOURCES += main.cpp \
    structureitemmodel.cpp \
    statics/two_dimentions/twodimensionalstaticsmodule.cpp \
    statics/elements/joint.cpp \
    statics/elements/force.cpp \
    statics/abstractstaticsmodule.cpp \
    statics/elements/abstractelement.cpp \
    statics/elements/beam.cpp \
    materialsetter.cpp \
    statics/abstracteventhandler.cpp \
    statics/two_dimentions/twodimentionaleventhandler.cpp \
    statics/abstractelementviewmodel.cpp \
    statics/elements/beamvm.cpp \
    statics/elements/forcevm.cpp \
    statics/elements/jointvm.cpp \
    staticshelper.cpp
HEADERS += \
    structureitemmodel.h \
    statics/two_dimentions/twodimensionalstaticsmodule.h \
    statics/elements/joint.h \
    statics/elements/force.h \
    statics/abstractstaticsmodule.h \
    statics/elements/abstractelement.h \
    statics/elements/beam.h \
    materialsetter.h \
    statics/abstracteventhandler.h \
    statics/two_dimentions/twodimentionaleventhandler.h \
    statics/abstractelementviewmodel.h \
    statics/elements/beamvm.h \
    statics/elements/forcevm.h \
    statics/elements/jointvm.h \
    staticshelper.h

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
