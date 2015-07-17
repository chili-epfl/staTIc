#ifndef BEAM_H
#define BEAM_H

#include<QString>
#include<QPair>
#include "joint.h"
#include "abstractelement.h"

class Beam : public AbstractElement
{
public:
    Beam(QString name);
    QPair<Joint*,Joint*> extremes;
    void setAxialForce(qreal val){this->axial_force=val;}
    qreal getAxialForce(){return axial_force;}
private:
    qreal axial_force;


};

#endif // BEAM_H
