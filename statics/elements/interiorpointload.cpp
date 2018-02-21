#include "interiorpointload.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"
#include "statics/abstractstaticsmodule.h"
#include <QMatrix4x4>

InteriorPointLoad::InteriorPointLoad(BeamPtr beam,qreal distance,
                                     QString name,QObject *parent):
    AbstractElement(name,parent),
    m_force(0,1,0)
{
    m_beam=beam.toWeakRef();
    m_distance=distance;
    connect(m_beam.data(),SIGNAL(destroyed(QObject*)),this, SIGNAL(killMe()));
}

void InteriorPointLoad::createQmlEntity(QVariantMap aesthetics)
{

}

WeakBeamPtr InteriorPointLoad::beam(){
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

QVector3D InteriorPointLoad::forceLocal(){

    WeakJointPtr e1,e2;
    m_beam.toStrongRef()->extremes(e1,e2);
    QVector3D c=e2.toStrongRef()->position()-e1.toStrongRef()->position();
    c.normalize();

    QMatrix4x4 mat;
    if( fabs(c.y())==1){
        mat.setRow(0,QVector4D(0,c.y(),0,0));
        mat.setRow(1,QVector4D(-c.y(),0,0,0));
        mat.setRow(2,QVector4D(0,0,1,0));
        mat.setRow(3,QVector4D(0,0,0,1));
    }
    else{
        qreal den=sqrt(1.0- c.y()*c.y() );
        mat.setRow(0,QVector4D(c.x(),c.y(),c.z(),0));
        mat.setRow(1,QVector4D(-c.x()*c.y()/den,den,-c.y()*c.z()/den,0));
        mat.setRow(2,QVector4D(-c.z()/den,0,c.x()/den,0));
        mat.setRow(3,QVector4D(0,0,0,1));
    }

    return mat.mapVector(m_force);

}
