#include "nodeload.h"
#include "joint.h"

NodeLoad::NodeLoad(JointPtr joint,QString name,QObject* parent):
    AbstractElement(name,parent),
    m_force(1,0,0),
    m_momentum(0,0,0)
{
    m_joint=joint.toWeakRef();
    connect(m_joint.data(),SIGNAL(destroyed(QObject*)),this,SIGNAL(killMe()));
}

void NodeLoad::setForce(QVector3D force){
    if(force!=m_force && !force.isNull()){
        m_force=force;
        emit forceChanged();
    }

}
void NodeLoad::setMomentum(QVector3D momentum){
    if(momentum!=m_momentum){
        m_momentum=momentum;
        emit momentumChanged();
    }
}


