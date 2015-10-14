#ifndef CONCENTRATEDFORCE_H
#define CONCENTRATEDFORCE_H

#include <QObject>
#include <QtPhysicsUnofficial/frontend/physicscollisionevent.h>
#include "statics/abstracteventhandler.h"

class ConcentratedForce: public QObject
{
    Q_OBJECT
    Q_PROPERTY(AbstractEventHandler* uiManager READ uiManager WRITE setuiManager)
public:
    ConcentratedForce(QObject* parent=0);
    AbstractEventHandler* uiManager(){return m_UIManager;}
    void setuiManager(AbstractEventHandler*);
public slots:
    void onCollition(Physics::PhysicsCollisionEvent* e);
private:
    AbstractEventHandler* m_UIManager;
    Qt3D::QEntity* m_attached_element;
    Force* m_force;
};

#endif // CONCENTRATEDFORCE_H
