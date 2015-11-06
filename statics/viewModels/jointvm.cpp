#include "jointvm.h"
#include "statics/elements/beam.h"
#include <QSet>

#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>

JointVM::JointVM(JointPtr joint,Qt3D::QEntity* sceneRoot,QObject* parent):
    AbstractElementViewModel(sceneRoot,parent),
    m_component3D(Q_NULLPTR)
{
    m_joint=joint.toWeakRef();
    initView();
    connect(m_joint.data(),SIGNAL(reactionChanged()),this,SLOT(onReactionChanged()));
    connect(m_joint.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
}

JointVM::~JointVM(){
    if(m_component3D)
        m_component3D->deleteLater();
}


void JointVM::onReactionChanged(){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    if(m_component3D){
        m_component3D->setProperty("reaction",joint_str_ref->reaction());
    }
}

void JointVM::onConnectedBeamChanged(){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    QList<WeakBeamPtr> previous_beams=m_beamsMap.values();

    Q_FOREACH(WeakBeamPtr b, joint_str_ref->connectedBeams()){
        if(!previous_beams.contains(b)){
            createEntityForBeam(b.toStrongRef());
        }
    }

    Q_FOREACH(Qt3D::QEntity* e,m_beamsMap.keys()){
        if(m_beamsMap[e].isNull()){
            m_beamsMap.remove(e);
            e->deleteLater();
        }
    }

}


void JointVM::initView(){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    QQmlComponent jointView_component(QQmlEngine::contextForObject(m_sceneRoot)->engine(),QUrl("qrc:/JointView.qml"));
    Qt3D::QEntity *jointView;
    jointView = qobject_cast<Qt3D::QEntity*>(jointView_component.create(new QQmlContext(QQmlEngine::contextForObject(m_sceneRoot))));
    m_component3D=jointView;

    m_component3D->setProperty("position",joint_str_ref->position());

    onReactionChanged();
    onConnectedBeamChanged();

    m_component3D->setParent(m_sceneRoot);
    append_3D_resources(m_component3D);

}

void JointVM::createEntityForBeam(BeamPtr b){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    QQmlComponent beamView_component(QQmlEngine::contextForObject(m_sceneRoot)->engine(),QUrl("qrc:/BeamView4JointView.qml"));
    Qt3D::QEntity *beamView;
    beamView = qobject_cast<Qt3D::QEntity*>(beamView_component.create(new QQmlContext(QQmlEngine::contextForObject(m_sceneRoot))));
    m_beamsMap[beamView]=b.toWeakRef();

    beamView->setProperty("extreme1",joint_str_ref->position());
    WeakJointPtr e1,e2;
    b->extremes(e1,e2);
    if(m_joint!=e1){
        beamView->setProperty("extreme2",e1.toStrongRef()->position());
    }else{
        beamView->setProperty("extreme2",e2.toStrongRef()->position());
    }


    int axial_type;
    qreal axial_force, dummy;
    b->ForcesAndMoments(axial_type,axial_force,dummy,dummy,dummy,dummy,dummy,-1);
    beamView->setProperty("axialForceType",axial_type);
    beamView->setProperty("axialForce",fabs(axial_force));


    connect(b.data(),SIGNAL(stressChanged()),this,SLOT(onConnectedBeamStressChanged()));
    //if the beam is destroyed, the notification will arrive through ConnectedBeamsChanged
    beamView->setParent(m_component3D);
}

void JointVM::onConnectedBeamStressChanged(){
    QObject* sender=QObject::sender();
    Beam* beam=qobject_cast<Beam*>(sender);
    if(!beam) return;
    Qt3D::QEntity* component3D=Q_NULLPTR;
    Q_FOREACH(Qt3D::QEntity* e, m_beamsMap.keys()){
        if(m_beamsMap[e].data()==beam){
            component3D=e;
            break;
        }
    }
    if(component3D==Q_NULLPTR) return;
    int axial_type;
    qreal axial_force, dummy;
    beam->ForcesAndMoments(axial_type,axial_force,dummy,dummy,dummy,dummy,dummy,-1);
    component3D->setProperty("axialForceType",axial_type);
    component3D->setProperty("axialForce",fabs(axial_force));

}



