#include "concentratedforce.h"
#include "statics/viewModels/jointvm.h"
#include "statics/elements/force.h"

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
    QObject(parent)
{
    m_UIManager=Q_NULLPTR;
    m_attached_element=Q_NULLPTR;
    m_force=Q_NULLPTR;
    m_emittingBodyInfo=Q_NULLPTR;
}

void ConcentratedForce::setUiManager(AbstractEventHandler * UIManager){
    if(UIManager!=m_UIManager){
        m_UIManager=UIManager;
    }
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
            m_force->deleteLater();
            qDebug()<<"Destroyed";
        }
    }
}

void ConcentratedForce::onCollition(Physics::PhysicsCollisionEvent* e){
    if(!m_UIManager) return;
    Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
    if(sender_body_info->entities().size()>1){
        qWarning()<<"Ambiguous sender";
        return;
    }
    Qt3D::QEntity* sender=sender_body_info->entities().at(0);
    if(!sender) return;

    Qt3D::QEntity* targetEntity=m_UIManager->getEntity3D(e->target());
    if(targetEntity!=Q_NULLPTR && targetEntity==m_attached_element){
        QMatrix4x4 M_sender=getTranformationMatrix(sender,true);
        QVector3D force_dir(-1,0,0);
        QVector3D force_dir_in_target=M_sender.mapVector(force_dir);
        m_force->setVector(force_dir_in_target);
    }
    else if(targetEntity!=Q_NULLPTR && m_attached_element==Q_NULLPTR){
        //The Object was not attached, and the force should be created
        AbstractElementViewModel* targetVM=m_UIManager->getAssociatedVM(targetEntity);
        if(targetVM->inherits("JointVM")) {
            JointVM* jointVM= static_cast<JointVM*>(targetVM);
            if(jointVM->getEntitiyRole(targetEntity)!=AbstractElementViewModel::Roles::PRIMARY) return;
            QMatrix4x4 M_sender=getTranformationMatrix(sender,true);
            QVector3D force_dir(-1,0,0);
            QVector3D force_dir_in_target=M_sender.mapVector(force_dir);
            Joint* joint=jointVM->joint();
            m_force=m_UIManager->staticsModule()->createForce(joint->position(),force_dir_in_target,joint);
            m_UIManager->createForceVM(m_force);
            m_attached_element=targetEntity;
        }
    }
}


