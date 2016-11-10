#ifndef ABSTRACTELEMENTVIEWMODEL_H
#define ABSTRACTELEMENTVIEWMODEL_H

#include <QObject>
#include <Qt3DCore>
#include <QPointer>

class AbstractElement;

typedef QPointer<Qt3DCore::QEntity> Qt3DEntityPtr;


class AbstractElementViewModel : public QObject
{
    Q_OBJECT
public:
    explicit AbstractElementViewModel(Qt3DEntityPtr sceneRoot,QObject *parent = 0);
    virtual QList<Qt3DCore::QNodeId> getEntities();
    virtual AbstractElement* element()=0;
    virtual Qt3DEntityPtr component3D()=0;
signals:
    void resourcesUpdated();
protected:    
    virtual void append_3D_resources(Qt3DCore::QEntity* root,bool recursive=true);
    virtual void remove_3D_resources(Qt3DCore::QEntity* root,bool recursive=true);

    Qt3DEntityPtr m_sceneRoot;
    QSet<Qt3DCore::QNodeId> m_3DEntities;


};

#endif // ABSTRACTELEMENTVIEWMODEL_H
