#include "beam.h"

Beam::Beam(QString name,QObject* parent):AbstractElement(name,parent)
{
}

Beam::Beam(QObject* parent):AbstractElement(parent)
{
}


void Beam::setExtremes(QPair<Joint *, Joint *> extremes){
    if(m_extremes!=extremes){
        m_extremes=extremes;
        connect(this,SIGNAL(extremesChanged()),m_extremes.first,SIGNAL(connectedBeamsChanged()));
        connect(this,SIGNAL(extremesChanged()),m_extremes.second,SIGNAL(connectedBeamsChanged()));
        emit extremesChanged();
    }
}
void Beam::setAxialForce(qreal val){
    if(m_axial_force!=val){
        m_axial_force=val;
        emit axialForceChanged(m_axial_force);
    }
}
