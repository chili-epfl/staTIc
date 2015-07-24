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

/*Tool definition
*   args[CurrentTool] -> QString tool name
*   >CLICKED
*   args[Point] -> QVector2D mouse position normalized
*   args[Entity] -> QEntity*
*   >DRAGGED
*   args[Point0] -> QVector2D mouse position 0 normalized
*   args[Entity0] -> QEntity* 0
*   args[Point1] -> QVector2D mouse position 1 normalized
*   args[Entity1] -> QEntity*  1
*   >HELD
*   args[Point] -> QVector2D mouse position normalized
*   args[Entity] -> QEntity*
*   args[Timespan] -> timespan
*/


void TwoDimentionalEventHandler::inputEventHandlerOnSelect(EventType type, QVariantMap args){


}
void TwoDimentionalEventHandler::inputEventHandlerOnForce(EventType type, QVariantMap args){

    if(type==CLICKED){
        if(args.size()!=3) return;
    }
    else if(type==HELD){
        if(args.size()!=4) return;
    }
    else if(type==DRAGGED){
        if(args.size()!=5) return;

        bool hasSwapped=false;
        QVector2D p0=args["Point0"].value<QVector2D>();
        QVector2D p1=args["Point1"].value<QVector2D>();
        Qt3D::QEntity* entity0=args["Entity0"].value<Qt3D::QEntity*>();
        Qt3D::QEntity* entity1=args["Entity1"].value<Qt3D::QEntity*>();
        if(entity0==NULL && entity1==NULL) return;

        if(entity0==NULL) {
            entity0=entity1;
            hasSwapped=true;
        }

        AbstractElement* entity0_element=m_staticsModule->getElement(entity0->objectName());

        /*CASE: Entity 0 is a joint*/
        if(entity0_element && entity0_element->inherits("Joint")){

            QMatrix4x4 matrix=getTranformationMatrix(m_sceneRoot->findChild<Qt3D::QEntity*>("Model"));

            Joint* element=dynamic_cast<Joint*>(entity0_element);

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
        /*CASE: Entity 0 is a force*/
        else if(!hasSwapped && entity0_element->inherits("Force")){

            Force* element=dynamic_cast<Force*>(entity0_element);
            QMatrix4x4 matrix=getTranformationMatrix(m_sceneRoot->findChild<Qt3D::QEntity*>("Model"));
            QVector2D diff2D=p0-p1;
            QVector3D diff3D(diff2D,0);
            diff3D=matrix.inverted().mapVector(diff3D);
            element->setVector(QVector3D(diff3D.x(),diff3D.y(),0).normalized());


        }

    }



}
void TwoDimentionalEventHandler::inputEventHandlerOnDelete(EventType type, QVariantMap args){

    if(type==CLICKED){
        if(args.size()!=3) return;
    }
    else if(type==HELD){
        if(args.size()!=4) return;
        Qt3D::QEntity* entity=args["Entity"].value<Qt3D::QEntity*>();
        m_staticsModule->removeElement(entity->objectName());
    }
    else if(type==DRAGGED){

    }


}


void TwoDimentionalEventHandler::inputEventHandler(EventType type, QVariantMap args){

    QString currentTool=args["CurrentTool"].toString();

    if(currentTool.compare("SELECT",Qt::CaseInsensitive)==0){
        inputEventHandlerOnSelect(type,args);
    }
    else if(currentTool.compare("DELETE",Qt::CaseInsensitive)==0){
        inputEventHandlerOnDelete(type,args);
    }
    else if(currentTool.compare("FORCE",Qt::CaseInsensitive)==0){
        inputEventHandlerOnForce(type,args);
    }
}



