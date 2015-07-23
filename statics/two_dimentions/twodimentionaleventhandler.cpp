#include "statics/two_dimentions/twodimentionaleventhandler.h"

QMatrix4x4 getTranformationMatrix(Qt3D::QEntity* entity){
    if(!entity) return  QMatrix4x4();
    Qt3D::QTransform* entity_transform=NULL;
    for(Qt3D::QComponent* component: entity->components()){
        if(component->isEnabled()){
            if(component->inherits("Qt3D::QTransform")){
                entity_transform=qobject_cast<Qt3D::QTransform*>(component);
            }
        }
    }
    if(entity_transform!=NULL){
        return getTranformationMatrix(entity->parentEntity())*entity_transform->matrix();
    }
    else return getTranformationMatrix(entity->parentEntity());
}

TwoDimentionalEventHandler::TwoDimentionalEventHandler(QObject* parent):
    AbstractEventHandler(parent)
{

}


void TwoDimentionalEventHandler::inputEventHandler(EventType type, QVariantMap args){

    if(type==CLICKED){
        if(args.size()!=2) return;
    }
    else if(type==HELD){
        if(args.size()!=3) return;
    }
    else if(type==DRAGGED){
        if(args.size()!=4) return;
        QVector2D p0=args["Point0"].value<QVector2D>();
        QVector2D p1=args["Point1"].value<QVector2D>();
        Qt3D::QEntity* entity0=args["Entity0"].value<Qt3D::QEntity*>();
        Qt3D::QEntity* entity1=args["Entity1"].value<Qt3D::QEntity*>();
        if(entity0==NULL && entity1==NULL) return;

        if(entity0==NULL) {
            entity0=entity1; ;
        }

        QMatrix4x4 matrix=getTranformationMatrix(m_sceneRoot->findChild<Qt3D::QEntity*>("Model"));

         Joint* element=dynamic_cast<Joint*>(m_staticsModule->getElement(entity0->objectName()));

        if(!element) return;
        QVector2D diff2D=p0-p1;
        QVector3D diff3D(diff2D,0);
        diff3D=matrix.inverted().mapVector(diff3D);

        QVariantList args;
        args.append("");
        args.append(QVector3D(element->getPosition()));
        args.append(QVector3D(diff3D.x(),diff3D.y(),0).normalized());
        args.append(element->objectName());

        m_staticsModule->createElement(AbstractElement::FORCE,args);


    }

}



