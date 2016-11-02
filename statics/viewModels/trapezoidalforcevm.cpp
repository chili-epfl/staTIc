#include "trapezoidalforcevm.h"
#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>
#include <Qt3DRender>

QQmlComponent* TrapezoidalForceVM::m_qqmlcomponent=NULL;


TrapezoidalForceVM::TrapezoidalForceVM(TrapezoidalForcePtr force, Qt3DCore::QEntity *sceneRoot, Qt3DCore::QEntity* parentEntity, QVariantHash properties, QObject *parent)
    :AbstractElementViewModel(sceneRoot,parent)
{
    m_qqmlcontext=Q_NULLPTR;
    m_component3D=Q_NULLPTR;
    m_asset_tmp_copy=QString();
    m_trapezoidalForce=force.toWeakRef();
    initView(parentEntity,properties);
    connect(m_trapezoidalForce.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    //connect(m_trapezoidalForce.data(),SIGNAL(destroyed(QObject*)),m_component3D,SLOT(deleteLater()));
    connect(m_component3D,SIGNAL(killMe()),this,SLOT(deleteLater()));
    connect(m_component3D,SIGNAL(killMe()),m_trapezoidalForce.data(),SIGNAL(killMe()));
}

TrapezoidalForceVM::~TrapezoidalForceVM()
{
   if(!m_asset_tmp_copy.isEmpty())
        QFile::remove(m_asset_tmp_copy.toLocalFile());
   if(m_component3D){
       m_component3D->setEnabled(false);
//       delete m_component3D;
   }
//   if(m_qqmlcontext)
//       delete m_qqmlcontext;
//   m_qqmlcontext=Q_NULLPTR;
//   m_component3D=Q_NULLPTR;

}

void TrapezoidalForceVM::setProperties(QVariantHash properties)
{
    if(m_component3D){
        Q_FOREACH(QString property, properties.keys()){
            if(property.compare("weight",Qt::CaseInsensitive)==0){
                m_component3D->setProperty("weight",properties[property]);
            }
            else if(property.compare("extent",Qt::CaseInsensitive)==0){
                m_component3D->setProperty("extent",properties[property]);
            }
            else if(property.compare("main_asset_url",Qt::CaseInsensitive)==0){
                m_component3D->setProperty("asset3DMeshURL",properties[property]);
            }
            else if(property.compare("main_asset_diffuse_map_url",Qt::CaseInsensitive)==0){
                m_component3D->setProperty("asset3DTextureURL",properties[property]);
            }
        }
    }
}

void TrapezoidalForceVM::setParentEntity(Qt3DCore::QEntity *parentEntity)
{
    if(parentEntity!=Q_NULLPTR && m_component3D!=Q_NULLPTR){
        QVariantHash properties;
        if(m_component3D){
//            properties["weight"]= m_component3D->property("weight");
//            properties["extent"]= m_component3D->property("extent");
//            properties["main_asset_url"]= m_component3D->property("asset3DMeshURL");
//            properties["main_asset_diffuse_map_url"]= m_component3D->property("asset3DTextureURL");
//            m_component3D->setParent(Q_NODE_NULLPTR);
//            delete m_component3D;
            m_component3D->setEnabled(false);
        }
//        if(m_qqmlcontext)
//            delete m_qqmlcontext;
//        m_qqmlcontext=Q_NULLPTR;
        m_component3D=Q_NULLPTR;
        initView(parentEntity,properties);
        connect(m_trapezoidalForce.data(),SIGNAL(destroyed(QObject*)),m_component3D,SLOT(deleteLater()));
        connect(m_component3D,SIGNAL(killMe()),this,SLOT(deleteLater()));
        connect(m_component3D,SIGNAL(killMe()),m_trapezoidalForce.data(),SIGNAL(killMe()));
    }
}

AbstractElement *TrapezoidalForceVM::element()
{
    if(m_trapezoidalForce.isNull())
        return Q_NULLPTR;
    else{
        return m_trapezoidalForce.toStrongRef().data();
    }
}


//void TrapezoidalForceVM::setTrapezoidalForcePtr(TrapezoidalForcePtr force)
//{
//    if(!force.isNull() && force!=m_trapezoidalForce){
//        disconnect(m_trapezoidalForce.data());
//        m_trapezoidalForce=force.toWeakRef();
//        connect(m_trapezoidalForce.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
//        connect(m_trapezoidalForce.data(),SIGNAL(destroyed(QObject*)),m_component3D,SLOT(deleteLater()));
//        connect(m_component3D,SIGNAL(killMe()),m_trapezoidalForce.data(),SIGNAL(killMe()));
//    }
//}



void TrapezoidalForceVM::onForceChanged()
{
    if(m_component3D!=Q_NULLPTR && !m_trapezoidalForce.isNull()){
        TrapezoidalForcePtr trp_f_ptr=m_trapezoidalForce.toStrongRef();
        trp_f_ptr->setForce(m_component3D->property("globalForce").value<QVector3D>());
    }
}

void TrapezoidalForceVM::onRelativePositionChanged()
{
    if(m_component3D!=Q_NULLPTR && !m_trapezoidalForce.isNull()){
        TrapezoidalForcePtr trp_f_ptr=m_trapezoidalForce.toStrongRef();
        trp_f_ptr->setRelativePosition(m_component3D->property("relativeLocalPosition").value<QVector3D>(),m_component3D->property("extent").value<QVector2D>());
    }
}

void TrapezoidalForceVM::initView(Qt3DCore::QEntity* parentEntity,QVariantHash properties)
{
    if(m_qqmlcomponent==NULL){
        m_qqmlcomponent=new QQmlComponent(qmlEngine(parentEntity),parentEntity);
        m_qqmlcomponent->loadUrl(QUrl("qrc:/element_views/Element_Views/TrapezoidalForce.qml"));
        connect(m_qqmlcomponent,&QQmlComponent::destroyed,[]() {
            TrapezoidalForceVM::m_qqmlcomponent=NULL;
          });
    }
    m_qqmlcontext=new QQmlContext(qmlContext(parentEntity));
    Qt3DCore::QEntity* forceView= qobject_cast<Qt3DCore::QEntity*>(m_qqmlcomponent->beginCreate(m_qqmlcontext));
    m_qqmlcontext->setContextObject(forceView);
    m_component3D=forceView;
    //QQmlEngine::setObjectOwnership(m_component3D,QQmlEngine::JavaScriptOwnership);
    setProperties(properties);
    connect(m_component3D,SIGNAL(globalForceChanged()),this,SLOT(onForceChanged()));
    connect(m_component3D,SIGNAL(relativeLocalPositionChanged()),SLOT(onRelativePositionChanged()));
    connect(m_component3D,SIGNAL(extentChanged()),SLOT(onRelativePositionChanged()));
    m_qqmlcomponent->completeCreate();
    m_component3D->setParent(parentEntity);
    onForceChanged();
    onRelativePositionChanged();
    append_3D_resources(m_component3D);
}
