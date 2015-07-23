#ifndef BEAMVM_H
#define BEAMVM_H

#include "statics/abstractelementviewmodel.h"

class BeamVM : public AbstractElementViewModel
{
    Q_OBJECT
public:
    BeamVM(QObject* parent=0);
public slots:
    void onElementChanged(){};
    void onPropertyChanged(){};

};

#endif // BEAMVM_H
