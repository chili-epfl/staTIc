#ifndef JOINTVM_H
#define JOINTVM_H
#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/joint.h"
#include "jointvmitemmodel.h"

class JointVM:public AbstractElementViewModel
{
    Q_OBJECT

public:
    explicit JointVM(Joint* joint,Qt3D::QEntity* sceneRoot,QObject* parent=0);

    Joint* joint(){return m_joint;}

public slots:
    /*Slots for signals from the model*/
    void onReactionChanged();
    void onConnectedBeamChanged();
    void onConnectedBeamStressChanged();
    /*---------*/
signals:
private:
    void createEntityForBeam(Beam* b);
    void initView();

    Qt3D::QEntity* m_component3D;
    /*A map between the beams and the relative entity*/
    QHash<Beam*,Qt3D::QEntity*> m_beamsMap;

    Joint* m_joint;
};

#endif // JOINTVM_H
