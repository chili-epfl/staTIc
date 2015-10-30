#include "abstractelement.h"

AbstractElement::AbstractElement(QString name,QObject *parent) :
    AbstractElement(parent)
{
    this->setObjectName(name);
}

AbstractElement::AbstractElement(QObject *parent) : QObject(parent)
{

}
