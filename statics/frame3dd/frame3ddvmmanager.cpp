#include "frame3ddvmmanager.h"
#include "../viewModels/beamvm.h"
#include "../viewModels/jointvm.h"
#include "../viewModels/trapezoidalforcevm.h"
Frame3DDVMManager::Frame3DDVMManager(QObject* parent):
    AbstractVMManager(parent),
    m_staticsModule(Q_NULLPTR),
    m_player(this)
{
    m_ready=false;
    m_dying=false;
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

Frame3DDVMManager::~Frame3DDVMManager()
{
    m_dying=true;
    Q_FOREACH(QObject* o, m_dependent_objects){
        //delete o;
    }

    Q_FOREACH(PoolEntity e,m_entity_pool.values()){
        delete e.entity;
        //delete e.contex;
    }
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
#ifdef ANDROID
//    /*Create some resources in the pool*/
//    QList<AbstractElementViewModel*> victims;
//    if(m_staticsModule->joints().length()>1)
//        for(int i=0; i<10;i++)
//           victims.append(createJointVM(m_staticsModule->joints().at(0)));
//    if(m_staticsModule->beams().length()>1)
//        for(int i=0; i<10;i++)
//           victims.append(createBeamVM(m_staticsModule->beams().at(0)));
//    Q_FOREACH(AbstractElementViewModel* victim,victims){
//        victim->deleteLater();
//    }
#endif
    m_ready=true;
    emit readyChanged();
}

BeamVM* Frame3DDVMManager::createBeamVM(BeamPtr b){
    if(!b.isNull()){
        BeamVM* beamVm;
        const QString classname=BeamVM::staticMetaObject.className();
        if(m_entity_pool.contains(classname)){
            PoolEntity _e=m_entity_pool.value(classname);
            m_entity_pool.remove(classname,_e);
            beamVm=new BeamVM(b,_e.entity,_e.contex,m_sceneRoot,this);
        }
        else
            beamVm=new BeamVM(b,m_sceneRoot,this);
        Q_FOREACH(auto e,beamVm->getEntities()){
            m_entityID2viewModel[e]=beamVm;
        }
        if(!beamVm->component3D()->objectName().isEmpty())
            m_entityNameMap[beamVm->component3D()->objectName()]=beamVm->component3D()->id();
        connect(beamVm,SIGNAL(destroyed(QObject*)),this,SLOT(onResourceDestroyed(QObject*)));
        connect(beamVm,SIGNAL(resourcesUpdated()),this,SLOT(onResourcesUpdate()));
        registerDependentObject(beamVm);
        return beamVm;
    }
    return Q_NULLPTR;
}

JointVM* Frame3DDVMManager::createJointVM(JointPtr j){
    if(!j.isNull()){
        JointVM* jointVm;
        const QString classname=JointVM::staticMetaObject.className();
        if(m_entity_pool.contains(classname)){
            PoolEntity _e=m_entity_pool.value(classname);
            m_entity_pool.remove(classname,_e);
            jointVm=new JointVM(j,_e.entity,_e.contex,m_sceneRoot,this);
        }
        else
            jointVm=new JointVM(j,m_sceneRoot,this);

        Q_FOREACH(auto e,jointVm->getEntities()){
            m_entityID2viewModel[e]=jointVm;
        }
        if(!jointVm->component3D()->objectName().isEmpty())
            m_entityNameMap[jointVm->component3D()->objectName()]=jointVm->component3D()->id();
        connect(jointVm,SIGNAL(destroyed(QObject*)),this,SLOT(onResourceDestroyed(QObject*)));
        connect(jointVm,SIGNAL(resourcesUpdated()),this,SLOT(onResourcesUpdate()));
        registerDependentObject(jointVm);
        return jointVm;
    }
    return Q_NULLPTR;
}


void Frame3DDVMManager::produceTPZForce(Qt3DCore::QEntity* parentEntity,QVariantHash properties)
{
    if(!m_entityID2viewModel.contains(parentEntity->id())) return;
    AbstractElementViewModel* avm= m_entityID2viewModel[parentEntity->id()];
    BeamVM* beam_vm=qobject_cast<BeamVM*>(avm);
    if(beam_vm==Q_NULLPTR || beam_vm->beam().isNull()) return;
    TrapezoidalForcePtr trpForce=m_staticsModule->createTPZLoad(QVector3D(0,-1,0),beam_vm->beam().toStrongRef(),
                                                                QVector3D(0,0,0),QVector2D(0,1));
    TrapezoidalForceVM* tpzForceVM=new TrapezoidalForceVM(trpForce,m_sceneRoot,parentEntity,properties,this);
    Q_FOREACH(auto e,tpzForceVM->getEntities()){
        m_entityID2viewModel[e]=tpzForceVM;
    }
    if(!tpzForceVM->component3D()->objectName().isEmpty())
        m_entityNameMap[tpzForceVM->objectName()]=tpzForceVM->component3D()->id();
    connect(tpzForceVM,SIGNAL(destroyed(QObject*)),this,SLOT(onResourceDestroyed(QObject*)));
    connect(tpzForceVM,SIGNAL(resourcesUpdated()),this,SLOT(onResourcesUpdate()));
    registerDependentObject(tpzForceVM);

}


void Frame3DDVMManager::registerDependentObject(QObject *o)
{
    m_dependent_objects.insert(o);
}

void Frame3DDVMManager::addPoolEntity(QString className, Qt3DEntityPtr e, QQmlContext *contex)
{
    if(m_dying) return;
    PoolEntity poolEntity;
    poolEntity.entity=e;
    poolEntity.contex=contex;
    m_entity_pool.insert(className,poolEntity);
}

void Frame3DDVMManager::tryRetrivePoolEntity(QString className, Qt3DEntityPtr &e, QQmlContext *&contex)
{
    if(m_entity_pool.contains(className)){
        PoolEntity _e=m_entity_pool.value(className);
        m_entity_pool.remove(className,_e);
        e=_e.entity;
        contex=_e.contex;
    }else{
        e=Q_NULLPTR;
        contex=Q_NULLPTR;
    }
}

Qt3DEntityPtr Frame3DDVMManager::getEntity3D(Qt3DCore::QNodeId id){
    if(m_entityID2viewModel.contains(id))
        return m_entityID2viewModel[id]->component3D();
    else
        return Q_NULLPTR;
}

AbstractElementViewModel* Frame3DDVMManager::getAssociatedVM(Qt3DCore::QNodeId id){
    if(m_entityID2viewModel.contains(id))
        return m_entityID2viewModel[id];
    else
        return Q_NULLPTR;
}

AbstractElementViewModel *Frame3DDVMManager::getAssociatedVM(AbstractElement *element)
{
    if(!element) return Q_NULLPTR;
    Q_FOREACH(AbstractElementViewModel* _vm,m_entityID2viewModel.values()){
        if(_vm->element()==element){
            return _vm;
        }
    }
    return Q_NULLPTR;
}

Qt3DCore::QEntity* Frame3DDVMManager::getEntity3D(QString entity_name)
{
    if(!entity_name.isEmpty() && m_entityNameMap.contains(entity_name))
        return m_entityID2viewModel[m_entityNameMap[entity_name]]->component3D();
    else
        return Q_NULLPTR;
}

QVariantList Frame3DDVMManager::beamEntities()
{
    QVariantList list;
    Q_FOREACH(Qt3DCore::QNode* node,m_sceneRoot->childNodes()){
        Qt3DCore::QEntity* entity=qobject_cast<Qt3DCore::QEntity*>(node);
        if(entity!=Q_NULLPTR && entity->property("type")=="beam"){
            list.append(QVariant::fromValue(entity));
        }
    }
    return list;
}



void Frame3DDVMManager::onResourceDestroyed(QObject * o){
    m_dependent_objects.remove(o);
    Q_FOREACH(auto e, m_entityID2viewModel.keys()){
        if(m_entityID2viewModel[e]==(AbstractElementViewModel*)o){
            m_entityID2viewModel.remove(e);
            Q_FOREACH(auto s, m_entityNameMap.keys())
                if(m_entityNameMap[s]==e)
                    m_entityNameMap.remove(s);
        }
    }
}

void Frame3DDVMManager::onResourcesUpdate(){
    auto previous_map=m_entityID2viewModel;
    AbstractElementViewModel* sender=qobject_cast<AbstractElementViewModel*>(QObject::sender());
    Q_FOREACH(auto e, sender->getEntities()){
        if(previous_map.contains(e)){
            previous_map.remove(e);
        }
        else{
            m_entityID2viewModel[e]=sender;
        }
    }
    if(!sender->component3D()->objectName().isEmpty())
        m_entityNameMap[sender->component3D()->objectName()]=sender->component3D()->id();
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
