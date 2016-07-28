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

void AbstractStaticsModule::setStability(AbstractStaticsModule::Stability val)
{
    m_stability=val;
    emit stabilityChanged();

}

/*From real world to tangible scale factor*/
qreal AbstractStaticsModule::modelScale(){
    return m_modelScale;
}
