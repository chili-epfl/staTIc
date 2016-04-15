#include "quaternionhelper.h"

QuaternionHelper::QuaternionHelper(QObject *parent) : QObject(parent)
{

}

QVector3D QuaternionHelper::rotateVector(QQuaternion q, QVector3D v)
{
    return q.rotatedVector(v);
}
