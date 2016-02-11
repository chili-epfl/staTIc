#ifndef TRAPEZOIDALFORCEVM_H
#define TRAPEZOIDALFORCEVM_H

#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/trapezoidalforce.h"
class TrapezoidalForceVM: public AbstractElementViewModel
{
    Q_OBJECT
public:
    explicit TrapezoidalForceVM(TrapezoidalForcePtr force,
                                Qt3DCore::QEntity* sceneRoot,
                                Qt3DCore::QEntity* parentEntity,
                                QObject* parent=0 );
    WeakTrapezoidalForcePtr trapezoidalForce(){return m_trapezoidalForce;}
public slots:
    void onForceChanged();
    void onRelativePositionChanged();
private:
    void initView(Qt3DCore::QEntity* parentEntity);
    WeakTrapezoidalForcePtr m_trapezoidalForce;
    Qt3DCore::QEntity* m_component3D;
};

#endif // TRAPEZOIDALFORCEVM_H
