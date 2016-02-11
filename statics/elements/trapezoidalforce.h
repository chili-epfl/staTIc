#ifndef TRAPEZOIDALFORCE_H
#define TRAPEZOIDALFORCE_H

#include "abstractelement.h"
#include <QVector3D>
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
    void setRelativePosition(QVector3D begin, QVector3D end);
    void relativePosition(QVector3D& begin, QVector3D& end);
    void positionOnBeam(QVector3D& begin, QVector3D& end);
signals:
    void forceChanged();
    void relativePositionChanged();
private:
    WeakBeamPtr m_beam;
    QVector3D m_force;
    //Forces are applied on elements,
    //these are the begin and end positions of the force
    //the component spam between 0 and 1 in relation to the size
    //of the element the ofrce is applied on
    QVector3D m_local_begin;
    QVector3D m_local_end;
};

typedef QSharedPointer<TrapezoidalForce> TrapezoidalForcePtr;
typedef QWeakPointer<TrapezoidalForce> WeakTrapezoidalForcePtr;

#endif // TRAPEZOIDALFORCE_H
