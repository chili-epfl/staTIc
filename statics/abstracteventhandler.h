#ifndef ABSTRACTEVENTHANDLER_H
#define ABSTRACTEVENTHANDLER_H

#include <QObject>
#include <Qt3DCore>
#include "statics/abstractstaticsmodule.h"

class AbstractEventHandler : public QObject
{
    Q_OBJECT
    Q_ENUMS(EventType)
    Q_PROPERTY(Qt3D::QCamera* camera READ camera WRITE setCamera)
public:
    enum EventType{CLICKED,DRAGGED,HELD,HOLDING};

    explicit AbstractEventHandler(QObject *parent = 0);

    Qt3D::QCamera* camera(){return m_camera;}
    void setCamera(Qt3D::QCamera* camera){this->m_camera=camera;}

    void setSceneRoot(Qt3D::QEntity* sceneRoot){this->m_sceneRoot=sceneRoot;}
    void setStaticsModule(AbstractStaticsModule* staticsModule){m_staticsModule=staticsModule;}

signals:

public slots:
    virtual void inputEventHandler(EventType type, QVariantMap args)=0;

protected:
    Qt3D::QEntity* m_sceneRoot;
    Qt3D::QCamera* m_camera;
    AbstractStaticsModule* m_staticsModule;
};

#endif // ABSTRACTEVENTHANDLER_H
