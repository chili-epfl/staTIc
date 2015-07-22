#include "joint.h"

Joint::Joint(QString name,QObject* parent):
    AbstractElement(name,parent)
{
    jointSupport=SupportType::NOSUPPORT;
}

Joint::Joint(QObject* parent):
    AbstractElement(parent)
{
}
