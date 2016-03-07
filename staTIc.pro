TEMPLATE = app

QT += qml quick widgets 3dcore 3drender 3dinput multimedia network

CONFIG += c++11
SOURCES += main.cpp \
    #statics/two_dimentions/twodimensionalstaticsmodule.cpp \
    statics/elements/joint.cpp \
    #statics/elements/force.cpp \
    statics/abstractstaticsmodule.cpp \
    statics/elements/abstractelement.cpp \
    statics/elements/beam.cpp \
    #materialsetter.cpp \
    #statics/two_dimentions/twodimentionaleventhandler.cpp \
    statics/viewModels/abstractelementviewmodel.cpp \
    statics/viewModels/beamvm.cpp \
    #statics/viewModels/forcevm.cpp \
    statics/viewModels/jointvm.cpp \
    #staticshelper.cpp \
    #statics/viewModels/jointvmitemmodel.cpp \
    #physicssetter.cpp \
    Tools/ConcentratedForce/concentratedforce.cpp \
    statics/frame3dd/coordtrans.c \
    statics/frame3dd/eig.c \
    statics/frame3dd/frame3dd.c \
    statics/frame3dd/HPGmatrix.c \
    statics/frame3dd/NRutil.c \
    statics/frame3dd/frame3ddkernel.cpp \
    statics/elements/nodeload.cpp \
    statics/elements/uniformlydistributedload.cpp \
    statics/abstractvmmanager.cpp \
    statics/frame3dd/frame3ddvmmanager.cpp \
    #qmlenginefactory.cpp \
    statics/elements/interiorpointload.cpp \
    Tools/Scaffold/scaffold.cpp \
    Tools/BeamRemover/beamremover.cpp \
    scenariolistmodel.cpp \
    #statics/elements/force.cpp \
    AR/backgroundsubtraction.cpp \
    AR/backgroundsubtractionfilter.cpp \
    AR/deformingbeammesh.cpp \
    materialsmanager.cpp \
    statics/elements/trapezoidalforce.cpp \
    statics/viewModels/trapezoidalforcevm.cpp \
    warehouse3d.cpp \
    instantiator43dentity.cpp \
    resourcesfetcher.cpp \
    AR/arucodetector.cpp \
    AR/arucodetectorthread.cpp \
    AR/arucoobject.cpp


HEADERS += \
    #statics/two_dimentions/twodimensionalstaticsmodule.h \
    statics/elements/joint.h \
    #statics/elements/force.h \
    statics/abstractstaticsmodule.h \
    statics/elements/abstractelement.h \
    statics/elements/beam.h \
    #materialsetter.h \
    #statics/two_dimentions/twodimentionaleventhandler.h \
    statics/viewModels/abstractelementviewmodel.h \
    statics/viewModels/beamvm.h \
    #statics/viewModels/forcevm.h \
    statics/viewModels/jointvm.h \
    #staticshelper.h \
    #statics/viewModels/jointvmitemmodel.h \
    #physicssetter.h \
    Tools/ConcentratedForce/concentratedforce.h \
    statics/frame3dd/microstran/config.h \
    statics/frame3dd/microstran/vec3.h \
    statics/frame3dd/common.h \
    statics/frame3dd/coordtrans.h \
    statics/frame3dd/eig.h \
    statics/frame3dd/frame3dd.h \
    statics/frame3dd/HPGmatrix.h \
    statics/frame3dd/NRutil.h \
    statics/frame3dd/frame3ddkernel.h \
    statics/elements/nodeload.h \
    statics/elements/uniformlydistributedload.h \
    statics/abstractvmmanager.h \
    statics/frame3dd/frame3ddvmmanager.h \
    #qmlenginefactory.h \
    statics/elements/interiorpointload.h \
    Tools/Scaffold/scaffold.h \
    Tools/BeamRemover/beamremover.h \
    scenariolistmodel.h \
    AR/backgroundsubtraction.h \
    AR/backgroundsubtractionfilter.h \
    AR/deformingbeammesh.h \
    materialsmanager.h \
    statics/elements/trapezoidalforce.h \
    statics/viewModels/trapezoidalforcevm.h \
    warehouse3d.h \
    instantiator43dentity.h \
    resourcesfetcher.h \
    static_global.h \
    AR/arucodetector.h \
    AR/arucodetectorthread.h \
    AR/arucoobject.h


RESOURCES += qml.qrc

INCLUDEPATH+= /home/chili/QTProjects/qml-chilitags/src $$[QT_INSTALL_HEADERS]/QtPhysicsUnofficial

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

LIBS += -lopencv_core -lopencv_video -lopencv_imgproc -lquazip -lz -lopencv_aruco -lopencv_calib3d -lopencv_highgui -lopencv_imgcodecs
#-lopencv_highgui -lopencv_imgproc
#-lopencv_video -lopencv_calib3d
LIBS += -lLinearMath -lBulletDynamics -lBulletCollision -lBulletSoftBody

LIBS += -lQtPhysicsUnofficial

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

DISTFILES += \
    Resources/Scenarios/Model1/Model1.dae \
    Resources/Scenarios/Model2/Model2.dae \
    Resources/Scenarios/Model1/Model1.png \
    Resources/Scenarios/Model2/Model2.png \
    Resources/Scenarios/Model1/Model1.structure \
    Resources/Scenarios/Model2/Model2.structure \
    Resources/Scenarios/Model1/Model1.yml \
    Resources/Scenarios/Model2/Model2.yml


