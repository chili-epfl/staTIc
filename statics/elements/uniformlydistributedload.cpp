#include "uniformlydistributedload.h"
#include <QVector3D>


UniformlyDistributedLoad::UniformlyDistributedLoad(Beam* beam,QString name,QObject* parent):
   AbstractElement(name,parent),
   m_force(1,0,0),
   m_beam(beam)
{
}

void UniformlyDistributedLoad::setForce(QVector3D force){
    if(force!=m_force && !force.isNull()){
        m_force=force;
        emit forceChanged();
    }
}
