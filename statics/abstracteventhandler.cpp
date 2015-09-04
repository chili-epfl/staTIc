#include "statics/abstracteventhandler.h"

AbstractEventHandler::AbstractEventHandler(QObject *parent) : QObject(parent)
{

    m_camera=Q_NULLPTR;
    m_sceneRoot=Q_NULLPTR;

}

void AbstractEventHandler::setSceneRoot(Qt3D::QEntity* sceneRoot){
    this->m_sceneRoot=sceneRoot;
}
