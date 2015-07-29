#ifndef BEAMVM_H
#define BEAMVM_H

#include "statics/abstractelementviewmodel.h"

class BeamVM : public AbstractElementViewModel
{
    Q_OBJECT
public:
    BeamVM(QObject* parent=0);
public slots:
    void onElementNameChanged(QString name);
    void onElementDestroyed();
    void onElementVmChanged();

    void onBeamExtremesChanged();
    void onBeamAxialForceChanged(qreal val);

    void onElementSelected();
    void onStatusComplete();

signals:
    void updateForceMagnitude(qreal val);
    void updateForceDirectionEx1(qreal val);
    void updateForceDirectionEx2(qreal val);


};

#endif // BEAMVM_H
