#ifndef NODELOAD_H
#define NODELOAD_H
#include "statics/elements/abstractelement.h"
#include <QVector3D>
class Joint;
typedef QSharedPointer<Joint> JointPtr;
typedef QWeakPointer<Joint> WeakJointPtr;

class NodeLoad : public AbstractElement
{
    Q_OBJECT
public:
    NodeLoad(JointPtr joint,QString name=QString(),QObject *parent=0);

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
};



#endif // NODELOAD_H
