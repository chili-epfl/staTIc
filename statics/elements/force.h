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
    ~Force(){
        last_ids.append(this->ID);
    }

    QVector3D getApplicationPoint(){return m_applicationPoint;}
    QString getApplicationElement(){return m_applicationElement;}
    QVector3D getVector(){return m_vector;}

    void setApplicationPoint(QVector3D applicationPoint){this->m_applicationPoint=applicationPoint; emit elementChanged();}
    void setApplicationElement(QString applicationElement){this->m_applicationElement=applicationElement;emit elementChanged();}
    void setVector(QVector3D vector){this->m_vector=vector;emit elementChanged();}

signals:

public slots:

private:    
    int generateExternalForceID();
    static QList<int> last_ids;    
    QVector3D m_applicationPoint;
    QString m_applicationElement;
    QVector3D m_vector;
    int ID;


};

#endif // FORCE_H
