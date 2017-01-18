#ifndef BEAMVM_H
#define BEAMVM_H

#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/beam.h"
#include <QQmlComponent>
class BeamVM : public AbstractElementViewModel
{
    Q_OBJECT

public:
    explicit BeamVM(BeamPtr beam,Qt3DEntityPtr sceneRoot,QObject* parent=0);
    ~BeamVM();

    WeakBeamPtr beam(){return m_beam;}
    Qt3DEntityPtr component3D(){return m_component3D;}
    AbstractElement* element();
public slots:

    /*Slots for signals from the model*/
    void onBeamAxialStressChanged();
    void onEnableChanged();
    void onBeamSplit();
    void onScaleFactorUpdated();
    void onSegmentsChanged();
    void onParametersChanged();
    void onTangibleSectionChanged();

    /*These come from the VM*/
    void onMaterialChangedVMSide();
    void onBeamSizeChangedVMSide();
    /*-----*/

    void onExtremeDisplacementChanged();
private:
    void initView();
    WeakBeamPtr m_beam;
    Qt3DEntityPtr m_component3D;
    static QQmlComponent* m_qqmlcomponent;
    QQmlContext* m_qqmlcontext;
};

typedef QList<QVector4D> QVector4List;

#endif // BEAMVM_H
