#ifndef ABSTRACTELEMENT_H
#define ABSTRACTELEMENT_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>
#include <Qt3DCore/QEntity>
#include <QPointer>
#define EPSILON 0.001

typedef QPointer<Qt3DCore::QEntity> Qt3DEntityPtr;

class AbstractElement : public QObject
{
    Q_OBJECT
public:
    AbstractElement(QObject *parent = 0);
    AbstractElement(QString name, QObject *parent = 0);
    void setSceneRoot(Qt3DCore::QEntity* sceneroot);
    virtual void createQmlEntity(QVariantMap aesthetics=QVariantMap())=0;
signals:
    /*Abstract Element should not be handled directly, but using smart pointers.
    * In this way, it's easier to check when they are null.
    * Since elements are stored in statics manager, to kill them it's necessary to remove the
    * relative strong pointer from the manager. Never use delete!*/
    void killMe();
protected:
    QTimer m_lazy_signal_emitter;
    Qt3DEntityPtr m_sceneRoot;
};

#endif // ABSTRACTELEMENT_H
