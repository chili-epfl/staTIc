TEMPLATE = app

QT += qml quick widgets 3dcore 3drender 3dinput 3dextras multimedia network concurrent
CONFIG += c++11 concurrent

INCLUDEPATH += $$_PRO_FILE_PWD_/3rdparty/

SOURCES += main.cpp \
    statics/elements/joint.cpp \
    statics/abstractstaticsmodule.cpp \
    statics/elements/abstractelement.cpp \
    statics/elements/beam.cpp \
    statics/frame3dd/coordtrans.c \
    statics/frame3dd/eig.c \
    statics/frame3dd/frame3dd.c \
    statics/frame3dd/HPGmatrix.c \
    statics/frame3dd/NRutil.c \
    statics/frame3dd/frame3ddkernel.cpp \
    statics/elements/nodeload.cpp \
    statics/elements/uniformlydistributedload.cpp \
    statics/elements/interiorpointload.cpp \
    AR/deformingbeammesh.cpp \
    materialsmanager.cpp \
    statics/elements/trapezoidalforce.cpp \
    warehouse3d.cpp \
    AR/quaternionhelper.cpp \
    logger.cpp \
    UI/RoofDesigner/src/jsonsketch.cpp \
    UI/RoofDesigner/src/constraints.cpp \
    AR/phongmaterialcustomdepthtest.cpp \
    UI/BoardDesigner/ioboardfile.cpp


HEADERS += \
    statics/elements/joint.h \
    statics/abstractstaticsmodule.h \
    statics/elements/abstractelement.h \
    statics/elements/beam.h \
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
    statics/elements/interiorpointload.h \
    AR/deformingbeammesh.h \
    materialsmanager.h \
    statics/elements/trapezoidalforce.h \
    warehouse3d.h \
    static_global.h \
    AR/quaternionhelper.h \
    logger.h \
    UI/RoofDesigner/src/jsonsketch.h \
    UI/RoofDesigner/src/constraints.h \
    AR/phongmaterialcustomdepthtest.h \
    UI/BoardDesigner/ioboardfile.h


RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

LIBS +=  -lslvs

android {
    #INCLUDEPATH += $(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/include
    LIBS += -L/home/chili/ARToolKit5-bin-5.3.2-Android/android/libs/armeabi-v7a
    LIBS+= -L$$_PRO_FILE_PWD_/3rdparty/solvespace/build-android-static/src


    ANDROID_EXTRA_LIBS = \
        /home/chili/ARToolKit5-bin-5.3.2-Android/android/libs/armeabi-v7a/libc++_shared.so\
        /home/chili/ARToolKit5-bin-5.3.2-Android/android/libs/armeabi-v7a/libARWrapper.so
    ANDROID_PERMISSIONS += \
        android.permission.CAMERA

    ANDROID_FEATURES += \
        android.hardware.camera
}
!android{
    LIBS += -L/home/chili/ARToolKit5-bin-5.3.2r1-Linux-x86_64/lib
    LIBS+= -lAR -lARICP -lARMulti -lAR2
    LIBS+= -L$$_PRO_FILE_PWD_/3rdparty/solvespace/build-unix/src

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
    Resources/Assets3D/girl/texture.jpg \
    Resources/Assets3D/jacuzzi/jacuzzi.jpg \
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
    Resources/Materials/D70/lorenzo.lucignano@epfl.ch-8316125742.material \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


