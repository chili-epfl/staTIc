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

    BeamVM* createBeamVM(BeamPtr b);
    JointVM* createJointVM(JointPtr j);

    Qt3DCore::QEntity* getEntity3D(Qt3DCore::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3DCore::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3DCore::QEntity*);

    /*Factory methods invoked by the QML side mainly*/
    Q_INVOKABLE void produceTPZForce(Qt3DCore::QEntity* parentEntity);


protected slots:
    void initViewModels();
signals:
    void staticsModuleChanged();
    void scaleFactorsUpdated();
private slots:
    void onResourceDestroyed(QObject*);
    void onResourcesUpdate();
    void updateScaleFactors();
private:

    Frame3DDKernel* m_staticsModule;
    QHash<Qt3DCore::QEntity*, AbstractElementViewModel*> m_Entity3D2ViewModel;
    QHash<Qt3DCore::QNodeId, Qt3DCore::QEntity* > m_entityID2Entity3D;

    QList<qreal> m_previousStresses;
};

#endif // FRAME3DDVMMANAGER_H
