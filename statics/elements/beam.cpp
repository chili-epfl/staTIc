#include "beam.h"

Beam::Beam(QString name,QObject* parent):AbstractElement(name,parent)
{
    m_vm=0;

}

Beam::Beam(QObject* parent):AbstractElement(parent)
{
    m_vm=0;

}

void Beam::setVm(AbstractElementViewModel *vm){
    if(m_vm==vm) return;
    m_vm=qobject_cast<BeamVM*>(vm);
    if(!m_vm) qFatal("Beam::setVm: invalid cast");
    m_vm->setElement(this);

    /*Default bindings*/
    connect(this,SIGNAL(objectNameChanged(QString)),m_vm,SLOT(onElementNameChanged(QString)));
    connect(this,SIGNAL(destroyed(QObject*)),m_vm,SLOT(onElementDestroyed()));
    connect(this,SIGNAL(VmChanged()),m_vm,SLOT(onElementVmChanged()));

    /*Element Specific bindings*/

    connect(this,SIGNAL(extremesChanged()),m_vm,SLOT(onBeamExtremesChanged()));
    connect(this,SIGNAL(axialForceChanged(qreal)),m_vm,SLOT(onBeamAxialForceChanged(qreal)));

}
