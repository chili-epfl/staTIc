#include "abstractstaticsmodule.h"

AbstractStaticsModule::AbstractStaticsModule():
    QObject()
{
    status=NOT_LOADED;
}
AbstractStaticsModule::AbstractStaticsModule(QString path):
    QObject()
{
}



void AbstractStaticsModule::addExternalForce(Force* force){
    force_list.append(force);
}
void AbstractStaticsModule::removeExternalForce(QString id){
    int i=0;
    for(Force* force: force_list){
        if(force->objectName().compare(id)==0){
            delete force;
            break;
        }
        i++;
    }
    force_list.removeAt(i);
}

