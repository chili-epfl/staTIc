#include "force.h"


QList<int> Force::last_ids={0};


Force::Force(QObject *parent) : AbstractElement(parent)
{
    this->setObjectName(generateExternalForceID());
}

QString Force::generateExternalForceID(){
    if(last_ids.size()==1)
        return QString::number(last_ids.at(0)+1);
    else{
        /*Recycle ids*/
        std::sort(last_ids.begin(),last_ids.end());
        int id=last_ids.front();
        last_ids.pop_front();
        return QString::number(id);
    }
}
