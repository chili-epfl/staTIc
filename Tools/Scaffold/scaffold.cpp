#include "scaffold.h"

#include "statics/elements/beam.h"
#include "statics/elements/joint.h"
#include "statics/elements/trapezoidalforce.h"

#include "statics/viewModels/jointvm.h"
#include "statics/viewModels/beamvm.h"
#include "statics/viewModels/trapezoidalforcevm.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>

const int discrete_step = 30;
const int max_distance=50;

/*********************Support functions************************/
QMatrix4x4 getTranformationMatrix(Qt3DCore::QEntity* entity,bool local){
    if(!entity) return  QMatrix4x4();
    Qt3DCore::QTransform* entity_transform=NULL;
    for(Qt3DCore::QComponent* component: entity->components()){
        if(component->isEnabled()){
            if(component->inherits("Qt3DCore::QTransform")){
                entity_transform=qobject_cast<Qt3DCore::QTransform*>(component);
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

    m_anchor_1_offset=-1;
    m_anchor_2_offset=-1;

    m_refractory_timer=new QTimer(this);
    m_refractory_timer->setInterval(1000);
    m_refractory_timer->setSingleShot(true);
    //connect(m_refractory_timer,SIGNAL(timeout()),this,SLOT(reactivate()));
    m_active=true;
    stability_1=0;
    stability_2=0;
    stability_3=0;

}

void Scaffold::setLocalExtreme1Pos(QVector3D v)
{
    if(v!=m_extreme_pos1){
        m_extreme_pos1=v;
        emit localExtreme1PosChanged();
    }
}
void Scaffold::setLocalExtreme2Pos(QVector3D v)
{
    if(v!=m_extreme_pos2){
        m_extreme_pos2=v;
        emit localExtreme2PosChanged();
    }
}


void Scaffold::setExtreme1(Physics::PhysicsBodyInfo* extreme1){
    if(m_extreme1!=extreme1){
        if(m_extreme1)
            disconnect(m_extreme1);
        m_extreme1=extreme1;
        connect(m_extreme1,SIGNAL(collided(Physics::PhysicsCollisionEventPtr)),this,SLOT(onCollisionExtreme1(Physics::PhysicsCollisionEventPtr)));
        emit extreme1Changed();
    }
}


void Scaffold::setExtreme2(Physics::PhysicsBodyInfo * extreme2){
    if(m_extreme2!=extreme2){
        if(m_extreme2)
            disconnect(m_extreme2);
        m_extreme2=extreme2;
        connect(m_extreme2,SIGNAL(collided(Physics::PhysicsCollisionEventPtr)),this,SLOT(onCollisionExtreme2(Physics::PhysicsCollisionEventPtr)));
        emit extreme2Changed();
    }
}

void Scaffold::setVMManager(AbstractVMManager * vmManager){
    if(vmManager!=m_VMManager){
        m_VMManager=vmManager;
        vmManager->registerDependentObject(this);
        connect(m_VMManager,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    }
}


void Scaffold::reset(){
    if(!m_beam_1.isNull())
        m_beam_1.toStrongRef()->setEnable(true);

    if(!m_beam_2.isNull())
        m_beam_2.toStrongRef()->setEnable(true);

    /*Reset loads*/
//    if(m_child2parent_beams.size()>0){
//        Q_FOREACH(TrapezoidalForcePtr l,m_VMManager->staticsModule()->TPLoads()){
//            if(m_child2parent_beams.contains(l->beam())){
//                WeakBeamPtr _parent=m_child2parent_beams[l->beam()];
//                if(!_parent.isNull()){
//                    l->setBeam(_parent.toStrongRef());
//                    AbstractElementViewModel* vm_parent=m_VMManager->getAssociatedVM(_parent.toStrongRef().data());
//                    AbstractElementViewModel* vm_load=m_VMManager->getAssociatedVM(l.data());
//                    if(vm_parent!=Q_NULLPTR && vm_load!=Q_NULLPTR){
//                        TrapezoidalForceVM* vm_load_cast=static_cast<TrapezoidalForceVM*>(vm_load);
//                        vm_load_cast->setParentEntity(vm_parent->getRootEntity());
//                    }
//                }
//            }
//        }
//    }

    /**/
    if(!m_beam.isNull())
        m_VMManager->staticsModule()->removeBeam(m_beam.toStrongRef());
    if(!m_joint_1.isNull())
        m_VMManager->staticsModule()->removeJoint(m_joint_1.toStrongRef());
    if(!m_joint_2.isNull())
        m_VMManager->staticsModule()->removeJoint(m_joint_2.toStrongRef());

    m_beam.clear();
    m_joint_1.clear();
    m_joint_2.clear();    

    m_beam_1.clear();
    m_beam_2.clear();

    m_anchor_1.clear();
    m_anchor_2.clear();

    m_anchor_1_offset=-1;
    m_anchor_2_offset=-1;

    stability_1=0;
    stability_2=0;
    stability_3=0;
    //m_child2parent_beams.clear();
    m_active=true;
    if(m_extreme1!=Q_NULLPTR)
        connect(m_extreme1,SIGNAL(collided(Physics::PhysicsCollisionEventPtr)),this,SLOT(onCollisionExtreme1(Physics::PhysicsCollisionEventPtr)));
    if(m_extreme2!=Q_NULLPTR)
        connect(m_extreme2,SIGNAL(collided(Physics::PhysicsCollisionEventPtr)),this,SLOT(onCollisionExtreme2(Physics::PhysicsCollisionEventPtr)));

}



void Scaffold::onCollisionExtreme1(Physics::PhysicsCollisionEventPtr e){
    if(!m_active) return;
    if(!m_VMManager) return;

    //Check stability
    if(!m_anchor_1.isNull()){
        if(m_reference_extreme_pos1.distanceToPoint(m_extreme_pos1)>max_distance){
            m_anchor_1.clear();
            stability_1=0;
        }else{
            if(++stability_1 > 15 ){
                stability_1--;
                onAnchorsChanged();
            }
            return;
        }
    }

    Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
    if(sender_body_info->entities().size()>1){
        qWarning()<<"Ambiguous sender";
        return;
    }
    Qt3DCore::QEntity* sender=sender_body_info->entities().at(0);
    if(!sender) return;

    Qt3DCore::QEntity* targetEntity=m_VMManager->getEntity3D(e->target());
    if(targetEntity!=Q_NULLPTR){
        AbstractElementViewModel* targetVM=m_VMManager->getAssociatedVM(targetEntity);
        if(targetVM!=Q_NULLPTR && targetVM->inherits("JointVM")) {
            JointVM* jointVM= static_cast<JointVM*>(targetVM);
            WeakJointPtr joint=jointVM->joint();            
            if(joint.isNull()) return;
            m_anchor_1=joint;
            m_anchor_1_offset=-1;
            m_reference_extreme_pos1=m_extreme_pos1;
            stability_1=0;
        }
        else if(targetVM!=Q_NULLPTR && targetVM->inherits("BeamVM")){
            BeamVM* beamVM=static_cast<BeamVM*>(targetVM);
            WeakBeamPtr beam=beamVM->beam();
            if(beam.isNull()) return;
            WeakJointPtr e1,e2;
            beam.toStrongRef()->extremes(e1,e2);
            if(e1.isNull() || e2.isNull()) return;
            //EXTREME1=scaledLength()/2 EXTREME2=-scaledLength()/2
            qreal contactDistance=-1*(e->contactPointOnTargetLocal().y()-beam.toStrongRef()->scaledLength()/2);
            if(contactDistance<discrete_step){
                m_anchor_1=e1;
                m_anchor_1_offset=-1;
            }
            else if(contactDistance>beam.toStrongRef()->scaledLength()-discrete_step){
                m_anchor_1=e2;
                m_anchor_1_offset=-1;
            }
            else{
                m_anchor_1=beam;
                qreal segments=floor(beam.toStrongRef()->scaledLength()/discrete_step);
                qreal beam_discrete_step=beam.toStrongRef()->scaledLength()/segments;
                QVector3D _v=m_extreme_pos1-e1.toStrongRef()->scaledPosition();
                qreal projection=QVector3D::dotProduct(_v,(e2.toStrongRef()->scaledPosition()-e1.toStrongRef()->scaledPosition()).normalized());
                m_anchor_1_offset=(floor(projection)/beam_discrete_step)*beam_discrete_step*beam.toStrongRef()->length()/beam.toStrongRef()->scaledLength();

            }
            m_reference_extreme_pos1=m_extreme_pos1;
            stability_1=0;
        }
    }
}

void Scaffold::onCollisionExtreme2(Physics::PhysicsCollisionEventPtr e){
    if(!m_active) return;
    if(!m_VMManager) return;

    //Check stability

    if(!m_anchor_2.isNull()){
        if(m_reference_extreme_pos2.distanceToPoint(m_extreme_pos2)>max_distance){
            m_anchor_2.clear();
            stability_2=0;
        }else{
            if(++stability_2 > 15 ){
                stability_2--;
                onAnchorsChanged();
            }
            return;
        }
    }
    Physics::PhysicsBodyInfo* sender_body_info=qobject_cast<Physics::PhysicsBodyInfo*>(QObject::sender());
    if(sender_body_info->entities().size()>1){
        qWarning()<<"Ambiguous sender";
        return;
    }
    Qt3DCore::QEntity* sender=sender_body_info->entities().at(0);
    if(!sender) return;

    Qt3DCore::QEntity* targetEntity=m_VMManager->getEntity3D(e->target());
    if(targetEntity!=Q_NULLPTR){
        AbstractElementViewModel* targetVM=m_VMManager->getAssociatedVM(targetEntity);
        if(targetVM!=Q_NULLPTR && targetVM->inherits("JointVM")) {
            JointVM* jointVM= static_cast<JointVM*>(targetVM);
            WeakJointPtr joint=jointVM->joint();
            if(joint.isNull()) return;
            m_anchor_2=joint;
            m_anchor_2_offset=-1;
            m_reference_extreme_pos2=m_extreme_pos2;
            stability_2=0;
        }
        else if(targetVM!=Q_NULLPTR && targetVM->inherits("BeamVM")){
            BeamVM* beamVM=static_cast<BeamVM*>(targetVM);
            WeakBeamPtr beam=beamVM->beam();
            if(beam.isNull()) return;
            WeakJointPtr e1,e2;
            beam.toStrongRef()->extremes(e1,e2);
            if(e1.isNull() || e2.isNull()) return;
            //EXTREME1=scaledLength()/2 EXTREME2=-scaledLength()/2
            qreal contactDistance=-1*(e->contactPointOnTargetLocal().y()-beam.toStrongRef()->scaledLength()/2);
            if(contactDistance<discrete_step){
                m_anchor_2=e1;
                m_anchor_2_offset=-1;
            }
            else if(contactDistance>beam.toStrongRef()->scaledLength()-discrete_step){
                m_anchor_2=e2;
                m_anchor_2_offset=-1;
            }
            else{
                m_anchor_2=beam;
                qreal segments=floor(beam.toStrongRef()->scaledLength()/discrete_step);
                qreal beam_discrete_step=beam.toStrongRef()->scaledLength()/segments;
                QVector3D _v=m_extreme_pos2-e1.toStrongRef()->scaledPosition();
                qreal projection=QVector3D::dotProduct(_v,(e2.toStrongRef()->scaledPosition()-e1.toStrongRef()->scaledPosition()).normalized());

                m_anchor_2_offset=(floor(projection)/beam_discrete_step)*beam_discrete_step*beam.toStrongRef()->length()/beam.toStrongRef()->scaledLength();

                //m_anchor_2_offset=(floor(contactDistance/beam_discrete_step)*beam_discrete_step)* beam.toStrongRef()->length()/beam.toStrongRef()->scaledLength();
            }
            m_reference_extreme_pos2=m_extreme_pos2;
            stability_2=0;
        }
    }
}

void Scaffold::onAnchorsChanged(){
    if(!m_anchor_1.isNull() && !m_anchor_2.isNull() && m_anchor_1!=m_anchor_2){
        //Need to clear everything first....
        JointPtr e1,e2;
        if(m_anchor_1_offset<=0){
            e1=m_anchor_1.toStrongRef().dynamicCast<Joint>();
        }else{
            BeamPtr b=m_anchor_1.toStrongRef().dynamicCast<Beam>();            
            e1=splitBeam(b,m_anchor_1_offset);
            m_joint_1=e1.toWeakRef();//Ownership
            m_beam_1=b.toWeakRef();
        }
        if(m_anchor_2_offset<=0){
            e2=m_anchor_2.toStrongRef().dynamicCast<Joint>();
        }else{
            BeamPtr b=m_anchor_2.toStrongRef().dynamicCast<Beam>();
            e2=splitBeam(b,m_anchor_2_offset);
            m_joint_2=e2.toWeakRef();//Ownership
            m_beam_2=b.toWeakRef();
        }
        BeamPtr new_beam=m_VMManager->staticsModule()->createBeam(e1,e2,QSizeF(120,220),"Default","Scaffold");
        m_beam=new_beam.toWeakRef();
        if(!new_beam.isNull()){
            if(!m_joint_1.isNull())
                m_VMManager->createJointVM(m_joint_1.toStrongRef());
            if(!m_joint_2.isNull())
                m_VMManager->createJointVM(m_joint_2.toStrongRef());

            m_VMManager->createBeamVM(new_beam);

            connect(new_beam.data(),SIGNAL(destroyed(QObject*)),this,SLOT(reset()));
            /*Do not listen to the collition bodies anymore*/
            disconnect(m_extreme1);
            disconnect(m_extreme2);
            m_active=false;
            stability_3=0;
            connect(this,SIGNAL(localExtreme1PosChanged()),this,SLOT(checkPositionExtremes()));
            connect(this,SIGNAL(localExtreme2PosChanged()),this,SLOT(checkPositionExtremes()));

        }
        else{
            reset();
            qDebug("ERROR IN CREATING THE NEW BEAM");
        }
    }
}

void Scaffold::checkPositionExtremes()
{
    if(m_extreme_pos1.distanceToPoint(m_reference_extreme_pos1)>max_distance
            ||
            m_extreme_pos2.distanceToPoint(m_reference_extreme_pos2)>max_distance ){
        if(++stability_3 > 15){
            stability_3=0;
            disconnect(this,SIGNAL(localExtreme1PosChanged()),0,0);
            disconnect(this,SIGNAL(localExtreme2PosChanged()),0,0);
            reset();
        }
    }
    else{
        stability_3=0;
    }
}

JointPtr Scaffold::splitBeam(BeamPtr b, qreal offset){
    WeakJointPtr extreme1_w, extreme2_w;
    b->extremes(extreme1_w,extreme2_w);
    JointPtr extreme1=extreme1_w.toStrongRef();
    JointPtr extreme2=extreme2_w.toStrongRef();
    QVector3D direction=extreme2->position()-extreme1->position();
    direction.normalize();
    QVector3D new_joint_position=extreme1->position()+direction*offset;
    new_joint_position.setX(round(new_joint_position.x()/10)*10);
    new_joint_position.setY(round(new_joint_position.y()/10)*10);
    new_joint_position.setZ(round(new_joint_position.z()/10)*10);

    qDebug()<<"New joint"<<new_joint_position;
    qDebug()<<offset;
    qDebug()<<direction;
    JointPtr new_joint;
    if(m_VMManager->staticsModule()->is2D())
        new_joint=m_VMManager->staticsModule()->createJoint(new_joint_position,"",0,0,1,1,1,0);
    else
        new_joint=m_VMManager->staticsModule()->createJoint(new_joint_position);
    BeamPtr segment1=m_VMManager->staticsModule()->createBeam(extreme1,new_joint,b->size(),b->materialID(),b->objectName()+"_1");
    BeamPtr segment2=m_VMManager->staticsModule()->createBeam(new_joint,extreme2,b->size(),b->materialID(),b->objectName()+"_2");

    BeamVM* segment1_vm=m_VMManager->createBeamVM(segment1);
    BeamVM* segment2_vm=m_VMManager->createBeamVM(segment2);

    Q_FOREACH(TrapezoidalForcePtr l,m_VMManager->staticsModule()->TPLoads()){
        if(b.toWeakRef()==l->beam()){
            QVector3D relative_position;
            QVector2D extent;
            l->relativePosition(relative_position,extent);
            if(relative_position.distanceToPoint(QVector3D(0,0,0))<relative_position.distanceToPoint(QVector3D(1,1,1))){
                l->setBeam(segment1);
                TrapezoidalForceVM* l_vm=static_cast<TrapezoidalForceVM*>(m_VMManager->getAssociatedVM(l.data()));
                l_vm->setParentEntity(segment1_vm->component3D());
            }
            else{
                l->setBeam(segment2);
                TrapezoidalForceVM* l_vm=static_cast<TrapezoidalForceVM*>(m_VMManager->getAssociatedVM(l.data()));
                l_vm->setParentEntity(segment2_vm->component3D());
            }
        }
    }
    b->setEnable(false);

    return new_joint;
}
