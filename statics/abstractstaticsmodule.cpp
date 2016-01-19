#include "abstractstaticsmodule.h"
#include "abstractvmmanager.h"

qreal AbstractStaticsModule::m_modelScale=1;

AbstractStaticsModule::AbstractStaticsModule(QObject* parent ):
    QObject(parent)
{
    m_status=NOT_LOADED;    
}

qreal AbstractStaticsModule::modelScale(){
    return m_modelScale;
}
