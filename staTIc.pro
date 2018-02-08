TEMPLATE = app

QT += qml quick widgets 3dcore 3drender 3dinput multimedia network sensors concurrent
DEFINES += VIDEOFORMAT=43
CONFIG += c++11 concurrent

INCLUDEPATH += $$_PRO_FILE_PWD_/3rdparty/

SOURCES += main.cpp \
    statics/elements/joint.cpp \
    statics/abstractstaticsmodule.cpp \
    statics/elements/abstractelement.cpp \
    statics/elements/beam.cpp \
    #statics/viewModels/abstractelementviewmodel.cpp \
    #statics/viewModels/beamvm.cpp \
    #statics/viewModels/jointvm.cpp \
    #Tools/ConcentratedForce/concentratedforce.cpp \
    statics/frame3dd/coordtrans.c \
    statics/frame3dd/eig.c \
    statics/frame3dd/frame3dd.c \
    statics/frame3dd/HPGmatrix.c \
    statics/frame3dd/NRutil.c \
    statics/frame3dd/frame3ddkernel.cpp \
    statics/elements/nodeload.cpp \
    statics/elements/uniformlydistributedload.cpp \
    #statics/abstractvmmanager.cpp \
    #statics/frame3dd/frame3ddvmmanager.cpp \
    statics/elements/interiorpointload.cpp \
    #Tools/Scaffold/scaffold.cpp \
    #Tools/BeamRemover/beamremover.cpp \
    scenariolistmodel.cpp \
    AR/deformingbeammesh.cpp \
    materialsmanager.cpp \
    statics/elements/trapezoidalforce.cpp \
    #statics/viewModels/trapezoidalforcevm.cpp \
    warehouse3d.cpp \
    resourcesfetcher.cpp \
    #Tools/TangibleTrapzLoad/tangibletrapzload.cpp \
    AR/quaternionhelper.cpp \
    #Tools/Snow/snow.cpp \
    logger.cpp \
    UI/RoofDesigner/src/jsonsketch.cpp \
    UI/RoofDesigner/src/constraints.cpp


HEADERS += \
    #statics/two_dimentions/twodimensionalstaticsmodule.h \
    statics/elements/joint.h \
    #statics/elements/force.h \
    statics/abstractstaticsmodule.h \
    statics/elements/abstractelement.h \
    statics/elements/beam.h \
    #materialsetter.h \
    #statics/two_dimentions/twodimentionaleventhandler.h \
    #statics/viewModels/abstractelementviewmodel.h \
    #statics/viewModels/beamvm.h \
    #statics/viewModels/forcevm.h \
    #statics/viewModels/jointvm.h \
    #staticshelper.h \
    #statics/viewModels/jointvmitemmodel.h \
    #physicssetter.h \
    #Tools/ConcentratedForce/concentratedforce.h \
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
    #statics/abstractvmmanager.h \
    #statics/frame3dd/frame3ddvmmanager.h \
    #qmlenginefactory.h \
    statics/elements/interiorpointload.h \
    #Tools/Scaffold/scaffold.h \
    #Tools/BeamRemover/beamremover.h \
    scenariolistmodel.h \
    AR/deformingbeammesh.h \
    materialsmanager.h \
    statics/elements/trapezoidalforce.h \
    #statics/viewModels/trapezoidalforcevm.h \
    warehouse3d.h \
    resourcesfetcher.h \
    static_global.h \
    #Tools/TangibleTrapzLoad/tangibletrapzload.h \
    AR/quaternionhelper.h \
    #Tools/Snow/snow.h \
    logger.h \
    UI/RoofDesigner/src/jsonsketch.h \
    UI/RoofDesigner/src/constraints.h


RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

LIBS += -lquazip -lz -lslvs

android {
    #INCLUDEPATH += $(ANDROID_STANDALONE_TOOLCHAIN)/sysroot/usr/include
    LIBS += -L/home/chili/ARToolKit5-bin-5.3.2-Android/android/libs/armeabi-v7a
    LIBS+= -L$$_PRO_FILE_PWD_/3rdparty/solvespace/build-android/src


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


