#ifndef ABSTRACTVMMANAGER_H
#define ABSTRACTVMMANAGER_H

#include <QObject>
#include <Qt3DCore>
#include "abstractstaticsmodule.h"

class BeamVM;
class JointVM;
class AbstractElementViewModel;

class AbstractVMManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QEntity* sceneRoot READ sceneRoot WRITE setSceneRoot NOTIFY sceneRootChanged)

public:
    explicit AbstractVMManager(QObject *parent = 0);

    Qt3D::QEntity* sceneRoot(){return m_sceneRoot;}
    virtual void setSceneRoot(Qt3D::QEntity* sceneRoot);

    virtual BeamVM* createBeamVM(BeamPtr b)=0;
    virtual JointVM* createJointVM(JointPtr j)=0;

    virtual AbstractStaticsModule* staticsModule()=0;

    virtual Qt3D::QEntity* getEntity3D(Qt3D::QNodeId id)=0;
    virtual AbstractElementViewModel* getAssociatedVM(Qt3D::QNodeId id)=0;
    virtual AbstractElementViewModel* getAssociatedVM(Qt3D::QEntity*)=0;

protected slots:
    virtual void initViewModels()=0;

signals:
    void sceneRootChanged();

protected:
    Qt3D::QEntity* m_sceneRoot;


};

#endif // ABSTRACTVMMANAGER_H
