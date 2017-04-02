#ifndef JOINT_H
#define JOINT_H

#include <QString>
#include <QList>
#include <QVector3D>
#include <QQmlComponent>

#include "statics/elements/abstractelement.h"

class Beam;
typedef QSharedPointer<Beam> BeamPtr;
typedef QWeakPointer<Beam> WeakBeamPtr;

class Joint:public AbstractElement
{
    Q_OBJECT
    Q_PROPERTY(QVector3D reaction READ reaction NOTIFY reactionChanged)
    Q_PROPERTY(QVector3D displacement READ displacement NOTIFY displacementChanged)
    Q_PROPERTY(QVector3D position READ position  NOTIFY positionChanged)
    Q_PROPERTY(QString supportType READ supportType WRITE setSupportType NOTIFY supportChanged)
    Q_PROPERTY(QVariantList connectedBeams READ connectedBeamsAsVariant NOTIFY connectedBeamsChanged)
public:
    Joint(QVector3D position, QString name=QString(),QObject* parent=0);

    void setSupport(bool  support_X,bool support_Y,bool support_Z, bool support_XX,bool support_YY,bool support_ZZ);
    void support(bool  &support_X,bool &support_Y,bool &support_Z, bool &support_XX,bool &support_YY,bool &support_ZZ);

    QVector3D position(){return m_pos;}

    void setReaction(QVector3D reaction);
    QVector3D reaction(){return m_reaction;}

    void setReactionMomentum(QVector3D reaction);
    QVector3D reactionMomentum(){return m_reaction_momentum;}

    void setDisplacement(QVector3D displacement);
    void setDisplacementRot(QVector3D displacement);

    QVector3D displacement() {return m_displacement;}
    QVector3D displacementRot(){return m_displacement_rot;}

    QList<WeakBeamPtr> connectedBeams();
    QVariantList connectedBeamsAsVariant();

    void addConnectedBeam(BeamPtr b);

    void createQmlEntity(QVariantMap aesthetics=QVariantMap());

    QString supportType();
    void setSupportType(QString type);
public slots:
    void onBeamDestroyed();

signals:
    void reactionChanged();
    void supportChanged();
    void connectedBeamsChanged();
    void displacementChanged();
    void displacementRotChanged();
    void positionChanged();
private:

    QVector3D m_pos;
    QVector3D m_reaction;
    QVector3D m_reaction_momentum;
    QVector3D m_displacement;
    QVector3D m_displacement_rot;

    QList<WeakBeamPtr> m_connected_beams;

    bool  m_support_X,m_support_Y,m_support_Z,m_support_XX,m_support_YY,m_support_ZZ;

    Qt3DEntityPtr m_component3D;

    static QQmlComponent* m_qqmlcomponent;


};

typedef QSharedPointer<Joint> JointPtr;
typedef QWeakPointer<Joint> WeakJointPtr;
Q_DECLARE_METATYPE(Joint*)



#endif // JOINT_H
