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
    BeamPtr ptr=m_beam.toStrongRef();
    if(!ptr.isNull()){
        if(force!=m_force && !force.isNull()){
            m_force=force/ptr->size().width();
            emit forceChanged();
        }
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
        QVector2D relativeExtent=m_extent/ptr->length();
        begin=QVector3D(qMax(0.0f,m_relative_position.x()+relativeExtent.x()),
                        qMax(0.0f,m_relative_position.y()+relativeExtent.x()),
                        qMax(0.0f,m_relative_position.z()+relativeExtent.x()))*ptr->length();
        end=QVector3D(qMin(1.0f,m_relative_position.x()+relativeExtent.y()),
                      qMin(1.0f,m_relative_position.y()+relativeExtent.y()),
                      qMin(1.0f,m_relative_position.z()+relativeExtent.y()))*ptr->length();

    }
}

