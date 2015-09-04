#ifndef BEAM_H
#define BEAM_H

#include<QString>
#include<QPair>

#include "statics/elements/joint.h"
#include "statics/elements/abstractelement.h"

typedef QPair<Joint*,Joint*> JointPair;

class Beam : public AbstractElement
{
    Q_OBJECT
    Q_PROPERTY(JointPair extremes READ extremes WRITE setExtremes NOTIFY extremesChanged)
    Q_PROPERTY(qreal axialForce READ axialForce WRITE setAxialForce NOTIFY axialForceChanged)

public:
    Beam(QString name,QObject* parent=0);
    Beam(QObject* parent=0);

    void setAxialForce(qreal val){if(m_axial_force!=val){m_axial_force=val;emit axialForceChanged(m_axial_force);}}
    qreal axialForce(){return m_axial_force;}

    QPair<Joint*,Joint*> extremes(){return m_extremes;}
    void setExtremes(QPair<Joint*,Joint*> extremes);

signals:
    void extremesChanged();
    void axialForceChanged(qreal axialForce);

private:
    qreal m_axial_force;
    QPair<Joint*,Joint*> m_extremes;

};

#endif // BEAM_H

