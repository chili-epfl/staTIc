#include "scaffold.h"

#include "statics/elements/beam.h"
#include "statics/elements/joint.h"

#include "statics/viewModels/jointvm.h"
#include "statics/viewModels/beamvm.h"

const int discrete_step = 10;

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




Scaffold::Scaffold(QObject* parent):
    QObject(parent)
{
    m_extreme1=Q_NULLPTR;
    m_extreme2=Q_NULLPTR;
    m_VMManager=Q_NULLPTR;
    m_attached_element_e1=Q_NULLPTR;
    m_attached_element_e2=Q_NULLPTR;
    m_anchor_1_offset=-1;
    m_anchor_2_offset=-1;
}

void Scaffold::setExtreme1(Physics::PhysicsBodyInfo * extreme1){
    if(m_extreme1!=extreme1){
        if(m_extreme1)
            disconnect(m_extreme1);
        m_extreme1=extreme1;
        connect(m_extreme1,SIGNAL(collided(Physics::PhysicsCollisionEvent*)),this,SLOT(onCollitionExtreme1(Physics::PhysicsCollisionEvent*)));
        connect(m_extreme1,SIGNAL(collitionsListChanged()),
                this,SLOT(checkCollitionAttachedElement1()));

        emit extreme1Changed();
    }
}


void Scaffold::setExtreme2(Physics::PhysicsBodyInfo * extreme2){
    if(m_extreme2!=extreme2){
        if(m_extreme2)
            disconnect(m_extreme2);
        m_extreme2=extreme2;
        connect(m_extreme2,SIGNAL(collided(Physics::PhysicsCollisionEvent*)),this,SLOT(onCollitionExtreme2(Physics::PhysicsCollisionEvent*)));
        connect(m_extreme2,SIGNAL(collitionsListChanged()),
                this,SLOT(checkCollitionAttachedElement2()));
        emit extreme2Changed();
    }
}

void Scaffold::checkCollitionAttachedElement1(){
    if(m_attached_element_e1){
        Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
        if(!sender_body_info->collitionTest(m_attached_element_e1->id())){
           reset();
        }
    }
}

void Scaffold::checkCollitionAttachedElement2(){
    if(m_attached_element_e2){
        Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
        if(!sender_body_info->collitionTest(m_attached_element_e2->id())){
           reset();
        }
    }
}

void Scaffold::reset(){
    //Very brutal
    if(!m_anchor_1.isNull()){
        if(m_anchor_1_offset>0){
            BeamPtr beam=m_anchor_1.toStrongRef().dynamicCast<Beam>();
            m_VMManager->staticsModule()->unifyBeam(beam);
        }
    }
    m_attached_element_e1=Q_NULLPTR;
    m_anchor_1.clear();
    m_anchor_1_offset=-1;
    if(!m_anchor_2.isNull()){
        if(m_anchor_2_offset>0){
            BeamPtr beam=m_anchor_2.toStrongRef().dynamicCast<Beam>();
            m_VMManager->staticsModule()->unifyBeam(beam);
        }
    }
    m_attached_element_e2=Q_NULLPTR;
    m_anchor_2.clear();
    m_anchor_2_offset=-1;
}



void Scaffold::setVMManager(AbstractVMManager * vmManager){
    if(vmManager!=m_VMManager){
        m_VMManager=vmManager;
    }
}

void Scaffold::onCollitionExtreme1(Physics::PhysicsCollisionEvent* e){
    if(!m_VMManager) return;
    Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
    if(sender_body_info->entities().size()>1){
        qWarning()<<"Ambiguous sender";
        return;
    }
    Qt3D::QEntity* sender=sender_body_info->entities().at(0);
    if(!sender) return;

    Qt3D::QEntity* targetEntity=m_VMManager->getEntity3D(e->target());

    if(targetEntity!=Q_NULLPTR && targetEntity==m_attached_element_e1){
        /*Update current status*/
    }
    else if(targetEntity!=Q_NULLPTR && m_attached_element_e1==Q_NULLPTR){
        AbstractElementViewModel* targetVM=m_VMManager->getAssociatedVM(targetEntity);
        if(targetVM->inherits("JointVM")) {
            JointVM* jointVM= static_cast<JointVM*>(targetVM);
            WeakJointPtr joint=jointVM->joint();
            m_anchor_1=joint;
            onAnchorsChanged();
            m_attached_element_e1=targetEntity;
        }
        else if(targetVM->inherits("BeamVM")){
            QMatrix4x4 transformMat=getTranformationMatrix(m_VMManager->sceneRoot(),false);
            BeamVM* beamVM=static_cast<BeamVM*>(targetVM);
            WeakBeamPtr beam=beamVM->beam();
            WeakJointPtr e1,e2;
            beam.toStrongRef()->extremes(e1,e2);
            QVector3D e1_pos=transformMat*e1.toStrongRef()->position();
            QVector3D e2_pos=transformMat*e2.toStrongRef()->position();
            QVector3D contactPoint=e->contactPointOnTarget();
            if(e1_pos.distanceToPoint(contactPoint)<discrete_step){
                m_anchor_1=e1;
            }
            else if(e2_pos.distanceToPoint(contactPoint)<discrete_step){
                m_anchor_1=e2;
            }
            else{
                qreal dist=round(e1_pos.distanceToPoint(contactPoint)/discrete_step);
                m_anchor_1=beam;
                m_anchor_1_offset=dist*discrete_step;
            }
            m_attached_element_e1=targetEntity;
            onAnchorsChanged();
        }
    }
}

void Scaffold::onCollitionExtreme2(Physics::PhysicsCollisionEvent* e){
    if(!m_VMManager) return;
    Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
    if(sender_body_info->entities().size()>1){
        qWarning()<<"Ambiguous sender";
        return;
    }
    Qt3D::QEntity* sender=sender_body_info->entities().at(0);
    if(!sender) return;

    Qt3D::QEntity* targetEntity=m_VMManager->getEntity3D(e->target());

    if(targetEntity!=Q_NULLPTR && targetEntity==m_attached_element_e2){
        /*Update current status*/
    }
    else if(targetEntity!=Q_NULLPTR && m_attached_element_e2==Q_NULLPTR){
        AbstractElementViewModel* targetVM=m_VMManager->getAssociatedVM(targetEntity);
        if(targetVM->inherits("JointVM")) {
            JointVM* jointVM= static_cast<JointVM*>(targetVM);
            WeakJointPtr joint=jointVM->joint();
            m_anchor_2=joint;
            onAnchorsChanged();
            m_attached_element_e2=targetEntity;
        }
        else if(targetVM->inherits("BeamVM")){
            QMatrix4x4 transformMat=getTranformationMatrix(m_VMManager->sceneRoot(),false);
            BeamVM* beamVM=static_cast<BeamVM*>(targetVM);
            WeakBeamPtr beam=beamVM->beam();
            WeakJointPtr e1,e2;
            beam.toStrongRef()->extremes(e1,e2);
            QVector3D e1_pos=transformMat*e1.toStrongRef()->position();
            QVector3D e2_pos=transformMat*e2.toStrongRef()->position();
            QVector3D contactPoint=e->contactPointOnTarget();
            if(e1_pos.distanceToPoint(contactPoint)<discrete_step){
                m_anchor_2=e1;
            }
            else if(e2_pos.distanceToPoint(contactPoint)<discrete_step){
                m_anchor_2=e2;
            }
            else{
                qreal dist=round(e1_pos.distanceToPoint(contactPoint)/discrete_step);
                m_anchor_2=beam;
                m_anchor_2_offset=dist*discrete_step;
            }
            m_attached_element_e2=targetEntity;
            onAnchorsChanged();
        }
    }

}

void Scaffold::onAnchorsChanged(){
    if(m_anchor_1!=Q_NULLPTR && m_anchor_2!=Q_NULLPTR){
        JointPtr e1,e2;
        if(m_anchor_1_offset<=0){
            e1=m_anchor_1.toStrongRef().dynamicCast<Joint>();
        }else{
            BeamPtr b=m_anchor_1.toStrongRef().dynamicCast<Beam>();
            if(!m_VMManager->staticsModule()->splitBeam(b,m_anchor_1_offset,e1))
                return;
        }
        if(m_anchor_2_offset<=0){
            e2=m_anchor_2.toStrongRef().dynamicCast<Joint>();
        }else{
            BeamPtr b=m_anchor_2.toStrongRef().dynamicCast<Beam>();
            if(!m_VMManager->staticsModule()->splitBeam(b,m_anchor_2_offset,e2))
                return;
        }
        BeamPtr new_beam=m_VMManager->staticsModule()->createBeam(e1,e2,"Scaffold");
        if(!new_beam.isNull()){
            new_beam->setMaterial("Wood");
            m_VMManager->createBeamVM(new_beam);
        }
    }
}
