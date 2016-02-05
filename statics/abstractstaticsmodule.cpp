#include "abstractstaticsmodule.h"
#include "abstractvmmanager.h"

qreal AbstractStaticsModule::m_modelScale=1;

AbstractStaticsModule::AbstractStaticsModule(QObject* parent ):
    QObject(parent)
{
    m_status=NOT_LOADED;
    m_materialsManager=new MaterialsManager();
    emit materialsManagerChanged();
}

AbstractStaticsModule::~AbstractStaticsModule()
{
    delete m_materialsManager;
}

qreal AbstractStaticsModule::modelScale(){
    return m_modelScale;
}
