#include "jointvm.h"
#include "statics/elements/beam.h"
#include "statics/frame3dd/frame3ddvmmanager.h"
#include <QSet>

#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>

const QString classname4beamEntity="Beam4Joint";
QQmlComponent* JointVM::m_qqmlcomponent=NULL;
QQmlComponent* JointVM::m_qqmlcomponent_beam_view=NULL;

JointVM::JointVM(JointPtr joint,Qt3DEntityPtr sceneRoot,QObject* parent):
    AbstractElementViewModel(sceneRoot,parent),
    m_component3D(Q_NULLPTR)
{
    m_qqmlcontext=Q_NULLPTR;
    m_joint=joint.toWeakRef();
    initView();
    connect(m_joint.data(),SIGNAL(reactionChanged()),this,SLOT(onReactionChanged()));
    connect(m_joint.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    connect(m_joint.data(),SIGNAL(connectedBeamsChanged()),this,SLOT(onConnectedBeamChanged()));
    connect(m_joint.data(),SIGNAL(displacementChanged()),this,SLOT(onDisplacementChanged()));
    connect(m_joint.data(),SIGNAL(supportChanged()),this,SLOT(onSupportChanged()));
    connect(m_component3D,SIGNAL(updateSupportType(QString)),this,SLOT(updateSupportType(QString)));
}

JointVM::JointVM(JointPtr joint, Qt3DEntityPtr entity, QQmlContext *context, Qt3DEntityPtr sceneRoot, QObject *parent)
    : AbstractElementViewModel(sceneRoot,parent)
{
    m_qqmlcontext=context;
    m_component3D=entity;
    m_joint=joint.toWeakRef();
    initView();
    connect(m_joint.data(),SIGNAL(reactionChanged()),this,SLOT(onReactionChanged()));
    connect(m_joint.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    connect(m_joint.data(),SIGNAL(connectedBeamsChanged()),this,SLOT(onConnectedBeamChanged()));
    connect(m_joint.data(),SIGNAL(displacementChanged()),this,SLOT(onDisplacementChanged()));
    connect(m_joint.data(),SIGNAL(supportChanged()),this,SLOT(onSupportChanged()));
    connect(m_component3D,SIGNAL(updateSupportType(QString)),this,SLOT(updateSupportType(QString)));

}

JointVM::~JointVM(){
    if(m_component3D){
        m_component3D->setEnabled(false);
        m_component3D->setProperty("position",QVector3D(0,0,400));
        Frame3DDVMManager* parent_vm_manager=static_cast<Frame3DDVMManager*>(parent());
        parent_vm_manager->addPoolEntity(metaObject()->className(),m_component3D,m_qqmlcontext);

        Q_FOREACH(auto e,m_beamEntitiesMap.values()){
            if(e){
                parent_vm_manager->addPoolEntity(metaObject()->className()+classname4beamEntity,
                                                 e,Q_NULLPTR);
                e->setEnabled(false);
            }
        }
    }
    m_qqmlcontext=Q_NULLPTR;
    m_component3D=Q_NULLPTR;
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

void JointVM::onDisplacementChanged(){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    if(m_component3D){
        m_component3D->setProperty("displacement",joint_str_ref->displacement()*AbstractStaticsModule::modelScale());
    }
}

void JointVM::onConnectedBeamChanged(){
    if(m_component3D){
        JointPtr joint_str_ref=m_joint.toStrongRef();
        QList<WeakBeamPtr> previous_beams=m_beamsMap.values();

        Q_FOREACH(WeakBeamPtr b, joint_str_ref->connectedBeams()){
            if(!previous_beams.contains(b)){
                createEntityForBeam(b.toStrongRef());
            }
        }

        Q_FOREACH(auto nodeId,m_beamsMap.keys()){
            auto b=m_beamsMap[nodeId];
            if(b.isNull() || !b.toStrongRef()->enable()){
                auto e= m_beamEntitiesMap[nodeId];
                m_beamsMap.remove(nodeId);
                m_beamEntitiesMap.remove(nodeId);
                Frame3DDVMManager* parent_vm_manager=static_cast<Frame3DDVMManager*>(parent());
                parent_vm_manager->addPoolEntity(metaObject()->className()+classname4beamEntity,
                                                 e,Q_NULLPTR);
                if(e){
                    e->setEnabled(false);
                    e->setProperty("parent_joint",QVariant());
                }
            }
        }
        QVariantList connected_beam;

        Q_FOREACH(Qt3DEntityPtr e,m_beamEntitiesMap.values()){
            connected_beam.append(QVariant::fromValue(e.data()));
        }
        m_component3D->setProperty("connected_beams",connected_beam);
    }
}

void JointVM::onSupportChanged(){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    QString supportType="special";
    if(m_component3D){
        bool  X,Y,Z,XX,YY,ZZ;
        joint_str_ref->support(X,Y,Z,XX,YY,ZZ);
        Frame3DDVMManager* parent_vm_manager=static_cast<Frame3DDVMManager*>(parent());
        if(parent_vm_manager->staticsModule()->is2D()){
            if(X && Y && Z && XX && YY && !ZZ)
                supportType="Pinned";
            else if(!X && Y && Z && XX && YY && !ZZ)
                supportType="Rolling";
            else if(!X && !Y && Z && XX && YY && !ZZ)
                supportType="none";
        }
        else
            if(X && Y && Z && !XX && !YY && !ZZ)
                supportType="Pinned";
            else if(!X && Y && !Z && !XX && !YY && !ZZ)
                supportType="Rolling";
            else if(!X && !Y && !Z && !XX && !YY && !ZZ)
                supportType="none";
        if (X && Y && Z && XX && YY && ZZ)
            supportType="Fixed";
        m_component3D->setProperty("supportType",supportType);
    }
}

void JointVM::updateSupportType(QString type){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    Frame3DDVMManager* parent_vm_manager=static_cast<Frame3DDVMManager*>(parent());
    if(parent_vm_manager->staticsModule()->is2D()){
        if(type=="none")
            joint_str_ref->setSupport(0,0,1,1,1,0);
        else if(type=="Pinned")
            joint_str_ref->setSupport(1,1,1,1,1,0);
        else if(type=="Rolling")
            joint_str_ref->setSupport(0,1,1,1,1,0);
    }else{
        if(type=="none")
            joint_str_ref->setSupport(0,0,0,0,0,0);
        else if(type=="Pinned")
            joint_str_ref->setSupport(0,1,0,0,0,0);
        else if(type=="Rolling")
            joint_str_ref->setSupport(1,1,1,0,0,0);
    }
    if(type=="Fixed")
        joint_str_ref->setSupport(1,1,1,1,1,1);

}

void JointVM::initView(){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    if(m_component3D==Q_NULLPTR){
        if(m_qqmlcomponent==NULL){
            m_qqmlcomponent=new QQmlComponent(qmlEngine(m_sceneRoot),m_sceneRoot);
            connect(m_qqmlcomponent,&QQmlComponent::destroyed,[]() {
                JointVM::m_qqmlcomponent=NULL;
              });
            m_qqmlcomponent->loadUrl(QUrl("qrc:/element_views/Element_Views/JointView.qml"));
        }
        m_qqmlcontext=new QQmlContext(qmlContext(m_sceneRoot),m_sceneRoot);
        Qt3DCore::QEntity* jointView= qobject_cast<Qt3DCore::QEntity*>(m_qqmlcomponent->create(m_qqmlcontext));
        m_qqmlcontext->setContextObject(jointView);
        m_component3D=jointView;
        m_component3D->setParent(m_sceneRoot);
    }
    m_component3D->setObjectName(joint_str_ref->objectName());
    m_component3D->setProperty("position",joint_str_ref->scaledPosition());
    onReactionChanged();
    onConnectedBeamChanged();
    onSupportChanged();
    append_3D_resources(m_component3D);
    m_component3D->setEnabled(true);
}

void JointVM::createEntityForBeam(BeamPtr b){
    JointPtr joint_str_ref=m_joint.toStrongRef();
    Frame3DDVMManager* parent_vm_manager=static_cast<Frame3DDVMManager*>(parent());
    if(m_qqmlcomponent_beam_view==NULL){
        m_qqmlcomponent_beam_view=new QQmlComponent(qmlEngine(m_sceneRoot),m_sceneRoot);
        connect(m_qqmlcomponent_beam_view,&QQmlComponent::destroyed,[]() {
            JointVM::m_qqmlcomponent_beam_view=NULL;
          });
        m_qqmlcomponent_beam_view->loadUrl(QUrl("qrc:/element_views/Element_Views/BeamView4JointView.qml"));

    }
    Qt3DEntityPtr beamView;
    QQmlContext* beamContext;
    parent_vm_manager->tryRetrivePoolEntity(metaObject()->className()+classname4beamEntity,beamView,beamContext);
    if(beamView==Q_NULLPTR){
        beamContext=new QQmlContext(qmlContext(m_sceneRoot),m_sceneRoot);
        beamView = qobject_cast<Qt3DCore::QEntity*>(m_qqmlcomponent_beam_view->create(beamContext));
        beamView->setParent(m_sceneRoot);
    }
    m_beamsMap[beamView->id()]=b.toWeakRef();
    m_beamEntitiesMap[beamView->id()]=beamView;

    beamView->setObjectName(b->objectName());
    beamView->setProperty("extreme1",joint_str_ref->scaledPosition());
    WeakJointPtr e1,e2;
    b->extremes(e1,e2);

    int extreme_of_joint,extreme_not_on_joint;
    if(m_joint!=e1){
        beamView->setProperty("flip_extremes",true);
        extreme_of_joint=2;
        extreme_not_on_joint=1;
        beamView->setProperty("extreme2_name",e1.toStrongRef()->objectName());
        beamView->setProperty("extreme2",e1.toStrongRef()->scaledPosition());
    }else{
        beamView->setProperty("flip_extremes",false);
        extreme_of_joint=1;
        extreme_not_on_joint=2;
        beamView->setProperty("extreme2_name",e2.toStrongRef()->objectName());
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
    Qt3DEntityPtr component3D=Q_NULLPTR;
    Q_FOREACH(auto node_id, m_beamsMap.keys()){
        auto b=m_beamsMap[node_id];
        if(b.data()==beam){
            component3D=m_beamEntitiesMap[node_id];
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
    Q_FOREACH(Qt3DEntityPtr e, m_beamEntitiesMap.values()){
        if(e)
            e->setProperty("visible",m_component3D->property("visible"));
    }
}



