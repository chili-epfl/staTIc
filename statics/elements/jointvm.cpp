#include "jointvm.h"


JointVM::JointVM(QObject* parent): AbstractElementViewModel(parent)
{

}




/*
 * Entities names are
 * Suffix:
 *  [blank] ->  entity rendering from collada
 *  _REACT  ->  reaction force
 *
*/
void JointVM::onElementChanged(){

    if(!m_element) return;
    if(m_entity_name.isEmpty()) return;
    if(!m_sceneRoot) return;


    Joint* joint=qobject_cast<Joint*>(m_element);
    qDebug()<< joint->getReaction();

    if(joint->getReaction().length()>=EPSILON){

        Qt3D::QEntity *forceEntity =m_sceneRoot->findChild<Qt3D::QEntity*>(m_entity_name+"_REACT");
        if(!forceEntity){
            /*Create the entity*/
            QQmlComponent component(&engine,QUrl("qrc:/ReactionArrow.qml"));
            forceEntity = qobject_cast<Qt3D::QEntity*>(component.create());
            forceEntity->setObjectName(m_entity_name+"_REACT");
            forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

        }

        forceEntity->setProperty("myAngle",atan2(joint->getReaction().y(),joint->getReaction().x()));
        forceEntity->setProperty("positionX",joint->getPosition().x());
        forceEntity->setProperty("positionY",joint->getPosition().y());
        forceEntity->setProperty("positionZ",joint->getPosition().z());
        forceEntity->setProperty("arrowLength",joint->getReaction().length()*15);
        forceEntity->setProperty("visible",true);

    }
     else{
        Qt3D::QEntity *forceEntity =m_sceneRoot->findChild<Qt3D::QEntity*>(m_entity_name+"_REACT");
        if(forceEntity){
            forceEntity->setProperty("visible",false);
        }
    }
}
