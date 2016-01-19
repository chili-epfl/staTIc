#include "beamremover.h"
#include "statics/viewModels/beamvm.h"

BeamRemover::BeamRemover(QObject* parent):
    QObject(parent),
    m_VMManager(Q_NULLPTR),
    m_emittingBodyInfo(Q_NULLPTR),
    m_attached_element(Q_NULLPTR)
{

}

void BeamRemover::setVMManager(AbstractVMManager * vmManager){
    if(vmManager!=m_VMManager){
        m_VMManager=vmManager;
    }
}

void BeamRemover::setEmittingBodyInfo(Physics::PhysicsBodyInfo* emittingBodyInfo){
    if(emittingBodyInfo!=m_emittingBodyInfo){
        if(m_emittingBodyInfo)
            disconnect(m_emittingBodyInfo);
        m_emittingBodyInfo=emittingBodyInfo;
        if(m_emittingBodyInfo){
            connect(m_emittingBodyInfo,SIGNAL(collided(Physics::PhysicsCollisionEvent*)),this,SLOT(onCollision(Physics::PhysicsCollisionEvent*)));
            connect(m_emittingBodyInfo,SIGNAL(collisionsListChanged()),this,SLOT(checkCollisionAttachedElement()));
        }
     }
}

void BeamRemover::checkCollisionAttachedElement(){
    if(m_attached_element){
            Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
            if(!sender_body_info->collisionTest(m_attached_element->id())){
                reset();
            }
        }
}

void BeamRemover::onCollision(Physics::PhysicsCollisionEvent* e){
    if(!m_VMManager) return;
    Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
    if(sender_body_info->entities().size()>1){
        qWarning()<<"Ambiguous sender";
        return;
    }
    Qt3DCore::QEntity* sender=sender_body_info->entities().at(0);
    if(!sender) return;

    Qt3DCore::QEntity* targetEntity=m_VMManager->getEntity3D(e->target());
    if(targetEntity!=Q_NULLPTR && m_attached_element==Q_NULLPTR){
        AbstractElementViewModel* targetVM=m_VMManager->getAssociatedVM(targetEntity);
        if(targetVM->inherits("BeamVM")){
            BeamVM* beamVM=static_cast<BeamVM*>(targetVM);
            m_beam=beamVM->beam();
            BeamPtr beam_str_ptr=m_beam.toStrongRef();
            if(beam_str_ptr.isNull()) return;
            beam_str_ptr->setEnable(false);
            m_attached_element=targetEntity;
            connect(m_attached_element,SIGNAL(destroyed(QObject*)),this,SLOT(reset()));
        }
    }
}


void BeamRemover::reset(){
    m_attached_element=Q_NULLPTR;
    BeamPtr beam_str_ptr=m_beam.toStrongRef();
    if(!beam_str_ptr.isNull()){
        beam_str_ptr->setEnable(true);
    }
    m_beam.clear();
}
