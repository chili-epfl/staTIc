#ifndef JOINTVM_H
#define JOINTVM_H
#include "statics/abstractelementviewmodel.h"
#include "statics/elements/joint.h"

class JointVM:public AbstractElementViewModel
{
    Q_OBJECT
public:
    JointVM(QObject* parent=0);
public slots:
    void onElementChanged();
    void onPropertyChanged(){};
    void onElementDestroyed(){};

private:
};

#endif // JOINTVM_H
