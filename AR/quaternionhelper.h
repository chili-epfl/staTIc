#ifndef QUATERNIONHELPER_H
#define QUATERNIONHELPER_H

#include <QObject>
#include <QQuaternion>
class QuaternionHelper : public QObject
{
    Q_OBJECT
public:
    explicit QuaternionHelper(QObject *parent = 0);
    Q_INVOKABLE QVector3D rotateVector(QQuaternion q,QVector3D v);
signals:

public slots:
};

#endif // QUATERNIONHELPER_H
