#include "joint.h"
#include "beam.h"

Joint::Joint(QString name):
    AbstractElement(name)
{
    jointSupport=SupportType::NOSUPPORT;
}

