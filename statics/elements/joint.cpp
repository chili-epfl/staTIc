#include "joint.h"
#include "beam.h"
#include "statics/abstractstaticsmodule.h"
Joint::Joint(QVector3D position,QString name,QObject* parent):
    AbstractElement(name,parent),
    m_pos(position),
    m_reaction(0,0,0),
    m_reaction_momentum(0,0,0),
    m_displacement(0,0,0),
    m_displacement_rot(0,0,0),
    m_support_X(0),
    m_support_Y(0),
    m_support_Z(0),
    m_support_XX(0),
    m_support_YY(0),
    m_support_ZZ(0)
{

}

void Joint::addConnectedBeam(BeamPtr b){
    m_connected_beams.append(b.toWeakRef());
    connect(b.data(),SIGNAL(destroyed(QObject*)),this,SLOT(onBeamDestroyed()));
    emit connectedBeamsChanged();
}

void Joint::onBeamDestroyed(){
    QList<WeakBeamPtr> newlist;
    Q_FOREACH(WeakBeamPtr b, m_connected_beams){
        if(!b.isNull()){
            newlist.append(b);
        }
    }
    m_connected_beams=newlist;
    emit connectedBeamsChanged();
}



void Joint::setReaction(QVector3D reaction){
    if(m_reaction!=reaction){
        m_reaction=reaction;
        emit reactionChanged();
    }
}

void Joint::setReactionMomentum(QVector3D momentum){
    if(m_reaction_momentum!=momentum){
        m_reaction_momentum=momentum;
        emit reactionChanged();
    }
}


void Joint::support(bool  &support_X,bool &support_Y,bool &support_Z,
                    bool &support_XX,bool &support_YY,bool &support_ZZ){

    support_X=m_support_X;
    support_Y=m_support_Y;
    support_Z=m_support_Z;
    support_XX=m_support_XX;
    support_YY=m_support_YY;
    support_ZZ=m_support_ZZ;

}

QVector3D Joint::scaledPosition()
{
    return m_pos*AbstractStaticsModule::modelScale();
}


void Joint::setDisplacement(QVector3D displacement){
    if(displacement!=m_displacement){
        m_displacement=displacement;
        emit displacementChanged();
    }

}
void Joint::setDisplacementRot(QVector3D displacement){
    if(displacement!=m_displacement_rot){
        m_displacement_rot=displacement;
        emit displacementRotChanged();
    }
}



void Joint::setSupport(bool  support_X,bool support_Y,bool support_Z,
                       bool support_XX,bool support_YY,bool support_ZZ){
    bool updated=false;
    if(support_X!=m_support_X){
        m_support_X=support_X;
        updated=true;
    }
    if(support_Y!=m_support_Y){
        m_support_Y=support_Y;
        updated=true;
    }
    if(support_Z!=m_support_Z){
        m_support_Z=support_Z;
        updated=true;
    }
    if(support_XX!=m_support_XX){
        m_support_XX=support_XX;
        updated=true;
    }
    if(support_YY!=m_support_YY){
        m_support_YY=support_YY;
        updated=true;
    }
    if(support_ZZ!=m_support_ZZ){
        m_support_ZZ=support_ZZ;
        updated=true;
    }
    if(updated){
        emit supportChanged();
    }



}