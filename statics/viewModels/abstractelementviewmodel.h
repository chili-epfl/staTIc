#ifndef ABSTRACTELEMENTVIEWMODEL_H
#define ABSTRACTELEMENTVIEWMODEL_H

#include <QObject>
#include <Qt3DCore>

class AbstractElement;

class AbstractElementViewModel : public QObject
{
    Q_OBJECT
public:
    explicit AbstractElementViewModel(Qt3D::QEntity* sceneRoot,QObject *parent = 0);
    virtual QList<Qt3D::QEntity*> getEntities();
signals:
    void resourcesUpdated();
protected:    
    virtual void append_3D_resources(Qt3D::QEntity* root,bool recursive=true);
    virtual void remove_3D_resources(Qt3D::QEntity* root,bool recursive=true);

    Qt3D::QEntity* m_sceneRoot;
    QSet<Qt3D::QEntity*> m_3DEntities;


};

#endif // ABSTRACTELEMENTVIEWMODEL_H
