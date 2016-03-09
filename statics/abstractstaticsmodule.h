#ifndef ABSTRACTSTATICSMODULE_H
#define ABSTRACTSTATICSMODULE_H

#include <QObject>
#include <QVector3D>
#include <QVector2D>
#include <QUrl>
#include "elements/abstractelement.h"
#include "materialsmanager.h"

class Joint;
typedef QSharedPointer<Joint> JointPtr;
class Beam;
typedef QSharedPointer<Beam> BeamPtr;
class NodeLoad;
typedef QSharedPointer<NodeLoad> NodeLoadPtr;
class UniformlyDistributedLoad;
typedef QSharedPointer<UniformlyDistributedLoad> UniformlyDistributedLoadPtr;
class InteriorPointLoad;
typedef QSharedPointer<InteriorPointLoad> InteriorPointLoadPtr;
class TrapezoidalForce;
typedef QSharedPointer<TrapezoidalForce> TrapezoidalForcePtr;

typedef QSharedPointer<AbstractElement> AbstractElementPtr;
typedef QWeakPointer<AbstractElement> WeakAbstractElementPtr;


class AbstractStaticsModule : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(Stability)

    Q_PROPERTY(QUrl sourceUrl WRITE setSourceUrl)
    Q_PROPERTY(QString source WRITE setSource)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(Stability stability READ stability NOTIFY stabilityChanged)

    Q_PROPERTY(bool is2D READ is2D NOTIFY is2DChanged)

    Q_PROPERTY(qreal maxForce READ maxForce NOTIFY maxForceChanged)
    Q_PROPERTY(qreal minForce READ minForce NOTIFY minForceChanged)

    Q_PROPERTY(MaterialsManager* materialsManager READ materialsManager NOTIFY materialsManagerChanged)

public:

    enum Status{NOT_LOADED,LOADED};

    enum Stability{UNSTABLE,DETERMINATE,INDETERMINATE};

    static qreal modelScale();

    AbstractStaticsModule(QObject *parent = 0);
    ~AbstractStaticsModule();
    void setSourceUrl(QUrl sourceUrl){readStructure(sourceUrl.toLocalFile());}
    void setSource(QString source){readStructure(source);}

    Status status(){return m_status;}
    Stability stability(){return m_stability;}
    bool is2D(){return m_is2D;}
    virtual qreal maxForce()=0;
    virtual qreal minForce()=0;

    virtual BeamPtr createBeam(JointPtr extreme1,JointPtr extreme2,QSizeF size,qreal E,
                               qreal G, qreal d,QString name=QString())=0;
    virtual BeamPtr createBeam(JointPtr extreme1,JointPtr extreme2,QSizeF size,QString materialID,QString name=QString())=0;
    virtual JointPtr createJoint(QVector3D position,QString name=QString(),
                               bool  support_X=false,bool support_Y=false,bool support_Z=false,
                               bool support_XX=false,bool support_YY=false,bool support_ZZ=false )=0;
    virtual NodeLoadPtr createNodeLoad(QVector3D force, JointPtr joint,QString name=QString())=0;
    virtual UniformlyDistributedLoadPtr createUDLoad(QVector3D force, BeamPtr beam,QString name=QString())=0;
    virtual InteriorPointLoadPtr createIPLoad(QVector3D force, BeamPtr beam,qreal distance=-1,QString name=QString())=0;
    virtual TrapezoidalForcePtr createTPZLoad(QVector3D force, BeamPtr beam, QVector3D localPosition, QVector2D extent, QString name=QString())=0;

    virtual void removeBeam(BeamPtr)=0;
    virtual void removeJoint(JointPtr)=0;
    virtual void removeNodeLoad(NodeLoadPtr)=0;
    virtual void removeUDLoad(UniformlyDistributedLoadPtr)=0;
    virtual void removeIPLoad(InteriorPointLoadPtr)=0;
    virtual void removeTPZLoad(TrapezoidalForcePtr)=0;

    virtual bool splitBeam(BeamPtr beam, qreal offset,JointPtr &new_joint)=0;
    virtual bool unifyBeam(BeamPtr beam)=0;

    MaterialsManager* materialsManager(){return m_materialsManager;}

signals:
    void statusChanged();
    void stabilityChanged();
    void minForceChanged();
    void maxForceChanged();
    void updated();
    void materialsManagerChanged();
    void is2DChanged();

protected:
    virtual bool readStructure(QString path) =0;
    virtual void update() =0;

protected:
    Status m_status;
    Stability m_stability;
    static qreal m_modelScale;
    MaterialsManager* m_materialsManager;
    bool m_is2D;

};

#endif // ABSTRACTSTATICSMODULE_H
