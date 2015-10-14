#include "abstractelementviewmodel.h"

AbstractElementViewModel::AbstractElementViewModel(QObject* uiRoot,Qt3D::QEntity* sceneRoot,QObject *parent):
    QObject(parent)
{
    m_sceneRoot=sceneRoot;
    m_uiRoot=uiRoot;
}

void AbstractElementViewModel::append_3D_resources(Qt3D::QEntity* root,Roles role,bool recursive){
    m_3DEntitiesRoles[root]=role;
    emit resourceAdded(root);
    if(recursive){
        for(Qt3D::QEntity* e : root->findChildren<Qt3D::QEntity*>())
            append_3D_resources(e,role,recursive);
    }
}


QList<Qt3D::QEntity*> AbstractElementViewModel::getEntitiesByRole(Roles role){
    QList<Qt3D::QEntity*> list;
    Q_FOREACH(Qt3D::QEntity* e, m_3DEntitiesRoles.keys()){
        if(m_3DEntitiesRoles[e]==role){
            list.append(e);
        }
    }
    return list;
}
AbstractElementViewModel::Roles AbstractElementViewModel::getEntitiyRole(Qt3D::QEntity* e){
    if(!m_3DEntitiesRoles.contains(e)) qCritical("VM is not linked to the entity");
    return m_3DEntitiesRoles[e];
}
