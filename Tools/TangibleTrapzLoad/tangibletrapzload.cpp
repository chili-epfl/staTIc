#include "tangibletrapzload.h"
#include "statics/viewModels/beamvm.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"
#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>

TangibleTrapzLoad::TangibleTrapzLoad(QObject *parent) :
    QObject(parent),
    m_VMManager(Q_NULLPTR),
    m_emittingBodyInfo(Q_NULLPTR)
{

    m_parentEntity=Q_NULLPTR;
    m_qqmlcomponent=Q_NULLPTR;
    m_qqmlcontext=Q_NULLPTR;
    m_component3D=Q_NULLPTR;
    m_delay_deleter.setInterval(2000);
    m_delay_deleter.setSingleShot(true);
    m_delay_deleter.stop();
    connect(&m_delay_deleter,SIGNAL(timeout()),this,SLOT(reset()));

}
void TangibleTrapzLoad::setVMManager(AbstractVMManager * vmManager){
    if(vmManager!=m_VMManager){
        m_VMManager=vmManager;
    }
}

void TangibleTrapzLoad::setEmittingBodyInfo(Physics::PhysicsBodyInfo* emittingBodyInfo){
    if(emittingBodyInfo!=m_emittingBodyInfo){
        if(m_emittingBodyInfo)
            disconnect(m_emittingBodyInfo);
        m_emittingBodyInfo=emittingBodyInfo;
        if(m_emittingBodyInfo){
            connect(m_emittingBodyInfo,SIGNAL(collided(Physics::PhysicsCollisionEventPtr)),this,SLOT(onCollision(Physics::PhysicsCollisionEventPtr)));
        }
    }
}

void TangibleTrapzLoad::setParentEntity(Qt3DCore::QEntity * parent)
{
    m_parentEntity=parent;
    if(m_parentEntity){
        connect(m_parentEntity,SIGNAL(globalForceChanged()),this,SLOT(onForceChanged()));
        connect(m_parentEntity,SIGNAL(asset3DTextureURLChanged()),this,SLOT(updateCustomShape()));
        connect(m_parentEntity,SIGNAL(extentChanged()),this,SLOT(onExtentChanged()));
        connect(m_parentEntity,SIGNAL(positionChanged()),this,SLOT(onPositionChanged()));
        connect(m_parentEntity,SIGNAL(selectedChanged()),this,SLOT(onSelected()));

    }
}

void TangibleTrapzLoad::onCollision(Physics::PhysicsCollisionEventPtr e)
{
    if(!m_parentEntity) return;
    if(!m_forceUpdate) return;
    if(!m_target.isNull() && !m_emittingBodyInfo->collisionTest(m_target))
        reset();
    if(m_target.isNull()){
        BeamVM* beam_vm=qobject_cast<BeamVM*>(m_VMManager->getAssociatedVM(e->target()));
        if(beam_vm!= Q_NULLPTR){
            m_target=e->target();
            m_trapezoidalForce=m_VMManager->staticsModule()->createTPZLoad(QVector3D(0,-1,0),beam_vm->beam().toStrongRef(),
                                                                           QVector3D(0,0,0),QVector2D(0,1)).toWeakRef();
            onForceChanged();
            updatePosition();
            /*Injecting an Entity in the beam representation*/
            create3DComponent(beam_vm->component3D(),e->contactPointOnTargetLocal());
            /*........*/
            m_parentEntity->setProperty("tangibleSection",beam_vm->component3D()->property("tangibleSection"));
            m_forceUpdate=false;
        }
    }
    if(!m_target.isNull() && m_target==e->target()){
        m_delay_deleter.stop();
        m_forceUpdate=false;
        if(m_component3D)
            m_component3D->setProperty("relativePosition",QVector3D(0,e->contactPointOnTargetLocal().y(),0));
        updatePosition();
    }
}

void TangibleTrapzLoad::updateCustomShape(){
    if(m_component3D){
        Qt3DCore::QEntity* beam_3DEntity=m_component3D->parentEntity();
        QVector3D position=m_component3D->property("relativePosition").value<QVector3D>();

        clear3DComponent();
        create3DComponent(beam_3DEntity,position);
    }
}



void TangibleTrapzLoad::reset()
{
    m_delay_deleter.stop();
    m_target=Qt3DCore::QNodeId();
    if(!m_trapezoidalForce.isNull())
        m_VMManager->staticsModule()->removeTPZLoad(m_trapezoidalForce.toStrongRef());
    m_trapezoidalForce.clear();
    clear3DComponent();
    m_parentEntity->setProperty("tangibleSection",QSize(0,0));

}

void TangibleTrapzLoad::create3DComponent(Qt3DCore::QEntity* parent, QVector3D relativePosition){
    m_qqmlcomponent=new QQmlComponent(qmlEngine(parent),parent);
    m_qqmlcomponent->loadUrl(QUrl("qrc:/tools/Tools/TangibleTrapzLoad/TangibleTrapzLoad_InjectedEntity.qml"));
    m_qqmlcontext=new QQmlContext(qmlContext(parent));
    m_component3D=qobject_cast<Qt3DCore::QEntity*>(m_qqmlcomponent->beginCreate(m_qqmlcontext));
    m_qqmlcontext->setContextObject(m_component3D);
    //QQmlEngine::setObjectOwnership(m_component3D,QQmlEngine::JavaScriptOwnership);
    m_component3D->setProperty("relativePosition",relativePosition);
    m_component3D->setProperty("asset3DMeshURL",m_parentEntity->property("asset3DMeshURL"));
    m_component3D->setProperty("asset3DTextureURL",m_parentEntity->property("asset3DTextureURL"));
    onSelected();
    m_qqmlcomponent->completeCreate();
    m_component3D->setParent(parent);

}

void TangibleTrapzLoad::clear3DComponent(){
    if(m_component3D){
        m_component3D->setParent(Q_NODE_NULLPTR);
        m_component3D->deleteLater();
    }
    if(m_qqmlcomponent)
        delete m_qqmlcomponent;
    if(m_qqmlcontext)
        delete m_qqmlcontext;
    m_qqmlcomponent=Q_NULLPTR;
    m_qqmlcontext=Q_NULLPTR;
    m_component3D=Q_NULLPTR;
}

void TangibleTrapzLoad::updatePosition(){
    if(m_parentEntity!=Q_NULLPTR && !m_trapezoidalForce.isNull()){
        TrapezoidalForcePtr trp_f_ptr=m_trapezoidalForce.toStrongRef();
        WeakJointPtr e1,e2;
        trp_f_ptr->beam().toStrongRef()->extremes(e1,e2);
        QVector3D beamDir=e2.toStrongRef()->position()-e1.toStrongRef()->position();
        beamDir.normalize();
        QVector3D globalPosition=m_parentEntity->property("globalPosition").value<QVector3D>()-e1.toStrongRef()->scaledPosition();
        qreal projection=QVector3D::dotProduct(globalPosition,beamDir);
        qreal lP=fabs(projection/trp_f_ptr->beam().toStrongRef()->scaledLength());
        QVector3D localPosition(lP,lP,lP);
        trp_f_ptr->setRelativePosition(localPosition,m_parentEntity->property("extent").value<QVector2D>());
    }
}

void TangibleTrapzLoad::onPositionChanged()
{
    m_forceUpdate=true;
    if(!m_target.isNull() && !m_emittingBodyInfo->collisionTest(m_target))
        m_delay_deleter.start();
}

void TangibleTrapzLoad::onForceChanged()
{
    if(m_parentEntity!=Q_NULLPTR && !m_trapezoidalForce.isNull()){
        TrapezoidalForcePtr trp_f_ptr=m_trapezoidalForce.toStrongRef();
        trp_f_ptr->setForce(m_parentEntity->property("globalForce").value<QVector3D>());
    }
}

void TangibleTrapzLoad::onExtentChanged(){
    if(m_parentEntity!=Q_NULLPTR && !m_trapezoidalForce.isNull()){
        TrapezoidalForcePtr trp_f_ptr=m_trapezoidalForce.toStrongRef();
        QVector3D position;
        QVector2D extent;
        trp_f_ptr->relativePosition(position,extent);
        trp_f_ptr->setRelativePosition(position,m_parentEntity->property("extent").value<QVector2D>());
    }
}

void TangibleTrapzLoad::onSelected()
{
    if(m_component3D){
        m_component3D->setProperty("selected",m_parentEntity->property("selected"));
    }
}




