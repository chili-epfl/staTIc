#include "controller.h"
#include <ChilitagsObject.h>
#include <QQmlEngine>

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

Controller::Controller(QObject* parent):
    QObject(parent)
{

}

void Controller::loadStructure( QString staticsFile){
    /*TODO: Check the nature of the shape 2d-3d*/
    staticsModule=new TwoDimensionalStaticsModule(staticsFile);
}


void Controller::loadStructure( QUrl staticsFile){
        loadStructure(staticsFile.toLocalFile());
}

void Controller::inputEventHandler(QString type, QVariantMap args){
    /*TODO: CONVERT TYPE INTO ENUM*/
    if(type.compare("CLICKED")==0){
        if(args.size()!=2) return;
    }
    else if(type.compare("HELD")==0){
        if(args.size()!=3) return;
    }
    else if(type.compare("DRAGGED")==0){
        if(args.size()!=4) return;
        QVector2D p0=args["Point0"].value<QVector2D>();
        QVector2D p1=args["Point1"].value<QVector2D>();
        Qt3D::QEntity* entity0=args["Entity0"].value<Qt3D::QEntity*>();
        Qt3D::QEntity* entity1=args["Entity1"].value<Qt3D::QEntity*>();

        if(entity0==NULL && entity1==NULL) return;
        /*Debatable*/
        if(entity0==NULL) {entity0=entity1; QVector2D temp=p0;p0=p1;p1=temp;};

        QMatrix4x4 matrix=getTranformationMatrix(entity0);
        Joint* element=dynamic_cast<Joint*>(staticsModule->getElementbyName(entity0->objectName()));
        if(!element) return;
        QVector3D element_position=element->getPosition();
        element_position=matrix.map(element_position);
        QVector3D p1_onCamera(p1);
        QVector3D diff=element_position-p1_onCamera;
        diff=matrix.inverted().map(diff);
        diff.normalize();

        Force* force=new Force();
        force->applicationPoint=element_position;
        force->applicationElement=element->objectName();
        force->vector=QVector3D(diff.x(),diff.y(),0);
        force->vector.normalize();//magnitude 1
        staticsModule->addExternalForce(force);

        QQmlEngine engine;
        QQmlComponent component(&engine,QUrl("qrc:/ArrowComponent.qml"));
        Qt3D::QEntity *forceEntity = qobject_cast<Qt3D::QEntity*>(component.create());
        forceEntity->setParent(sceneroot->findChild<Qt3D::QNode*>("Model"));
        forceEntity->setProperty("myAngle",atan2(force->vector.y(),force->vector.x()));
        QVector2D diff2D=p0-p1;
        forceEntity->setProperty("arrowLength",diff2D.length()*15);
        qDebug()<<diff2D.length()*15;

        /*Qt3D::QEntity* forceEntity=new Qt3D::QEntity(sceneroot->findChild<Qt3D::QNode*>("Model"));
        forceEntity->setObjectName(force->objectName());
        Qt3D::QMesh* mesh=new Qt3D::QMesh(forceEntity);
        mesh->setSource(QUrl("qrc:/icons/icons/arrow.obj"));
        Qt3D::QTransform* transform= new Qt3D::QTransform(forceEntity);
        Qt3D::QRotateTransform* rotation=new Qt3D::QRotateTransform(transform);
        rotation->setAxis(QVector3D(0,1,0));
        //rotation->setAngleRad(atan2(force->vector.y(),force->vector.x()));
        transform->addTransform(rotation);
        forceEntity->addComponent(mesh);
        forceEntity->addComponent(transform);

        Qt3D::QEntity* cylinder=new Qt3D::QEntity(forceEntity);
        Qt3D::QCylinderMesh* cyl_mesh=new Qt3D::QCylinderMesh(cylinder);
        Qt3D::QTransform* transform_cyl= new Qt3D::QTransform(cylinder);
        Qt3D::QRotateTransform* rotation_cyl=new Qt3D::QRotateTransform(transform_cyl);
        rotation_cyl->setAxis(QVector3D(0,1,0));
        rotation_cyl->setAngleDeg(90);
        transform_cyl->addTransform(rotation_cyl);
        cyl_mesh->setRadius(10);
        cyl_mesh->setLength(20);
        cylinder->addComponent(cyl_mesh);
        cylinder->addComponent(transform_cyl);*/

    }

}



