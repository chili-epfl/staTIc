#include <QtQml>

#include <QApplication>
#include <QQuickView>

#include "scenariolistmodel.h"
#include "materialsmanager.h"
#include "warehouse3d.h"

#include "statics/frame3dd/frame3ddkernel.h"

//#include "Tools/ConcentratedForce/concentratedforce.h"
//#include "Tools/Scaffold/scaffold.h"
//#include "Tools/BeamRemover/beamremover.h"
//#include "Tools/TangibleTrapzLoad/tangibletrapzload.h"
//#include "Tools/Snow/snow.h"

#include "AR/deformingbeammesh.h"

#include "resourcesfetcher.h"
#include "static_global.h"

#include "AR/quaternionhelper.h"
#include "logger.h"
#include <Qt3DExtras/QSkyboxEntity>
#include "UI/RoofDesigner/src/constraints.h"
#include "UI/RoofDesigner/src/jsonsketch.h"
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

    qmlRegisterType<Logger>("Logger", 1, 0, "Logger");

    qmlRegisterType<ScenarioListModel>("ScenarioListModel", 1, 0, "ScenarioListModel");
    qmlRegisterType<MaterialsManager>("MaterialsManager", 1, 0, "MaterialsManager");
    qmlRegisterType<Warehouse3D>("Warehouse3D", 1, 0, "Warehouse3D");

    qmlRegisterType<ResourcesFetcher>("ResourcesFetcher", 1, 0, "ResourcesFetcher");

    qmlRegisterType<Frame3DDKernel>("Frame3DDKernel",1,0,"Frame3DDKernel");

    qmlRegisterType<QuaternionHelper>("QuaternionHelper",1,0,"QuaternionHelper");

    qmlRegisterType<JSONSketch>("JSONSketch", 1, 0, "JSONSketch");
    qmlRegisterType<Constraints>("Constraints", 1, 0, "Constraints");
    //qmlRegisterSingletonType(QUrl("qrc:/ui/UI/RoofDesigner/RoofDesignerSettings.qml"), "RoofDesignerSettings", 1, 0, "RoofDesignerSettings");

    /*Tools*/
//    qmlRegisterType<ConcentratedForce>("QuickConcentratedForce", 1, 0, "QuickConcentratedForce");
//    qmlRegisterType<Scaffold>("QuickScaffold", 1, 0, "QuickScaffold");
//    qmlRegisterType<BeamRemover>("QuickBeamRemover", 1, 0, "QuickBeamRemover");
//    qmlRegisterType<TangibleTrapzLoad>("QuickTangibleTPZLoad", 1, 0, "QuickTangibleTPZLoad");
//    qmlRegisterType<Snow>("QuickSnow", 1, 0, "QuickSnow");
    qmlRegisterType<DeformingBeamMesh>("DeformingBeamMesh", 1, 0, "DeformingBeamMesh");

    Logger logger(&view);
#ifdef ANDROID
    view.rootContext()->setContextProperty("Platform","ANDROID");
    view.rootContext()->setContextProperty("custom_board_url",QString("file://")+QString(getenv("EXTERNAL_STORAGE"))+QString("/staTIc/board_configuration.data"));
#else
    view.rootContext()->setContextProperty("Platform","UNIX");
#endif

#ifdef ANDROID
    view.rootContext()->setContextProperty("Platform","ANDROID");
    QString conf_folder="file:"+QString(getenv("EXTERNAL_STORAGE"))+"/staTIc/";
    view.rootContext()->setContextProperty("Conf_folder",conf_folder);
#else
    view.rootContext()->setContextProperty("Platform","UNIX");
    QString conf_folder="file:"+QString(getenv("HOME"));
    view.rootContext()->setContextProperty("Conf_folder",conf_folder);
#endif

    view.rootContext()->setContextProperty("logger",&logger);
    view.rootContext()->setContextProperty("scenariosPath", "file://"+scenariosPath);

    view.resize(2560, 1600);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
//    view.setSource(QUrl("qrc:/main.qml"));
    view.setSource(QUrl("qrc:/Experiment-Mode.qml"));
    view.show();

   return app.exec();
}
