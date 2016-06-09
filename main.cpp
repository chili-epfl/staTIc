#include <QtQml>

#include <QApplication>
#include <QQuickView>

#include "AR/backgroundsubtraction.h"

#include "scenariolistmodel.h"
#include "materialsmanager.h"
#include "warehouse3d.h"

#include "statics/frame3dd/frame3ddkernel.h"
#include "statics/frame3dd/frame3ddvmmanager.h"

#include "Tools/ConcentratedForce/concentratedforce.h"
#include "Tools/Scaffold/scaffold.h"
#include "Tools/BeamRemover/beamremover.h"
#include "Tools/TangibleTrapzLoad/tangibletrapzload.h"
#include "Tools/Snow/snow.h"

#include "AR/deformingbeammesh.h"

#include <opencv2/core.hpp>
#include "resourcesfetcher.h"
#include "static_global.h"
#include "AR/arucodetector.h"
#include "AR/arucoobject.h"
#include "AR/quaternionhelper.h"
#include "logger.h"
int main(int argc, char *argv[])
{
//    QApplication app(argc, argv);
//    QQmlApplicationEngine engine;
    QApplication app(argc, argv);

    if(!QDir(materialsPath).exists()){
        qDebug()<<"Creating material Path";
        QDir().mkpath(materialsPath);
    }
    if(!QDir(assets3DPath).exists()){
        qDebug()<<"Creating assets3d Path";
        QDir().mkpath(assets3DPath);
    }
    if(!QDir(scenariosPath).exists()){
        qDebug()<<"Creating scenarios Path";
        QDir().mkpath(scenariosPath);
    }

    if(!QDir(materialsPrivatePath).exists()){
        qDebug()<<"Creating material Private Path";
        QDir().mkpath(materialsPrivatePath);
    }
    if(!QDir(assets3DPrivatePath).exists()){
        qDebug()<<"Creating assets3d Private Path";
        QDir().mkpath(assets3DPrivatePath);
    }
    if(!QDir(scenariosPrivatePath).exists()){
        qDebug()<<"Creating scenarios Private Path";
        QDir().mkpath(scenariosPrivatePath);
    }

    QQuickView view;

    qRegisterMetaType<cv::Mat>("cv::Mat");

    qmlRegisterType<Logger>("Logger", 1, 0, "Logger");

    qmlRegisterType<BackgroundSubtraction>("BackgroundSubtraction", 1, 0, "BackgroundSubtraction");

    qmlRegisterType<ScenarioListModel>("ScenarioListModel", 1, 0, "ScenarioListModel");
    qmlRegisterType<MaterialsManager>("MaterialsManager", 1, 0, "MaterialsManager");
    qmlRegisterType<Warehouse3D>("Warehouse3D", 1, 0, "Warehouse3D");

    qmlRegisterType<ResourcesFetcher>("ResourcesFetcher", 1, 0, "ResourcesFetcher");

    //qmlRegisterType<TwoDimensionalStaticsModule>("StaticsModule2D", 1, 0, "StaticsModule2D");
    //qmlRegisterType<TwoDimentionalEventHandler>("EventHandler2D", 1, 0, "EventHandler2D");
    qmlRegisterType<Frame3DDKernel>("Frame3DDKernel",1,0,"Frame3DDKernel");
    qmlRegisterType<Frame3DDVMManager>("Frame3DDVMManager",1,0,"Frame3DDVMManager");

    qmlRegisterType<ArucoDetector>("ArucoDetector",1,0,"ArucoDetector");
    qmlRegisterType<ArucoObject>("ArucoObject",1,0,"ArucoObject");

    qmlRegisterType<QuaternionHelper>("QuaternionHelper",1,0,"QuaternionHelper");

    //qmlRegisterType<MaterialSetter>("MaterialSetter", 1, 0, "MaterialSetter");
    //qmlRegisterType<PhysicsSetter>("PhysicsSetter", 1, 0, "PhysicsSetter");

    /*Tools*/
    qmlRegisterType<ConcentratedForce>("QuickConcentratedForce", 1, 0, "QuickConcentratedForce");
    qmlRegisterType<Scaffold>("QuickScaffold", 1, 0, "QuickScaffold");
    qmlRegisterType<BeamRemover>("QuickBeamRemover", 1, 0, "QuickBeamRemover");
    qmlRegisterType<TangibleTrapzLoad>("QuickTangibleTPZLoad", 1, 0, "QuickTangibleTPZLoad");
    qmlRegisterType<Snow>("QuickSnow", 1, 0, "QuickSnow");


    qmlRegisterType<DeformingBeamMesh>("DeformingBeamMesh", 1, 0, "DeformingBeamMesh");

//    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    view.resize(2560, 1600);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    //Frame3DDKernel* k=new Frame3DDKernel();
    //k->setSource(QString(":/models/models/Model1/Model1.lol"));
   return app.exec();
}
