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
    Q_PROPERTY(QVariant eventHandler WRITE setEventHandler)

public:

    enum Status{NOT_LOADED,LOADED};
    enum Stability{UNSTABLE, DETERMINATE,INDETERMINATE};

    AbstractStaticsModule(QObject *parent = 0);

    void setSourceUrl(QUrl sourceUrl){readStructure(sourceUrl.toLocalFile());}
    void setSource(QString source){readStructure(source);}
    void setEventHandler(QVariant eventHandler);

    Status status(){return m_status;}
    Stability stability(){return m_stability;}
    //QVariant eventHandler(){return QVariant::fromValue(m_event_handler);}

    virtual void addElement(AbstractElement* element)=0;
    virtual void removeElement(AbstractElement* element)=0;
    virtual AbstractElement* getElement(QString elementName)=0;
    virtual bool containsElement(QString elementName)=0;

signals:
    void statusChanged();
    void stabilityChanged();
protected:
    virtual bool readStructure(QString path) =0;
    virtual void update() =0;

protected:
    Status m_status;
    Stability m_stability;
    AbstractEventHandler* m_event_handler;





};

#endif // ABSTRACTSTATICSMODULE_H
