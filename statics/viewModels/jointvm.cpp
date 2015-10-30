#include "jointvm.h"
#include "statics/elements/beam.h"
#include <QSet>

#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>

JointVM::JointVM(Joint* joint,Qt3D::QEntity* sceneRoot,QObject* parent):
    AbstractElementViewModel(sceneRoot,parent),
    m_component3D(Q_NULLPTR)
{
    m_joint=joint;
    initView();
    connect(m_joint,SIGNAL(reactionChanged()),this,SLOT(onReactionChanged()));
    connect(m_joint,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
}

void JointVM::onReactionChanged(){
    if(m_component3D){
        m_component3D->setProperty("reaction",m_joint->reaction());
    }
}

void JointVM::onConnectedBeamChanged(){
    QList<Beam*> previous_beams=m_beamsMap.keys();
    Q_FOREACH(Beam* b, m_joint->connectedBeams()){
        if(previous_beams.contains(b)){
           previous_beams.removeAll(b);
        }
        else{
            createEntityForBeam(b);
        }
    }
    Q_FOREACH(Beam* b,previous_beams){
        m_beamsMap[b]->deleteLater();
        m_beamsMap.remove(b);
    }

}


void JointVM::initView(){

    QQmlComponent jointView_component(QQmlEngine::contextForObject(m_sceneRoot)->engine(),QUrl("qrc:/JointView.qml"));
    Qt3D::QEntity *jointView;
    jointView = qobject_cast<Qt3D::QEntity*>(jointView_component.create(new QQmlContext(QQmlEngine::contextForObject(m_sceneRoot))));
    m_component3D=jointView;

    m_component3D->setProperty("position",m_joint->position());

    onReactionChanged();
    onConnectedBeamChanged();

    m_component3D->setParent(m_sceneRoot);
    append_3D_resources(m_component3D);

}

void JointVM::createEntityForBeam(Beam* b){
    QQmlComponent beamView_component(QQmlEngine::contextForObject(m_sceneRoot)->engine(),QUrl("qrc:/BeamView4JointView.qml"));
    Qt3D::QEntity *beamView;
    beamView = qobject_cast<Qt3D::QEntity*>(beamView_component.create(new QQmlContext(QQmlEngine::contextForObject(m_sceneRoot))));
    m_beamsMap[b]=beamView;

    beamView->setProperty("extreme1",m_joint->position());
    Joint* e1,*e2;
    b->extremes(e1,e2);
    if(m_joint!=e1){
        beamView->setProperty("extreme2",e1->position());
    }else{
        beamView->setProperty("extreme2",e2->position());
    }


    int axial_type;
    qreal axial_force, dummy;
    b->ForcesAndMoments(axial_type,axial_force,dummy,dummy,dummy,dummy,dummy,-1);
    beamView->setProperty("axialForceType",axial_type);
    beamView->setProperty("axialForce",fabs(axial_force));


    connect(b,SIGNAL(stressChanged()),this,SLOT(onConnectedBeamStressChanged()));
    //if the beam is destroyed, the notification will arrive through ConnectedBeamsChanged
    beamView->setParent(m_component3D);
}

void JointVM::onConnectedBeamStressChanged(){
    QObject* sender=QObject::sender();
    Beam* beam=qobject_cast<Beam*>(sender);
    if(!beam) return;
    if(!m_beamsMap.contains(beam)) return;
    Qt3D::QEntity* component3D=m_beamsMap[beam];
    int axial_type;
    qreal axial_force, dummy;
    beam->ForcesAndMoments(axial_type,axial_force,dummy,dummy,dummy,dummy,dummy,-1);
    component3D->setProperty("axialForceType",axial_type);
    component3D->setProperty("axialForce",fabs(axial_force));

}



