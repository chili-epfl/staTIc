#include "physicssetter.h"

PhysicsSetter::PhysicsSetter(QObject *parent) :
    QObject(parent),
    m_entityName()
{
    m_sceneroot=Q_NULLPTR;
    m_bodyInfo=Q_NULLPTR;
}

void PhysicsSetter::onAnyChange(){
    if(!m_sceneroot) return;
    if(!m_bodyInfo) return;

    Qt3D::QEntity* entity;
    if(m_sceneroot->objectName()==m_entityName)
        entity=m_sceneroot;
    else
        entity=m_sceneroot->findChild<Qt3D::QEntity*>(m_entityName);
    recursive_step(entity);

}

void PhysicsSetter::recursive_step(Qt3D::QEntity* e){
    if(!e) return;
    Qt3D::QTransform* t=Q_NULLPTR;
    for(Qt3D::QComponent* c: e->components()){
        if(c->inherits("Qt3D::QTransform")){
            t=static_cast<Qt3D::QTransform*>(c);
        }
    }
    Physics::PhysicsBodyInfo* bodyInfo=new Physics::PhysicsBodyInfo(e);
    bodyInfo->setEnabled(m_bodyInfo->isEnabled());
    bodyInfo->setFallInertia(m_bodyInfo->fallInertia());
    bodyInfo->setFriction(m_bodyInfo->friction());
    bodyInfo->setGroup(m_bodyInfo->group());
    bodyInfo->setKinematic(m_bodyInfo->kinematic());
    bodyInfo->setMask(m_bodyInfo->mask());
    bodyInfo->setMass(m_bodyInfo->mass());
    bodyInfo->setRestitution(m_bodyInfo->restitution());
    bodyInfo->setRollingFriction(m_bodyInfo->rollingFriction());
    bodyInfo->setShapeDetails(m_bodyInfo->shapeDetails());
    if(t)
        bodyInfo->setInputTransform(t);
    else
        bodyInfo->setInputTransform(new Qt3D::QTransform(bodyInfo));

    e->addComponent(bodyInfo);


    QList<Qt3D::QEntity*> children=e->findChildren<Qt3D::QEntity*>();
    for(Qt3D::QEntity* child:children){
        recursive_step(child);
    }

}
