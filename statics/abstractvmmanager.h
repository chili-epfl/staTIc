#ifndef ABSTRACTVMMANAGER_H
#define ABSTRACTVMMANAGER_H

#include <QObject>
#include "abstractstaticsmodule.h"
#include "statics/viewModels/abstractelementviewmodel.h"

class BeamVM;
class JointVM;
class TrapezoidalForceVM;
class AbstractElementViewModel;

class AbstractVMManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QEntity* sceneRoot READ sceneRoot WRITE setSceneRoot NOTIFY sceneRootChanged)

public:
    explicit AbstractVMManager(QObject *parent = 0);

    Qt3DCore::QEntity* sceneRoot(){return m_sceneRoot;}
    virtual void setSceneRoot(Qt3DCore::QEntity* sceneRoot);

    virtual BeamVM* createBeamVM(BeamPtr b)=0;
    virtual JointVM* createJointVM(JointPtr j)=0;

    virtual AbstractStaticsModule* staticsModule()=0;

    virtual Qt3DEntityPtr getEntity3D(Qt3DCore::QNodeId id)=0;
    virtual AbstractElementViewModel* getAssociatedVM(Qt3DCore::QNodeId id)=0;
    virtual AbstractElementViewModel *getAssociatedVM(AbstractElement *)=0;
    virtual void registerDependentObject(QObject* o)=0;
    Q_INVOKABLE virtual  Qt3DCore::QEntity* getEntity3D(QString entity_name)=0;

protected slots:
    virtual void initViewModels()=0;

signals:
    void sceneRootChanged();

protected:
    Qt3DCore::QEntity* m_sceneRoot;


};

#endif // ABSTRACTVMMANAGER_H
