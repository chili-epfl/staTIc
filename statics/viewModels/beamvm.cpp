#include "beamvm.h"
#include "statics/elements/beam.h"
#include "staticshelper.h"

BeamVM::BeamVM(Beam* beam,Qt3D::QEntity* sceneRoot,QObject* parent):AbstractElementViewModel(sceneRoot,parent)
{
    m_beam=beam;
    m_visible=false;

    //Default bindings
    connect(m_beam,SIGNAL(destroyed(QObject*)),this,SLOT(onElementDestroyed()));

    //Element Specific bindings
    connect(m_beam,SIGNAL(extremesChanged()),this,SLOT(onBeamExtremesChanged()));
    connect(m_beam,SIGNAL(axialForceChanged(qreal)),this,SLOT(onBeamAxialForceChanged(qreal)));

    initView();

}

void BeamVM::initView(){
    Joint* extreme1=m_beam->extremes().first;
    Joint* extreme2=m_beam->extremes().second;

    QVector3D dir=extreme1->position()-extreme2->position();

    /*Compression > 0
        * Tension < 0 */
    if(m_beam->axialForce()>0)
        dir*=-1;

    QQmlComponent componentArrow(&engine,QUrl("qrc:/ForceArrow.qml"));
    Qt3D::QEntity* forceEntity1= qobject_cast<Qt3D::QEntity*>(componentArrow.create());

    forceEntity1->setProperty("myAngle",atan2(dir.y(),dir.x()));
    forceEntity1->setProperty("position",extreme1->position());
    forceEntity1->setProperty("arrowLength",fabs(m_beam->axialForce()));
    forceEntity1->setProperty("isPointingAtPosition",false);
    forceEntity1->setProperty("visible",m_visible);
    forceEntity1->setProperty("type","Internal");

    connect(this,SIGNAL(updateForceMagnitude(qreal)),forceEntity1,SIGNAL(changeArrowLength(qreal)));
    connect(this,SIGNAL(updateForceDirectionEx1(qreal)),forceEntity1,SIGNAL(changeMyAngle(qreal)));
    connect(this,SIGNAL(visibilityChanged(bool)),forceEntity1,SIGNAL(changeVisible(bool)));

    append_3D_resources(forceEntity1);

    //TODO:Better name for model
    forceEntity1->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

    Qt3D::QEntity* forceEntity2= qobject_cast<Qt3D::QEntity*>(componentArrow.create());

    forceEntity2->setProperty("myAngle",atan2(-dir.y(),-dir.x()));
    forceEntity2->setProperty("position",extreme2->position());
    forceEntity2->setProperty("arrowLength",fabs(m_beam->axialForce()));
    forceEntity2->setProperty("isPointingAtPosition",false);
    forceEntity2->setProperty("visible",m_visible);
    forceEntity2->setProperty("type","Internal");

    connect(this,SIGNAL(updateForceMagnitude(qreal)),forceEntity2,SIGNAL(changeArrowLength(qreal)));

    connect(this,SIGNAL(updateForceDirectionEx2(qreal)),forceEntity2,SIGNAL(changeMyAngle(qreal)));

    connect(this,SIGNAL(visibilityChanged(bool)),forceEntity2,SIGNAL(changeVisible(bool)));

    //forceEntity2->setObjectName(StaticsHelper::NameResolution(m_entity_name,
    //                                                          StaticsHelper::Roles::MODEL,StaticsHelper::Roles::F_INTERNAL));

    append_3D_resources(forceEntity2);

    //TODO:Better name for model
    forceEntity2->setParent(m_sceneRoot->findChild<Qt3D::QNode*>("Model"));

}


void BeamVM::onElementDestroyed(){
    /*Remove all the graphics associated*/

}

void BeamVM::onBeamExtremesChanged(){
    /*Future Implementation*/
}

void BeamVM::onBeamAxialForceChanged(qreal val){
    Joint* extreme1=m_beam->extremes().first;
    Joint* extreme2=m_beam->extremes().second;
    QVector3D dir=extreme1->position()-extreme2->position();
    /*Compression > 0
    * Tension < 0 */
    if(val>0)
        dir*=-1;

    emit updateForceDirectionEx1(atan2(dir.y(),dir.x()));
    emit updateForceDirectionEx2(atan2(-dir.y(),-dir.x()));
    emit updateForceMagnitude(fabs(val));
}





