#ifndef BEAMVM_H
#define BEAMVM_H

#include "statics/abstractelementviewmodel.h"

class BeamVM : public AbstractElementViewModel
{
    Q_OBJECT
    Q_PROPERTY(bool visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
public:
    BeamVM(QObject* parent=0);

    bool visibility(){return m_visible;}
    void setVisibility(bool val){if(val!=m_visible){m_visible=val;emit visibilityChanged(m_visible);}}

    void select(){setVisibility(!m_visible);}

public slots:        
    void onElementNameChanged(QString name);
    void onElementDestroyed();
    void onElementVmChanged();

    /*Slots for signals from the model*/
    void onBeamExtremesChanged();
    void onBeamAxialForceChanged(qreal val);
    /*-----*/

    void onStatusComplete();

signals:
    void updateForceMagnitude(qreal val);
    void updateForceDirectionEx1(qreal val);
    void updateForceDirectionEx2(qreal val);
    void visibilityChanged(bool val);

private:
    bool m_visible;

};

#endif // BEAMVM_H
