#ifndef BEAMVM_H
#define BEAMVM_H

#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/beam.h"

class BeamVM : public AbstractElementViewModel
{
    Q_OBJECT

public:
    explicit BeamVM(Beam* beam,Qt3D::QEntity* sceneRoot,QObject* parent=0);
    ~BeamVM();

    Beam* beam(){return m_beam;}

public slots:        

    /*Slots for signals from the model*/
    void onBeamAxialStressChanged();
    /*-----*/

private:
    void initView();
    Beam* m_beam;
    Qt3D::QEntity* m_component3D;
};

#endif // BEAMVM_H
