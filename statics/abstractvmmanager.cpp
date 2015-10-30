#include "statics/abstractvmmanager.h"

AbstractVMManager::AbstractVMManager(QObject *parent) : QObject(parent)
{
    m_sceneRoot=Q_NULLPTR;
}


void AbstractVMManager::setSceneRoot(Qt3D::QEntity* sceneRoot){
    if(m_sceneRoot!=sceneRoot){
        m_sceneRoot=sceneRoot;
        emit sceneRootChanged();
    }
}

