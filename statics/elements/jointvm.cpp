#include "jointvm.h"
#include "staticshelper.h"
#include "statics/elements/beam.h"


/*class BeamSignalProxy: public QObject{
  Q_OBJECT
public:
    BeamSignalProxy(AbstractElement* element ,QObject* parent=0);
    BeamSignalProxy(QObject* parent=0);
public slots:
    void onBeamAxialForceChanged(qreal val);
    void onEntityDestroyed();
signals:
    void updateFBDForceMagnitude(qreal val);
    void updateFBDForceDirection(qreal val);
private:
    AbstractElement* m_element;
};

BeamSignalProxy::BeamSignalProxy(QObject* parent):QObject(parent){
}

BeamSignalProxy::BeamSignalProxy(AbstractElement* element,QObject* parent):QObject(parent){
    m_element=element;
}

void BeamSignalProxy::onBeamAxialForceChanged(qreal val){
    Beam* beam=qobject_cast<Beam*>(QObject::sender());

    if(!beam) return;

    Joint* joint=qobject_cast<Joint*>(m_element);

    Joint* extreme1=beam->extremes().first;
    Joint* extreme2=beam->extremes().second;
    QVector3D dir;
    if(extreme1==joint)
        dir=extreme1->position()-extreme2->position();
    else
        dir=extreme2->position()-extreme1->position();
    if(val<0)
        dir*=-1;

    emit updateFBDForceMagnitude(fabs(val));
    emit updateFBDForceDirection(atan2(dir.y(),dir.x()));

}

void BeamSignalProxy::onEntityDestroyed(){
    this->deleteLater();
}
*/





JointVM::JointVM(QObject* parent): AbstractElementViewModel(parent)
{
    m_isSupport=false;
    m_reactionIsVisible=m_isSupport;
    m_FBDIsVisible=false;
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


void JointVM::onJointConnectedBeamsChanged(){
    Joint* joint=qobject_cast<Joint*>(m_element);
    if(!m_sceneRoot) return;

    /*Check if the 3d entity has been created*/
    QString entityName=StaticsHelper::NameResolution(m_entity_name,StaticsHelper::Roles::MODEL,StaticsHelper::Roles::FBD);

    Qt3D::QEntity* FBDEntity=m_sceneRoot->findChild<Qt3D::QEntity*>(entityName);
    if(!FBDEntity) return;

    QList<Qt3D::QEntity*> prevEntities=FBDEntity->findChildren<Qt3D::QEntity*>(QRegExp("^"+FBDEntity->objectName()),Qt::FindDirectChildrenOnly);

    QVector<bool> toDelete(prevEntities.size(),true);
    QQmlComponent componentArrow(&engine,QUrl("qrc:/ForceArrow.qml"));

    for(Beam* beam: joint->connected_beams){
        bool exists=false;
        int i=0;
        for(Qt3D::QEntity* prevEntity: prevEntities){
            QString entityPlainName=StaticsHelper::NameResolution(prevEntity->objectName(),StaticsHelper::Roles::FBD_ELEMENT,StaticsHelper::MODEL);
            if(entityPlainName.compare(beam->objectName())==0){
                exists=true;
                toDelete[i]=false;
                break;
            }
            i++;
        }
        if(!exists){
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
            forceEntity->setProperty("arrowLength",fabs(beam->axialForce()));
            forceEntity->setProperty("isPointingAtPosition",false);
            forceEntity->setProperty("visible",m_FBDIsVisible);
            forceEntity->setObjectName(FBDEntity->objectName()+
                                       StaticsHelper::NameResolution(beam->objectName(),StaticsHelper::Roles::MODEL,
                                                                     StaticsHelper::Roles::FBD_ELEMENT));
            forceEntity->setParent(FBDEntity);

            connect(this,SIGNAL(FBDisVisibleChanged(bool)),forceEntity,SIGNAL(changeVisible(bool)));

            m_beamsMap[beam]=forceEntity;
            connect(beam,SIGNAL(axialForceChanged(qreal)),this,SLOT(onBeamAxialForceChanged(qreal)));

        }
    }

    int i=0;
    for(Qt3D::QEntity* prevEntity: prevEntities){
        if(toDelete[i]){
            prevEntity->deleteLater();
            AbstractElement* key=m_beamsMap.key(prevEntity);
            m_beamsMap.remove(key);
        }
        i++;
    }

}


void JointVM::onStatusComplete(){

    Joint* joint=qobject_cast<Joint*>(m_element);
    if(joint->supportType()!=Joint::SupportType::NOSUPPORT){
        m_isSupport=true;
        m_reactionIsVisible=m_isSupport;
    }
    /*Create the reaction force entity*/
    Qt3D::QEntity *forceEntity;
    QQmlComponent componentArrow(&engine,QUrl("qrc:/ForceArrow.qml"));
    forceEntity = qobject_cast<Qt3D::QEntity*>(componentArrow.create());
    forceEntity->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                             StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_REACTION));


    forceEntity->setProperty("myAngle",atan2(joint->reaction().y(),joint->reaction().x()));
    forceEntity->setProperty("position",joint->position());
    forceEntity->setProperty("arrowLength",joint->reaction().length());
    forceEntity->setProperty("isPointingAtPosition",false);
    forceEntity->setProperty("visible",m_reactionIsVisible);


    connect(this,SIGNAL(updateReactionDirection(qreal)),forceEntity,SIGNAL(changeMyAngle(qreal)));
    connect(this,SIGNAL(updateReactionMagnitude(qreal)),forceEntity,SIGNAL(changeArrowLength(qreal)));
    connect(this,SIGNAL(reactionIsVisibleChanged(bool)),forceEntity,SIGNAL(changeVisible(bool)));

    forceEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));
    /*Create the FBD*/

    QQmlComponent componentFBD(&engine,QUrl("qrc:/FreeBodyDiagram.qml"));

    Qt3D::QEntity* FBDEntity = qobject_cast<Qt3D::QEntity*>(componentFBD.create());
    FBDEntity->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                           StaticsHelper::Roles::MODEL,StaticsHelper::Roles::FBD));
    FBDEntity->setProperty("position",joint->position());
    FBDEntity->setProperty("visible",m_FBDIsVisible);
    connect(this,SIGNAL(FBDisVisibleChanged(bool)),FBDEntity,SIGNAL(changeVisible(bool)));


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
        forceEntity->setProperty("arrowLength",fabs(beam->axialForce()));
        forceEntity->setProperty("isPointingAtPosition",false);
        forceEntity->setProperty("visible",m_FBDIsVisible);
        forceEntity->setObjectName(FBDEntity->objectName()+
                                   StaticsHelper::NameResolution(beam->objectName(),StaticsHelper::Roles::MODEL,
                                                                 StaticsHelper::Roles::FBD_ELEMENT));
        forceEntity->setParent(FBDEntity);

        connect(this,SIGNAL(FBDisVisibleChanged(bool)),forceEntity,SIGNAL(changeVisible(bool)));

        m_beamsMap[beam]=forceEntity;
        connect(beam,SIGNAL(axialForceChanged(qreal)),this,SLOT(onBeamAxialForceChanged(qreal)));


    }

    FBDEntity->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

}

void JointVM::onBeamAxialForceChanged(qreal val){
    Beam* beam=qobject_cast<Beam*>(QObject::sender());

    if(!beam) return;

    Joint* joint=qobject_cast<Joint*>(m_element);

    Joint* extreme1=beam->extremes().first;
    Joint* extreme2=beam->extremes().second;
    QVector3D dir;
    if(extreme1==joint)
        dir=extreme1->position()-extreme2->position();
    else
        dir=extreme2->position()-extreme1->position();
    if(val<0)
        dir*=-1;

    Qt3D::QEntity* forceEntity=m_beamsMap[beam];

    QMetaObject::invokeMethod(forceEntity, "changeMyAngle",Qt::AutoConnection,
                              Q_ARG(qreal, atan2(dir.y(),dir.x())));
    QMetaObject::invokeMethod(forceEntity, "changeArrowLength",Qt::AutoConnection,
                              Q_ARG(qreal, fabs(val)));


}





