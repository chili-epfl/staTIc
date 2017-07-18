#ifndef TRAPEZOIDALFORCE_H
#define TRAPEZOIDALFORCE_H

#include "abstractelement.h"
#include <QVector3D>
#include <QVector2D>
#include <QQmlComponent>

class Beam;
typedef QSharedPointer<Beam> BeamPtr;
typedef QWeakPointer<Beam> WeakBeamPtr;

class TrapezoidalForce: public AbstractElement
{
    Q_OBJECT
    Q_PROPERTY(QVector3D relativePosition READ relativePosition WRITE setRelativePosition NOTIFY relativePositionChanged)
    Q_PROPERTY(QVector2D extent READ extent WRITE setExtent NOTIFY extentChanged)
    Q_PROPERTY(QVector3D force READ force WRITE setForce NOTIFY forceChanged)
public:
    TrapezoidalForce(BeamPtr beam,QString name,QObject* parent=0);
    void createQmlEntity(QVariantMap aesthetics=QVariantMap());

    WeakBeamPtr beam(){return m_beam;}
    QVector3D relativePosition(){return m_relative_position;}
    QVector2D extent(){return m_extent;}
    void setRelativePosition(QVector3D relativePosition);
    void setExtent(QVector2D extent);

    QVector3D force(){return m_force;}
    QVector3D forceLocal();
    void setForce(QVector3D force);
    void setRelativePosition(QVector3D relativePosition, QVector2D extent);
    void relativePosition(QVector3D& relativePosition, QVector2D &extent);
    void positionOnBeam(QVector3D& begin, QVector3D& end);
    void setBeam(BeamPtr beam);
    Qt3DEntityPtr component3D(){return m_component3D;}
signals:
    void forceChanged();
    void relativePositionChanged();
    void extentChanged();
private:
    WeakBeamPtr m_beam;
    QVector3D m_force;
    //Forces are applied on elements,
    //the component spam between 0 and 1 in relation to the size
    QVector3D m_relative_position;
    //Extent---real size
    QVector2D m_extent;
    static QQmlComponent* m_qqmlcomponent;
    Qt3DEntityPtr m_component3D;

};

typedef QSharedPointer<TrapezoidalForce> TrapezoidalForcePtr;
typedef QWeakPointer<TrapezoidalForce> WeakTrapezoidalForcePtr;

#endif // TRAPEZOIDALFORCE_H
