#ifndef TANGIBLETRAPZLOAD_H
#define TANGIBLETRAPZLOAD_H
#include <QObject>
#include <frontend/physicscollisionevent.h>
#include <QtPhysicsUnofficial/frontend/physicsbodyinfo.h>
#include "statics/abstractvmmanager.h"
#include "statics/elements/trapezoidalforce.h"
#include <QQmlComponent>

class TangibleTrapzLoad : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AbstractVMManager* vmManager READ vmManager WRITE setVMManager)
    Q_PROPERTY(Physics::PhysicsBodyInfo* emittingBodyInfo READ emittingBodyInfo WRITE setEmittingBodyInfo)
    Q_PROPERTY(Qt3DCore::QEntity* parentEntity WRITE setParentEntity)
public:
    explicit TangibleTrapzLoad(QObject *parent = 0);
    AbstractVMManager* vmManager(){return m_VMManager;}
    void setVMManager(AbstractVMManager*);
    Physics::PhysicsBodyInfo* emittingBodyInfo(){return m_emittingBodyInfo;}
    void setEmittingBodyInfo(Physics::PhysicsBodyInfo*);
    void setParentEntity(Qt3DCore::QEntity*);
signals:

public slots:
    void onCollision(Physics::PhysicsCollisionEventPtr);
    void onForceChanged();
    void onPositionChanged();
    void updateCustomShape();
    void onExtentChanged();
    void onSelected();
private slots:
    void reset();

private:

    AbstractVMManager* m_VMManager;
    Physics::PhysicsBodyInfo* m_emittingBodyInfo;
    Qt3DCore::QNodeId m_target;
    WeakTrapezoidalForcePtr m_trapezoidalForce;
    Qt3DCore::QEntity* m_parentEntity;
    QVector3D m_target_beam_extreme1;

    Qt3DCore::QEntity* m_component3D;
    QQmlComponent* m_qqmlcomponent;
    QQmlContext* m_qqmlcontext;
    QTimer m_delay_deleter;

    QVector3D prev_position;
    bool m_forceUpdate;
    void updatePosition();
    void clear3DComponent();
    void create3DComponent(Qt3DCore::QEntity *parent, QVector3D relativePosition);
};

#endif // TANGIBLETRAPZLOAD_H
