#include "abstractelement.h"

AbstractElement::AbstractElement(QString name,QObject *parent) : QObject(parent)
{
    this->setObjectName(name);
    m_vm=0;

}

AbstractElement::AbstractElement(QObject *parent) : QObject(parent)
{
    m_vm=0;

}
