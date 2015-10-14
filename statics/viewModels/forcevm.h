#ifndef FORCEVM_H
#define FORCEVM_H

#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/force.h"

class ForceVM: public AbstractElementViewModel
{
    Q_OBJECT

public:
    explicit ForceVM(Force* force,QObject* uiRoot,Qt3D::QEntity* sceneRoot,QObject* parent=0);

    bool visibility(){return m_visible;}
    void setVisibility(bool val){if(val!=m_visible){m_visible=val;emit visibilityChanged(m_visible);}}

    void setTipOnApplicationPoint(bool val){if(m_hasTipOnApplicationPoint!=val){m_hasTipOnApplicationPoint=val;emit tipOnApplicationPointChanged(m_hasTipOnApplicationPoint);}}
    bool tipOnApplicationPoint(){return m_hasTipOnApplicationPoint; }

    Force* force(){return m_force;}

public slots:
    void onElementDestroyed();

    /*Slots for signals from the model*/
    void onForceApplicationPointChanged(QVector3D val);
    void onForceApplicationElementChanged(QString);
    void onForceVectorChanged(QVector3D);
    /*------*/
signals:
    void updateForcePosition(QVector3D val);
    void updateForceMagnitude(qreal val);
    void updateForceDirection(qreal val);

    void visibilityChanged(bool val);
    void tipOnApplicationPointChanged(bool val);

private:
    void initView();

    bool m_hasTipOnApplicationPoint;
    bool m_visible;

    Force* m_force;

};

#endif // FORCEVM_H
