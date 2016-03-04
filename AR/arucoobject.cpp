#include "arucoobject.h"

ArucoObject::ArucoObject(QQuickItem *parent):
    QQuickItem(parent)
{
    m_visible=false;
    m_posematrix=QMatrix4x4();
    m_objectId=QString();
}

