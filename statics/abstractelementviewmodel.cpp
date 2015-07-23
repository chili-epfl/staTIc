#include "abstractelementviewmodel.h"

AbstractElementViewModel::AbstractElementViewModel(QObject *parent) : QObject(parent)
{
    m_element=0;
    m_sceneRoot=0;
    connect(this,SIGNAL(propertyChanged()),this,SLOT(onPropertyChanged()));
}

