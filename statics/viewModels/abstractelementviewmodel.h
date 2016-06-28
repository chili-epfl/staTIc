#ifndef ABSTRACTELEMENTVIEWMODEL_H
#define ABSTRACTELEMENTVIEWMODEL_H

#include <QObject>
#include <Qt3DCore>

class AbstractElement;

class AbstractElementViewModel : public QObject
{
    Q_OBJECT
public:
    explicit AbstractElementViewModel(Qt3DCore::QEntity* sceneRoot,QObject *parent = 0);
    virtual QList<Qt3DCore::QEntity*> getEntities();
    virtual AbstractElement* element()=0;
    virtual Qt3DCore::QEntity* component3D()=0;
signals:
    void resourcesUpdated();
protected:    
    virtual void append_3D_resources(Qt3DCore::QEntity* root,bool recursive=true);
    virtual void remove_3D_resources(Qt3DCore::QEntity* root,bool recursive=true);

    Qt3DCore::QEntity* m_sceneRoot;
    QSet<Qt3DCore::QEntity*> m_3DEntities;


};

#endif // ABSTRACTELEMENTVIEWMODEL_H
