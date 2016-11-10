#include "abstractelementviewmodel.h"
#include <QQmlContext>
AbstractElementViewModel::AbstractElementViewModel(Qt3DEntityPtr sceneRoot,QObject *parent):
    QObject(parent)
{
    m_sceneRoot=sceneRoot;
}

void AbstractElementViewModel::append_3D_resources(Qt3DCore::QEntity* root,bool recursive){

    m_3DEntities.insert(root->id());
    if(recursive){
        for(Qt3DCore::QEntity* e : root->findChildren<Qt3DCore::QEntity*>())
            append_3D_resources(e,recursive);
    }
    emit resourcesUpdated();

}

void AbstractElementViewModel::remove_3D_resources(Qt3DCore::QEntity* root,bool recursive){
    m_3DEntities.remove(root->id());
    if(recursive){
        for(Qt3DEntityPtr e : root->findChildren<Qt3DCore::QEntity*>())
            remove_3D_resources(e,recursive);
    }
    emit resourcesUpdated();
}



QList<Qt3DCore::QNodeId> AbstractElementViewModel::getEntities(){
    QList<Qt3DCore::QNodeId> list;
    Q_FOREACH(Qt3DCore::QNodeId e, m_3DEntities){
        list.append(e);
    }
    return list;

}

