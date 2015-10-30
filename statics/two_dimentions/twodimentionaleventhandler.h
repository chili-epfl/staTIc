#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "statics/abstractvmmanager.h"

#include "statics/two_dimentions/twodimensionalstaticsmodule.h"



class TwoDimentionalEventHandler : public AbstractEventHandler
{
    Q_OBJECT
    Q_PROPERTY(TwoDimensionalStaticsModule* staticsModule2D READ staticsModule2D WRITE setStaticsModule2D NOTIFY staticsModule2DChanged)
public:
    TwoDimentionalEventHandler(QObject* parent=0);

    TwoDimensionalStaticsModule* staticsModule2D(){return m_staticsModule; }
    void setStaticsModule2D(TwoDimensionalStaticsModule* staticsModule);

    AbstractStaticsModule* staticsModule(){return m_staticsModule;};


    ForceVM* createForceVM(Force* f);
    BeamVM* createBeamVM(Beam* b);
    JointVM* createJointVM(Joint* j);

    Qt3D::QEntity* getEntity3D(Qt3D::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3D::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3D::QEntity*);

signals:
    void staticsModule2DChanged();

public slots:
    //void inputEventHandler(EventType type, QVariantMap args);
    void onAddedResource(Qt3D::QEntity* e);

private slots:
    void initViewModels();

private:

    /*void inputEventHandlerOnSelect(EventType type, QVariantMap args);
    void inputEventHandlerOnForce(EventType type, QVariantMap args);
    void inputEventHandlerOnDelete(EventType type, QVariantMap args);*/

    TwoDimensionalStaticsModule* m_staticsModule;

    QHash<Qt3D::QEntity*, AbstractElementViewModel*> m_Entity3D2ViewModel;
    QHash<Qt3D::QNodeId, Qt3D::QEntity* > m_entityID2Entity3D;


};

#endif // CONTROLLER_H
