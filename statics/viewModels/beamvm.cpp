#include "beamvm.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"

#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>
BeamVM::BeamVM(Beam* beam,Qt3D::QEntity* sceneRoot,QObject* parent):
    AbstractElementViewModel(sceneRoot,parent),
    m_component3D(Q_NULLPTR)
{
    m_beam=beam;

    connect(m_beam,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));

    //Element Specific bindings
    initView();
    connect(m_beam,SIGNAL(stressChanged()),this,SLOT(onBeamAxialStressChanged()));

}
BeamVM::~BeamVM(){
    if(m_component3D)
        m_component3D->deleteLater();
}


void BeamVM::initView(){

    QQmlComponent beamView_component(QQmlEngine::contextForObject(m_sceneRoot)->engine(),QUrl("qrc:/BeamView.qml"));
    Qt3D::QEntity* beamView= qobject_cast<Qt3D::QEntity*>(beamView_component.create(new QQmlContext(QQmlEngine::contextForObject(m_sceneRoot))));
    m_component3D=beamView;

    Joint* extreme1;
    Joint* extreme2;
    m_beam->extremes(extreme1,extreme2);

    m_component3D->setProperty("extreme1",extreme1->position());
    m_component3D->setProperty("extreme2",extreme2->position());

    onBeamAxialStressChanged();

    append_3D_resources(m_component3D);
    m_component3D->setParent(m_sceneRoot);

}


void BeamVM::onBeamAxialStressChanged(){
    if(m_component3D){
        int axial_type;
        qreal axial_force, dummy;
        m_beam->ForcesAndMoments(axial_type,axial_force,dummy,dummy,dummy,dummy,dummy,-1);
        m_component3D->setProperty("axialForceType",axial_type);
        m_component3D->setProperty("axialForce",fabs(axial_force));
    }
}





