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
    explicit JointVM(JointPtr joint,Qt3DCore::QEntity* sceneRoot,QObject* parent=0);
    explicit JointVM(JointPtr joint,Qt3DCore::QEntity* entity,QQmlComponent* component,QQmlContext* context,Qt3DCore::QEntity* sceneRoot, QObject* parent=0);
    ~JointVM();
    WeakJointPtr joint(){return m_joint;}
    AbstractElement* element();
    inline Qt3DCore::QEntity *component3D(){return m_component3D;}

public slots:
    /*Slots for signals from the model*/
    void onReactionChanged();
    void onConnectedBeamChanged();
    void onConnectedBeamStressChanged();
    void visibilityChanged();
    void onDisplacementChanged();
    /*---------*/
signals:
private:
    void createEntityForBeam(BeamPtr b);
    void initView();

    Qt3DCore::QEntity* m_component3D;
    /*A map between the beams and the relative entity*/
    QHash<Qt3DCore::QEntity*,WeakBeamPtr> m_beamsMap;
    QHash<Qt3DCore::QEntity*,QPair<QQmlComponent*,QQmlContext*>> m_beams_entities_extras;

    WeakJointPtr m_joint;
    QQmlComponent* m_qqmlcomponent;
    QQmlContext* m_qqmlcontext;
};

#endif // JOINTVM_H
