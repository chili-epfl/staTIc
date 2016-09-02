#include "quaternionhelper.h"

QuaternionHelper::QuaternionHelper(QObject *parent) : QObject(parent)
{

}

QVector3D QuaternionHelper::rotateVector(QQuaternion q, QVector3D v)
{
    return q.rotatedVector(v);
}

QQuaternion QuaternionHelper::product(QQuaternion q1, QQuaternion q2)
{
    return (q1*q2).normalized();
}

QQuaternion QuaternionHelper::invert(QQuaternion q)
{
    return q.inverted();
}

QVector3D QuaternionHelper::eulerAngles(QQuaternion q)
{
    return q.toEulerAngles();
}
