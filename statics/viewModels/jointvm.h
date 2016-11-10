#ifndef JOINTVM_H
#define JOINTVM_H
#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/joint.h"
#include "jointvmitemmodel.h"
#include <QQmlComponent>

class JointVM:public AbstractElementViewModel
{
    Q_OBJECT

public:
    explicit JointVM(JointPtr joint,Qt3DEntityPtr sceneRoot,QObject* parent=0);
    explicit JointVM(JointPtr joint, Qt3DEntityPtr entity, QQmlContext* context, Qt3DEntityPtr sceneRoot, QObject* parent=0);
    ~JointVM();
    WeakJointPtr joint(){return m_joint;}
    AbstractElement* element();
    inline Qt3DEntityPtr component3D(){return m_component3D;}

public slots:
    /*Slots for signals from the model*/
    void onReactionChanged();
    void onConnectedBeamChanged();
    void onConnectedBeamStressChanged();
    void visibilityChanged();
    void onDisplacementChanged();
    void onSupportChanged();
    /*---------*/
    void updateSupportType(QString type);

signals:
private:
    void createEntityForBeam(BeamPtr b);
    void initView();

    Qt3DEntityPtr m_component3D;

    /*A map between the beams and the relative entity*/
    QHash<Qt3DCore::QNodeId,WeakBeamPtr> m_beamsMap;
    QHash<Qt3DCore::QNodeId,Qt3DEntityPtr> m_beamEntitiesMap;

    WeakJointPtr m_joint;

    static QQmlComponent* m_qqmlcomponent;
    static QQmlComponent* m_qqmlcomponent_beam_view;

    QQmlContext* m_qqmlcontext;
};

#endif // JOINTVM_H
