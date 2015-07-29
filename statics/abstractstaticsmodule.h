#ifndef ABSTRACTSTATICSMODULE_H
#define ABSTRACTSTATICSMODULE_H

#include <QObject>
#include <QVector4D>
#include <QUrl>
#include "elements/abstractelement.h"

class AbstractEventHandler;

class AbstractStaticsModule : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(Stability)
    Q_PROPERTY(QUrl sourceUrl WRITE setSourceUrl)
    Q_PROPERTY(QString source WRITE setSource)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(Stability stability READ stability NOTIFY stabilityChanged)
    Q_PROPERTY(QVariant eventHandler READ eventHandler WRITE setEventHandler)
    Q_PROPERTY(Qt3D::QEntity* sceneRoot READ sceneRoot WRITE setSceneRoot NOTIFY sceneRootChanged)

public:

    enum Status{NOT_LOADED,LOADED};
    enum Stability{UNSTABLE, DETERMINATE,INDETERMINATE};

    AbstractStaticsModule(QObject *parent = 0);

    void setSourceUrl(QUrl sourceUrl){readStructure(sourceUrl.toLocalFile());}
    void setSource(QString source){readStructure(source);}
    void setEventHandler(QVariant eventHandler);
    void setSceneRoot(Qt3D::QEntity* sceneRoot);

    Status status(){return m_status;}
    Stability stability(){return m_stability;}
    QVariant eventHandler(){return qVariantFromValue((void*)m_event_handler);}
    Qt3D::QEntity* sceneRoot(){return m_sceneRoot;}

    virtual AbstractElement* createElement(AbstractElement::Element_Type type, QVariantList args )=0;
    virtual void removeElement(QString element)=0;
    virtual AbstractElement* getElement(QString elementName)=0;
    virtual bool containsElement(QString elementName)=0;

signals:
    void statusChanged();
    void stabilityChanged();
    void sceneRootChanged(Qt3D::QEntity* sceneRoot);
protected:
    virtual bool readStructure(QString path) =0;
    virtual void update() =0;

protected:
    Status m_status;
    Stability m_stability;
    AbstractEventHandler* m_event_handler;
    Qt3D::QEntity* m_sceneRoot;
};

#endif // ABSTRACTSTATICSMODULE_H
