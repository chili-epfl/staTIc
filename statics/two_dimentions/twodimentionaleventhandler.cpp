#include "statics/two_dimentions/twodimentionaleventhandler.h"

#include "staticshelper.h"
#include "statics/viewModels/beamvm.h"
#include "statics/viewModels/forcevm.h"
#include "statics/viewModels/jointvm.h"


TwoDimentionalEventHandler::TwoDimentionalEventHandler(QObject* parent):
    AbstractEventHandler(parent)
{
    m_staticsModule=Q_NULLPTR;
    connect(this,SIGNAL(uiRootChanged()),this,SLOT(initViewModels()));
    connect(this,SIGNAL(sceneRootChanged()),this,SLOT(initViewModels()));
    connect(this,SIGNAL(staticsModule2DChanged()),this,SLOT(initViewModels()));

}

void TwoDimentionalEventHandler::initViewModels(){
    if(m_staticsModule==Q_NULLPTR || m_sceneRoot==Q_NULLPTR || m_uiRoot==Q_NULLPTR) return;
    for (Joint* joint:m_staticsModule->joints){
        JointVM* jointVM=createJointVM(joint);
        Qt3D::QEntity* entity= m_sceneRoot->findChild<Qt3D::QEntity*>(joint->objectName());
        if(entity){
            jointVM->append_3D_resources(entity,AbstractElementViewModel::Roles::PRIMARY);
        }
    }
    for(Beam* beam: m_staticsModule->beams){
        BeamVM* beamVM=createBeamVM(beam);
        Qt3D::QEntity* entity= m_sceneRoot->findChild<Qt3D::QEntity*>(beam->objectName());
        if(entity){
            beamVM->append_3D_resources(entity,AbstractElementViewModel::Roles::PRIMARY);
        }
    }
    /*Probably not necessary, since there are no forces at the very beginning*/
    for(Force* force: m_staticsModule->forces){
    }

}

void TwoDimentionalEventHandler::setStaticsModule2D(TwoDimensionalStaticsModule* staticsModule ){
    if(staticsModule){
        m_staticsModule=staticsModule;
        emit staticsModule2DChanged();
    }
    else{
        qWarning("SetStaticsModule: Invalid argument");
    }
}

ForceVM* TwoDimentionalEventHandler::createForceVM(Force* f){
    ForceVM* forceVM= new ForceVM(f,m_uiRoot,m_sceneRoot,this);
    connect(forceVM,SIGNAL(resourceAdded(Qt3D::QEntity*)),this,SLOT(onAddedResource(Qt3D::QEntity*)));
    return forceVM;
}
BeamVM* TwoDimentionalEventHandler::createBeamVM(Beam* b){
    BeamVM* beamVM=new BeamVM(b,m_uiRoot,m_sceneRoot,this);
    connect(beamVM,SIGNAL(resourceAdded(Qt3D::QEntity*)),this,SLOT(onAddedResource(Qt3D::QEntity*)));
    return beamVM;
}
JointVM* TwoDimentionalEventHandler::createJointVM(Joint* j){
    JointVM* jointVM=new JointVM(j,m_uiRoot,m_sceneRoot,this);
    connect(jointVM,SIGNAL(resourceAdded(Qt3D::QEntity*)),this,SLOT(onAddedResource(Qt3D::QEntity*)));
    return jointVM;
}

void TwoDimentionalEventHandler::onAddedResource(Qt3D::QEntity* e){
    //qDebug()<<e->id();
    m_Entity3D2ViewModel[e]=(AbstractElementViewModel*)QObject::sender();
    m_entityID2Entity3D[e->id()]=e;
}


Qt3D::QEntity* TwoDimentionalEventHandler::getEntity3D(Qt3D::QNodeId id){
    if(m_entityID2Entity3D.contains(id))
        return m_entityID2Entity3D[id];
    else
        return Q_NULLPTR;
}

AbstractElementViewModel* TwoDimentionalEventHandler::getAssociatedVM(Qt3D::QNodeId id){
    return getAssociatedVM(getEntity3D(id));
}

AbstractElementViewModel* TwoDimentionalEventHandler::getAssociatedVM(Qt3D::QEntity* e){
    if(!e) return Q_NULLPTR;
    if(!m_Entity3D2ViewModel.contains(e)) return Q_NULLPTR;
    return m_Entity3D2ViewModel[e];
}




