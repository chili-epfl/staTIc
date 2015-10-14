#include "joint.h"

Joint::Joint(QString name,QObject* parent):
    AbstractElement(name,parent)
{
    m_jointSupport=SupportType::NOSUPPORT;
}

Joint::Joint(QObject* parent):
    AbstractElement(parent)
{
}

void Joint::setSupportType(SupportType jointSupport){
    if(m_jointSupport!=jointSupport){
        m_jointSupport=jointSupport;
        emit supportTypeChanged();
    }
}


void Joint::setPosition(QVector3D pos){
    if(m_pos!=pos){
        m_pos=pos;
        emit positionChanged(m_pos);
    }
}
void Joint::setReaction(QVector3D reaction){
    if(m_reaction!=reaction){
        m_reaction=reaction;
        emit reactionChanged(m_reaction);
    }
}
