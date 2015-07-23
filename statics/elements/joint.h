#ifndef JOINT_H
#define JOINT_H

#include <QString>
#include <QList>
#include <QVector3D>
#include "statics/elements/abstractelement.h"
#include "statics/elements/jointvm.h"

class Beam;

class Joint:public AbstractElement
{
    Q_OBJECT

public:
    enum SupportType {NOSUPPORT,FIXED,ROLLING};

    Joint(QString name,QObject* parent=0);
    Joint(QObject* parent=0);
    QList<Beam*> connected_beams;

    void setSupport(SupportType jointSupport){this->jointSupport=jointSupport;}
    SupportType getSupport(){return jointSupport;}

    void setPosition(QVector3D pos){this->pos=pos;}
    QVector3D getPosition(){return pos;}

    void setReaction(QVector3D reaction){this->reaction=reaction;emit elementChanged();}

    QVector3D getReaction(){return reaction;}


private:
    QVector3D pos;
    QVector3D reaction;
    SupportType jointSupport;
};

#endif // JOINT_H
