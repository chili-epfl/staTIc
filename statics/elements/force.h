#ifndef FORCE_H
#define FORCE_H

#include <QObject>
#include <QVector3D>
#include "abstractelement.h"
#include "statics/elements/forcevm.h"

class Force : public AbstractElement
{
    Q_OBJECT
public:
    Force(QObject *parent = 0);
    ~Force(){ last_ids.append(this->objectName().toInt());}
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
