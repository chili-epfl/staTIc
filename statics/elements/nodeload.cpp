#include "nodeload.h"

NodeLoad::NodeLoad(Joint* joint,QString name,QObject* parent):
    AbstractElement(name,parent),
    m_force(1,0,0),
    m_momentum(0,0,0),
    m_joint(joint)
{

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


