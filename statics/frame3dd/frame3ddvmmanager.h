#ifndef FRAME3DDVMMANAGER_H
#define FRAME3DDVMMANAGER_H

#include "../abstractvmmanager.h"
#include "frame3ddkernel.h"
#include <QMediaPlayer>
#include <QQmlContext>
#include <QQmlComponent>
struct PoolEntity;
class Frame3DDVMManager: public AbstractVMManager
{
    Q_OBJECT
    Q_PROPERTY(Frame3DDKernel* staticsModule READ staticsModule WRITE setStaticsModule NOTIFY staticsModuleChanged)
public:
    Frame3DDVMManager(QObject* parent=0);
    ~Frame3DDVMManager();
    Frame3DDKernel* staticsModule(){return m_staticsModule;}
    void setStaticsModule(Frame3DDKernel* staticsModule);

    BeamVM* createBeamVM(BeamPtr b);
    JointVM* createJointVM(JointPtr j);

    Qt3DCore::QEntity* getEntity3D(Qt3DCore::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3DCore::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3DCore::QEntity*);
    AbstractElementViewModel*getAssociatedVM(AbstractElement *);
    Q_INVOKABLE Qt3DCore::QEntity *getEntity3D(QString entity_name);
    Q_INVOKABLE QVariantList beamEntities();
    /*Factory methods invoked by the QML side mainly*/
    Q_INVOKABLE void produceTPZForce(Qt3DCore::QEntity* parentEntity, QVariantHash properties=QVariantHash());
    void registerDependentObject(QObject* o);

    void addPoolEntity(QString className,Qt3DCore::QEntity* e, QQmlContext* contex, QQmlComponent* component);
    void tryRetrivePoolEntity(QString className,Qt3DCore::QEntity* &e, QQmlContext* &contex, QQmlComponent* &component);
protected slots:
    void initViewModels();
signals:
    void staticsModuleChanged();
    void scaleFactorsUpdated();
private slots:
    void onResourceDestroyed(QObject*);
    void onResourcesUpdate();
    void updateScaleFactors();
    void generateSoundEffects();
    void playEffect(QMediaPlayer::State state);

private:
    bool m_dying;
    Frame3DDKernel* m_staticsModule;
    QHash<Qt3DCore::QEntity*, AbstractElementViewModel*> m_Entity3D2ViewModel;
    QHash<Qt3DCore::QNodeId, Qt3DCore::QEntity* > m_entityID2Entity3D;
    QHash<QString, Qt3DCore::QEntity* > m_entityNameMap;

    QList<qreal> m_previousStresses;

    QMediaPlayer m_player;
    QList<QUrl> m_effectList;
    QTimer m_lazy_player_timer;

    QMultiHash<QString,PoolEntity> m_entity_pool;
    QSet<QObject*> m_dependent_objects;
};

struct PoolEntity{
    Qt3DCore::QEntity* entity;
    QQmlContext* contex;
    QQmlComponent* component;
    bool operator==(const PoolEntity& a) const
    {
        return (this->component==a.component && this->entity==a.entity && this->contex==a.contex);
    }
};



#endif // FRAME3DDVMMANAGER_H
