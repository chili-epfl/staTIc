#include "interiorpointload.h"
#include "statics/elements/beam.h"

InteriorPointLoad::InteriorPointLoad(Beam* beam,qreal distance,
                                     QString name,QObject *parent):
    AbstractElement(name,parent),
    m_force(0,1,0)
{
    m_beam=beam;
    m_distance=distance;
}

Beam* InteriorPointLoad::beam(){
    return m_beam;
}

void InteriorPointLoad::setDistance(qreal distance){
    if(distance!=m_distance){
        m_distance=distance;
        emit distanceChanged();
    }
}

void InteriorPointLoad::setForce(QVector3D force){
    if(m_force!=force && !force.isNull()){
        m_force=force;
        emit forceChanged();
    }
}

