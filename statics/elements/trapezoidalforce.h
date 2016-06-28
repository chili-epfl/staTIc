#ifndef TRAPEZOIDALFORCE_H
#define TRAPEZOIDALFORCE_H

#include "abstractelement.h"
#include <QVector3D>
#include <QVector2D>
class Beam;
typedef QSharedPointer<Beam> BeamPtr;
typedef QWeakPointer<Beam> WeakBeamPtr;

class TrapezoidalForce: public AbstractElement
{
    Q_OBJECT
public:
    TrapezoidalForce(BeamPtr beam,QString name,QObject* parent=0);

    WeakBeamPtr beam(){return m_beam;}
    QVector3D force(){return m_force;}
    QVector3D forceLocal();
    void setForce(QVector3D force);
    void setRelativePosition(QVector3D relativePosition, QVector2D extent);
    void relativePosition(QVector3D& relativePosition, QVector2D &extent);
    void positionOnBeam(QVector3D& begin, QVector3D& end);
    void setBeam(BeamPtr beam);

signals:
    void forceChanged();
    void relativePositionChanged();
private:
    WeakBeamPtr m_beam;
    QVector3D m_force;
    //Forces are applied on elements,
    //the component spam between 0 and 1 in relation to the size
    QVector3D m_relative_position;
    //Extent---real size
    QVector2D m_extent;
};

typedef QSharedPointer<TrapezoidalForce> TrapezoidalForcePtr;
typedef QWeakPointer<TrapezoidalForce> WeakTrapezoidalForcePtr;

#endif // TRAPEZOIDALFORCE_H
