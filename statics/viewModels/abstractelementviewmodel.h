#ifndef ABSTRACTELEMENTVIEWMODEL_H
#define ABSTRACTELEMENTVIEWMODEL_H

#include <QObject>
#include <Qt3DCore>
#include <QQmlEngine>
#include <QQmlComponent>

class AbstractElement;

class AbstractElementViewModel : public QObject
{
    Q_OBJECT
public:
    enum Roles{PRIMARY, DECORATOR};

    explicit AbstractElementViewModel(QObject* uiRoot,Qt3D::QEntity* sceneRoot,QObject *parent = 0);

    virtual void append_3D_resources(Qt3D::QEntity* root,Roles role=DECORATOR,bool recursive=true);
    virtual QList<Qt3D::QEntity*> getEntitiesByRole(Roles role);
    virtual Roles getEntitiyRole(Qt3D::QEntity* e);

signals:
    void resourceAdded(Qt3D::QEntity* e);
protected:    

    QQmlEngine engine;
    Qt3D::QEntity* m_sceneRoot;
    QObject* m_uiRoot;

    QHash<Qt3D::QEntity*, Roles> m_3DEntitiesRoles;


};

#endif // ABSTRACTELEMENTVIEWMODEL_H
