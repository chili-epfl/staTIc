#ifndef QMLENGINEFACTORY_H
#define QMLENGINEFACTORY_H
#include <QQmlEngine>

class QmlEngineFactory
{
public:
    static void setEngine(QQmlEngine* e);
    static QQmlEngine* engine();
private:
    static QQmlEngine* m_engine;
};

#endif // QMLENGINEFACTORY_H
