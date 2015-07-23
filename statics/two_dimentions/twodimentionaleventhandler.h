#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <Qt3DCore/QEntity>
#include <Qt3DRenderer>
#include "statics/elements/force.h"
#include "statics/two_dimentions/twodimensionalstaticsmodule.h"
#include "statics/abstracteventhandler.h"



class TwoDimentionalEventHandler : public AbstractEventHandler
{
    Q_OBJECT

public:
    TwoDimentionalEventHandler(QObject* parent=0);

public slots:
    void inputEventHandler(EventType type, QVariantMap args);

private:

};

#endif // CONTROLLER_H
