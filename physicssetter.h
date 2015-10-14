#ifndef PHYSICSSETTER_H
#define PHYSICSSETTER_H

#include <Qt3DCore>
#include <QtPhysicsUnofficial/frontend/physicsbodyinfo.h>

class PhysicsSetter: public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QEntity* sceneroot READ sceneroot WRITE setSceneroot NOTIFY sceneRootChanged)
    /*if entityName is not provided the material is applied to all the nodes from the sceneroot*/
    Q_PROPERTY(QString entityName READ entityName WRITE setEntityName)
    Q_PROPERTY(Physics::PhysicsBodyInfo* bodyInfo READ bodyInfo WRITE setBodyInfo NOTIFY bodyInfoChanged)

public:
    explicit PhysicsSetter(QObject *parent = 0);

    Qt3D::QEntity* sceneroot(){return m_sceneroot;}
    void setSceneroot(Qt3D::QEntity* sceneroot){
        if(m_sceneroot!=sceneroot){
            m_sceneroot=sceneroot;
        }
    }

    Physics::PhysicsBodyInfo* bodyInfo(){return m_bodyInfo;}
    void setBodyInfo(Physics::PhysicsBodyInfo* bodyInfo){
        if(m_bodyInfo!=bodyInfo){
            m_bodyInfo=bodyInfo;
            emit bodyInfoChanged();
        }
    }

    QString entityName(){return m_entityName;}
    void setEntityName(QString entityName){m_entityName=entityName;}

signals:
    void bodyInfoChanged();
    void sceneRootChanged();
public slots:
    void onAnyChange();

private:
    void recursive_step(Qt3D::QEntity* e);
    Qt3D::QEntity* m_sceneroot;
    Physics::PhysicsBodyInfo* m_bodyInfo;
    QString m_entityName;
};

#endif // PHYSICSSETTER_H
