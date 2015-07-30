#include "statics/two_dimentions/twodimentionaleventhandler.h"
#include "staticshelper.h"


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


qreal mapScreenDistanceToNewtons(qreal val){
    if(val>2) val=2;
    return val;
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
    if(type==CLICKED){
        if(args.size()!=3) return;
        QVector2D point=args["Point"].value<QVector2D>();
        Qt3D::QEntity* entity=args["Entity"].value<Qt3D::QEntity*>();
        if(!entity) return;

        AbstractElement* entity_element=m_staticsModule->getElement(StaticsHelper::NameResolution(entity->objectName(),
                                                                                                  StaticsHelper::Roles::ENTITY3D,StaticsHelper::Roles::MODEL));
        if(!entity_element) return;

        if(entity_element->inherits("Joint")){
            JointVM* tmp=m_staticsModule->findChild<JointVM*>(StaticsHelper::NameResolution(entity->objectName(),
                                                                                            StaticsHelper::Roles::ENTITY3D,StaticsHelper::Roles::VIEWMODEL));
            tmp->select();
        }
        else if (entity_element->inherits("Beam")){

            BeamVM* tmp=m_staticsModule->findChild<BeamVM*>(StaticsHelper::NameResolution(entity->objectName(),
                                                                                          StaticsHelper::Roles::ENTITY3D,StaticsHelper::Roles::VIEWMODEL));
            tmp->select();

        }

    }

}
void TwoDimentionalEventHandler::inputEventHandlerOnForce(EventType type, QVariantMap args){

    static Force* currentHoldingFocus=0;

    if(type==CLICKED){
        currentHoldingFocus=0;
        if(args.size()!=3) return;
    }
    else if(type==HELD){
        currentHoldingFocus=0;
        if(args.size()!=4) return;
    }
    else if(type==DRAGGED){
        currentHoldingFocus=0;

    }
    else if (type==HOLDING){
        if(args.size()!=6) return;

        QVector2D p0=args["Point0"].value<QVector2D>();
        QVector2D p1=args["Point1"].value<QVector2D>();
        Qt3D::QEntity* entity0=args["Entity0"].value<Qt3D::QEntity*>();
        Qt3D::QEntity* entity1=args["Entity1"].value<Qt3D::QEntity*>();

        if(entity0==NULL){
            if(!entity1){
                QMatrix4x4 matrix=getTranformationMatrix(m_sceneRoot->findChild<Qt3D::QEntity*>("Model"));
                QVector4D ray_clip(p0.x(),p0.y(),-1,1);
                QVector4D ray_eye = m_camera->projectionMatrix().inverted().map(ray_clip);
                ray_eye.setZ(-1);ray_eye.setW(0);
                QVector4D ray_wor_4D = m_camera->matrix().inverted().map(ray_eye);
                QVector3D ray_wor(ray_wor_4D.x(),ray_wor_4D.y(),ray_wor_4D.z());
                ray_wor.normalize();
                Qt3D::QRay3D ray;
                ray.setOrigin(m_camera->position());
                ray.setDirection(ray_wor);

                QVector2D diff2D=p1-p0;

                QVector3D diff3D(diff2D,0);
                diff3D=matrix.inverted().mapVector(diff3D);

                QVector3D p03D=ray.origin()-ray.direction()*matrix.operator ()(2,3);

                p03D=matrix.inverted().map(p03D);

                QVariantList args;
                args.append("");
                args.append(p03D);
                args.append(QVector3D(diff3D.x(),diff3D.y(),0).normalized()*mapScreenDistanceToNewtons(diff2D.length()));
                args.append(QString());
                args.append(false);

                if(!currentHoldingFocus){
                    currentHoldingFocus=dynamic_cast<Force*>(m_staticsModule->createElement(AbstractElement::FORCE,args));
                }
                else{
                    currentHoldingFocus->setVector(QVector3D(diff3D.x(),diff3D.y(),0).normalized()*mapScreenDistanceToNewtons(diff2D.length()));
                }
            }
            else{
               if(currentHoldingFocus){

                   AbstractElement* entity1_element=m_staticsModule->getElement(StaticsHelper::NameResolution(entity1->objectName(),
                                                                                                              StaticsHelper::Roles::ENTITY3D,StaticsHelper::Roles::MODEL));

                   /*CASE: Entity 0 is a joint*/
                   if(entity1_element && entity1_element->inherits("Joint")){

                       QMatrix4x4 matrix=getTranformationMatrix(m_sceneRoot->findChild<Qt3D::QEntity*>("Model"));

                       Joint* element=dynamic_cast<Joint*>(entity1_element);

                       QVector2D diff2D=p1-p0;
                       QVector3D diff3D(diff2D,0);
                       diff3D=matrix.inverted().mapVector(diff3D);

                       if(currentHoldingFocus->applicationElement().isEmpty()){
                            currentHoldingFocus->setApplicationPoint(QVector3D(element->position()));
                            currentHoldingFocus->setApplicationElement(element->objectName());
                       }
                       currentHoldingFocus->setVector(QVector3D(diff3D.x(),diff3D.y(),0).normalized()*mapScreenDistanceToNewtons(diff2D.length()));
                       ForceVM* vm=m_staticsModule->findChild<ForceVM*>(StaticsHelper::NameResolution(currentHoldingFocus->objectName(),
                                                                                                      StaticsHelper::Roles::MODEL,StaticsHelper::Roles::VIEWMODEL));
                       vm->setTipOnApplicationPoint(true);

                   }

               }


            }

        }

        else {
            if(!currentHoldingFocus){
                AbstractElement* entity0_element=m_staticsModule->getElement(StaticsHelper::NameResolution(entity0->objectName(),
                                                                                                           StaticsHelper::Roles::ENTITY3D,StaticsHelper::Roles::MODEL));

                /*CASE: Entity 0 is a joint*/
                if(entity0_element && entity0_element->inherits("Joint")){

                    QMatrix4x4 matrix=getTranformationMatrix(m_sceneRoot->findChild<Qt3D::QEntity*>("Model"));

                    Joint* element=dynamic_cast<Joint*>(entity0_element);

                    QVector2D diff2D=p1-p0;
                    QVector3D diff3D(diff2D,0);
                    diff3D=matrix.inverted().mapVector(diff3D);

                    QVariantList args;
                    args.append("");
                    args.append(QVector3D(element->position()));
                    args.append(QVector3D(diff3D.x(),diff3D.y(),0).normalized()*mapScreenDistanceToNewtons(diff2D.length()));
                    args.append(element->objectName());
                    args.append(false);

                    currentHoldingFocus=dynamic_cast<Force*>(m_staticsModule->createElement(AbstractElement::FORCE,args));

                }
                /*CASE: Entity 0 is a force*/
                else if( entity0_element->inherits("Force")){
                    Force* element=dynamic_cast<Force*>(entity0_element);
                    QMatrix4x4 matrix=getTranformationMatrix(m_sceneRoot->findChild<Qt3D::QEntity*>("Model"));
                    QVector2D diff2D=p1-p0;
                    QVector3D diff3D(diff2D,0);
                    diff3D=matrix.inverted().mapVector(diff3D);
                    element->setVector(QVector3D(diff3D.x(),diff3D.y(),0).normalized()*mapScreenDistanceToNewtons(diff2D.length()));
                    currentHoldingFocus=element;
                }
            }
            else{
                QMatrix4x4 matrix=getTranformationMatrix(m_sceneRoot->findChild<Qt3D::QEntity*>("Model"));
                QVector2D diff2D=p1-p0;
                QVector3D diff3D(diff2D,0);
                diff3D=matrix.inverted().mapVector(diff3D);
                currentHoldingFocus->setVector(QVector3D(diff3D.x(),diff3D.y(),0).normalized()*mapScreenDistanceToNewtons(diff2D.length()));
            }
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
        m_staticsModule->removeElement(StaticsHelper::NameResolution(entity->objectName(),
                                                                     StaticsHelper::Roles::ENTITY3D,StaticsHelper::Roles::MODEL));
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



