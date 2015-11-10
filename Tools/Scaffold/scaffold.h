#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <QObject>
#include <QtPhysicsUnofficial/frontend/physicscollisionevent.h>
#include <QtPhysicsUnofficial/frontend/physicsbodyinfo.h>
#include "statics/abstractvmmanager.h"

class Scaffold : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Physics::PhysicsBodyInfo* extreme1 READ extreme1 WRITE setExtreme1 NOTIFY extreme1Changed)
    Q_PROPERTY(Physics::PhysicsBodyInfo* extreme2 READ extreme2 WRITE setExtreme2 NOTIFY extreme2Changed)
    Q_PROPERTY(AbstractVMManager* vmManager READ vmManager WRITE setVMManager)

public:
    Scaffold(QObject* parent=0);
    Physics::PhysicsBodyInfo* extreme1(){return m_extreme1;}
    Physics::PhysicsBodyInfo* extreme2(){return m_extreme2;}

    void setExtreme1(Physics::PhysicsBodyInfo*);
    void setExtreme2(Physics::PhysicsBodyInfo*);

    AbstractVMManager* vmManager(){return m_VMManager;}
    void setVMManager(AbstractVMManager*);

private slots:
    void onCollitionExtreme1(Physics::PhysicsCollisionEvent* e);
    void onCollitionExtreme2(Physics::PhysicsCollisionEvent* e);
    void onAnchorsChanged();
    void checkCollitionAttachedElement1();
    void checkCollitionAttachedElement2();
    void reactivate();

signals:
    void extreme1Changed();
    void extreme2Changed();
private:
    void reset();

    Physics::PhysicsBodyInfo* m_extreme1,*m_extreme2;
    AbstractVMManager* m_VMManager;
    Qt3D::QEntity* m_attached_element_e1,*m_attached_element_e2;

    BeamVM* m_new_beamVM;

    /*Anchors*/
    WeakAbstractElementPtr m_anchor_1,m_anchor_2;
    int m_anchor_1_offset,m_anchor_2_offset;
    QVector3D m_extreme_pos1,m_extreme_pos2;

    QTimer* m_refractory_timer;
    bool m_active;
};

#endif // SCAFFOLD_H
