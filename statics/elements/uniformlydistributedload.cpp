#include "uniformlydistributedload.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"

#include <QVector3D>
#include <QMatrix4x4>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>
QQmlComponent* UniformlyDistributedLoad::m_qqmlcomponent=NULL;

UniformlyDistributedLoad::UniformlyDistributedLoad(BeamPtr beam,QString name,QObject* parent):
   AbstractElement(name,parent),
   m_force(1,0,0)
{
    m_beam=beam.toWeakRef();
    connect(m_beam.data(),SIGNAL(destroyed(QObject*)),this, SIGNAL(killMe()));
}

void UniformlyDistributedLoad::createQmlEntity(QVariantMap aesthetics)
{
    if(!m_sceneRoot.isNull()){
        if(!aesthetics.contains("parent_entity")) return;
        Qt3DCore::QEntity* parentEntity= qvariant_cast<Qt3DCore::QEntity*>(aesthetics["parent_entity"]);
        if(parentEntity==Q_NULLPTR) return;
        if(m_qqmlcomponent==NULL){
            m_qqmlcomponent=new QQmlComponent(qmlEngine(parentEntity),parentEntity);
            m_qqmlcomponent->loadUrl(QUrl("qrc:/element_views/Element_Views/UniformForce.qml"));
            connect(m_qqmlcomponent,&QQmlComponent::destroyed,[]() {
                UniformlyDistributedLoad::m_qqmlcomponent=NULL;
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

void UniformlyDistributedLoad::setForce(QVector3D force){
    if(force!=m_force && !force.isNull()){
        m_force=force;
        emit forceChanged();
    }
}
QVector3D UniformlyDistributedLoad::forceLocal(){
    WeakJointPtr e1,e2;
    m_beam.toStrongRef()->extremes(e1,e2);
    QVector3D c=e2.toStrongRef()->position()-e1.toStrongRef()->position();
    c.normalize();

    QMatrix4x4 mat;
    if( fabs(c.y())==1){
        mat.setRow(0,QVector4D(0,c.y(),0,0));
        mat.setRow(1,QVector4D(-c.y(),0,0,0));
        mat.setRow(2,QVector4D(0,0,1,0));
        mat.setRow(3,QVector4D(0,0,0,1));
    }
    else{
        qreal den=sqrt(1.0- c.y()*c.y() );
        mat.setRow(0,QVector4D(c.x(),c.y(),c.z(),0));
        mat.setRow(1,QVector4D(-c.x()*c.y()/den,den,-c.y()*c.z()/den,0));
        mat.setRow(2,QVector4D(-c.z()/den,0,c.x()/den,0));
        mat.setRow(3,QVector4D(0,0,0,1));
    }
    return mat.mapVector(m_force);

}
