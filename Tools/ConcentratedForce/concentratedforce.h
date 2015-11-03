#ifndef CONCENTRATEDFORCE_H
#define CONCENTRATEDFORCE_H

#include <QObject>
#include <QtPhysicsUnofficial/frontend/physicscollisionevent.h>
#include <QtPhysicsUnofficial/frontend/physicsbodyinfo.h>
#include "statics/abstractvmmanager.h"

class InteriorPointLoad;
class NodeLoad;

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
    void onCollition(Physics::PhysicsCollisionEvent* e);
    void checkCollitionAttachedElement();
private:
    AbstractVMManager* m_VMManager;
    Physics::PhysicsBodyInfo* m_emittingBodyInfo;
    InteriorPointLoad* m_pointLoad;
    NodeLoad* m_nodeLoad;
    QStringList m_targets;
    Qt3D::QEntity* m_attached_element;
};

#endif // CONCENTRATEDFORCE_H
