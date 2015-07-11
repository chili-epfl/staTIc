#include "controller.h"

Controller::Controller(QObject* parent):
    QObject(parent)
{

}

void Controller::loadStructure(QString modelFile, QString staticsFile){
    staticsModule=new TwoDimensionalStaticsModule(staticsFile);
    Qt3D::QSceneLoader* sceneLoader=new Qt3D::QSceneLoader(sceneroot);
    sceneLoader->setSource(modelFile);
}


