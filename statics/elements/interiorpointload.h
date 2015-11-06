#ifndef INTERIORPOINTLOAD_H
#define INTERIORPOINTLOAD_H
#include "abstractelement.h"
#include <QVector3D>

class Beam;
typedef QSharedPointer<Beam> BeamPtr;
typedef QWeakPointer<Beam> WeakBeamPtr;
class InteriorPointLoad : public AbstractElement
{
    Q_OBJECT
public:
    InteriorPointLoad(BeamPtr beam,qreal distance,QString name=QString(),QObject *parent=0);

    WeakBeamPtr beam();

    void setDistance(qreal distance);
    qreal distance(){return m_distance;}

    void setForce(QVector3D force);
    QVector3D force(){return m_force;}
    QVector3D forceLocal();

signals:
    void forceChanged();
    void distanceChanged();
private:
    WeakBeamPtr m_beam;
    qreal m_distance;
    QVector3D m_force;
};



#endif // INTERIORPOINTLOAD_H
