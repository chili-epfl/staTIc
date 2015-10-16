#include "forcevm.h"
#include "statics/elements/force.h"
#include "staticshelper.h"
ForceVM::ForceVM(Force* force,QObject* uiRoot,Qt3D::QEntity* sceneRoot,QObject* parent):AbstractElementViewModel(uiRoot,sceneRoot,parent)
{
    m_force=force;
    m_visible=true;
    m_hasTipOnApplicationPoint=true;

    connect(m_force,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    connect(m_force,SIGNAL(applicationPointChanged(QVector3D)),this,SLOT(onForceApplicationPointChanged(QVector3D)));
    connect(m_force,SIGNAL(applicationElementChanged(AbstractElement*)),this,SLOT(onForceApplicationElementChanged(AbstractElement*)));
    connect(m_force,SIGNAL(vectorChanged(QVector3D)),this,SLOT(onForceVectorChanged(QVector3D)));

    initView();
}

ForceVM::~ForceVM(){
    Q_FOREACH(Qt3D::QEntity* e, m_3DEntitiesRoles.keys()){
        e->deleteLater();
    }
}


void ForceVM::onForceApplicationPointChanged(QVector3D val){
    emit updateForcePosition(val);
}

void ForceVM::onForceApplicationElementChanged(AbstractElement* val){

}

void ForceVM::onForceVectorChanged(QVector3D val){
    emit updateForceDirection(atan2(val.y(),val.x()));
    emit updateForceMagnitude(val.length());

}


void  ForceVM::initView(){

    Qt3D::QEntity *forceEntity;
    QQmlComponent component(&engine,QUrl("qrc:/ForceArrow.qml"));
    forceEntity = qobject_cast<Qt3D::QEntity*>(component.create());

    //TODO:Better name for model

    forceEntity->setProperty("myAngle",atan2(m_force->vector().y(),m_force->vector().x()));
    forceEntity->setProperty("position",m_force->applicationPoint());
    forceEntity->setProperty("arrowLength",m_force->vector().length());
    forceEntity->setProperty("isPointingAtPosition",m_hasTipOnApplicationPoint);
    forceEntity->setProperty("visible",m_visible);
    forceEntity->setProperty("type","External");

    append_3D_resources(forceEntity);

    connect(this,SIGNAL(updateForceDirection(qreal)),forceEntity,SIGNAL(changeMyAngle(qreal)));
    connect(this,SIGNAL(updateForceMagnitude(qreal)),forceEntity,SIGNAL(changeArrowLength(qreal)));
    connect(this,SIGNAL(updateForcePosition(QVector3D)),forceEntity,SIGNAL(changePosition(QVector3D)));
    connect(this,SIGNAL(visibilityChanged(bool)),forceEntity,SIGNAL(changeVisible(bool)));
    connect(this,SIGNAL(tipOnApplicationPointChanged(bool)),forceEntity,SIGNAL(changeIsPointingAtPosition(bool)));

    forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

}

