#ifndef JOINTVM_H
#define JOINTVM_H
#include "statics/abstractelementviewmodel.h"

class Joint;

class JointVM:public AbstractElementViewModel
{
    Q_OBJECT
    Q_PROPERTY(bool reactionIsVisible READ reactionIsVisible WRITE setReactionIsVisible NOTIFY reactionIsVisibleChanged)
    Q_PROPERTY(bool FBDisVisible READ FBDisVisible WRITE setFBDisVisible NOTIFY FBDisVisibleChanged)
public:
    JointVM(QObject* parent=0);
    bool reactionIsVisible(){return m_reactionIsVisible;}
    void setReactionIsVisible(bool val){if(m_isSupport && m_reactionIsVisible!=val){m_reactionIsVisible=val;emit reactionIsVisibleChanged(m_reactionIsVisible);}}
    bool FBDisVisible(){return m_FBDIsVisible;}
    void setFBDisVisible(bool val){if(m_FBDIsVisible!=val){m_FBDIsVisible=val;emit FBDisVisibleChanged(m_FBDIsVisible);}}

    void select(){setFBDisVisible(!m_FBDIsVisible);setReactionIsVisible(!m_reactionIsVisible);}

public slots:
    void onElementNameChanged(QString val);
    void onElementDestroyed();
    void onElementVmChanged();

    /*Slots for signals from the model*/
    void onJointReactionChanged(QVector3D val);
    void onJointPositionChanged(QVector3D val);
    void onJointSupportTypeChanged();
    void onJointConnectedBeamsChanged();

    void onBeamAxialForceChanged(qreal val);
    /*---------*/
    void onStatusComplete();

signals:
    void updateReactionDirection(qreal val);
    void updateReactionMagnitude(qreal val);
    void reactionIsVisibleChanged(bool val);
    void FBDisVisibleChanged(bool val);
private:
    bool m_reactionIsVisible;
    bool m_isSupport;
    bool m_FBDIsVisible;

    QHash<AbstractElement*,Qt3D::QEntity*> m_beamsMap;

};

#endif // JOINTVM_H
