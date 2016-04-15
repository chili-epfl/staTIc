#ifndef SNOW_H
#define SNOW_H

#include <QObject>
#include "statics/abstractvmmanager.h"

class Snow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AbstractVMManager* vmManager READ vmManager WRITE setVMManager)

public:
    explicit Snow(QObject *parent = 0);
    Q_INVOKABLE void findExposedBeams();
    AbstractVMManager* vmManager(){return m_vm_manager;}
    void setVMManager(AbstractVMManager*);
signals:

public slots:

private:
    AbstractVMManager* m_vm_manager;
};

#endif // SNOW_H
