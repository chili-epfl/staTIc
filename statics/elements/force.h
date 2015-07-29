#ifndef FORCE_H
#define FORCE_H

#include <QObject>
#include <QVector3D>
#include "abstractelement.h"
#include "statics/elements/forcevm.h"

class Force : public AbstractElement
{
    Q_OBJECT
    Q_PROPERTY(QVector3D applicationPoint READ applicationPoint WRITE setApplicationPoint NOTIFY applicationPointChanged)
    Q_PROPERTY(QString applicationElement READ applicationElement WRITE setApplicationElement NOTIFY applicationElementChanged)
    Q_PROPERTY(QVector3D vector READ vector WRITE setVector NOTIFY vectorChanged)
public:
    Force(QObject *parent = 0);
    ~Force(){
        last_ids.append(this->ID);
    }

    QVector3D applicationPoint(){return m_applicationPoint;}
    QString applicationElement(){return m_applicationElement;}
    QVector3D vector(){return m_vector;}

    void setApplicationPoint(QVector3D applicationPoint){if(m_applicationPoint!=applicationPoint){m_applicationPoint=applicationPoint; emit applicationPointChanged(m_applicationPoint);}}
    void setApplicationElement(QString applicationElement){if(m_applicationElement!=applicationElement){m_applicationElement=applicationElement;emit applicationElementChanged(m_applicationElement);}}
    void setVector(QVector3D vector){if(m_vector!=vector){m_vector=vector;emit vectorChanged(m_vector);}}

    AbstractElementViewModel* vm(){return (AbstractElementViewModel* )m_vm;}
    void setVm(AbstractElementViewModel* vm);

signals:
    void applicationPointChanged(QVector3D applicationPoint);
    void applicationElementChanged(QString applicationElementChanged);
    void vectorChanged(QVector3D vector);


private:    
    int generateExternalForceID();
    static QList<int> last_ids;    
    QVector3D m_applicationPoint;
    QString m_applicationElement;
    QVector3D m_vector;
    int ID;
    ForceVM* m_vm;

};

#endif // FORCE_H
