#include "force.h"


QList<int> Force::last_ids={0};


Force::Force(QObject *parent) : AbstractElement(parent)
{
    m_vm=0;
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

void Force::setVm(AbstractElementViewModel *vm){
    if(m_vm==vm) return;
    m_vm=qobject_cast<ForceVM*>(vm);
    if(!m_vm) qFatal("Force::setVm: invalid cast");
    m_vm->setElement(this);

    /*Default bindings*/
    connect(this,SIGNAL(objectNameChanged(QString)),m_vm,SLOT(onElementNameChanged(QString)));
    connect(this,SIGNAL(destroyed(QObject*)),m_vm,SLOT(onElementDestroyed()));
    connect(this,SIGNAL(VmChanged()),m_vm,SLOT(onElementVmChanged()));

    /*Element Specific bindings*/

    connect(this,SIGNAL(applicationPointChanged(QVector3D)),m_vm,SLOT(onForceApplicationPointChanged(QVector3D)));
    connect(this,SIGNAL(applicationElementChanged(QString)),m_vm,SLOT(onForceApplicationElementChanged(QString)));
    connect(this,SIGNAL(vectorChanged(QVector3D)),m_vm,SLOT(onForceVectorChanged(QVector3D)));

}
