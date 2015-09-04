#include "statics/abstracteventhandler.h"

AbstractEventHandler::AbstractEventHandler(QObject *parent) : QObject(parent)
{
    m_camera=Q_NULLPTR;
    m_sceneRoot=Q_NULLPTR;
    m_uiRoot=Q_NULLPTR;
}

void AbstractEventHandler::setCamera(Qt3D::QCamera* camera){
    if(m_camera!=camera){
        this->m_camera=camera;
        emit cameraChanged();
    }
}

void AbstractEventHandler::setSceneRoot(Qt3D::QEntity* sceneRoot){
    if(m_sceneRoot!=sceneRoot){
        m_sceneRoot=sceneRoot;
        emit sceneRootChanged();
    }
}

void AbstractEventHandler::setUiRoot(QObject* ui){
    if(m_uiRoot!=ui){
        m_uiRoot=ui;
        emit uiRootChanged();
    }
}
