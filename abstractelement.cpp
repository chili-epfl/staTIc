#include "abstractelement.h"

AbstractElement::AbstractElement(QString name,QObject *parent) : QObject(parent)
{
    this->setObjectName(name);
}

