#ifndef BEAMVM_H
#define BEAMVM_H

#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/beam.h"

class BeamVM : public AbstractElementViewModel
{
    Q_OBJECT

public:
    explicit BeamVM(BeamPtr beam,Qt3D::QEntity* sceneRoot,QObject* parent=0);
    ~BeamVM();

    WeakBeamPtr beam(){return m_beam;}

public slots:        

    /*Slots for signals from the model*/
    void onBeamAxialStressChanged();
    void onEnableChanged();
    /*-----*/

private:
    void initView();
    WeakBeamPtr m_beam;
    Qt3D::QEntity* m_component3D;
};

#endif // BEAMVM_H
