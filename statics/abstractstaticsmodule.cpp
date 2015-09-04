#include "abstractstaticsmodule.h"
#include "abstracteventhandler.h"
AbstractStaticsModule::AbstractStaticsModule(QObject* parent ):
    QObject(parent)
{
    m_status=NOT_LOADED;

}
