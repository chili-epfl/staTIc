#ifndef ABSTRACTSTATICSMODULE_H
#define ABSTRACTSTATICSMODULE_H

#include <QObject>
#include <QVector4D>
#include <QUrl>
#include "elements/abstractelement.h"

class AbstractEventHandler;
class Joint;
class Beam;
class Force;

class AbstractStaticsModule : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(Stability)

    Q_PROPERTY(QUrl sourceUrl WRITE setSourceUrl)
    Q_PROPERTY(QString source WRITE setSource)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(Stability stability READ stability NOTIFY stabilityChanged)

public:

    enum Status{NOT_LOADED,LOADED};
    enum Stability{UNSTABLE, DETERMINATE,INDETERMINATE};

    AbstractStaticsModule(QObject *parent = 0);

    void setSourceUrl(QUrl sourceUrl){readStructure(sourceUrl.toLocalFile());}
    void setSource(QString source){readStructure(source);}

    Status status(){return m_status;}
    Stability stability(){return m_stability;}

    virtual Force* createForce(QVector3D applicationPoint, QVector3D force_vector, AbstractElement* applicationElement=Q_NULLPTR)=0;
    virtual Beam* createBeam(Joint* extreme1,Joint* extreme2,QString name=QString())=0;
    virtual Joint* createJoint(QVector3D position, QString supportType=QString(),  QString name=QString() )=0;

signals:
    void statusChanged();
    void stabilityChanged();

protected:
    virtual bool readStructure(QString path) =0;
    virtual void update() =0;

protected:
    Status m_status;
    Stability m_stability;
};

#endif // ABSTRACTSTATICSMODULE_H
