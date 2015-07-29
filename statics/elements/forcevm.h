#ifndef FORCEVM_H
#define FORCEVM_H

#include "statics/abstractelementviewmodel.h"

class ForceVM: public AbstractElementViewModel
{
    Q_OBJECT
public:
    ForceVM(QObject* parent=0);
    void setHasTipOnApplicationPoint(bool val){m_hasTipOnApplicationPoint=val;}
    bool hasTipOnApplicationPoint(){return m_hasTipOnApplicationPoint; }

public slots:
    void onElementNameChanged(QString name);
    void onElementDestroyed();
    void onElementVmChanged();

    void onStatusComplete();

    void onForceApplicationPointChanged(QVector3D val);
    void onForceApplicationElementChanged(QString);
    void onForceVectorChanged(QVector3D);
signals:
    void updateForcePosition(QVector3D val);
    void updateForceMagnitude(qreal val);
    void updateForceDirection(qreal val);

private:
    bool m_hasTipOnApplicationPoint;
};

#endif // FORCEVM_H
