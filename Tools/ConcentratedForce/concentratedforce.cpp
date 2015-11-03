#include "concentratedforce.h"

#include "statics/viewModels/jointvm.h"
#include "statics/viewModels/beamvm.h"

#include "statics/elements/interiorpointload.h"
#include "statics/elements/nodeload.h"
#include "statics/abstractvmmanager.h"

/*********************Support functions************************/
QMatrix4x4 getTranformationMatrix(Qt3D::QEntity* entity,bool local){
    if(!entity) return  QMatrix4x4();
    Qt3D::QTransform* entity_transform=NULL;
    for(Qt3D::QComponent* component: entity->components()){
        if(component->isEnabled()){
            if(component->inherits("Qt3D::QTransform")){
                entity_transform=qobject_cast<Qt3D::QTransform*>(component);
            }
        }
    }
    if(local)
        if(entity_transform!=NULL)
            return entity_transform->matrix();
        else
            return QMatrix4x4();
    else{
        if(entity_transform!=NULL){
            return getTranformationMatrix(entity->parentEntity(),false)*entity_transform->matrix();
        }
        else return getTranformationMatrix(entity->parentEntity(),false);
    }
}
/**********************************************************************/


ConcentratedForce::ConcentratedForce(QObject* parent):
    QObject(parent),
    m_VMManager(Q_NULLPTR),
    m_emittingBodyInfo(Q_NULLPTR),
    m_pointLoad(Q_NULLPTR),
    m_nodeLoad(Q_NULLPTR),
    m_attached_element(Q_NULLPTR)
{

}

void ConcentratedForce::setVMManager(AbstractVMManager * vmManager){
    if(vmManager!=m_VMManager){
        m_VMManager=vmManager;
    }
}
void ConcentratedForce::setRegisteredTargets(QStringList targets){
    m_targets=targets;
}

void ConcentratedForce::setEmittingBodyInfo(Physics::PhysicsBodyInfo* emittingBodyInfo){
    if(emittingBodyInfo!=m_emittingBodyInfo){
        if(m_emittingBodyInfo)
            disconnect(m_emittingBodyInfo);
        m_emittingBodyInfo=emittingBodyInfo;
        if(m_emittingBodyInfo){
            connect(m_emittingBodyInfo,SIGNAL(collided(Physics::PhysicsCollisionEvent*)),this,SLOT(onCollition(Physics::PhysicsCollisionEvent*)));
            connect(m_emittingBodyInfo,SIGNAL(collitionsListChanged()),this,SLOT(checkCollitionAttachedElement()));
        }
     }
}

void ConcentratedForce::checkCollitionAttachedElement(){
    if(m_attached_element){
            Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
            if(!sender_body_info->collitionTest(m_attached_element->id())){
                m_attached_element=Q_NULLPTR;
                if(m_nodeLoad)
                    m_nodeLoad->deleteLater();
                if(m_pointLoad)
                    m_pointLoad->deleteLater();
                m_nodeLoad=Q_NULLPTR;
                m_pointLoad=Q_NULLPTR;
            }
        }
}

void ConcentratedForce::onCollition(Physics::PhysicsCollisionEvent* e){
    if(!m_VMManager) return;
    Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
    if(sender_body_info->entities().size()>1){
        qWarning()<<"Ambiguous sender";
        return;
    }
    Qt3D::QEntity* sender=sender_body_info->entities().at(0);
    if(!sender) return;

    Qt3D::QEntity* targetEntity=m_VMManager->getEntity3D(e->target());
    if(targetEntity!=Q_NULLPTR && targetEntity==m_attached_element){
        /*Update current status*/
    }
    else if(targetEntity!=Q_NULLPTR && m_attached_element==Q_NULLPTR){
        qDebug()<<targetEntity;
        //The Object was not attached, and the force should be created
        AbstractElementViewModel* targetVM=m_VMManager->getAssociatedVM(targetEntity);
        if(targetVM->inherits("JointVM")) {
            JointVM* jointVM= static_cast<JointVM*>(targetVM);
            Joint* joint=jointVM->joint();
            m_nodeLoad=m_VMManager->staticsModule()->createNodeLoad(QVector3D(0,-1,0),joint);
            m_attached_element=targetEntity;
        }
        else if(targetVM->inherits("BeamVM")){
            BeamVM* beamVM=static_cast<BeamVM*>(targetVM);
            Beam* beam=beamVM->beam();
            m_pointLoad=m_VMManager->staticsModule()->createIPLoad(QVector3D(0,-1,0),beam);
            m_attached_element=targetEntity;
        }
    }
}


