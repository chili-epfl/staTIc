#ifndef JOINT_H
#define JOINT_H

#include <QString>
#include <QList>
#include <QVector3D>
#include "statics/elements/abstractelement.h"

class Beam;

class Joint:public AbstractElement
{
    Q_OBJECT
    Q_PROPERTY(QVector3D reaction READ reaction WRITE setReaction NOTIFY reactionChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(SupportType supportType READ supportType WRITE setSupportType NOTIFY supportTypeChanged)

public:
    enum SupportType {NOSUPPORT,FIXED,ROLLING};

    Joint(QString name,QObject* parent=0);
    Joint(QObject* parent=0);

    void setSupportType(SupportType jointSupport){if(m_jointSupport!=jointSupport){m_jointSupport=jointSupport;emit supportTypeChanged();}}
    SupportType supportType(){return m_jointSupport;}

    void setPosition(QVector3D pos){if(m_pos!=pos){m_pos=pos;emit positionChanged(m_pos);}}
    QVector3D position(){return m_pos;}

    void setReaction(QVector3D reaction){if(m_reaction!=reaction){m_reaction=reaction;emit reactionChanged(m_reaction);}}
    QVector3D reaction(){return m_reaction;}

    QList<Beam*> connected_beams;

signals:
    void reactionChanged(QVector3D reaction);
    void positionChanged(QVector3D position);
    void supportTypeChanged();
    void connectedBeamsChanged();

private:
    QVector3D m_pos;
    QVector3D m_reaction;
    SupportType m_jointSupport;

};

#endif // JOINT_H
