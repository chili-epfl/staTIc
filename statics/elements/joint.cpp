#include "joint.h"
#include "beam.h"
#include "statics/abstractstaticsmodule.h"
#include <QQmlContext>
QQmlComponent* Joint::m_qqmlcomponent=NULL;

Joint::Joint(QVector3D position,QString name,QObject* parent):
    AbstractElement(name,parent),
    m_pos(position),
    m_reaction(0,0,0),
    m_reaction_momentum(0,0,0),
    m_displacement(0,0,0),
    m_displacement_rot(0,0,0),
    m_support_X(0),
    m_support_Y(0),
    m_support_Z(0),
    m_support_XX(0),
    m_support_YY(0),
    m_support_ZZ(0),
    m_component3D()
{

}

void Joint::addConnectedBeam(BeamPtr b){
    m_connected_beams.append(b.toWeakRef());
    connect(b.data(),SIGNAL(destroyed(QObject*)),this,SLOT(onBeamDestroyed()));
    connect(b.data(),SIGNAL(enabledChanged(bool)),this,SLOT(onBeamDestroyed()));
    emit connectedBeamsChanged();
}

void Joint::createQmlEntity(QVariantMap aesthetics)
{
    if(!m_sceneRoot.isNull()){
        if(m_qqmlcomponent==NULL){
            m_qqmlcomponent=new QQmlComponent(qmlEngine(m_sceneRoot),m_sceneRoot);
            connect(m_qqmlcomponent,&QQmlComponent::destroyed,[]() {
                Joint::m_qqmlcomponent=NULL;
            });
            m_qqmlcomponent->loadUrl(QUrl("qrc:/element_views/Element_Views/JointView.qml"));
        }
        QQmlContext* m_qqmlcontext=new QQmlContext(qmlContext(m_sceneRoot),m_sceneRoot);
        Qt3DCore::QEntity* jointView= qobject_cast<Qt3DCore::QEntity*>(m_qqmlcomponent->beginCreate(m_qqmlcontext));

        m_qqmlcontext->setContextObject(jointView);
        m_component3D=jointView;
        m_component3D->setProperty("backend_entity",QVariant::fromValue(this));
        m_qqmlcomponent->completeCreate();
        m_component3D->setParent(m_sceneRoot);
        connect(this,SIGNAL(destroyed(QObject*)),m_component3D.data(),SLOT(deleteLater()));

    }else{
        qDebug()<<"Sceneroot is null";
    }
}

QString Joint::supportType()
{
    QString supportType="special";

    AbstractStaticsModule* static_module=static_cast<AbstractStaticsModule*>(parent());
    if(static_module->is2D()){
        if(m_support_X && m_support_Y && m_support_Z && m_support_XX && m_support_YY && !m_support_ZZ)
            supportType="Pinned";
        else if(!m_support_X && m_support_Y && m_support_Z && m_support_XX && m_support_YY && !m_support_ZZ)
            supportType="Rolling";
        else if(!m_support_X && !m_support_Y && m_support_Z && m_support_XX && m_support_YY && !m_support_ZZ)
            supportType="none";
    }
    else
        if(m_support_X && m_support_Y && m_support_Z && !m_support_XX && !m_support_YY && !m_support_ZZ)
            supportType="Pinned";
        else if(!m_support_X && m_support_Y && !m_support_Z && !m_support_XX && !m_support_YY && !m_support_ZZ)
            supportType="Rolling";
        else if(!m_support_X && !m_support_Y && !m_support_Z && !m_support_XX && !m_support_YY && !m_support_ZZ)
            supportType="none";
    if (m_support_X && m_support_Y && m_support_Z && m_support_XX && m_support_YY && m_support_ZZ)
        supportType="Fixed";

    return supportType;

}

void Joint::setSupportType(QString type)
{
    AbstractStaticsModule* static_module=static_cast<AbstractStaticsModule*>(parent());
    if(static_module->is2D()){
        if(type=="none")
            setSupport(0,0,1,1,1,0);
        else if(type=="Pinned")
            setSupport(1,1,1,1,1,0);
        else if(type=="Rolling")
            setSupport(0,1,1,1,1,0);
    }else{
        if(type=="none")
            setSupport(0,0,0,0,0,0);
        else if(type=="Pinned")
            setSupport(0,1,0,0,0,0);
        else if(type=="Rolling")
            setSupport(1,1,1,0,0,0);
    }
    if(type=="Fixed")
        setSupport(1,1,1,1,1,1);
}


void Joint::onBeamDestroyed(){
    QList<WeakBeamPtr> newlist;
    Q_FOREACH(WeakBeamPtr b, m_connected_beams){
        if(!b.isNull()){
            newlist.append(b);
        }
    }
    m_connected_beams=newlist;
    emit connectedBeamsChanged();
}



void Joint::setReaction(QVector3D reaction){
    if(m_reaction!=reaction){
        m_reaction=reaction;
        emit reactionChanged();
    }
}

void Joint::setReactionMomentum(QVector3D momentum){
    if(m_reaction_momentum!=momentum){
        m_reaction_momentum=momentum;
        emit reactionChanged();
    }
}


void Joint::support(bool  &support_X,bool &support_Y,bool &support_Z,
                    bool &support_XX,bool &support_YY,bool &support_ZZ){

    support_X=m_support_X;
    support_Y=m_support_Y;
    support_Z=m_support_Z;
    support_XX=m_support_XX;
    support_YY=m_support_YY;
    support_ZZ=m_support_ZZ;

}




void Joint::setDisplacement(QVector3D displacement){
    if(displacement!=m_displacement){
        m_displacement=displacement;
        emit displacementChanged();
    }

}
void Joint::setDisplacementRot(QVector3D displacement){
    if(displacement!=m_displacement_rot){
        m_displacement_rot=displacement;
        emit displacementRotChanged();
    }
}

QList<WeakBeamPtr> Joint::connectedBeams()
{
    QList<WeakBeamPtr> _list;
    _list.reserve(m_connected_beams.size());
    Q_FOREACH(WeakBeamPtr b,m_connected_beams){
        if(b.toStrongRef()->enabled()){
            _list.append(b);
        }
    }
    return _list;

    //return m_connected_beams;
}

QVariantList Joint::connectedBeamsAsVariant()
{
    QVariantList _list;
    _list.reserve(m_connected_beams.size());
    Q_FOREACH(WeakBeamPtr b,m_connected_beams){
        if(b.toStrongRef()->enabled()){
            _list.append(QVariant::fromValue(b.data()));
        }
    }
    return _list;

}



void Joint::setSupport(bool  support_X,bool support_Y,bool support_Z,
                       bool support_XX,bool support_YY,bool support_ZZ){
    bool updated=false;
    if(support_X!=m_support_X){
        m_support_X=support_X;
        updated=true;
    }
    if(support_Y!=m_support_Y){
        m_support_Y=support_Y;
        updated=true;
    }
    if(support_Z!=m_support_Z){
        m_support_Z=support_Z;
        updated=true;
    }
    if(support_XX!=m_support_XX){
        m_support_XX=support_XX;
        updated=true;
    }
    if(support_YY!=m_support_YY){
        m_support_YY=support_YY;
        updated=true;
    }
    if(support_ZZ!=m_support_ZZ){
        m_support_ZZ=support_ZZ;
        updated=true;
    }
    if(updated){
        emit supportChanged();
    }



}
