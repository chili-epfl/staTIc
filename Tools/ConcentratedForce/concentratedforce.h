#ifndef CONCENTRATEDFORCE_H
#define CONCENTRATEDFORCE_H

#include <QObject>
#include <QtPhysicsUnofficial/frontend/physicscollisionevent.h>
#include <QtPhysicsUnofficial/frontend/physicsbodyinfo.h>
#include "statics/abstracteventhandler.h"

class ConcentratedForce: public QObject
{
    Q_OBJECT
    Q_PROPERTY(AbstractEventHandler* uiManager READ uiManager WRITE setUiManager)
    Q_PROPERTY(Physics::PhysicsBodyInfo* emittingBodyInfo READ emittingBodyInfo WRITE setEmittingBodyInfo)
public:
    ConcentratedForce(QObject* parent=0);

    AbstractEventHandler* uiManager(){return m_UIManager;}
    void setUiManager(AbstractEventHandler*);

    Physics::PhysicsBodyInfo* emittingBodyInfo(){return m_emittingBodyInfo;}
    void setEmittingBodyInfo(Physics::PhysicsBodyInfo*);

public slots:
    void onCollition(Physics::PhysicsCollisionEvent* e);
    void checkCollitionAttachedElement();
private:
    AbstractEventHandler* m_UIManager;
    Qt3D::QEntity* m_attached_element;
    Force* m_force;
    Physics::PhysicsBodyInfo* m_emittingBodyInfo;
};

#endif // CONCENTRATEDFORCE_H
