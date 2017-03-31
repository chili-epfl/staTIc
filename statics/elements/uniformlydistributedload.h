#ifndef UNIFORMLYDISTRIBUTEDLOAD_H
#define UNIFORMLYDISTRIBUTEDLOAD_H

#include "abstractelement.h"
#include <QVector3D>
#include <QQmlComponent>
class Beam;
typedef QSharedPointer<Beam> BeamPtr;
typedef QWeakPointer<Beam> WeakBeamPtr;

class UniformlyDistributedLoad: public AbstractElement
{
    Q_OBJECT
    Q_PROPERTY(QVector3D force READ force WRITE setForce NOTIFY forceChanged)
public:
    UniformlyDistributedLoad(BeamPtr beam,QString name,QObject* parent=0);
    void createQmlEntity(QVariantMap aesthetics=QVariantMap());

    WeakBeamPtr beam(){return m_beam;}
    QVector3D force(){return m_force;}
    QVector3D forceLocal();
    void setForce(QVector3D force);

signals:
    void forceChanged();
private:
    WeakBeamPtr m_beam;
    QVector3D m_force;
    static QQmlComponent* m_qqmlcomponent;
    Qt3DEntityPtr m_component3D;

};


#endif // UNIFORMLYDISTRIBUTEDLOAD_H
