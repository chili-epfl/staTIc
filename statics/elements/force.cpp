#include "force.h"


QList<int> Force::last_ids={0};


Force::Force(QObject *parent) : AbstractElement(parent)
{
    ID=generateExternalForceID();
    this->setObjectName("Force_"+QString::number(ID));
}

int Force::generateExternalForceID(){
    if(last_ids.size()==1){
        last_ids[0]=last_ids[0]+1;
        return (last_ids.at(0));
    }
    else{
        /*Recycle ids*/
        std::sort(last_ids.begin(),last_ids.end());
        int id=last_ids.front();
        last_ids.pop_front();
        return (id);
    }
}

void Force::setApplicationPoint(QVector3D applicationPoint){
    if(m_applicationPoint!=applicationPoint){
        m_applicationPoint=applicationPoint;
        emit applicationPointChanged(m_applicationPoint);
    }
}
void Force::setApplicationElement(AbstractElement* applicationElement){
    if(m_applicationElement!=applicationElement){
        m_applicationElement=applicationElement;
        emit applicationElementChanged(m_applicationElement);
    }
}
void Force::setVector(QVector3D vector){
    if(m_vector!=vector){
        m_vector=vector;
        emit vectorChanged(m_vector);
    }
}
