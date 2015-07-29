#include "beamvm.h"
#include "statics/elements/beam.h"
#include "staticshelper.h"

BeamVM::BeamVM(QObject* parent):AbstractElementViewModel(parent)
{

}


void BeamVM::onStatusComplete(){

    Beam* beam=qobject_cast<Beam*>(m_element);
    Joint* extreme1=beam->extremes().first;
    Joint* extreme2=beam->extremes().second;

    QVector3D dir=extreme1->position()-extreme2->position();

    /*Compression > 0
    * Tension < 0 */
    if(beam->axialForce()>0)
        dir*=-1;

    QQmlComponent componentArrow(&engine,QUrl("qrc:/ForceArrow.qml"));
    Qt3D::QEntity* forceEntity1= qobject_cast<Qt3D::QEntity*>(componentArrow.create());

    forceEntity1->setProperty("myAngle",atan2(dir.y(),dir.x()));
    forceEntity1->setProperty("position",extreme1->position());
    forceEntity1->setProperty("arrowLength",fabs(beam->axialForce())*15);
    forceEntity1->setProperty("isPointingAtPosition",false);
    forceEntity1->setProperty("visible",false);

    connect(this,SIGNAL(updateForceMagnitude(qreal)),forceEntity1,SIGNAL(changeArrowLength(qreal)));

    connect(this,SIGNAL(updateForceDirectionEx1(qreal)),forceEntity1,SIGNAL(changeMyAngle(qreal)));


    forceEntity1->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                              StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_INTERNAL));
    forceEntity1->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

    Qt3D::QEntity* forceEntity2= qobject_cast<Qt3D::QEntity*>(componentArrow.create());

    forceEntity2->setProperty("myAngle",atan2(-dir.y(),-dir.x()));
    forceEntity2->setProperty("position",extreme2->position());
    forceEntity2->setProperty("arrowLength",fabs(beam->axialForce())*15);
    forceEntity2->setProperty("isPointingAtPosition",false);
    forceEntity2->setProperty("visible",false);

    connect(this,SIGNAL(updateForceMagnitude(qreal)),forceEntity2,SIGNAL(changeArrowLength(qreal)));

    connect(this,SIGNAL(updateForceDirectionEx2(qreal)),forceEntity2,SIGNAL(changeMyAngle(qreal)));

    forceEntity2->setObjectName(StaticsHelper::NameResolution(m_entity_name,
                                                              StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_INTERNAL));
    forceEntity2->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

}

void BeamVM::onElementSelected(){


}

void BeamVM::onElementNameChanged(QString name){

    /*IT's not so easy....*/
    m_entity_name=name;

}

void BeamVM::onElementDestroyed(){
    /*Remove all the graphics associated*/

}

void BeamVM::onElementVmChanged(){
    /*Destroy*/

}

void BeamVM::onBeamExtremesChanged(){
    /*Future Implementation*/
}

void BeamVM::onBeamAxialForceChanged(qreal val){
    Beam* beam=qobject_cast<Beam*>(m_element);
    Joint* extreme1=beam->extremes().first;
    Joint* extreme2=beam->extremes().second;
    QVector3D dir=extreme1->position()-extreme2->position();
    /*Compression > 0
    * Tension < 0 */
    if(val>0)
        dir*=-1;

    emit updateForceDirectionEx1(atan2(dir.y(),dir.x()));
    emit updateForceDirectionEx2(atan2(-dir.y(),-dir.x()));
    emit updateForceMagnitude(fabs(val));
}






