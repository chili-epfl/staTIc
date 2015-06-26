#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "structureitemmodel.h"
#include <QScreen>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<StructureItemModel>("StructureItemModel", 1, 0, "StructureItemModel");
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
