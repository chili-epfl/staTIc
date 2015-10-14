#ifndef JOINTVM_H
#define JOINTVM_H
#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/joint.h"
#include "jointvmitemmodel.h"

class JointVM:public AbstractElementViewModel
{
    Q_OBJECT

public:
    enum Roles{PRIMARY, DECORATOR};
    explicit JointVM(Joint* joint,QObject* uiRoot,Qt3D::QEntity* sceneRoot,QObject* parent=0);

    bool reactionIsVisible(){return m_reactionIsVisible;}
    void setReactionIsVisible(bool val){if(m_isSupport && m_reactionIsVisible!=val){m_reactionIsVisible=val;emit reactionIsVisibleChanged(m_reactionIsVisible);}}

    bool FBDisVisible(){return m_FBDIsVisible;}
    void setFBDisVisible(bool val){if(m_FBDIsVisible!=val){m_FBDIsVisible=val;emit FBDisVisibleChanged(m_FBDIsVisible);}}

    bool detailIsVisible(){return m_detailIsVisible;}
    void setDetailIsVisible(bool val);
    Joint* joint(){return m_joint;}


public slots:
    void onElementDestroyed();

    /*Slots for signals from the model*/
    void onJointReactionChanged(QVector3D val);
    void onJointPositionChanged(QVector3D val);
    void onJointSupportTypeChanged();
    void onJointConnectedBeamsChanged();

    void onBeamAxialForceChanged(qreal val);
    /*---------*/

    void onDetailViewChangedModel();

signals:
    void updateReactionDirection(qreal val);
    void updateReactionMagnitude(qreal val);
    void reactionIsVisibleChanged(bool val);
    void FBDisVisibleChanged(bool val);
    void detailIsVisibleChanged(bool val);
private:

    void initView();

    bool m_reactionIsVisible;
    bool m_isSupport;
    bool m_FBDIsVisible;
    bool m_detailIsVisible;

    /*A map between the beams and the relative entity*/
    QHash<Beam*,Qt3D::QEntity*> m_beamsMap;
    QVector<Beam*> m_beamsVector;


    JointVMItemModel m_itemModel;

    Joint* m_joint;
};

#endif // JOINTVM_H
