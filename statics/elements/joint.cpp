#include "joint.h"

Joint::Joint(QString name,QObject* parent):
    AbstractElement(name,parent)
{
    m_jointSupport=SupportType::NOSUPPORT;
    m_vm=0;

}

Joint::Joint(QObject* parent):
    AbstractElement(parent)
{
    m_vm=0;

}

void Joint::setVm(AbstractElementViewModel *vm){
    if(m_vm==vm) return;
    m_vm=qobject_cast<JointVM*>(vm);
    if(!m_vm) qFatal("Joint::setVm: invalid cast");
    m_vm->setElement(this);

    /*Default bindings*/
    connect(this,SIGNAL(objectNameChanged(QString)),m_vm,SLOT(onElementNameChanged(QString)));
    connect(this,SIGNAL(destroyed(QObject*)),m_vm,SLOT(onElementDestroyed()));
    connect(this,SIGNAL(VmChanged()),m_vm,SLOT(onElementVmChanged()));

    /*Element Specific bindings*/

    connect(this,SIGNAL(reactionChanged(QVector3D)),m_vm,SLOT(onJointReactionChanged(QVector3D)));
    connect(this,SIGNAL(positionChanged(QVector3D)),m_vm,SLOT(onJointPositionChanged(QVector3D)));
    connect(this,SIGNAL(supportTypeChanged()),m_vm,SLOT(onJointSupportTypeChanged()));
    connect(this,SIGNAL(connectedBeamsChanged()),m_vm,SLOT(onJointConnectedBeamsChanged()));

}
