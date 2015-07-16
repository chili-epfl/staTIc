#ifndef JOINT_H
#define JOINT_H

#include <QString>
#include <QList>
#include <QVector3D>
#include "abstractelement.h"

class Member;

class Joint:public AbstractElement
{

public:
    enum SupportType {NOSUPPORT,FIXED,ROLLING};

    Joint(QString name);
    QList<Member*> connected_members;

    void setSupport(SupportType jointSupport){this->jointSupport=jointSupport;}
    SupportType getSupport(){return jointSupport;}

    void setPosition(QVector3D pos){this->pos=pos;}
    QVector3D getPosition(){return pos;}

    void setReaction(QVector3D reaction){this->reaction=reaction;}
    QVector3D getReaction(){return reaction;}

private:
    QVector3D pos;
    QVector3D reaction;
    SupportType jointSupport;
};

#endif // JOINT_H
