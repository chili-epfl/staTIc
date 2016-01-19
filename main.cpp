#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "AR/backgroundsubtraction.h"

#include "scenariolistmodel.h"

//#include "statics/two_dimentions/twodimensionalstaticsmodule.h"
//#include "statics/two_dimentions/twodimentionaleventhandler.h"
#include "statics/frame3dd/frame3ddkernel.h"
#include "statics/frame3dd/frame3ddvmmanager.h"

//#include "physicssetter.h"
#include "Tools/ConcentratedForce/concentratedforce.h"
#include "Tools/Scaffold/scaffold.h"
#include "Tools/BeamRemover/beamremover.h"
#include <opencv2/core.hpp>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    qRegisterMetaType<cv::Mat>("cv::Mat");

    qmlRegisterType<BackgroundSubtraction>("BackgroundSubtraction", 1, 0, "BackgroundSubtraction");

    qmlRegisterType<ScenarioListModel>("ScenarioListModel", 1, 0, "ScenarioListModel");
    //qmlRegisterType<TwoDimensionalStaticsModule>("StaticsModule2D", 1, 0, "StaticsModule2D");
    //qmlRegisterType<TwoDimentionalEventHandler>("EventHandler2D", 1, 0, "EventHandler2D");
    qmlRegisterType<Frame3DDKernel>("Frame3DDKernel",1,0,"Frame3DDKernel");
    qmlRegisterType<Frame3DDVMManager>("Frame3DDVMManager",1,0,"Frame3DDVMManager");


    //qmlRegisterType<MaterialSetter>("MaterialSetter", 1, 0, "MaterialSetter");
    //qmlRegisterType<PhysicsSetter>("PhysicsSetter", 1, 0, "PhysicsSetter");

    /*Tools*/
    qmlRegisterType<ConcentratedForce>("QuickConcentratedForce", 1, 0, "QuickConcentratedForce");
    qmlRegisterType<Scaffold>("QuickScaffold", 1, 0, "QuickScaffold");
    qmlRegisterType<BeamRemover>("QuickBeamRemover", 1, 0, "QuickBeamRemover");

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    //Frame3DDKernel* k=new Frame3DDKernel();
    //k->setSource(QString(":/models/models/Model1/Model1.lol"));
    return app.exec();
}
