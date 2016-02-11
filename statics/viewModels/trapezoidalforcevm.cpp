#include "trapezoidalforcevm.h"
#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlEngine>


TrapezoidalForceVM::TrapezoidalForceVM(TrapezoidalForcePtr force, Qt3DCore::QEntity *sceneRoot, Qt3DCore::QEntity* parentEntity,QObject *parent)
    :AbstractElementViewModel(sceneRoot,parent)
{
    m_trapezoidalForce=force.toWeakRef();
    initView(parentEntity);
    connect(m_trapezoidalForce.data(),SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    connect(m_trapezoidalForce.data(),SIGNAL(destroyed(QObject*)),m_component3D,SLOT(deleteLater()));
    connect(m_component3D,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    connect(m_component3D,SIGNAL(destroyed(QObject*)),m_trapezoidalForce.data(),SIGNAL(killMe()));
}



void TrapezoidalForceVM::onForceChanged()
{
    if(m_component3D!=Q_NULLPTR && !m_trapezoidalForce.isNull()){
        TrapezoidalForcePtr trp_f_ptr=m_trapezoidalForce.toStrongRef();
        trp_f_ptr->setForce(m_component3D->property("force").value<QVector3D>());
    }
}

void TrapezoidalForceVM::onRelativePositionChanged()
{
    if(m_component3D!=Q_NULLPTR && !m_trapezoidalForce.isNull()){
        TrapezoidalForcePtr trp_f_ptr=m_trapezoidalForce.toStrongRef();
        trp_f_ptr->setRelativePosition(m_component3D->property("begin").value<QVector3D>(),m_component3D->property("end").value<QVector3D>());
    }
}

void TrapezoidalForceVM::initView(Qt3DCore::QEntity* parentEntity)
{
    QQmlComponent component(QQmlEngine::contextForObject(parentEntity)->engine(),QUrl("qrc:/element_views/Element_Views/TrapezoidalForce.qml"));
    Qt3DCore::QEntity* forceView= qobject_cast<Qt3DCore::QEntity*>(component.create(new QQmlContext(QQmlEngine::contextForObject(parentEntity))));
    m_component3D=forceView;
    QQmlEngine::setObjectOwnership(m_component3D,QQmlEngine::JavaScriptOwnership);
    onForceChanged();
    onRelativePositionChanged();
    connect(m_component3D,SIGNAL(forceChanged()),this,SLOT(onForceChanged()));
    connect(m_component3D,SIGNAL(relativePositionChanged()),SLOT(onRelativePositionChanged()));

    m_component3D->setParent(parentEntity);
}
