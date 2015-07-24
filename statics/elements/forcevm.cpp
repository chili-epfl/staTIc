#include "forcevm.h"
#include "statics/elements/force.h"

ForceVM::ForceVM(QObject* parent):AbstractElementViewModel(parent)
{

}

void ForceVM::onElementChanged(){
    if(!m_element) return;
    if(m_entity_name.isEmpty()) return;
    if(!m_sceneRoot) return;


    Qt3D::QEntity *forceEntity =m_sceneRoot->findChild<Qt3D::QEntity*>(m_entity_name);
    if(!forceEntity){
        QQmlComponent component(&engine,QUrl("qrc:/ArrowComponent.qml"));
        forceEntity = qobject_cast<Qt3D::QEntity*>(component.create());
        forceEntity->setObjectName(m_entity_name);
        forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));
    }

    Force* element=qobject_cast<Force*>(m_element);
    forceEntity->setProperty("myAngle",atan2(element->getVector().y(),element->getVector().x()));
    forceEntity->setProperty("positionX",element->getApplicationPoint().x());
    forceEntity->setProperty("positionY",element->getApplicationPoint().y());
    forceEntity->setProperty("positionZ",element->getApplicationPoint().z());
    forceEntity->setProperty("arrowLength",element->getVector().length()*15);

}

void  ForceVM::onPropertyChanged(){

    if(!m_element) return;
    if(m_entity_name.isEmpty()) return;
    if(!m_sceneRoot) return;

    Qt3D::QEntity *forceEntity =m_sceneRoot->findChild<Qt3D::QEntity*>(m_entity_name);

    if(!forceEntity){
        QQmlComponent component(&engine,QUrl("qrc:/ArrowComponent.qml"));
        forceEntity = qobject_cast<Qt3D::QEntity*>(component.create());
        forceEntity->setObjectName(m_entity_name);
        forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));
    }

    Force* element=qobject_cast<Force*>(m_element);
    forceEntity->setProperty("myAngle",atan2(element->getVector().y(),element->getVector().x()));
    forceEntity->setProperty("positionX",element->getApplicationPoint().x());
    forceEntity->setProperty("positionY",element->getApplicationPoint().y());
    forceEntity->setProperty("positionZ",element->getApplicationPoint().z());
    forceEntity->setProperty("arrowLength",element->getVector().length()*15);


}

void ForceVM::onElementDestroyed(){
    Qt3D::QEntity *forceEntity=m_sceneRoot->findChild<Qt3D::QEntity*>(m_entity_name);
    if(forceEntity)
        forceEntity->deleteLater();
}
