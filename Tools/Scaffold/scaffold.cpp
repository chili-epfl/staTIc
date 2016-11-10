#include "scaffold.h"

#include "statics/elements/beam.h"
#include "statics/elements/joint.h"
#include "statics/elements/trapezoidalforce.h"

#include "statics/viewModels/jointvm.h"
#include "statics/viewModels/beamvm.h"
#include "statics/viewModels/trapezoidalforcevm.h"

#include "logger.h"

const int discrete_step = 40;
const int max_distance=50;
const int stability_threshold=1000;//milliseconds

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
    m_newSupport_possible=0;
    connect(this,SIGNAL(localExtreme1PosChanged()),this,SLOT(checkPosition4NewSupport()));
    connect(this,SIGNAL(localExtreme2PosChanged()),this,SLOT(checkPosition4NewSupport()));

    m_name_s1_is_available=true;
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
    m_name_s1_is_available=true;
    m_logger_map.clear();

    QVariantList removed_beams;
    QVariantList removed_joints;
    QVariantList enabled_beams;

    if(!m_beam_1.isNull()){
        m_beam_1.toStrongRef()->setEnable(true);
        enabled_beams.append(m_beam_1.toStrongRef()->objectName());

    }

    if(!m_beam_2.isNull()){
        m_beam_2.toStrongRef()->setEnable(true);
        enabled_beams.append(m_beam_2.toStrongRef()->objectName());
    }

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
    if(!m_beam.isNull()){
        removed_beams.append(m_beam.toStrongRef()->objectName());
        m_VMManager->staticsModule()->removeBeam(m_beam.toStrongRef());
    }
    if(!m_joint_1.isNull()){
         removed_joints.append(m_joint_1.toStrongRef()->objectName());
        m_VMManager->staticsModule()->removeJoint(m_joint_1.toStrongRef());
    }
    if(!m_joint_2.isNull()){
        removed_joints.append(m_joint_2.toStrongRef()->objectName());
        m_VMManager->staticsModule()->removeJoint(m_joint_2.toStrongRef());
    }

    if(removed_beams.size()>0)
        m_logger_map["Removed_beams"]=removed_beams;
    if(removed_joints.size()>0)
        m_logger_map["Removed_joints"]=removed_joints;
    if(enabled_beams.size()>0)
        m_logger_map["Enabled_beams"]=enabled_beams;

    m_logger.log("Scaffold_cleanup",m_logger_map);

    m_beam.clear();
    m_joint_1.clear();
    m_joint_2.clear();    

    m_beam_1.clear();
    m_beam_2.clear();

    m_anchor_1.clear();
    m_anchor_2.clear();

    m_anchor_1_offset=-1;
    m_anchor_2_offset=-1;

    //m_child2parent_beams.clear();
    m_active=true;
    if(m_extreme1!=Q_NULLPTR)
        connect(m_extreme1,SIGNAL(collided(Physics::PhysicsCollisionEventPtr)),this,SLOT(onCollisionExtreme1(Physics::PhysicsCollisionEventPtr)));
    if(m_extreme2!=Q_NULLPTR)
        connect(m_extreme2,SIGNAL(collided(Physics::PhysicsCollisionEventPtr)),this,SLOT(onCollisionExtreme2(Physics::PhysicsCollisionEventPtr)));
    m_newSupport_possible=0;
    connect(this,SIGNAL(localExtreme1PosChanged()),this,SLOT(checkPosition4NewSupport()));
    connect(this,SIGNAL(localExtreme2PosChanged()),this,SLOT(checkPosition4NewSupport()));
}



void Scaffold::onCollisionExtreme1(Physics::PhysicsCollisionEventPtr e){
    if(!m_active|| m_ignoreEvents) return;
    if(!m_VMManager) return;
    if(!m_VMManager->staticsModule()) return;
    //if(m_newSupport_possible==1) return;
    //Check stability
    if(!m_anchor_1.isNull()){
        if(m_reference_extreme_pos1.distanceToPoint(m_extreme_pos1)>max_distance){
            m_anchor_1.clear();
        }else{
            if( timer_1.elapsed() > stability_threshold ){
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
        AbstractElementViewModel* targetVM=m_VMManager->getAssociatedVM(targetEntity->id());
        if(targetVM!=Q_NULLPTR && targetVM->inherits("JointVM")) {
            JointVM* jointVM= static_cast<JointVM*>(targetVM);
            WeakJointPtr joint=jointVM->joint();            
            if(joint.isNull()) return;
            m_anchor_1=joint;
            m_anchor_1_offset=-1;
            m_reference_extreme_pos1=m_extreme_pos1;
            timer_1.start();
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
            timer_1.start();
        }
    }
}

void Scaffold::onCollisionExtreme2(Physics::PhysicsCollisionEventPtr e){
    if(!m_active|| m_ignoreEvents) return;
    if(!m_VMManager) return;
    if(!m_VMManager->staticsModule()) return;
    //if(m_newSupport_possible==2) return;

    //Check stability
    if(!m_anchor_2.isNull()){
        if(m_reference_extreme_pos2.distanceToPoint(m_extreme_pos2)>max_distance){
            m_anchor_2.clear();

        }else{
            if(timer_2.elapsed() > stability_threshold ){
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
        AbstractElementViewModel* targetVM=m_VMManager->getAssociatedVM(targetEntity->id());
        if(targetVM!=Q_NULLPTR && targetVM->inherits("JointVM")) {
            JointVM* jointVM= static_cast<JointVM*>(targetVM);
            WeakJointPtr joint=jointVM->joint();
            if(joint.isNull()) return;
            m_anchor_2=joint;
            m_anchor_2_offset=-1;
            m_reference_extreme_pos2=m_extreme_pos2;
            timer_2.start();
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
            timer_2.start();
        }
    }
}

void Scaffold::onAnchorsChanged(){
    if( (!m_anchor_1.isNull() || m_newSupport_possible==1 ) &&
            (!m_anchor_2.isNull() || m_newSupport_possible==2 )
            && m_anchor_1!=m_anchor_2){
        //Need to clear everything first....
        JointPtr e1,e2;
        m_logger_map.clear();
        QVariantList logger_tmp;
        if(!m_anchor_1.isNull()){
            if(m_anchor_1_offset<=0){
                e1=m_anchor_1.toStrongRef().dynamicCast<Joint>();
            }else{
                BeamPtr b=m_anchor_1.toStrongRef().dynamicCast<Beam>();
                e1=splitBeam(b,m_anchor_1_offset);
                m_joint_1=e1.toWeakRef();//Ownership
                m_beam_1=b.toWeakRef();
            }
        }
        else if(m_newSupport_possible==1){
            if(m_name_s1_is_available){
                e1=vmManager()->staticsModule()->createJoint(m_newSupport_position,"S1",1,1,1,1,1,1);
                m_name_s1_is_available=false;
            }
            else
                e1=vmManager()->staticsModule()->createJoint(m_newSupport_position,"S2",1,1,1,1,1,1);

            logger_tmp.clear();
            if(m_logger_map.contains("created_joints")){
                logger_tmp=m_logger_map["created_joints"].toList();
            }
            logger_tmp.append(e1->objectName());
            m_logger_map["created_joints"]=logger_tmp;
            m_joint_1=e1.toWeakRef();//Ownership
        }
        if(!m_anchor_2.isNull()){
            if(m_anchor_2_offset<=0){
                e2=m_anchor_2.toStrongRef().dynamicCast<Joint>();
            }else{
                BeamPtr b=m_anchor_2.toStrongRef().dynamicCast<Beam>();
                e2=splitBeam(b,m_anchor_2_offset);
                m_joint_2=e2.toWeakRef();//Ownership
                m_beam_2=b.toWeakRef();
            }
        }
        else if(m_newSupport_possible==2){
            if(m_name_s1_is_available){
                e2=vmManager()->staticsModule()->createJoint(m_newSupport_position,"S1",1,1,1,1,1,1);
                m_name_s1_is_available=false;
            }else
                e2=vmManager()->staticsModule()->createJoint(m_newSupport_position,"S2",1,1,1,1,1,1);

            logger_tmp.clear();
            if(m_logger_map.contains("created_joints")){
                logger_tmp=m_logger_map["created_joints"].toList();
            }
            logger_tmp.append(e2->objectName());
            m_logger_map["created_joints"]=logger_tmp;

            m_joint_2=e2.toWeakRef();//Ownership
        }

        BeamPtr new_beam=m_VMManager->staticsModule()->createBeam(e1,e2,QSizeF(120,220),"Default",e1->objectName()+e2->objectName());

        m_beam=new_beam.toWeakRef();
        if(!new_beam.isNull()){
            if(!m_joint_1.isNull())
                m_VMManager->createJointVM(m_joint_1.toStrongRef());
            if(!m_joint_2.isNull())
                m_VMManager->createJointVM(m_joint_2.toStrongRef());

            m_VMManager->createBeamVM(new_beam);
            //connect(new_beam.data(),SIGNAL(destroyed(QObject*)),this,SLOT(reset()));
            /*Do not listen to the collition bodies anymore*/
            disconnect(m_extreme1);
            disconnect(m_extreme2);
            disconnect(this,SIGNAL(localExtreme1PosChanged()),this,SLOT(checkPosition4NewSupport()));
            disconnect(this,SIGNAL(localExtreme2PosChanged()),this,SLOT(checkPosition4NewSupport()));
            m_active=false;
            stability_3=0;
            connect(this,SIGNAL(localExtreme1PosChanged()),this,SLOT(checkPositionExtremes()));
            connect(this,SIGNAL(localExtreme2PosChanged()),this,SLOT(checkPositionExtremes()));

            logger_tmp.clear();
            if(m_logger_map.contains("created_beams")){
                logger_tmp=m_logger_map["created_beams"].toList();
            }
            logger_tmp.append(new_beam->objectName());
            m_logger_map["created_beams"]=logger_tmp;

            m_logger.log("Scaffold_create",m_logger_map);
        }
        else{
            reset();
            qDebug("ERROR IN CREATING THE NEW BEAM");
        }
    }
}

void Scaffold::checkPositionExtremes()
{
    if(m_ignoreEvents) return;
    if(m_extreme_pos1.distanceToPoint(m_reference_extreme_pos1)>max_distance
            ||
            m_extreme_pos2.distanceToPoint(m_reference_extreme_pos2)>max_distance ){
        if(++stability_3 > 30){
            stability_3=0;
            disconnect(this,SIGNAL(localExtreme1PosChanged()),this,SLOT(checkPositionExtremes()));
            disconnect(this,SIGNAL(localExtreme2PosChanged()),this,SLOT(checkPositionExtremes()));
            reset();
        }
    }
    else{
        stability_3=0;
    }
}

void Scaffold::checkPosition4NewSupport(){
    if(!m_active || m_ignoreEvents) return;
    if(!m_VMManager) return;
    if(!m_VMManager->staticsModule()) return;
    if(m_extreme1==Q_NULLPTR || m_extreme2==Q_NULLPTR) return;

    if(m_newSupport_possible>0) {
        if((m_newSupport_possible==1 && m_reference_extreme_pos1.distanceToPoint(m_extreme_pos1)>
                max_distance) ||
                (m_newSupport_possible==2 && m_reference_extreme_pos2.distanceToPoint(m_extreme_pos2)>
                max_distance)){
            m_newSupport_possible=0;
        }
        else{
            if(stability_4>30){
                onAnchorsChanged();
            }
            else{
                stability_4++;
            }
            return;
        }
    }
    if(m_anchor_1.isNull() && m_extreme_pos1.y()<discrete_step){
        if(m_VMManager->staticsModule()->is2D()){
            if(fabs(m_extreme_pos1.z())>20) return;
            m_newSupport_position=QVector3D(m_extreme_pos1.x(),0,0)/m_VMManager->staticsModule()->modelScale();
        }
        else
            m_newSupport_position=QVector3D(m_extreme_pos1.x(),0,m_extreme_pos1.z())/m_VMManager->staticsModule()->modelScale();
        m_reference_extreme_pos1=m_extreme_pos1;
        m_newSupport_possible=1;
        stability_4=0;
    }
    else if(m_anchor_2.isNull() && m_extreme_pos2.y()<discrete_step){
        if(m_VMManager->staticsModule()->is2D()){
            if(fabs(m_extreme_pos2.z())>20) return;
            m_newSupport_position=QVector3D(m_extreme_pos2.x(),0,0)/m_VMManager->staticsModule()->modelScale();
        }
        else
            m_newSupport_position=QVector3D(m_extreme_pos2.x(),0,m_extreme_pos2.z())/m_VMManager->staticsModule()->modelScale();
        m_reference_extreme_pos2=m_extreme_pos2;
        m_newSupport_possible=2;
        stability_4=0;
    }
    if(m_newSupport_possible>0){
        Q_FOREACH(JointPtr j,m_VMManager->staticsModule()->joints()){
            if(j->scaledPosition().distanceToPoint(m_newSupport_position*m_VMManager->staticsModule()->modelScale())<max_distance){
                if(m_newSupport_possible==1){
                    m_anchor_1=j.toWeakRef();
                    m_anchor_1_offset=-1;
                    timer_1.start();
                }
                else
                {
                    m_anchor_2=j.toWeakRef();
                    m_anchor_2_offset=-1;
                    timer_2.start();
                }
                m_newSupport_possible=0;
                break;
            }
        }
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

    QVariantList logger_tmp;

    JointPtr new_joint;
    if(m_VMManager->staticsModule()->is2D())
        if(m_name_s1_is_available){
            new_joint=m_VMManager->staticsModule()->createJoint(new_joint_position,"S1",0,0,1,1,1,0);
            m_name_s1_is_available=false;
        }
        else
            new_joint=m_VMManager->staticsModule()->createJoint(new_joint_position,"S2",0,0,1,1,1,0);
    else
        if(m_name_s1_is_available){
            new_joint=m_VMManager->staticsModule()->createJoint(new_joint_position,"S1");
            m_name_s1_is_available=false;
        }
        else
            new_joint=m_VMManager->staticsModule()->createJoint(new_joint_position,"S2");

    BeamPtr segment1=m_VMManager->staticsModule()->createBeam(extreme1,new_joint,b->size(),b->materialID(),extreme1->objectName()+new_joint->objectName());
    BeamPtr segment2=m_VMManager->staticsModule()->createBeam(new_joint,extreme2,b->size(),b->materialID(),new_joint->objectName()+extreme2->objectName());

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

    logger_tmp.clear();
    if(m_logger_map.contains("split_beams")){
        logger_tmp=m_logger_map["split_beams"].toList();
    }
    logger_tmp.append(b->objectName());
    m_logger_map["split_beams"]=logger_tmp;

    logger_tmp.clear();
    if(m_logger_map.contains("created_joints")){
        logger_tmp=m_logger_map["created_joints"].toList();
    }
    logger_tmp.append(new_joint->objectName());
    m_logger_map["created_joints"]=logger_tmp;

    logger_tmp.clear();
    if(m_logger_map.contains("created_beams")){
        logger_tmp=m_logger_map["created_beams"].toList();
    }
    logger_tmp.append(segment1->objectName());
    logger_tmp.append(segment2->objectName());
    m_logger_map["created_beams"]=logger_tmp;

    return new_joint;
}
