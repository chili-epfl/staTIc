#ifndef FORCE_H
#define FORCE_H

#include <QObject>
#include <QVector3D>
#include "abstractelement.h"

class Force : public AbstractElement
{
    Q_OBJECT
public:
    Force(QObject *parent = 0);
    ~Force(){
        last_ids.append(this->ID);
    }

    QVector3D applicationPoint(){return m_applicationPoint;}
    AbstractElement* applicationElement(){return m_applicationElement;}
    QVector3D vector(){return m_vector;}

    void setApplicationPoint(QVector3D applicationPoint);
    void setApplicationElement(AbstractElement* applicationElement);
    void setVector(QVector3D vector);

signals:
    void applicationPointChanged(QVector3D applicationPoint);
    void applicationElementChanged(AbstractElement* applicationElementChanged);
    void vectorChanged(QVector3D vector);

private:    
    int generateExternalForceID();
    static QList<int> last_ids;

    QVector3D m_applicationPoint;
    AbstractElement* m_applicationElement;
    QVector3D m_vector;
    int ID;

};

#endif // FORCE_H
