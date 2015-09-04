#include "abstractelementviewmodel.h"

AbstractElementViewModel::AbstractElementViewModel(Qt3D::QEntity* sceneRoot,QObject *parent):
    QObject(parent)
{
    m_sceneRoot=sceneRoot;
}

void AbstractElementViewModel::append_3D_resources(Qt3D::QEntity* root){
    m_3Dresources.insert(root);
    for(Qt3D::QEntity* e : root->findChildren<Qt3D::QEntity*>())
        append_3D_resources(e);
}


