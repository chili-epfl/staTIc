#include <QtQml>

#include <QApplication>
#include <QQuickView>

#include "materialsmanager.h"
#include "warehouse3d.h"

#include "statics/frame3dd/frame3ddkernel.h"

#include "AR/deformingbeammesh.h"
#include "AR/phongmaterialcustomdepthtest.h"

#include "static_global.h"

#include "AR/quaternionhelper.h"
#include "logger.h"
#include <Qt3DExtras/QSkyboxEntity>
#include "UI/RoofDesigner/src/constraints.h"
#include "UI/RoofDesigner/src/jsonsketch.h"
#include "UI/BoardDesigner/ioboardfile.h"
int main(int argc, char *argv[])
{
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
    if(!QDir(boardsPath).exists()){
        qDebug()<<"Creating boards Path";
        QDir().mkpath(boardsPath);
    }

    QQuickView view;

    qmlRegisterType<Logger>("Logger", 1, 0, "Logger");

    qmlRegisterType<MaterialsManager>("MaterialsManager", 1, 0, "MaterialsManager");
    qmlRegisterType<Warehouse3D>("Warehouse3D", 1, 0, "Warehouse3D");

    qmlRegisterType<Frame3DDKernel>("Frame3DDKernel",1,0,"Frame3DDKernel");

    qmlRegisterType<QuaternionHelper>("QuaternionHelper",1,0,"QuaternionHelper");

    qmlRegisterType<JSONSketch>("JSONSketch", 1, 0, "JSONSketch");
    qmlRegisterType<Constraints>("Constraints", 1, 0, "Constraints");

    qmlRegisterType<IOBoardFile>("IOBoardFile", 1, 0, "IOBoardFile");

    qmlRegisterType<DeformingBeamMesh>("DeformingBeamMesh", 1, 0, "DeformingBeamMesh");
    qmlRegisterType<PhongMaterialCustomDepthTest>("PhongMaterialCustomDepthTest", 1, 0, "PhongMaterialCustomDepthTest");

    Logger logger(&view);

    view.rootContext()->setContextProperty("logger",&logger);

    view.rootContext()->setContextProperty("scenariosPath", scenariosPath);
    view.rootContext()->setContextProperty("boardsPath", boardsPath);

    view.resize(2560, 1600);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

   return app.exec();
}
