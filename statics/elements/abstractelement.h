#ifndef ABSTRACTELEMENT_H
#define ABSTRACTELEMENT_H

#include <QObject>
#include <Qt3DCore>
#include "statics/abstractelementviewmodel.h"

#define EPSILON 0.001

class AbstractElement : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AbstractElementViewModel* Vm READ vm WRITE setVm NOTIFY VmChanged)
public:
    enum Element_Type{JOINT,BEAM,FORCE};

    AbstractElement(QObject *parent = 0);
    AbstractElement(QString name, QObject *parent = 0);

    virtual AbstractElementViewModel* vm()=0;
    virtual void setVm(AbstractElementViewModel* vm)=0;

signals:

    void VmChanged();

public slots:



};

#endif // ABSTRACTELEMENT_H
