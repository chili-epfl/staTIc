TEMPLATE = app

QT += qml quick widgets 3dcore 3drender 3dinput 3dextras multimedia network concurrent
CONFIG += c++11 concurrent

#If you compiled solvespace, plase fix the path
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
    #Set the path for ARToolkit
    LIBS += -L/home/chili/ARToolKit5-bin-5.3.2-Android/android/libs/armeabi-v7a

    #If you compiled solvespace, plase fix the path
    LIBS+= -L$$_PRO_FILE_PWD_/3rdparty/solvespace/build-android-static/src

    #Set the path for ARToolkit. If you compiled solvespace as shared library, add the libslsv.so file here
    ANDROID_EXTRA_LIBS = \
        /home/chili/ARToolKit5-bin-5.3.2-Android/android/libs/armeabi-v7a/libc++_shared.so\
        /home/chili/ARToolKit5-bin-5.3.2-Android/android/libs/armeabi-v7a/libARWrapper.so
    ANDROID_PERMISSIONS += \
        android.permission.CAMERA

    ANDROID_FEATURES += \
        android.hardware.camera
}
!android{
    #Set the path for ARToolkit
    LIBS += -L/home/chili/ARToolKit5-bin-5.3.2r1-Linux-x86_64/lib
    LIBS+= -lAR -lARICP -lARMulti -lAR2
    #If you compiled solvespace, plase fix the path
    LIBS+= -L$$_PRO_FILE_PWD_/3rdparty/solvespace/build-unix/src
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


