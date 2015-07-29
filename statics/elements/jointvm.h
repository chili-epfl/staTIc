#ifndef JOINTVM_H
#define JOINTVM_H
#include "statics/abstractelementviewmodel.h"

class Joint;

class JointVM:public AbstractElementViewModel
{
    Q_OBJECT
public:
    JointVM(QObject* parent=0);
public slots:
    void onElementNameChanged(QString val);
    void onElementDestroyed();
    void onElementVmChanged();


    void onJointReactionChanged(QVector3D val);
    void onJointPositionChanged(QVector3D val);
    void onJointSupportTypeChanged();
    void onJointConnectedBeamsChanged();


    void onElementSelected();
    void onStatusComplete();

signals:
    void updateReactionDirection(qreal val);
    void updateReactionMagnitude(qreal val);
private:
};

#endif // JOINTVM_H
