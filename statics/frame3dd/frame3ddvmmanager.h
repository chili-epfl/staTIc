#ifndef FRAME3DDVMMANAGER_H
#define FRAME3DDVMMANAGER_H

#include "../abstractvmmanager.h"
#include "frame3ddkernel.h"
class Frame3DDVMManager: public AbstractVMManager
{
    Q_OBJECT
    Q_PROPERTY(Frame3DDKernel* staticsModule READ staticsModule WRITE setStaticsModule NOTIFY staticsModuleChanged)
public:
    Frame3DDVMManager(QObject* parent=0);

    Frame3DDKernel* staticsModule(){return m_staticsModule;}
    void setStaticsModule(Frame3DDKernel* staticsModule);

    BeamVM* createBeamVM(Beam* b);
    JointVM* createJointVM(Joint* j);

    Qt3D::QEntity* getEntity3D(Qt3D::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3D::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3D::QEntity*);

protected slots:
    void initViewModels();

signals:
    void staticsModuleChanged();
private:
    Frame3DDKernel* m_staticsModule;
    QHash<Qt3D::QEntity*, AbstractElementViewModel*> m_Entity3D2ViewModel;
    QHash<Qt3D::QNodeId, Qt3D::QEntity* > m_entityID2Entity3D;

};

#endif // FRAME3DDVMMANAGER_H
