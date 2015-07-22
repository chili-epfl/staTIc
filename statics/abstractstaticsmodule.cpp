#include "abstractstaticsmodule.h"
#include "abstracteventhandler.h"
AbstractStaticsModule::AbstractStaticsModule(QObject* parent ):
    QObject(parent)
{
    m_status=NOT_LOADED;
}

void AbstractStaticsModule::setEventHandler(QVariant eventHandler){
    if(!eventHandler.canConvert<AbstractEventHandler*>()) return;
    m_event_handler=eventHandler.value<AbstractEventHandler*>();
    m_event_handler->setStaticsModule(this);
}


