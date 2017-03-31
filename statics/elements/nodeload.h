#ifndef NODELOAD_H
#define NODELOAD_H
#include "statics/elements/abstractelement.h"
#include <QVector3D>
#include <QQmlComponent>

class Joint;
typedef QSharedPointer<Joint> JointPtr;
typedef QWeakPointer<Joint> WeakJointPtr;

class NodeLoad : public AbstractElement
{
    Q_OBJECT
    Q_PROPERTY(QVector3D force READ force WRITE setForce NOTIFY forceChanged)
    Q_PROPERTY(QVector3D momentum READ momentum WRITE setMomentum NOTIFY momentumChanged)
public:
    NodeLoad(JointPtr joint,QString name=QString(),QObject *parent=0);
    void createQmlEntity(QVariantMap aesthetics=QVariantMap());

    QVector3D force(){return m_force;}
    QVector3D momentum(){return m_momentum;}
    WeakJointPtr joint(){return m_joint;}

    void setForce(QVector3D force);
    void setMomentum(QVector3D momentum);
signals:
    void forceChanged();
    void momentumChanged();
private:
    WeakJointPtr m_joint;
    QVector3D m_force;
    QVector3D m_momentum;
    static QQmlComponent* m_qqmlcomponent;
    Qt3DEntityPtr m_component3D;

};



#endif // NODELOAD_H
