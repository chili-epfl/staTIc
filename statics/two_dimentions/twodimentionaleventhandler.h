#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "statics/abstracteventhandler.h"

#include "statics/two_dimentions/twodimensionalstaticsmodule.h"



class TwoDimentionalEventHandler : public AbstractEventHandler
{
    Q_OBJECT

public:
    TwoDimentionalEventHandler(QObject* parent=0);

    QVariant staticsModule(){return qVariantFromValue((void *) m_staticsModule); }
    void setStaticsModule(QVariant staticsModule);

    void setSceneRoot(Qt3D::QEntity* sceneRoot);

    void initViewModels();


public slots:
    void inputEventHandler(EventType type, QVariantMap args);

private:
    void inputEventHandlerOnSelect(EventType type, QVariantMap args);
    void inputEventHandlerOnForce(EventType type, QVariantMap args);
    void inputEventHandlerOnDelete(EventType type, QVariantMap args);

    TwoDimensionalStaticsModule* m_staticsModule;

};

#endif // CONTROLLER_H
