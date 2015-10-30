#ifndef ABSTRACTSTATICSMODULE_H
#define ABSTRACTSTATICSMODULE_H

#include <QObject>
#include <QVector3D>
#include <QUrl>
#include "elements/abstractelement.h"

class Joint;
class Beam;
class Force;
class NodeLoad;
class UniformlyDistributedLoad;
class InteriorPointLoad;

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
    enum Stability{UNSTABLE,DETERMINATE,INDETERMINATE};

    AbstractStaticsModule(QObject *parent = 0);

    void setSourceUrl(QUrl sourceUrl){readStructure(sourceUrl.toLocalFile());}
    void setSource(QString source){readStructure(source);}

    Status status(){return m_status;}
    Stability stability(){return m_stability;}

    virtual Beam* createBeam(Joint* extreme1,Joint* extreme2,QString name=QString(),
                             qreal Ax=0, qreal Asy=0, qreal Asz=0, qreal Jx=0,
                             qreal Iy=0, qreal Iz=0, qreal E=0, qreal G=0,
                             qreal p=0, qreal d=0)=0;
    virtual Joint* createJoint(QVector3D position,QString name=QString(),
                               bool  support_X=false,bool support_Y=false,bool support_Z=false,
                               bool support_XX=false,bool support_YY=false,bool support_ZZ=false )=0;
    virtual NodeLoad* createNodeLoad(QVector3D force, Joint* joint,QString name=QString())=0;
    virtual UniformlyDistributedLoad* createUDLoad(QVector3D force, Beam* beam,QString name=QString())=0;
    virtual InteriorPointLoad* createIPLoad(QVector3D force, Beam* beam,QString name=QString())=0;

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
