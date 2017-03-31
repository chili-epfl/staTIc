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
                                Qt3DEntityPtr sceneRoot,
                                Qt3DEntityPtr parentEntity,
                                QVariantHash properties=QVariantHash(),
                                QObject* parent=0 );
    ~TrapezoidalForceVM();
    WeakTrapezoidalForcePtr trapezoidalForce(){return m_trapezoidalForce;}
    Q_INVOKABLE void setProperties(QVariantHash properties);
    void setParentEntity(Qt3DEntityPtr parentEntity);
    AbstractElement *element();
    inline Qt3DEntityPtr component3D(){return m_component3D;}

//    void setTrapezoidalForcePtr(TrapezoidalForcePtr force);
public slots:
    void onForceChanged();
    void onRelativePositionChanged();
private:
    void initView(Qt3DEntityPtr parentEntity, QVariantHash properties);
    WeakTrapezoidalForcePtr m_trapezoidalForce;
    Qt3DEntityPtr m_component3D;
    QUrl m_asset_tmp_copy;
    static QQmlComponent* m_qqmlcomponent;
    QQmlContext* m_qqmlcontext;
};

#endif // TRAPEZOIDALFORCEVM_H
