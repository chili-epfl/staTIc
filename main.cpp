#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "structureitemmodel.h"
#include <QScreen>
#include "abstractstaticsmodule.h"
#include "twodimensionalstaticsmodule.h"
#include "controller.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<StructureItemModel>("StructureItemModel", 1, 0, "StructureItemModel");
    qmlRegisterType<Controller>("Controller", 1, 0, "Controller");

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));



    qDebug()<<"Testing...";
    return app.exec();
}
