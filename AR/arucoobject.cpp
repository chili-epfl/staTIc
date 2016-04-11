#include "arucoobject.h"

ArucoObject::ArucoObject(QQuickItem *parent):
    QQuickItem(parent),
    m_rotation(),
    m_translation()
{
    m_visible=false;
    m_objectId=QString();
}

void ArucoObject::prependQuaternion(QQuaternion q)
{
    m_pre=q*m_pre;
}

void ArucoObject::appendQuaternion(QQuaternion q)
{
    m_app=m_app*q;
}

