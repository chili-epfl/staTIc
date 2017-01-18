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
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
public:
    Frame3DDVMManager(QObject* parent=0);
    ~Frame3DDVMManager();
    Frame3DDKernel* staticsModule(){return m_staticsModule;}
    void setStaticsModule(Frame3DDKernel* staticsModule);

    BeamVM* createBeamVM(BeamPtr b);
    JointVM* createJointVM(JointPtr j);

    Qt3DEntityPtr getEntity3D(Qt3DCore::QNodeId id);
    AbstractElementViewModel* getAssociatedVM(Qt3DCore::QNodeId id);
    AbstractElementViewModel*getAssociatedVM(AbstractElement*);
    Q_INVOKABLE Qt3DCore::QEntity* getEntity3D(QString entity_name);

    Q_INVOKABLE QVariantList beamEntities();
    /*Factory methods invoked by the QML side mainly*/
    Q_INVOKABLE void produceTPZForce(Qt3DCore::QEntity* parentEntity, QVariantHash properties);
    void registerDependentObject(QObject* o);

     bool ready(){return m_ready;}
protected slots:
    void initViewModels();
signals:
    void staticsModuleChanged();
    void scaleFactorsUpdated();
    void readyChanged();
private slots:
    void onResourceDestroyed(QObject*);
    void onResourcesUpdate();
    void updateScaleFactors();
    void generateSoundEffects();
    void playEffect(QMediaPlayer::State state);

private:
    bool m_dying;
    Frame3DDKernel* m_staticsModule;
    QHash<Qt3DCore::QNodeId, AbstractElementViewModel*> m_entityID2viewModel;
    QHash<QString, Qt3DCore::QNodeId > m_entityNameMap;

    QList<qreal> m_previousStresses;

    QMediaPlayer m_player;
    QList<QUrl> m_effectList;
    QTimer m_lazy_player_timer;

    QSet<QObject*> m_dependent_objects;
    bool m_ready;
};


#endif // FRAME3DDVMMANAGER_H
