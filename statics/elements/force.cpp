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
