#include "qmlenginefactory.h"

QQmlEngine* QmlEngineFactory::m_engine=Q_NULLPTR;

void QmlEngineFactory::setEngine(QQmlEngine* e){
    QmlEngineFactory::m_engine=e;
}
QQmlEngine* QmlEngineFactory::engine(){
    return  QmlEngineFactory::m_engine;
}
