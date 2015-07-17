#ifndef FORCE_H
#define FORCE_H

#include <QObject>
#include <QVector3D>
#include "abstractelement.h"
class Force : public QObject
{
    Q_OBJECT
public:
    explicit Force(QObject *parent = 0);
    QVector3D applicationPoint;
    QString applicationElement;
    QVector3D vector;
signals:

public slots:
private:
    QString generateExternalForceID();
    static QList<int> last_ids;

};

#endif // FORCE_H
