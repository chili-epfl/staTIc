#include "beamvm.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"
#include "statics/abstractvmmanager.h"

#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>


BeamVM::BeamVM(BeamPtr beam,Qt3DCore::QEntity* sceneRoot,QObject* parent):
    AbstractElementViewModel(sceneRoot,parent),
    m_component3D(Q_NULLPTR)
{
    m_beam=beam.toWeakRef();

    connect(m_beam.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));

    //Element Specific bindings
    initView();
    connect(m_beam.data(),SIGNAL(stressChanged()),this,SLOT(onBeamAxialStressChanged()));
    connect(m_beam.data(),SIGNAL(enableChanged(bool)),this,SLOT(onEnableChanged()));
    connect(m_beam.data(),SIGNAL(hasBeenSplit()),this,SLOT(onBeamSplit()));
    connect(m_beam.data(),SIGNAL(segmentsChanged()),this,SLOT(onSegmentsChanged()));

}
BeamVM::~BeamVM(){
    if(m_component3D)
        m_component3D->deleteLater();
}

void BeamVM::initView(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(!beam_str_ref->enable()) return;
    QQmlComponent beamView_component(QQmlEngine::contextForObject(m_sceneRoot)->engine(),QUrl("qrc:/element_views/Element_Views/BeamView.qml"));
    Qt3DCore::QEntity* beamView= qobject_cast<Qt3DCore::QEntity*>(beamView_component.create(new QQmlContext(QQmlEngine::contextForObject(m_sceneRoot))));
    m_component3D=beamView;

    m_component3D->setObjectName(beam_str_ref->objectName());
    WeakJointPtr extreme1,extreme2;
    beam_str_ref->extremes(extreme1,extreme2);

    m_component3D->setProperty("extreme1",extreme1.toStrongRef()->scaledPosition());
    m_component3D->setProperty("extreme2",extreme2.toStrongRef()->scaledPosition());
    m_component3D->setProperty("beamSize",beam_str_ref->scaledSize());

    onBeamAxialStressChanged();
    onSegmentsChanged();
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
        qreal axial_stress1,axial_stress2;
        beam_str_ref->stress(axial_stress1,dummy,dummy,dummy,1);
        beam_str_ref->stress(axial_stress2,dummy,dummy,dummy,2);

        if(axial_type>0)
            m_component3D->setProperty("relativeAxialStress",(0.5*fabs(axial_stress1)+0.5*fabs(axial_stress2))/8);
        else
            m_component3D->setProperty("relativeAxialStress",(0.5*fabs(axial_stress1)+0.5*fabs(axial_stress2))/12);

    }
}

void BeamVM::onEnableChanged(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(!beam_str_ref->enable() && m_component3D!=Q_NULLPTR){
        remove_3D_resources(m_component3D);
        m_component3D->deleteLater();
        m_component3D=Q_NULLPTR;
    }
    else if(beam_str_ref->enable() && m_component3D==Q_NULLPTR){
        initView();
    }
}

void BeamVM::onBeamSplit(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    AbstractVMManager* parent_vm=qobject_cast<AbstractVMManager*>(this->parent());
    if(!parent_vm) return;
    Q_FOREACH(WeakBeamPtr b,beam_str_ref->subParts()){
        if(!b.isNull()){
            parent_vm->createBeamVM(b.toStrongRef());
        }
    }
}

void BeamVM::onScaleFactorUpdated(){

}

void BeamVM::onSegmentsChanged(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(m_component3D){
       QVector4List segments= beam_str_ref->stress_segments();
       m_component3D->setProperty("segments",QVariant::fromValue(segments));
    }
}




