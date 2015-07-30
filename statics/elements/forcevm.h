#ifndef FORCEVM_H
#define FORCEVM_H

#include "statics/abstractelementviewmodel.h"

class ForceVM: public AbstractElementViewModel
{
    Q_OBJECT
    Q_PROPERTY(bool visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(bool tipOnApplicationPoint READ tipOnApplicationPoint WRITE setTipOnApplicationPoint NOTIFY tipOnApplicationPointChanged)
public:
    ForceVM(QObject* parent=0);

    bool visibility(){return m_visible;}
    void setVisibility(bool val){if(val!=m_visible){m_visible=val;emit visibilityChanged(m_visible);}}

    void setTipOnApplicationPoint(bool val){if(m_hasTipOnApplicationPoint!=val){m_hasTipOnApplicationPoint=val;emit tipOnApplicationPointChanged(m_hasTipOnApplicationPoint);}}
    bool tipOnApplicationPoint(){return m_hasTipOnApplicationPoint; }

    void select(){setVisibility(!m_visible);}
public slots:
    void onElementNameChanged(QString name);
    void onElementDestroyed();
    void onElementVmChanged();

    void onStatusComplete();

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
    bool m_hasTipOnApplicationPoint;
    bool m_visible;

};

#endif // FORCEVM_H
