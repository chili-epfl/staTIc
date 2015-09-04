#ifndef ABSTRACTEVENTHANDLER_H
#define ABSTRACTEVENTHANDLER_H

#include <QObject>
#include <Qt3DCore>
#include "statics/abstractstaticsmodule.h"
#include "statics/viewModels/abstractelementviewmodel.h"

class AbstractEventHandler : public QObject
{
    Q_OBJECT
    Q_ENUMS(EventType)    

    Q_PROPERTY(Qt3D::QCamera* camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(Qt3D::QEntity* sceneRoot READ sceneRoot WRITE setSceneRoot NOTIFY sceneRootChanged)
    Q_PROPERTY(QVariant staticsModule READ staticsModule WRITE setStaticsModule NOTIFY staticsModuleChanged)

    Q_PROPERTY(QObject* uiRoot READ uiRoot WRITE setUiRoot NOTIFY uiRootChanged)

public:
    enum EventType{CLICKED,DRAGGED,HELD,HOLDING};

    explicit AbstractEventHandler(QObject *parent = 0);

    Qt3D::QCamera* camera(){return m_camera;}
    void setCamera(Qt3D::QCamera* camera);

    Qt3D::QEntity* sceneRoot(){return m_sceneRoot;}
    virtual void setSceneRoot(Qt3D::QEntity* sceneRoot);

    QObject* uiRoot(){return m_uiRoot;}
    void setUiRoot(QObject* ui);

    virtual QVariant staticsModule()=0;
    virtual void setStaticsModule(QVariant staticsModule)=0;

    virtual void initViewModels()=0;

    QHash<Qt3D::QEntity* , AbstractElementViewModel* > m_Entity3D2ViewModel;

signals:
    void uiRootChanged();
    void sceneRootChanged();
    void staticsModuleChanged();
    void cameraChanged();

public slots:
    virtual void inputEventHandler(EventType type, QVariantMap args)=0;
protected:
    QObject* m_uiRoot;

    Qt3D::QEntity* m_sceneRoot;
    Qt3D::QCamera* m_camera;

    QSet<AbstractElementViewModel*> m_viewmodels;


};

#endif // ABSTRACTEVENTHANDLER_H
