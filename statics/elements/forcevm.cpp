#include "forcevm.h"
#include "statics/elements/force.h"
#include "staticshelper.h"
ForceVM::ForceVM(QObject* parent):AbstractElementViewModel(parent)
{
    m_visible=true;
    m_hasTipOnApplicationPoint=true;
}


void ForceVM::onElementNameChanged(QString name){

}

void ForceVM::onElementVmChanged(){

}

void ForceVM::onForceApplicationPointChanged(QVector3D val){
    emit updateForcePosition(val);
}

void ForceVM::onForceApplicationElementChanged(QString val){

}

void ForceVM::onForceVectorChanged(QVector3D val){
    emit updateForceDirection(atan2(val.y(),val.x()));
    emit updateForceMagnitude(val.length());

}


void  ForceVM::onStatusComplete(){

    if(!m_element) return;
    if(m_entity_name.isEmpty()) return;
    if(!m_sceneRoot) return;

    Qt3D::QEntity *forceEntity =m_sceneRoot->findChild<Qt3D::QEntity*>(StaticsHelper::NameResolution(m_entity_name,
                                                                                                     StaticsHelper::Roles::MODEL,StaticsHelper::Roles::ENTITY3D));

    if(!forceEntity){
        QQmlComponent* component;
        component=new QQmlComponent(&engine,QUrl("qrc:/ForceArrow.qml"));
        forceEntity = qobject_cast<Qt3D::QEntity*>(component->create());
        forceEntity->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                                 StaticsHelper::Roles::MODEL,StaticsHelper::Roles::ENTITY3D));
        forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));
        component->deleteLater();
    }

    Force* element=qobject_cast<Force*>(m_element);
    forceEntity->setProperty("myAngle",atan2(element->vector().y(),element->vector().x()));
    forceEntity->setProperty("position",element->applicationPoint());
    forceEntity->setProperty("arrowLength",element->vector().length());
    forceEntity->setProperty("isPointingAtPosition",m_hasTipOnApplicationPoint);
    forceEntity->setProperty("visible",m_visible);

    connect(this,SIGNAL(updateForceDirection(qreal)),forceEntity,SIGNAL(changeMyAngle(qreal)));
    connect(this,SIGNAL(updateForceMagnitude(qreal)),forceEntity,SIGNAL(changeArrowLength(qreal)));
    connect(this,SIGNAL(updateForcePosition(QVector3D)),forceEntity,SIGNAL(changePosition(QVector3D)));
    connect(this,SIGNAL(visibilityChanged(bool)),forceEntity,SIGNAL(changeVisible(bool)));
    connect(this,SIGNAL(tipOnApplicationPointChanged(bool)),forceEntity,SIGNAL(changeIsPointingAtPosition(bool)));

}

void ForceVM::onElementDestroyed(){
    Qt3D::QEntity *forceEntity=m_sceneRoot->findChild<Qt3D::QEntity*>(StaticsHelper::NameResolution(m_entity_name,
                                                                                                    StaticsHelper::Roles::MODEL,StaticsHelper::Roles::ENTITY3D));
    if(forceEntity)
        forceEntity->deleteLater();
}
