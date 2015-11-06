#ifndef UNIFORMLYDISTRIBUTEDLOAD_H
#define UNIFORMLYDISTRIBUTEDLOAD_H

#include "abstractelement.h"
#include <QVector3D>
class Beam;
typedef QSharedPointer<Beam> BeamPtr;
typedef QWeakPointer<Beam> WeakBeamPtr;

class UniformlyDistributedLoad: public AbstractElement
{
    Q_OBJECT
public:
    UniformlyDistributedLoad(BeamPtr beam,QString name,QObject* parent=0);
    WeakBeamPtr beam(){return m_beam;}
    QVector3D force(){return m_force;}
    QVector3D forceLocal();
    void setForce(QVector3D force);

signals:
    void forceChanged();
private:
    WeakBeamPtr m_beam;
    QVector3D m_force;
};


#endif // UNIFORMLYDISTRIBUTEDLOAD_H
