#ifndef TRAPEZOIDALFORCEVM_H
#define TRAPEZOIDALFORCEVM_H

#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/trapezoidalforce.h"
#include <QVariantHash>
#include <QQmlComponent>
class TrapezoidalForceVM: public AbstractElementViewModel
{
    Q_OBJECT
public:
    explicit TrapezoidalForceVM(TrapezoidalForcePtr force,
                                Qt3DCore::QEntity* sceneRoot,
                                Qt3DCore::QEntity* parentEntity,
                                QVariantHash properties=QVariantHash(),
                                QObject* parent=0 );
    ~TrapezoidalForceVM();
    WeakTrapezoidalForcePtr trapezoidalForce(){return m_trapezoidalForce;}
    Q_INVOKABLE void setProperties(QVariantHash properties);
    void setParentEntity(Qt3DCore::QEntity* parentEntity);
    AbstractElement *element();
    inline Qt3DCore::QEntity *component3D(){return m_component3D;}

//    void setTrapezoidalForcePtr(TrapezoidalForcePtr force);
public slots:
    void onForceChanged();
    void onRelativePositionChanged();
private:
    void initView(Qt3DCore::QEntity* parentEntity, QVariantHash properties);
    WeakTrapezoidalForcePtr m_trapezoidalForce;
    Qt3DCore::QEntity* m_component3D;
    QUrl m_asset_tmp_copy;
    static QSharedPointer<QQmlComponent> m_qqmlcomponent;
    QQmlContext* m_qqmlcontext;
};

#endif // TRAPEZOIDALFORCEVM_H
