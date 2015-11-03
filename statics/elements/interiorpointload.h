#ifndef INTERIORPOINTLOAD_H
#define INTERIORPOINTLOAD_H
#include "abstractelement.h"
#include <QVector3D>

class Beam;

class InteriorPointLoad : public AbstractElement
{
    Q_OBJECT
public:
    InteriorPointLoad(Beam* beam,qreal distance,QString name=QString(),QObject *parent=0);

    Beam* beam();

    void setDistance(qreal distance);
    qreal distance(){return m_distance;}

    void setForce(QVector3D force);
    QVector3D force(){return m_force;}

signals:
    void forceChanged();
    void distanceChanged();
private:
    Beam* m_beam;
    qreal m_distance;
    QVector3D m_force;
};

#endif // INTERIORPOINTLOAD_H
