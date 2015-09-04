#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "structureitemmodel.h"

#include "statics/two_dimentions/twodimensionalstaticsmodule.h"
#include "statics/two_dimentions/twodimentionaleventhandler.h"
#include "materialsetter.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<StructureItemModel>("StructureItemModel", 1, 0, "StructureItemModel");
    qmlRegisterType<TwoDimensionalStaticsModule>("StaticsModule2D", 1, 0, "StaticsModule2D");
    qmlRegisterType<TwoDimentionalEventHandler>("EventHandler2D", 1, 0, "EventHandler2D");
    qmlRegisterType<MaterialSetter>("MaterialSetter", 1, 0, "MaterialSetter");

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    //engine.load(QUrl(QStringLiteral("qrc:/test_main.qml")));

    return app.exec();
}
