#include "nodeload.h"
#include "joint.h"
#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>

QQmlComponent* NodeLoad::m_qqmlcomponent=NULL;

NodeLoad::NodeLoad(JointPtr joint,QString name,QObject* parent):
    AbstractElement(name,parent),
    m_force(1,0,0),
    m_momentum(0,0,0),
    m_component3D()
{
    m_joint=joint.toWeakRef();
    connect(m_joint.data(),SIGNAL(destroyed(QObject*)),this,SIGNAL(killMe()));
}

void NodeLoad::createQmlEntity(QVariantMap aesthetics)
{
    if(!m_sceneRoot.isNull()){
        if(!aesthetics.contains("parent_entity")) return;
        Qt3DCore::QEntity* parentEntity= qvariant_cast<Qt3DCore::QEntity*>(aesthetics["parent_entity"]);
        if(parentEntity==Q_NULLPTR) return;
        if(m_qqmlcomponent==NULL){
            m_qqmlcomponent=new QQmlComponent(qmlEngine(parentEntity),parentEntity);
            m_qqmlcomponent->loadUrl(QUrl("qrc:/element_views/Element_Views/NodeForce.qml"));
            connect(m_qqmlcomponent,&QQmlComponent::destroyed,[]() {
                NodeLoad::m_qqmlcomponent=NULL;
              });
        }
        QQmlContext* m_qqmlcontext=new QQmlContext(qmlContext(parentEntity),parentEntity);
        Qt3DCore::QEntity* forceView= qobject_cast<Qt3DCore::QEntity*>(m_qqmlcomponent->beginCreate(m_qqmlcontext));
        m_qqmlcontext->setContextObject(forceView);
        m_component3D=forceView;
        m_component3D->setProperty("backend_entity",QVariant::fromValue(this));
        if(aesthetics.contains("warehouse_index"))
            m_component3D->setProperty("warehouse_index",aesthetics["warehouse_index"]);
        m_qqmlcomponent->completeCreate();
        m_component3D->setParent(parentEntity);
        connect(this,SIGNAL(destroyed(QObject*)),m_component3D.data(),SLOT(deleteLater()));
    }
}

void NodeLoad::setForce(QVector3D force){
    if(force!=m_force && !force.isNull()){
        m_force=force;
        emit forceChanged();
    }

}
void NodeLoad::setMomentum(QVector3D momentum){
    if(momentum!=m_momentum){
        m_momentum=momentum;
        emit momentumChanged();
    }
}


