#include "abstractstaticsmodule.h"
#include "abstractvmmanager.h"
AbstractStaticsModule::AbstractStaticsModule(QObject* parent ):
    QObject(parent)
{
    m_status=NOT_LOADED;    
}
