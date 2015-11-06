#include "beamvm.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"

#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>
BeamVM::BeamVM(BeamPtr beam,Qt3D::QEntity* sceneRoot,QObject* parent):
    AbstractElementViewModel(sceneRoot,parent),
    m_component3D(Q_NULLPTR)
{
    m_beam=beam.toWeakRef();

    connect(m_beam.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));

    //Element Specific bindings
    initView();
    connect(m_beam.data(),SIGNAL(stressChanged()),this,SLOT(onBeamAxialStressChanged()));
    connect(m_beam.data(),SIGNAL(enableChanged(bool)),this,SLOT(onEnableChanged()));
}
BeamVM::~BeamVM(){
    if(m_component3D)
        m_component3D->deleteLater();
}


void BeamVM::initView(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(!beam_str_ref->enable()) return;
    QQmlComponent beamView_component(QQmlEngine::contextForObject(m_sceneRoot)->engine(),QUrl("qrc:/BeamView.qml"));
    Qt3D::QEntity* beamView= qobject_cast<Qt3D::QEntity*>(beamView_component.create(new QQmlContext(QQmlEngine::contextForObject(m_sceneRoot))));
    m_component3D=beamView;

    m_component3D->setObjectName(this->objectName());
    WeakJointPtr extreme1,extreme2;
    beam_str_ref->extremes(extreme1,extreme2);

    m_component3D->setProperty("extreme1",extreme1.toStrongRef()->position());
    m_component3D->setProperty("extreme2",extreme2.toStrongRef()->position());

    onBeamAxialStressChanged();

    append_3D_resources(m_component3D);
    m_component3D->setParent(m_sceneRoot);

}


void BeamVM::onBeamAxialStressChanged(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(m_component3D){
        int axial_type;
        qreal axial_force, dummy;
        beam_str_ref->ForcesAndMoments(axial_type,axial_force,dummy,dummy,dummy,dummy,dummy,-1);
        m_component3D->setProperty("axialForceType",axial_type);
        m_component3D->setProperty("axialForce",fabs(axial_force));
    }
}

void BeamVM::onEnableChanged(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(!beam_str_ref->enable() && m_component3D!=Q_NULLPTR){
        m_component3D->deleteLater();
        m_component3D=Q_NULLPTR;
    }
    else if(!beam_str_ref->enable() && m_component3D!=Q_NULLPTR){
        initView();
    }
}




