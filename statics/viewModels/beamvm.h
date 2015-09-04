#ifndef BEAMVM_H
#define BEAMVM_H

#include "statics/viewModels/abstractelementviewmodel.h"
#include "statics/elements/beam.h"

class BeamVM : public AbstractElementViewModel
{
    Q_OBJECT
    Q_PROPERTY(bool visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
public:
    explicit BeamVM(Beam* beam,Qt3D::QEntity* sceneRoot,QObject* parent=0);

    bool visibility(){return m_visible;}
    void setVisibility(bool val){if(val!=m_visible){m_visible=val;emit visibilityChanged(m_visible);}}
    void onSelect(){setVisibility(!m_visible);}
    AbstractElement* model(){return m_beam;}

public slots:        
    void onElementDestroyed();

    /*Slots for signals from the model*/
    void onBeamExtremesChanged();
    void onBeamAxialForceChanged(qreal val);
    /*-----*/

signals:
    void updateForceMagnitude(qreal val);
    void updateForceDirectionEx1(qreal val);
    void updateForceDirectionEx2(qreal val);
    void visibilityChanged(bool val);

private:

    void initView();

    bool m_visible;
    Beam* m_beam;

};

#endif // BEAMVM_H
