#ifndef BEAMREMOVER_H
#define BEAMREMOVER_H

#include <QObject>
#include <QtPhysicsUnofficial/frontend/physicscollisionevent.h>
#include <QtPhysicsUnofficial/frontend/physicsbodyinfo.h>
#include "statics/abstractvmmanager.h"

class Beam;
typedef QWeakPointer<Beam> WeakBeamPtr;

class BeamRemover: public QObject
{
    Q_OBJECT
    Q_PROPERTY(AbstractVMManager* vmManager READ vmManager WRITE setVMManager)
    Q_PROPERTY(Physics::PhysicsBodyInfo* emittingBodyInfo READ emittingBodyInfo WRITE setEmittingBodyInfo)
public:
    BeamRemover(QObject* parent=0);
    AbstractVMManager* vmManager(){return m_VMManager;}
    void setVMManager(AbstractVMManager*);

    Physics::PhysicsBodyInfo* emittingBodyInfo(){return m_emittingBodyInfo;}
    void setEmittingBodyInfo(Physics::PhysicsBodyInfo*);

public slots:
    void onCollision(Physics::PhysicsCollisionEvent* e);
    void checkCollisionAttachedElement();
private slots:
    void reset();
private:
    AbstractVMManager* m_VMManager;
    Physics::PhysicsBodyInfo* m_emittingBodyInfo;

    Qt3DCore::QEntity* m_attached_element;
    WeakBeamPtr m_beam;

};

#endif // BEAMREMOVER_H
