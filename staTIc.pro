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
    statics/viewModels/abstractelementviewmodel.cpp \
    statics/viewModels/beamvm.cpp \
    statics/viewModels/forcevm.cpp \
    statics/viewModels/jointvm.cpp \
    staticshelper.cpp \
    statics/viewModels/jointvmitemmodel.cpp \
    physicssetter.cpp \
    Tools/ConcentratedForce/concentratedforce.cpp

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
    statics/viewModels/abstractelementviewmodel.h \
    statics/viewModels/beamvm.h \
    statics/viewModels/forcevm.h \
    statics/viewModels/jointvm.h \
    staticshelper.h \
    statics/viewModels/jointvmitemmodel.h \
    physicssetter.h \
    Tools/ConcentratedForce/concentratedforce.h

RESOURCES += qml.qrc \
    Tools/tools.qrc

INCLUDEPATH+= /home/chili/QTProjects/qml-chilitags/src

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

LIBS += -lopencv_core
#-lopencv_highgui -lopencv_imgproc
#-lopencv_video -lopencv_calib3d
LIBS += -lLinearMath -lBulletDynamics -lBulletCollision -lBulletSoftBody

LIBS += -lQtPhysicsUnofficialFrontend

android {
    #INCLUDEPATH += $(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/include
    INCLUDEPATH += /home/chili/opencv/platforms/build-android/install/sdk/native/jni/include
    INCLUDEPATH += /home/chili/chilitags/build-android/install/include

    #LIBS += -L$(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/lib
    LIBS += -L/home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/
    LIBS += -L/home/chili/chilitags/build-android/install/lib

    ANDROID_EXTRA_LIBS = \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_core.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_imgproc.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_imgcodecs.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_flann.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_ml.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_features2d.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_calib3d.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_video.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_videoio.so \
        /home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/libopencv_highgui.so \
        /home/chili/chilitags/build-android/install/lib/libchilitags.so \
        $$[QT_INSTALL_QML]/Chilitags/libchilitagsplugin.so \
        /home/chili/android-ndk-r10d/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/user/lib/libassimp.so


    ANDROID_PERMISSIONS += \
        android.permission.CAMERA

    ANDROID_FEATURES += \
        android.hardware.camera


}
!android{
    INCLUDEPATH += /home/chili/opencv/build-linux/install/include
    INCLUDEPATH += /home/chili/chilitags/build-linux/install/include
    LIBS += -L/home/chili/opencv/build-linux/install/lib
    LIBS += -L/home/chili/chilitags/build-linux/install/lib
    LIBS += -L/home/chili/Downloads/bullet3-2.83.5/build-linux/install/lib
}
