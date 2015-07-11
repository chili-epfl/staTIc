#include "joint.h"
#include "member.h"

Joint::Joint(QString name):
    AbstractElement(name)
{
    jointSupport=SupportType::NOSUPPORT;
}

