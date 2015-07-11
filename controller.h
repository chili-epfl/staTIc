#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <Qt3DCore/QEntity>
#include <Qt3DRenderer>
#include "abstractstaticsmodule.h"
#include "twodimensionalstaticsmodule.h"


class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QEntity* sceneRoot WRITE setSceneroot)
public:
    Controller(QObject* parent=0);
    void setSceneroot(Qt3D::QEntity* sceneroot){this->sceneroot=sceneroot;}

public slots:
    void onClickItem(Qt3D::QEntity* item);
    void loadStructure(QString modelFile, QString staticsFile);
private:
    Qt3D::QEntity* sceneroot;
    AbstractStaticsModule* staticsModule;
};

#endif // CONTROLLER_H
