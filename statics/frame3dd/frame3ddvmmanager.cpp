#include "frame3ddvmmanager.h"
#include "../viewModels/beamvm.h"
#include "../viewModels/jointvm.h"
Frame3DDVMManager::Frame3DDVMManager(QObject* parent):
    AbstractVMManager(parent),
    m_staticsModule(Q_NULLPTR)
{
    connect(this,SIGNAL(staticsModuleChanged()),this,SLOT(initViewModels()));
    connect(this,SIGNAL(sceneRootChanged()),this,SLOT(initViewModels()));
}

void Frame3DDVMManager::setStaticsModule(Frame3DDKernel *staticsModule){
    if(m_staticsModule!=staticsModule){
        m_staticsModule=staticsModule;
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
        Q_FOREACH(Qt3D::QEntity* e,beamVm->getEntities()){
            m_Entity3D2ViewModel[e]=beamVm;
            m_entityID2Entity3D[e->id()]=e;
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
        Q_FOREACH(Qt3D::QEntity* e,jointVm->getEntities()){
            m_Entity3D2ViewModel[e]=jointVm;
            m_entityID2Entity3D[e->id()]=e;
        }
        connect(jointVm,SIGNAL(destroyed(QObject*)),this,SLOT(onResourceDestroyed(QObject*)));
        connect(jointVm,SIGNAL(resourcesUpdated()),this,SLOT(onResourcesUpdate()));
        return jointVm;
    }
    return Q_NULLPTR;
}


Qt3D::QEntity* Frame3DDVMManager::getEntity3D(Qt3D::QNodeId id){
    if(m_entityID2Entity3D.contains(id))
        return m_entityID2Entity3D[id];
    else
        return Q_NULLPTR;
}

AbstractElementViewModel* Frame3DDVMManager::getAssociatedVM(Qt3D::QNodeId id){
    return getAssociatedVM(getEntity3D(id));
}

AbstractElementViewModel* Frame3DDVMManager::getAssociatedVM(Qt3D::QEntity* e){
    if(!e) return Q_NULLPTR;
    if(!m_Entity3D2ViewModel.contains(e)) return Q_NULLPTR;
    return m_Entity3D2ViewModel[e];
}

void Frame3DDVMManager::onResourceDestroyed(QObject * o){
    Q_FOREACH(Qt3D::QEntity* e, m_Entity3D2ViewModel.keys()){
        if(m_Entity3D2ViewModel[e]==o){
            m_Entity3D2ViewModel.remove(e);
            m_entityID2Entity3D.remove(e->id());
        }
    }
}

void Frame3DDVMManager::onResourcesUpdate(){
    QHash<Qt3D::QEntity*,AbstractElementViewModel*> previous_map=m_Entity3D2ViewModel;
    AbstractElementViewModel* sender=qobject_cast<AbstractElementViewModel*>(QObject::sender());
    Q_FOREACH(Qt3D::QEntity* e, sender->getEntities()){
        if(previous_map.contains(e)){
            previous_map.remove(e);
        }
        else{
            m_Entity3D2ViewModel[e]=sender;
            m_entityID2Entity3D[e->id()]=e;
        }
    }
    Q_FOREACH(Qt3D::QEntity* e,previous_map.keys()){
        if(m_Entity3D2ViewModel[e]==sender){
            m_Entity3D2ViewModel.remove(e);
            m_entityID2Entity3D.remove(e->id());
        }
    }
}
