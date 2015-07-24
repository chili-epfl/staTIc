#ifndef BEAM_H
#define BEAM_H

#include<QString>
#include<QPair>

#include "statics/elements/joint.h"
#include "statics/elements/abstractelement.h"
#include "statics/elements/beamvm.h"

class Beam : public AbstractElement
{
    Q_OBJECT

public:
    Beam(QString name,QObject* parent=0);
    Beam(QObject* parent=0);
    QPair<Joint*,Joint*> extremes;
    void setAxialForce(qreal val){this->axial_force=val;}
    qreal getAxialForce(){return axial_force;}

private:
    qreal axial_force;


};

#endif // BEAM_H
