#include "beamvm.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"
#include "statics/abstractvmmanager.h"
#include "statics/frame3dd/frame3ddvmmanager.h"

#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>

QQmlComponent* BeamVM::m_qqmlcomponent=NULL;

BeamVM::BeamVM(BeamPtr beam,Qt3DEntityPtr sceneRoot,QObject* parent):
    AbstractElementViewModel(sceneRoot,parent),
    m_component3D(Q_NULLPTR)
{
    m_qqmlcontext=Q_NULLPTR;
    m_beam=beam.toWeakRef();
    connect(m_beam.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));

    //Element Specific bindings
    initView();
    connect(m_beam.data(),SIGNAL(stressChanged()),this,SLOT(onBeamAxialStressChanged()));
    connect(m_beam.data(),SIGNAL(enableChanged(bool)),this,SLOT(onEnableChanged()));
    connect(m_beam.data(),SIGNAL(hasBeenSplit()),this,SLOT(onBeamSplit()));
    connect(m_beam.data(),SIGNAL(segmentsChanged()),this,SLOT(onSegmentsChanged()));
    connect(m_beam.data(),SIGNAL(parametersChanged()),this,SLOT(onParametersChanged()));
    connect(m_beam.data(),SIGNAL(tangibleSectionChanged()),this,SLOT(onTangibleSectionChanged()));
    connect(m_beam.data(),SIGNAL(extremeDisplacementsChanged()),this,SLOT(onExtremeDisplacementChanged()));

    /*Material*/
    connect(m_component3D,SIGNAL(materialIDChanged()),this,SLOT(onMaterialChangedVMSide()));
    /*Size*/
    connect(m_component3D,SIGNAL(realBeamSizeChanged()),this,SLOT(onBeamSizeChangedVMSide()));
}

BeamVM::~BeamVM(){
    if(m_component3D){
        m_component3D->deleteLater();
    }
    //m_qqmlcomponent=Q_NULLPTR;
    m_qqmlcontext=Q_NULLPTR;
    m_component3D=Q_NULLPTR;
}

AbstractElement *BeamVM::element()
{
    if(m_beam.isNull())
        return Q_NULLPTR;
    else{
        return m_beam.toStrongRef().data();
    }
}

void BeamVM::initView(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(!beam_str_ref->enable()) return;
    if(m_qqmlcomponent==NULL){
        m_qqmlcomponent=new QQmlComponent(qmlEngine(m_sceneRoot),m_sceneRoot);
        m_qqmlcomponent->loadUrl(QUrl("qrc:/element_views/Element_Views/BeamView.qml"));
        connect(m_qqmlcomponent,&QQmlComponent::destroyed,[]() {
            BeamVM::m_qqmlcomponent=NULL;
        });
    }
    m_qqmlcontext=new QQmlContext(qmlContext(m_sceneRoot),m_component3D);
    Qt3DCore::QEntity* beamView= qobject_cast<Qt3DCore::QEntity*>(m_qqmlcomponent->beginCreate(m_qqmlcontext));
    m_qqmlcontext->setContextObject(beamView);
    m_component3D=beamView;
//    m_component3D->setObjectName(beam_str_ref->objectName());
//    WeakJointPtr extreme1,extreme2;
//    beam_str_ref->extremes(extreme1,extreme2);
//    m_component3D->setProperty("extreme1",extreme1.toStrongRef()->scaledPosition());
//    m_component3D->setProperty("extreme2",extreme2.toStrongRef()->scaledPosition());
//    m_component3D->setProperty("extreme1_name",extreme1.toStrongRef()->objectName());
//    m_component3D->setProperty("extreme2_name",extreme2.toStrongRef()->objectName());
//    onParametersChanged();
//    onBeamAxialStressChanged();
//    onSegmentsChanged();
//    onTangibleSectionChanged();
//    onExtremeDisplacementChanged();
    m_component3D->setProperty("backend_entity",QVariant::fromValue(beam_str_ref.data()));
    m_qqmlcomponent->completeCreate();
    m_component3D->setParent(m_sceneRoot);
    //append_3D_resources(m_component3D,true);


}

void BeamVM::onParametersChanged(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(m_component3D){
        m_component3D->setProperty("beamSize",beam_str_ref->scaledSize());
        m_component3D->setProperty("realBeamSize",beam_str_ref->size());
        m_component3D->setProperty("materialID",beam_str_ref->materialID());
    }
}

void BeamVM::onTangibleSectionChanged()
{
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(m_component3D){
        m_component3D->setProperty("tangibleSection",beam_str_ref->tangibleSection());
    }
}

void BeamVM::onExtremeDisplacementChanged(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(m_component3D){
        QVector3D ex1Disp,ex2Disp;
        beam_str_ref->extremeDisplacements(ex1Disp,ex2Disp);
        //        qDebug()<<ex1Disp<<ex2Disp;
        m_component3D->setProperty("extreme1Displacement",ex1Disp*AbstractStaticsModule::modelScale());
        m_component3D->setProperty("extreme2Displacement",ex2Disp*AbstractStaticsModule::modelScale());
    }
}

void BeamVM::onBeamAxialStressChanged(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(m_component3D){
        int axial_type;
        qreal axial_force, dummy;
        beam_str_ref->ForcesAndMoments(axial_type,axial_force,dummy,dummy,dummy,dummy,dummy,3);
        m_component3D->setProperty("axialForceType",axial_type);
        m_component3D->setProperty("axialForce",fabs(axial_force));
        qreal axial_stress;
        beam_str_ref->stressRatio(axial_stress,dummy,3);
        m_component3D->setProperty("relativeAxialStress",axial_stress);
        QVector4D min_disp,max_disp;
        beam_str_ref->peakDisplacements(min_disp,max_disp);
        m_component3D->setProperty("dispY",qMax(fabs(min_disp.y()),fabs(max_disp.y())));
        m_component3D->setProperty("relativeDisplacementY",qMax(fabs(min_disp.y()),fabs(max_disp.y()))*250/beam_str_ref->length());
        m_component3D->setProperty("dispZ",qMax(fabs(min_disp.z()),fabs(max_disp.z())));
        m_component3D->setProperty("relativeDisplacementZ",qMax(fabs(min_disp.z()),fabs(max_disp.z()))*250/beam_str_ref->length());
    }
}

void BeamVM::onEnableChanged(){
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(!beam_str_ref->enable() && m_component3D!=Q_NULLPTR){
        deleteLater();
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

void BeamVM::onMaterialChangedVMSide()
{
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(!beam_str_ref.isNull() && m_component3D!=Q_NULLPTR){
        beam_str_ref->setMaterial(m_component3D->property("materialID").toString());
    }
}

void BeamVM::onBeamSizeChangedVMSide()
{
    BeamPtr beam_str_ref=m_beam.toStrongRef();
    if(!beam_str_ref.isNull() && m_component3D!=Q_NULLPTR){
        beam_str_ref->setSize(m_component3D->property("realBeamSize").toSizeF());
    }

}




