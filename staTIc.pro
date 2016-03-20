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
    resourcesfetcher.h \
    static_global.h \
    AR/arucodetector.h \
    AR/arucodetectorthread.h \
    AR/arucoobject.h


RESOURCES += qml.qrc

INCLUDEPATH+= $$[QT_INSTALL_HEADERS]/QtPhysicsUnofficial

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

    #LIBS += -L$(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/lib
    LIBS += -L/home/chili/opencv/platforms/build-android/install/sdk/native/libs/armeabi-v7a/

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
        /home/chili/android-ndk-r10d/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/user/lib/libassimp.so


    ANDROID_PERMISSIONS += \
        android.permission.CAMERA

    ANDROID_FEATURES += \
        android.hardware.camera


}
!android{
    INCLUDEPATH += /home/chili/opencv/build-linux/install/include
    LIBS += -L/home/chili/opencv/build-linux/install/lib
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
    Resources/Scenarios/Model2/Model2.yml \
    Resources/Assets3D/solar_pannel/Solar.blend \
    Resources/Assets3D/girl/girl.obj \
    Resources/Assets3D/jacuzzi/jacuzzi.obj \
    Resources/Assets3D/snow/snow.obj \
    Resources/Assets3D/solar_pannel/Solar.obj \
    Resources/Assets3D/tonweight/tonweight.obj \
    Resources/Assets3D/waterTank/watertank.obj \
    Resources/Assets3D/girl/girl.asset~ \
    Resources/Assets3D/jacuzzi/jacuzzi.asset~ \
    Resources/Assets3D/snow/snow.asset~ \
    Resources/Assets3D/solar_pannel/solar_pannel.asset~ \
    Resources/Assets3D/tonweight/watertank.asset~ \
    Resources/Assets3D/waterTank/watertank.asset~ \
    Resources/Assets3D/girl/texture.jpg \
    Resources/Assets3D/jacuzzi/jacuzzi.jpg \
    Resources/Assets3D/solar_pannel/preview.jpg \
    Resources/Materials/oak.jpeg \
    Resources/Assets3D/jacuzzi/layout.png \
    Resources/Assets3D/snow/texture.png \
    Resources/Assets3D/solar_pannel/Solar.png \
    Resources/Assets3D/tonweight/texture.png \
    Resources/Assets3D/waterTank/watertank.png \
    Resources/Assets3D/girl/girl.asset \
    Resources/Assets3D/jacuzzi/jacuzzi.asset \
    Resources/Assets3D/snow/snow.asset \
    Resources/Assets3D/solar_pannel/solar_pannel.asset \
    Resources/Assets3D/tonweight/watertank.asset \
    Resources/Assets3D/waterTank/watertank.asset \
    Resources/Materials/material1.material \
    Resources/Assets3D/tonweight/tonweight.asset~ \
    Resources/Assets3D/tonweight/preview.jpg \
    Resources/Assets3D/waterTank/preview.jpg \
    Resources/Assets3D/girl/preview.png \
    Resources/Assets3D/snow/preview.png \
    Resources/Assets3D/solar_pannel/preview.png \
    Resources/Assets3D/tonweight/tonweight.asset \
    Resources/Materials/C18/lorenzo.lucignano@epfl.ch-8316125742.material~ \
    Resources/Materials/C22/lorenzo.lucignano@epfl.ch-8316125742.material~ \
    Resources/Materials/C30/lorenzo.lucignano@epfl.ch-8316125742.material~ \
    Resources/Materials/C40/lorenzo.lucignano@epfl.ch-8316125742.material~ \
    Resources/Materials/D30/lorenzo.lucignano@epfl.ch-8316125742.material~ \
    Resources/Materials/D50/lorenzo.lucignano@epfl.ch-8316125742.material~ \
    Resources/Materials/D70/lorenzo.lucignano@epfl.ch-8316125742.material~ \
    Resources/Materials/C18/texture.png \
    Resources/Materials/C22/texture.png \
    Resources/Materials/C30/texture.png \
    Resources/Materials/C40/texture.png \
    Resources/Materials/D30/texture.png \
    Resources/Materials/D50/texture.png \
    Resources/Materials/D70/texture.png \
    Resources/Materials/C18/lorenzo.lucignano@epfl.ch-8316125742.material \
    Resources/Materials/C22/lorenzo.lucignano@epfl.ch-8316125742.material \
    Resources/Materials/C30/lorenzo.lucignano@epfl.ch-8316125742.material \
    Resources/Materials/C40/lorenzo.lucignano@epfl.ch-8316125742.material \
    Resources/Materials/D30/lorenzo.lucignano@epfl.ch-8316125742.material \
    Resources/Materials/D50/lorenzo.lucignano@epfl.ch-8316125742.material \
    Resources/Materials/D70/lorenzo.lucignano@epfl.ch-8316125742.material


