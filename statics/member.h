#ifndef MEMBER_H
#define MEMBER_H

#include<QString>
#include<QPair>
#include "joint.h"
#include "abstractelement.h"

class Member : public AbstractElement
{
public:
    Member(QString name);
    QPair<Joint*,Joint*> extremes;
    void setAxialForce(qreal val){this->axial_force=val;}
    qreal getAxialForce(){return axial_force;}
private:
    qreal axial_force;


};

#endif // MEMBER_H
