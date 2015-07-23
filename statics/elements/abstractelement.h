#ifndef ABSTRACTELEMENT_H
#define ABSTRACTELEMENT_H

#include <QObject>
#include <Qt3DCore>
#include "statics/abstractelementviewmodel.h"

class AbstractElement : public QObject
{
    Q_OBJECT
public:
    enum Element_Type{JOINT,BEAM,FORCE};
    AbstractElement(QObject *parent = 0);
    AbstractElement(QString name, QObject *parent = 0);

    void setVM(AbstractElementViewModel* vm){
        m_vm=vm;
        m_vm->setElement(this);
        connect(this,SIGNAL(elementChanged()),m_vm,SLOT(onElementChanged()));
    }

signals:
    void elementChanged();
public slots:

protected:
    AbstractElementViewModel* m_vm;

};

#endif // ABSTRACTELEMENT_H
