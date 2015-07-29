#include "abstractelementviewmodel.h"

AbstractElementViewModel::AbstractElementViewModel(QObject *parent) : QObject(parent)
{
    m_element=0;
    m_sceneRoot=0;
    connect(this,SIGNAL(statusComplete()),this,SLOT(onStatusComplete()));
}

void AbstractElementViewModel::setEntityName(QString name){
    m_entity_name=name;
    if(m_element!=NULL && !m_entity_name.isEmpty() && m_sceneRoot!=NULL)
        emit statusComplete();
}
void AbstractElementViewModel::setElement(AbstractElement* element){
    m_element=element;
    if(m_element!=NULL && !m_entity_name.isEmpty() && m_sceneRoot!=NULL)
        emit statusComplete();
}

void AbstractElementViewModel::setSceneRoot(Qt3D::QEntity* sceneRoot){
    m_sceneRoot=sceneRoot;
    if(m_element!=NULL && !m_entity_name.isEmpty() && m_sceneRoot!=NULL)
        emit statusComplete();
}
