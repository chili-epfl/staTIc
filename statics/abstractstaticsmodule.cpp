#include "abstractstaticsmodule.h"


AbstractStaticsModule::AbstractStaticsModule(QObject* parent ):
    QObject(parent)
{
    m_modelScale=1.0;
    m_status=NOT_LOADED;
    m_sceneRoot=Q_NULLPTR;
    m_materialsManager=new MaterialsManager();
    emit materialsManagerChanged();
}

AbstractStaticsModule::~AbstractStaticsModule()
{
    delete m_materialsManager;
}

void AbstractStaticsModule::setSceneRoot(Qt3DCore::QEntity *sceneRoot)
{
    if(m_sceneRoot!=sceneRoot){
        m_sceneRoot=sceneRoot;
        emit sceneRootChanged();
    }
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

qreal AbstractStaticsModule::setModelScale(qreal val)
{
    if(val!=m_modelScale && val>0.0){
        m_modelScale=val;
        emit modelScaleChanged();
    }
}
