#include "forcevm.h"
#include "statics/elements/force.h"

ForceVM::ForceVM(QObject* parent):AbstractElementViewModel(parent)
{

}

void ForceVM::onElementChanged(){


}

void  ForceVM::onPropertyChanged(){

    if(!m_element) return;
    if(m_entity_name.isEmpty()) return;
    if(!m_sceneRoot) return;


    QQmlEngine engine;
    QQmlComponent component(&engine,QUrl("qrc:/ArrowComponent.qml"));
    Qt3D::QEntity *forceEntity = qobject_cast<Qt3D::QEntity*>(component.create());

    forceEntity->setObjectName(m_entity_name);
    forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

    Force* element=qobject_cast<Force*>(m_element);

    forceEntity->setProperty("myAngle",atan2(element->vector.y(),element->vector.x()));
    forceEntity->setProperty("positionX",element->applicationPoint.x());
    forceEntity->setProperty("positionY",element->applicationPoint.y());
    forceEntity->setProperty("positionZ",element->applicationPoint.z());
    forceEntity->setProperty("arrowLength",element->vector.length()*15);


}

