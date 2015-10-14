#include "concentratedforce.h"
#include "statics/viewModels/jointvm.h"
#include "statics/elements/force.h"

/*********************Support functions************************/
QMatrix4x4 getTranformationMatrix(Qt3D::QEntity* entity){
    if(!entity) return  QMatrix4x4();
    Qt3D::QTransform* entity_transform=NULL;
    for(Qt3D::QComponent* component: entity->components()){
        if(component->isEnabled()){
            if(component->inherits("Qt3D::QTransform")){
                entity_transform=qobject_cast<Qt3D::QTransform*>(component);
            }
        }
    }
    if(entity_transform!=NULL){
        return getTranformationMatrix(entity->parentEntity())*entity_transform->matrix();
    }
    else return getTranformationMatrix(entity->parentEntity());
}
/**********************************************************************/


ConcentratedForce::ConcentratedForce(QObject* parent):
    QObject(parent)
{
    m_UIManager=Q_NULLPTR;
    m_attached_element=Q_NULLPTR;
    m_force=Q_NULLPTR;
}

void ConcentratedForce::setuiManager(AbstractEventHandler * UIManager){
    if(UIManager!=m_UIManager){
        m_UIManager=UIManager;
    }
}


void ConcentratedForce::onCollition(Physics::PhysicsCollisionEvent* e){
    if(!m_UIManager) return;

    Qt3D::QEntity* sender=qobject_cast<Qt3D::QEntity*>(QObject::sender());
    if(!sender) return;

    Qt3D::QEntity* targetEntity=m_UIManager->getEntity3D(e->target());
    if(targetEntity){
       AbstractElementViewModel* targetVM=m_UIManager->getAssociatedVM(targetEntity);
       if(targetVM->inherits("JointVM")) {
           JointVM* jointVM= static_cast<JointVM*>(targetVM);
           if(jointVM->getEntitiyRole(targetEntity)!=AbstractElementViewModel::Roles::PRIMARY) return;

           QMatrix4x4 M_sender=getTranformationMatrix(sender);
           QMatrix4x4 M_target=getTranformationMatrix(targetEntity);
           QMatrix4x4 M_toTargetSpace= M_target.inverted()*M_sender;
           QVector3D force_dir(1,0,0);
           QVector3D force_dir_in_target=M_toTargetSpace.mapVector(force_dir);

           //Case 1: The Object was not attached, and the force should be created
           if(!m_attached_element){
               Joint* joint=jointVM->joint();
               m_force=m_UIManager->staticsModule()->createForce(joint->position(),force_dir_in_target,joint);
               m_UIManager->createForceVM(m_force);
               m_attached_element=targetEntity;

           }
           //Case 2: The Object was attached, and the force should be updated
           else{
               m_force->setVector(force_dir_in_target);
           }
       }
    }
}



