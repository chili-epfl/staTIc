#include "abstractstaticsmodule.h"
#include "abstracteventhandler.h"
AbstractStaticsModule::AbstractStaticsModule(QObject* parent ):
    QObject(parent)
{
    m_status=NOT_LOADED;
}

void AbstractStaticsModule::setEventHandler(QVariant eventHandler){

    AbstractEventHandler* tmp=eventHandler.value<AbstractEventHandler*>();
    m_event_handler=eventHandler.value<AbstractEventHandler*>();
    m_event_handler->setStaticsModule(this);
    m_event_handler->setParent(this);
    if(m_sceneRoot)
        m_event_handler->setSceneRoot(m_sceneRoot);
}

void AbstractStaticsModule::setSceneRoot(Qt3D::QEntity* sceneRoot){
    this->m_sceneRoot=sceneRoot;
    if(m_event_handler)
        m_event_handler->setSceneRoot(m_sceneRoot);
    emit sceneRootChanged(m_sceneRoot);

}

