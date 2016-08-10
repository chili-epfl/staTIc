#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <QObject>
#include <QtPhysicsUnofficial/frontend/physicscollisionevent.h>
#include <QtPhysicsUnofficial/frontend/physicsbodyinfo.h>
#include "statics/abstractvmmanager.h"
#include "statics/elements/joint.h"
#include "logger.h"
class Scaffold : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Physics::PhysicsBodyInfo* extreme1 READ extreme1 WRITE setExtreme1 NOTIFY extreme1Changed)
    Q_PROPERTY(Physics::PhysicsBodyInfo* extreme2 READ extreme2 WRITE setExtreme2 NOTIFY extreme2Changed)
    Q_PROPERTY(QVector3D localExtreme1Pos READ localExtreme1Pos WRITE setLocalExtreme1Pos NOTIFY localExtreme1PosChanged)
    Q_PROPERTY(QVector3D localExtreme2Pos READ localExtreme2Pos WRITE setLocalExtreme2Pos NOTIFY localExtreme2PosChanged)

    Q_PROPERTY(AbstractVMManager* vmManager READ vmManager WRITE setVMManager)

public:
    Scaffold(QObject* parent=0);
    Physics::PhysicsBodyInfo* extreme1(){return m_extreme1;}
    Physics::PhysicsBodyInfo* extreme2(){return m_extreme2;}
    inline QVector3D localExtreme1Pos(){return m_extreme_pos1;}
    inline QVector3D localExtreme2Pos(){return m_extreme_pos2;}
    void setLocalExtreme1Pos(QVector3D v);
    void setLocalExtreme2Pos(QVector3D v);

    void setExtreme1(Physics::PhysicsBodyInfo*);
    void setExtreme2(Physics::PhysicsBodyInfo*);

    AbstractVMManager* vmManager(){return m_VMManager;}
    void setVMManager(AbstractVMManager*);

public slots:
    void checkPosition4NewSupport();
private slots:
    void onCollisionExtreme1(Physics::PhysicsCollisionEventPtr e);
    void onCollisionExtreme2(Physics::PhysicsCollisionEventPtr e);
    void onAnchorsChanged();
    void checkPositionExtremes();
    //void reactivate();
    void reset();
signals:
    void extreme1Changed();
    void extreme2Changed();
    void localExtreme1PosChanged();
    void localExtreme2PosChanged();
private:

    Physics::PhysicsBodyInfo* m_extreme1,*m_extreme2;
    AbstractVMManager* m_VMManager;

    /*Anchors*/
    WeakAbstractElementPtr m_anchor_1,m_anchor_2;
    int m_anchor_1_offset,m_anchor_2_offset;
    QVector3D m_extreme_pos1,m_extreme_pos2;
    QVector3D m_reference_extreme_pos1,m_reference_extreme_pos2;

    /*Variables for creating a new support*/
    QVector3D m_newSupport_position;
    short m_newSupport_possible;

    /*Joints created during the split*/
    WeakJointPtr m_joint_1,m_joint_2;
    /*Beam added to the structure*/
    WeakBeamPtr m_beam;
    /*Split beams*/
    WeakBeamPtr m_beam_1,m_beam_2;

    QTimer* m_refractory_timer;
    /*Active is true when the */
    bool m_active;

    JointPtr splitBeam(BeamPtr b, qreal offset);
    //QHash<WeakBeamPtr, WeakBeamPtr> m_child2parent_beams;
    short stability_1,stability_2,stability_3,stability_4;
    QTime timer_1,timer_2;

    bool m_name_s1_is_available;

    QVariantMap m_logger_map;
    Logger m_logger;
};

#endif // SCAFFOLD_H
