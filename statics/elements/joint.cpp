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

