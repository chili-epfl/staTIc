#ifndef BEAMVM_H
#define BEAMVM_H

#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/beam.h"

class BeamVM : public AbstractElementViewModel
{
    Q_OBJECT

public:
    explicit BeamVM(BeamPtr beam,Qt3DCore::QEntity* sceneRoot,QObject* parent=0);
    ~BeamVM();

    WeakBeamPtr beam(){return m_beam;}

public slots:        

    /*Slots for signals from the model*/
    void onBeamAxialStressChanged();
    void onEnableChanged();
    void onBeamSplit();
    void onScaleFactorUpdated();
    void onSegmentsChanged();
    void onParametersChanged();

    /*These come from the VM*/
    void onMaterialChangedVMSide();
    void onBeamSizeChangedVMSide();
    /*-----*/

private:
    void initView();
    WeakBeamPtr m_beam;
    Qt3DCore::QEntity* m_component3D;
};

typedef QList<QVector4D> QVector4List;

#endif // BEAMVM_H
