#ifndef JOINTVM_H
#define JOINTVM_H
#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/joint.h"
#include "jointvmitemmodel.h"

class JointVM:public AbstractElementViewModel
{
    Q_OBJECT

public:
    explicit JointVM(JointPtr joint,Qt3D::QEntity* sceneRoot,QObject* parent=0);
    ~JointVM();
    WeakJointPtr joint(){return m_joint;}

public slots:
    /*Slots for signals from the model*/
    void onReactionChanged();
    void onConnectedBeamChanged();
    void onConnectedBeamStressChanged();
    /*---------*/
signals:
private:
    void createEntityForBeam(BeamPtr b);
    void initView();

    Qt3D::QEntity* m_component3D;
    /*A map between the beams and the relative entity*/
    QHash<Qt3D::QEntity*,WeakBeamPtr> m_beamsMap;
    WeakJointPtr m_joint;
};

#endif // JOINTVM_H
