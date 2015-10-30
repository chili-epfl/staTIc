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

    Q_FOREACH(Joint* joint,m_staticsModule->joints()){
        createJointVM(joint);
    }
    Q_FOREACH(Beam* beam,m_staticsModule->beams()){
        createBeamVM(beam);
    }
}

BeamVM* Frame3DDVMManager::createBeamVM(Beam* b){
    if(b){
        BeamVM* beamVm=new BeamVM(b,m_sceneRoot,this);
        Q_FOREACH(Qt3D::QEntity* e,beamVm->getEntities()){
            m_Entity3D2ViewModel[e]=beamVm;
            m_entityID2Entity3D[e->id()]=e;
        }
        return beamVm;
    }
    return Q_NULLPTR;
}

JointVM* Frame3DDVMManager::createJointVM(Joint* j){
    if(j){
        JointVM* jointVm=new JointVM(j,m_sceneRoot,this);
        Q_FOREACH(Qt3D::QEntity* e,jointVm->getEntities()){
            m_Entity3D2ViewModel[e]=jointVm;
            m_entityID2Entity3D[e->id()]=e;
        }
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
