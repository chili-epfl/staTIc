#include "trapezoidalforce.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"

#include <QVector3D>
#include <QVector2D>

#include <QMatrix4x4>
TrapezoidalForce::TrapezoidalForce(BeamPtr beam, QString name, QObject *parent)
    :AbstractElement(name,parent),
    m_force(1,0,0),
    m_relative_position(0,0,0),
    m_extent(0,1)
{
    m_beam=beam.toWeakRef();
    connect(m_beam.data(),SIGNAL(destroyed(QObject*)),this, SIGNAL(killMe()));

}

QVector3D TrapezoidalForce::forceLocal()
{
    WeakJointPtr e1,e2;
    m_beam.toStrongRef()->extremes(e1,e2);
    QVector3D c=e2.toStrongRef()->position()-e1.toStrongRef()->position();
    QVector3D force=m_force;

    if(fabs(m_extent.x()-m_extent.y())>m_beam.toStrongRef()->length()){
        force/=m_beam.toStrongRef()->length();
    }
    else {
        force/=fabs(m_extent.x()-m_extent.y());
    }
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
    return mat.mapVector(force);
}

void TrapezoidalForce::setForce(QVector3D force)
{
    if(force!=m_force && !force.isNull()){
        m_force=force;
        emit forceChanged();
    }

}

void TrapezoidalForce::setRelativePosition(QVector3D relativePosition, QVector2D extent)
{
    if(relativePosition!=m_relative_position || (extent!=m_extent)){
        m_relative_position=relativePosition;
        m_extent=extent;
        emit relativePositionChanged();
    }
}

void TrapezoidalForce::relativePosition(QVector3D &relativePosition,QVector2D& extent)
{
    relativePosition=m_relative_position;
    extent=m_extent;
}

void TrapezoidalForce::positionOnBeam(QVector3D &begin, QVector3D &end)
{
    BeamPtr ptr=m_beam.toStrongRef();
    if(!ptr.isNull()){
        qreal actualPosition=m_relative_position.x()*ptr->length();
        qreal tmp_begin=actualPosition+m_extent.x();
        qreal tmp_end=actualPosition+m_extent.y();

        qreal surplus_begin=0;
        if(tmp_begin<0){
             surplus_begin=-tmp_begin;
             tmp_begin=0;
        }
        qreal surplus_end=0;
        if(tmp_end>ptr->length()){
            surplus_end=tmp_end-ptr->length();
            tmp_end=ptr->length();
        }

        tmp_begin=qMax(0.0,tmp_begin-surplus_end);
        tmp_end=qMin(ptr->length(),tmp_end+surplus_begin);

        begin=QVector3D(tmp_begin,tmp_begin,tmp_begin);
        end=QVector3D(tmp_end,tmp_end,tmp_end);

    }
}

void TrapezoidalForce::setBeam(BeamPtr beam)
{
    if(m_beam!=beam){
        disconnect(m_beam.data());
        m_beam=beam.toWeakRef();
        connect(m_beam.data(),SIGNAL(destroyed(QObject*)),this, SIGNAL(killMe()));
    }
}

