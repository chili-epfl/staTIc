#ifndef FORCEVM_H
#define FORCEVM_H

#include "statics/abstractelementviewmodel.h"

class ForceVM: public AbstractElementViewModel
{
    Q_OBJECT
public:
    ForceVM(QObject* parent=0);
public slots:
    void onElementChanged();
    void onPropertyChanged();
    void onElementDestroyed();

};

#endif // FORCEVM_H
