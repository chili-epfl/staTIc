#ifndef CONCENTRATEDFORCE_H
#define CONCENTRATEDFORCE_H

#include <QObject>
#include <QtPhysicsUnofficial/frontend/physicscollisionevent.h>
#include <QtPhysicsUnofficial/frontend/physicsbodyinfo.h>
#include "statics/abstractvmmanager.h"

class InteriorPointLoad;
typedef QWeakPointer<InteriorPointLoad> WeakInteriorPointLoadPtr;
class NodeLoad;
typedef QWeakPointer<NodeLoad> WeakNodeLoadPtr;

class ConcentratedForce: public QObject
{
    Q_OBJECT
    Q_PROPERTY(AbstractVMManager* vmManager READ vmManager WRITE setVMManager)
    Q_PROPERTY(Physics::PhysicsBodyInfo* emittingBodyInfo READ emittingBodyInfo WRITE setEmittingBodyInfo)
    Q_PROPERTY(QStringList registeredTargets READ registeredTargets WRITE setRegisteredTargets)
public:
    ConcentratedForce(QObject* parent=0);

    AbstractVMManager* vmManager(){return m_VMManager;}
    void setVMManager(AbstractVMManager*);

    Physics::PhysicsBodyInfo* emittingBodyInfo(){return m_emittingBodyInfo;}
    void setEmittingBodyInfo(Physics::PhysicsBodyInfo*);
    QStringList registeredTargets(){return m_targets;}
    void setRegisteredTargets(QStringList targets);
public slots:
    void onCollision(Physics::PhysicsCollisionEvent* e);
    void checkCollisionAttachedElement();
private slots:
    void reset();

private:

    AbstractVMManager* m_VMManager;
    Physics::PhysicsBodyInfo* m_emittingBodyInfo;
    WeakInteriorPointLoadPtr m_pointLoad;
    WeakNodeLoadPtr m_nodeLoad;
    QStringList m_targets;
    Qt3DCore::QEntity* m_attached_element;
};

#endif // CONCENTRATEDFORCE_H
