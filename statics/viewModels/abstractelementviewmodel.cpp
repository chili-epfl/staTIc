#include "abstractelementviewmodel.h"
#include <QQmlContext>
AbstractElementViewModel::AbstractElementViewModel(Qt3D::QEntity* sceneRoot,QObject *parent):
    QObject(parent)
{
    m_sceneRoot=sceneRoot;
}

void AbstractElementViewModel::append_3D_resources(Qt3D::QEntity* root,bool recursive){
    m_3DEntities.insert(root);
    emit resourceAdded(root);
    if(recursive){
        for(Qt3D::QEntity* e : root->findChildren<Qt3D::QEntity*>())
            append_3D_resources(e,recursive);
    }
}


QList<Qt3D::QEntity*> AbstractElementViewModel::getEntities(){
    QList<Qt3D::QEntity*> list;
    Q_FOREACH(Qt3D::QEntity* e, m_3DEntities){
        list.append(e);
    }
    return list;
}

