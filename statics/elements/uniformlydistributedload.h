#ifndef UNIFORMLYDISTRIBUTEDLOAD_H
#define UNIFORMLYDISTRIBUTEDLOAD_H

#include "beam.h"
#include <QVector3D>
class UniformlyDistributedLoad: public AbstractElement
{
    Q_OBJECT
public:
    UniformlyDistributedLoad(Beam* beam,QString name,QObject* parent=0);

    Beam* beam(){return m_beam;}
    QVector3D force(){return m_force;}

    void setForce(QVector3D force);

signals:
    void forceChanged();
private:
    Beam* m_beam;
    QVector3D m_force;
};

#endif // UNIFORMLYDISTRIBUTEDLOAD_H
