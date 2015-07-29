#include "jointvm.h"
#include "staticshelper.h"
#include "statics/elements/beam.h"

JointVM::JointVM(QObject* parent): AbstractElementViewModel(parent)
{

}

void JointVM::onElementNameChanged(QString val){}
void JointVM::onElementDestroyed(){}
void JointVM::onElementVmChanged(){}

void JointVM::onJointReactionChanged(QVector3D val){
    emit updateReactionMagnitude(val.length());
    emit updateReactionDirection(atan2(val.y(),val.x()));
}


void JointVM::onJointPositionChanged(QVector3D val){}
void JointVM::onJointSupportTypeChanged(){}
void JointVM::onJointConnectedBeamsChanged(){}

/*

*/
/*void JointVM::onElementChanged(){

    if(!m_element) return;
    if(m_entity_name.isEmpty()) return;
    if(!m_sceneRoot) return;


    Joint* joint=qobject_cast<Joint*>(m_element);

    if(joint->getReaction().length()>=EPSILON){

        Qt3D::QEntity *forceEntity =m_sceneRoot->findChild<Qt3D::QEntity*>(StaticsHelper::NameResolution(m_entity_name,
                                                                                                         StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_REACTION));
        if(!forceEntity){
            /*Create the entity*/
           /* QQmlComponent component(&engine,QUrl("qrc:/ForceArrow.qml"));
            forceEntity = qobject_cast<Qt3D::QEntity*>(component.create());
            forceEntity->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                                     StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_REACTION));
            forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

        }

        //qDebug()<<joint->getReaction();

        forceEntity->setProperty("myAngle",atan2(joint->getReaction().y(),joint->getReaction().x()));
        forceEntity->setProperty("positionX",joint->getPosition().x());
        forceEntity->setProperty("positionY",joint->getPosition().y());
        forceEntity->setProperty("positionZ",joint->getPosition().z());
        forceEntity->setProperty("arrowLength",joint->getReaction().length()*15);
        forceEntity->setProperty("isPointingAtPosition",false);
        forceEntity->setProperty("visible",true);

    }
     else{
        Qt3D::QEntity *forceEntity =m_sceneRoot->findChild<Qt3D::QEntity*>(StaticsHelper::NameResolution(m_entity_name,
                                                                                                         StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_REACTION));
        if(forceEntity){
            forceEntity->setProperty("visible",false);
        }
    }
}*/

void JointVM::onStatusComplete(){

    Joint* joint=qobject_cast<Joint*>(m_element);


    Qt3D::QEntity *forceEntity =m_sceneRoot->findChild<Qt3D::QEntity*>(StaticsHelper::NameResolution(m_entity_name,
                                                                                                     StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_REACTION));
    if(!forceEntity){
        /*Create the entity*/
        QQmlComponent component(&engine,QUrl("qrc:/ForceArrow.qml"));
        forceEntity = qobject_cast<Qt3D::QEntity*>(component.create());
        forceEntity->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                                 StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_REACTION));
        forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

    }

    //qDebug()<<joint->getReaction();

    forceEntity->setProperty("myAngle",atan2(joint->reaction().y(),joint->reaction().x()));
    forceEntity->setProperty("position",joint->position());
    forceEntity->setProperty("arrowLength",joint->reaction().length()*15);
    forceEntity->setProperty("isPointingAtPosition",false);

    connect(this,SIGNAL(updateReactionDirection(qreal)),forceEntity,SIGNAL(changeMyAngle(qreal)));
    connect(this,SIGNAL(updateReactionMagnitude(qreal)),forceEntity,SIGNAL(changeArrowLength(qreal)));


    //if(joint->getReaction().length()>=EPSILON)
        forceEntity->setProperty("visible",true);
    //else
    //    forceEntity->setProperty("visible",false);


}





    /*Joint* joint=qobject_cast<Joint*>(m_element);

    QQmlComponent componentFBD(&engine,QUrl("qrc:/FreeBodyDiagram.qml"));
    QQmlComponent componentArrow(&engine,QUrl("qrc:/ForceArrow.qml"));

    Qt3D::QEntity* FBDEntity = qobject_cast<Qt3D::QEntity*>(componentFBD.create());
    FBDEntity->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                           StaticsHelper::Roles::MODEL,StaticsHelper::Roles::FBD));

    FBDEntity->setProperty("position",joint->position());

    for(Beam* beam: joint->connected_beams){
        Joint* extreme1=beam->extremes().first;
        Joint* extreme2=beam->extremes().second;
        QVector3D dir;
        if(extreme1==joint)
            dir=extreme1->position()-extreme2->position();
        else
            dir=extreme2->position()-extreme1->position();
        Qt3D::QEntity* forceEntity= qobject_cast<Qt3D::QEntity*>(componentArrow.create());
        if(beam->axialForce()<0)
            dir*=-1;
        forceEntity->setProperty("myAngle",atan2(dir.y(),dir.x()));
        forceEntity->setProperty("arrowLength",fabs(beam->axialForce())*15);
        forceEntity->setProperty("isPointingAtPosition",false);
        forceEntity->setProperty("visible",false);
        forceEntity->setObjectName(FBDEntity->objectName());
        forceEntity->setParent(FBDEntity);


    }

    FBDEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

*/



void JointVM::onElementSelected(){
/*
    Joint* joint=qobject_cast<Joint*>(m_element);

    QQmlComponent componentFBD(&engine,QUrl("qrc:/FreeBodyDiagram.qml"));
    QQmlComponent componentArrow(&engine,QUrl("qrc:/ForceArrow.qml"));

    Qt3D::QEntity* FBDEntity = qobject_cast<Qt3D::QEntity*>(componentFBD.create());
    FBDEntity->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                           StaticsHelper::Roles::MODEL,StaticsHelper::Roles::FBD));

    FBDEntity->setProperty("position",joint->position());

    for(Beam* beam: joint->connected_beams){
        Joint* extreme1=beam->extremes().first;
        Joint* extreme2=beam->extremes().second;
        QVector3D dir;
        if(extreme1==joint)
            dir=extreme1->position()-extreme2->position();
        else
            dir=extreme2->position()-extreme1->position();
        Qt3D::QEntity* forceEntity= qobject_cast<Qt3D::QEntity*>(componentArrow.create());
        if(beam->axialForce()<0)
            dir*=-1;
        forceEntity->setProperty("myAngle",atan2(dir.y(),dir.x()));
        forceEntity->setProperty("arrowLength",fabs(beam->axialForce())*15);
        forceEntity->setProperty("isPointingAtPosition",false);
        forceEntity->setProperty("visible",true);
        forceEntity->setObjectName(FBDEntity->objectName());
        forceEntity->setParent(FBDEntity);


    }

    FBDEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));
*/

}
