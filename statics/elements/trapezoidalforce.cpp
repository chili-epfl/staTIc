#include "trapezoidalforce.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"

#include <QVector3D>
#include <QMatrix4x4>
TrapezoidalForce::TrapezoidalForce(BeamPtr beam, QString name, QObject *parent)
    :AbstractElement(name,parent),
    m_force(1,0,0),
    m_local_begin(0,0,0),
    m_local_end(0.1,0,0)
{
    m_beam=beam.toWeakRef();
    connect(m_beam.data(),SIGNAL(destroyed(QObject*)),this, SIGNAL(killMe()));

}

QVector3D TrapezoidalForce::forceLocal()
{
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

void TrapezoidalForce::setForce(QVector3D force)
{
    if(force!=m_force && !force.isNull()){
        m_force=force;
        emit forceChanged();
    }
}

void TrapezoidalForce::setRelativePosition(QVector3D begin, QVector3D end)
{
    if(begin!=end && (begin!=m_local_begin || end!=m_local_end)){
        m_local_begin=begin;
        m_local_end=end;
        emit relativePositionChanged();
    }
}

void TrapezoidalForce::relativePosition(QVector3D &begin, QVector3D &end)
{
    begin=m_local_begin;
    end=m_local_end;
}

void TrapezoidalForce::positionOnBeam(QVector3D &begin, QVector3D &end)
{
    BeamPtr ptr=m_beam.toStrongRef();
    if(!ptr.isNull()){
        begin=m_local_begin*QVector3D(ptr->length(),ptr->size().width(),ptr->size().height());
        end=m_local_end*QVector3D(ptr->length(),ptr->size().width(),ptr->size().height());

    }
}

