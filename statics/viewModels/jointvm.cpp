#include "jointvm.h"
#include "statics/elements/beam.h"
#include "statics/frame3dd/frame3ddvmmanager.h"
#include <QSet>

#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>

const QString classname4beamEntity="Beam4Joint";

JointVM::JointVM(JointPtr joint,Qt3DCore::QEntity* sceneRoot,QObject* parent):
    AbstractElementViewModel(sceneRoot,parent),
    m_component3D(Q_NULLPTR)
{
    m_qqmlcomponent=Q_NULLPTR;
    m_qqmlcontext=Q_NULLPTR;
    m_joint=joint.toWeakRef();
    initView();
    connect(m_joint.data(),SIGNAL(reactionChanged()),this,SLOT(onReactionChanged()));
    connect(m_joint.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    connect(m_joint.data(),SIGNAL(connectedBeamsChanged()),this,SLOT(onConnectedBeamChanged()));
}

JointVM::JointVM(JointPtr joint, Qt3DCore::QEntity *entity, QQmlComponent *component, QQmlContext *context, Qt3DCore::QEntity *sceneRoot, QObject *parent)
    : AbstractElementViewModel(sceneRoot,parent)
{
    m_qqmlcomponent=component;
    m_qqmlcontext=context;
    m_component3D=entity;
    m_joint=joint.toWeakRef();
    initView();
    connect(m_joint.data(),SIGNAL(reactionChanged()),this,SLOT(onReactionChanged()));
    connect(m_joint.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
}

JointVM::~JointVM(){
    m_component3D->setEnabled(false);
    m_component3D->setProperty("position",QVector3D(0,0,400));
    Frame3DDVMManager* parent_vm_manager=static_cast<Frame3DDVMManager*>(parent());
    parent_vm_manager->addPoolEntity(metaObject()->className(),m_component3D,m_qqmlcontext,m_qqmlcomponent);

    m_qqmlcomponent=Q_NULLPTR;
    m_qqmlcontext=Q_NULLPTR;
    m_component3D=Q_NULLPTR;
    Q_FOREACH(Qt3DCore::QEntity* e,m_beamsMap.keys()){
            auto extras=m_beams_entities_extras[e];
            parent_vm_manager->addPoolEntity(metaObject()->className()+classname4beamEntity,
                                             e,extras.second,extras.first);
            e->setEnabled(false);
            e->setProperty("parent_joint",QVariant());
    }
}

AbstractElement *JointVM::element()
{
    if(m_joint.isNull())
        return Q_NULLPTR;
    else{
        return m_joint.toStrongRef().data();
    }
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

    Q_FOREACH(Qt3DCore::QEntity* e,m_beamsMap.keys()){
        if(m_beamsMap[e].isNull() || !m_beamsMap[e].toStrongRef()->enable()){
            m_beamsMap.remove(e);
            auto extras=m_beams_entities_extras[e];
            Frame3DDVMManager* parent_vm_manager=static_cast<Frame3DDVMManager*>(parent());
            parent_vm_manager->addPoolEntity(metaObject()->className()+classname4beamEntity,
                                             e,extras.second,extras.first);
            e->setEnabled(false);
            e->setProperty("parent_joint",QVariant());
        }
    }
    QVariantList connected_beam;

    Q_FOREACH(Qt3DCore::QEntity* e,m_beamsMap.keys()){
        connected_beam.append(QVariant::fromValue(e));
    }
   m_component3D->setProperty("connected_beams",connected_beam);

}


void JointVM::initView(){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    if(m_component3D==Q_NULLPTR){
        m_qqmlcomponent=new QQmlComponent(qmlEngine(m_sceneRoot),m_sceneRoot);
        m_qqmlcomponent->loadUrl(QUrl("qrc:/element_views/Element_Views/JointView.qml"));
        m_qqmlcontext=new QQmlContext(qmlContext(m_sceneRoot));
        Qt3DCore::QEntity* jointView= qobject_cast<Qt3DCore::QEntity*>(m_qqmlcomponent->create(m_qqmlcontext));
        m_qqmlcontext->setContextObject(jointView);
        m_component3D=jointView;
        m_component3D->setParent(m_sceneRoot);

    }
    m_component3D->setObjectName(joint_str_ref->objectName());
    m_component3D->setProperty("position",joint_str_ref->scaledPosition());
    onReactionChanged();
    onConnectedBeamChanged();
    append_3D_resources(m_component3D);
    m_component3D->setEnabled(true);

}

void JointVM::createEntityForBeam(BeamPtr b){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    Frame3DDVMManager* parent_vm_manager=static_cast<Frame3DDVMManager*>(parent());
    QQmlComponent* beamView_component;
    Qt3DCore::QEntity *beamView;
    QQmlContext* beamContext;
    parent_vm_manager->tryRetrivePoolEntity(metaObject()->className()+classname4beamEntity,beamView,beamContext,beamView_component);
    if(beamView==Q_NULLPTR){
        beamView_component=new QQmlComponent(qmlEngine(m_sceneRoot),m_sceneRoot);
        beamView_component->loadUrl(QUrl("qrc:/element_views/Element_Views/BeamView4JointView.qml"));
        beamContext=new QQmlContext(qmlContext(m_sceneRoot));
        beamView = qobject_cast<Qt3DCore::QEntity*>(beamView_component->create(beamContext));
        beamView->setParent(m_sceneRoot);
    }
    m_beamsMap[beamView]=b.toWeakRef();
    QPair<QQmlComponent*,QQmlContext*> extras(beamView_component,beamContext);
    m_beams_entities_extras[beamView]=extras;

    beamView->setObjectName(b->objectName());
    beamView->setProperty("extreme1",joint_str_ref->scaledPosition());
    WeakJointPtr e1,e2;
    b->extremes(e1,e2);


    int extreme_of_joint,extreme_not_on_joint;
    if(m_joint!=e1){
        beamView->setProperty("flip_extremes",true);
        extreme_of_joint=2;
        extreme_not_on_joint=1;
        beamView->setProperty("extreme2",e1.toStrongRef()->scaledPosition());
    }else{
        beamView->setProperty("flip_extremes",false);
        extreme_of_joint=1;
        extreme_not_on_joint=2;
        beamView->setProperty("extreme2",e2.toStrongRef()->scaledPosition());
    }


    int axial_type;
    qreal axial_force, dummy;
    b->ForcesAndMoments(axial_type,axial_force,dummy,dummy,dummy,dummy,dummy,extreme_of_joint);
    beamView->setProperty("axialForceType",axial_type);
    beamView->setProperty("axialForce",fabs(axial_force));

    qreal shearY,shearZ;
    b->ForcesAndMoments(axial_type,axial_force,shearY,shearZ,dummy,dummy,dummy,extreme_of_joint);
    beamView->setProperty("axialForce_extreme1",axial_force);
    beamView->setProperty("shearYForce_extreme1",shearY);
    beamView->setProperty("shearZForce_extreme1",shearZ);

    b->ForcesAndMoments(axial_type,axial_force,shearY,shearZ,dummy,dummy,dummy,extreme_not_on_joint);
    beamView->setProperty("axialForce_extreme2",axial_force);
    beamView->setProperty("shearYForce_extreme2",shearY);
    beamView->setProperty("shearZForce_extreme2",shearZ);

    connect(b.data(),SIGNAL(stressChanged()),this,SLOT(onConnectedBeamStressChanged()));
    //if the beam is destroyed, the notification will arrive through ConnectedBeamsChanged
    beamView->setProperty("visible",m_component3D->property("visible"));
    beamView->setProperty("position",joint_str_ref->scaledPosition());
    connect(m_component3D,SIGNAL(visibleChanged()),SLOT(visibilityChanged()));
    beamView->setEnabled(true);
}

void JointVM::onConnectedBeamStressChanged(){
    QObject* sender=QObject::sender();
    Beam* beam=qobject_cast<Beam*>(sender);
    if(!beam) return;
    Qt3DCore::QEntity* component3D=Q_NULLPTR;
    Q_FOREACH(Qt3DCore::QEntity* e, m_beamsMap.keys()){
        if(m_beamsMap[e].data()==beam){
            component3D=e;
            break;
        }
    }
    if(component3D==Q_NULLPTR) return;
    int axial_type;
    qreal axial_force, dummy;


    WeakJointPtr e1,e2;
    beam->extremes(e1,e2);
    int extreme_of_joint,extreme_not_on_joint;
    if(m_joint!=e1){
        component3D->setProperty("flip_extremes",true);
        extreme_of_joint=2;
        extreme_not_on_joint=1;
    }else{
        component3D->setProperty("flip_extremes",false);
        extreme_of_joint=1;
        extreme_not_on_joint=2;
    }

    qreal shearY,shearZ;

    beam->ForcesAndMoments(axial_type,axial_force,shearY,shearZ,dummy,dummy,dummy,extreme_of_joint);
    component3D->setProperty("axialForce_extreme1",axial_force);
    component3D->setProperty("shearYForce_extreme1",shearY);
    component3D->setProperty("shearZForce_extreme1",shearZ);

    component3D->setProperty("axialForceType",axial_type);
    component3D->setProperty("axialForce",fabs(axial_force));

    beam->ForcesAndMoments(axial_type,axial_force,shearY,shearZ,dummy,dummy,dummy,extreme_not_on_joint);
    component3D->setProperty("axialForce_extreme2",axial_force);
    component3D->setProperty("shearYForce_extreme2",shearY);
    component3D->setProperty("shearZForce_extreme2",shearZ);
}

void JointVM::visibilityChanged()
{
    Q_FOREACH(Qt3DCore::QEntity* e, m_beamsMap.keys()){
        e->setProperty("visible",m_component3D->property("visible"));
    }
}



