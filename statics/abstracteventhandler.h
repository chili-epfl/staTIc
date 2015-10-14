#ifndef ABSTRACTEVENTHANDLER_H
#define ABSTRACTEVENTHANDLER_H

#include <QObject>
#include <Qt3DCore>
#include "statics/abstractstaticsmodule.h"
#include "statics/viewModels/abstractelementviewmodel.h"

class Force;
class Beam;
class Joint;

class ForceVM;
class BeamVM;
class JointVM;

class AbstractEventHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QCamera* camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(Qt3D::QEntity* sceneRoot READ sceneRoot WRITE setSceneRoot NOTIFY sceneRootChanged)
    Q_PROPERTY(QObject* uiRoot READ uiRoot WRITE setUiRoot NOTIFY uiRootChanged)


public:
    explicit AbstractEventHandler(QObject *parent = 0);

    Qt3D::QCamera* camera(){return m_camera;}
    void setCamera(Qt3D::QCamera* camera);

    Qt3D::QEntity* sceneRoot(){return m_sceneRoot;}
    virtual void setSceneRoot(Qt3D::QEntity* sceneRoot);

    QObject* uiRoot(){return m_uiRoot;}
    void setUiRoot(QObject* ui);

    virtual AbstractStaticsModule* staticsModule()=0;

    virtual void initViewModels()=0;

    virtual ForceVM* createForceVM(Force* f)=0;
    virtual BeamVM* createBeamVM(Beam* b)=0;
    virtual JointVM* createJointVM(Joint* j)=0;

    virtual Qt3D::QEntity* getEntity3D(Qt3D::QNodeId id)=0;
    virtual AbstractElementViewModel* getAssociatedVM(Qt3D::QNodeId id)=0;
    virtual AbstractElementViewModel* getAssociatedVM(Qt3D::QEntity*)=0;


signals:
    void uiRootChanged();
    void sceneRootChanged();
    void cameraChanged();


protected:
    QObject* m_uiRoot;
    Qt3D::QEntity* m_sceneRoot;
    Qt3D::QCamera* m_camera;


};

#endif // ABSTRACTEVENTHANDLER_H
