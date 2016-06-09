#include "frame3ddvmmanager.h"
#include "../viewModels/beamvm.h"
#include "../viewModels/jointvm.h"
#include "../viewModels/trapezoidalforcevm.h"
Frame3DDVMManager::Frame3DDVMManager(QObject* parent):
    AbstractVMManager(parent),
    m_staticsModule(Q_NULLPTR),
    m_player(this)
{
    m_effectList.append(QUrl("qrc:/soundeffects/AR/SoundEffects/creak_low_1.ogg"));
    m_effectList.append(QUrl("qrc:/soundeffects/AR/SoundEffects/creak_low_2.ogg"));
    m_effectList.append(QUrl("qrc:/soundeffects/AR/SoundEffects/creak_low_3.ogg"));
    m_effectList.append(QUrl("qrc:/soundeffects/AR/SoundEffects/creak_low_4.ogg"));
    m_effectList.append(QUrl("qrc:/soundeffects/AR/SoundEffects/crash_wood_1.ogg"));
    m_effectList.append(QUrl("qrc:/soundeffects/AR/SoundEffects/crash_wood_2.ogg"));
    //connect(&m_player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(playEffect(QMediaPlayer::State)));
    connect(this,SIGNAL(staticsModuleChanged()),this,SLOT(initViewModels()));
    connect(this,SIGNAL(sceneRootChanged()),this,SLOT(initViewModels()));
    m_lazy_player_timer.setInterval(500);
    m_lazy_player_timer.setSingleShot(true);
    connect(&m_lazy_player_timer,SIGNAL(timeout()),this,SLOT(generateSoundEffects()));
    m_player.setVolume(50);
}

void Frame3DDVMManager::setStaticsModule(Frame3DDKernel *staticsModule){
    if(m_staticsModule!=staticsModule){
        m_staticsModule=staticsModule;
        connect(m_staticsModule,SIGNAL(updated()),&m_lazy_player_timer,SLOT(start()));
        emit staticsModuleChanged();
    }
}

void Frame3DDVMManager::initViewModels(){
    if(m_sceneRoot==Q_NULLPTR || m_staticsModule==Q_NULLPTR) return;

    Q_FOREACH(JointPtr joint,m_staticsModule->joints()){
        createJointVM(joint);
    }
    Q_FOREACH(BeamPtr beam,m_staticsModule->beams()){
        createBeamVM(beam);
    }
}

BeamVM* Frame3DDVMManager::createBeamVM(BeamPtr b){
    if(!b.isNull()){
        BeamVM* beamVm=new BeamVM(b,m_sceneRoot,this);
        Q_FOREACH(Qt3DCore::QEntity* e,beamVm->getEntities()){
            m_Entity3D2ViewModel[e]=beamVm;
            m_entityID2Entity3D[e->id()]=e;
            m_entityNameMap[e->objectName()]=e;
        }
        connect(beamVm,SIGNAL(destroyed(QObject*)),this,SLOT(onResourceDestroyed(QObject*)));
        connect(beamVm,SIGNAL(resourcesUpdated()),this,SLOT(onResourcesUpdate()));
        return beamVm;
    }
    return Q_NULLPTR;
}

JointVM* Frame3DDVMManager::createJointVM(JointPtr j){
    if(!j.isNull()){
        JointVM* jointVm=new JointVM(j,m_sceneRoot,this);
        Q_FOREACH(Qt3DCore::QEntity* e,jointVm->getEntities()){
            m_Entity3D2ViewModel[e]=jointVm;
            m_entityID2Entity3D[e->id()]=e;
            m_entityNameMap[e->objectName()]=e;
        }
        connect(jointVm,SIGNAL(destroyed(QObject*)),this,SLOT(onResourceDestroyed(QObject*)));
        connect(jointVm,SIGNAL(resourcesUpdated()),this,SLOT(onResourcesUpdate()));
        return jointVm;
    }
    return Q_NULLPTR;
}


void Frame3DDVMManager::produceTPZForce(Qt3DCore::QEntity *parentEntity,QVariantHash properties)
{
    if(parentEntity==Q_NULLPTR) return;
    if(!m_Entity3D2ViewModel.contains(parentEntity)) return;
    AbstractElementViewModel* avm= m_Entity3D2ViewModel[parentEntity];
    BeamVM* beam_vm=qobject_cast<BeamVM*>(avm);
    if(beam_vm==Q_NULLPTR || beam_vm->beam().isNull()) return;
    TrapezoidalForcePtr trpForce=m_staticsModule->createTPZLoad(QVector3D(0,-1,0),beam_vm->beam().toStrongRef(),
                                                                QVector3D(0,0,0),QVector2D(0,1));
    TrapezoidalForceVM* tpzForceVM=new TrapezoidalForceVM(trpForce,m_sceneRoot,parentEntity,properties,this);
    Q_FOREACH(Qt3DCore::QEntity* e,tpzForceVM->getEntities()){
        m_Entity3D2ViewModel[e]=tpzForceVM;
        m_entityID2Entity3D[e->id()]=e;
    }
    connect(tpzForceVM,SIGNAL(destroyed(QObject*)),this,SLOT(onResourceDestroyed(QObject*)));
    connect(tpzForceVM,SIGNAL(resourcesUpdated()),this,SLOT(onResourcesUpdate()));
}

Qt3DCore::QEntity* Frame3DDVMManager::getEntity3D(Qt3DCore::QNodeId id){
    if(m_entityID2Entity3D.contains(id))
        return m_entityID2Entity3D[id];
    else
        return Q_NULLPTR;
}

AbstractElementViewModel* Frame3DDVMManager::getAssociatedVM(Qt3DCore::QNodeId id){
    return getAssociatedVM(getEntity3D(id));
}

AbstractElementViewModel* Frame3DDVMManager::getAssociatedVM(Qt3DCore::QEntity* e){
    if(!e) return Q_NULLPTR;
    if(!m_Entity3D2ViewModel.contains(e)) return Q_NULLPTR;
    return m_Entity3D2ViewModel[e];
}

Qt3DCore::QEntity *Frame3DDVMManager::getEntity3D(QString entity_name)
{
    if(!entity_name.isEmpty() && m_entityNameMap.contains(entity_name))
        return m_entityNameMap[entity_name];
    else
        return Q_NULLPTR;
}



void Frame3DDVMManager::onResourceDestroyed(QObject * o){
    Q_FOREACH(Qt3DCore::QEntity* e, m_Entity3D2ViewModel.keys()){
        if(m_Entity3D2ViewModel[e]==o){
            m_Entity3D2ViewModel.remove(e);
            m_entityID2Entity3D.remove(e->id());
            if(!e->objectName().isEmpty())
                m_entityNameMap.remove(e->objectName());
        }
    }
}

void Frame3DDVMManager::onResourcesUpdate(){
    QHash<Qt3DCore::QEntity*,AbstractElementViewModel*> previous_map=m_Entity3D2ViewModel;
    AbstractElementViewModel* sender=qobject_cast<AbstractElementViewModel*>(QObject::sender());
    Q_FOREACH(Qt3DCore::QEntity* e, sender->getEntities()){
        if(previous_map.contains(e)){
            previous_map.remove(e);
        }
        else{
            m_Entity3D2ViewModel[e]=sender;
            m_entityID2Entity3D[e->id()]=e;
        }
    }
    Q_FOREACH(Qt3DCore::QEntity* e,previous_map.keys()){
        if(m_Entity3D2ViewModel[e]==sender){
            m_Entity3D2ViewModel.remove(e);
            m_entityID2Entity3D.remove(e->id());
        }
    }
}

void Frame3DDVMManager::updateScaleFactors(){
//    QList<qreal> currentStress,combinedStresses;
//    Q_FOREACH(BeamVM* b, m_beamvms){
//        currentStress.append(fabs(b->stress()));
//    }
//    Q_FOREACH(JointVM* j, m_joints){
//        currentStress.append(fabs(j->reaction));
//    }
//    combinedStresses=m_previous_stresses;
//    combinedStresses.append(currentStress);
//    qSort(combinedStresses);
//    Q_FOREACH(BeamVM* b, m_beamvms){
//        qreal factor=combinedStresses.indexOf(b->stress())/combinedStresses.size();
//        b->setScaleFactor(factor);
//    }
//    Q_FOREACH(JointVM* j, m_joints){
//        qreal factor=combinedStresses.indexOf(b->stress())/combinedStresses.size();
//        j-setScaleFactor(factor);
//    }
    //    m_previous_stresses=currentStress;
}

void Frame3DDVMManager::generateSoundEffects()
{
    if(m_staticsModule){
        qreal max=0;
        Q_FOREACH(BeamPtr beam,m_staticsModule->beams()){
            qreal shear,axial;
            beam->stressRatio(axial,shear,3);
            max=qMax(max,shear);
            max=qMax(max,axial);
        }
        if(max>1){
            qreal r=qrand()/RAND_MAX;
            int index=qRound(r+4);
            m_player.setMedia(m_effectList.at(index));
            m_player.play();

        }
        else if(max>0.5){
            qreal r=qrand()/RAND_MAX;
            int index=qRound(r*3);
            m_player.setMedia(m_effectList.at(index));
            m_player.play();
        }

    }
}

void Frame3DDVMManager::playEffect(QMediaPlayer::State state)
{
    if(state==QMediaPlayer::LoadedMedia){
        m_player.play();
    }
}
