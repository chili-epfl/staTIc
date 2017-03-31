#include "abstractelement.h"

AbstractElement::AbstractElement(QString name,QObject *parent) :
    AbstractElement(parent)
{
    this->setObjectName(name);
}

void AbstractElement::setSceneRoot(Qt3DCore::QEntity *sceneroot)
{
    m_sceneRoot=sceneroot;
}

AbstractElement::AbstractElement(QObject *parent) : QObject(parent)
{
    m_sceneRoot=Q_NULLPTR;
}
