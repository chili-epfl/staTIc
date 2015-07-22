#include "materialsetter.h"

MaterialSetter::MaterialSetter(QObject *parent) : QObject(parent)
{

}

void MaterialSetter::onAnyChange(){
    if(!m_sceneroot) return;
    if(!m_material) return;
    Qt3D::QEntity* entity=m_sceneroot->findChild<Qt3D::QEntity*>(m_entityName);
    QList<Qt3D::QEntity*> tree_entities=entity->findChildren<Qt3D::QEntity*>();
    tree_entities.append(entity);
    for(Qt3D::QEntity* e:tree_entities){
        bool has_material=false;
        for(Qt3D::QComponent* c: e->components()){
            if(c->inherits("Qt3D::QMaterial")){
                c->setEnabled(false);
                has_material=true;
            }
        }
        if(has_material)
            e->addComponent(m_material);
    }

}

