#ifndef ABSTRACTSTATICSMODULE_H
#define ABSTRACTSTATICSMODULE_H

#include <QObject>
#include <QVector4D>
#include <QUrl>
#include "force.h"
class AbstractStaticsModule : public QObject
{
        Q_OBJECT
public:
    enum Status{NOT_LOADED,LOADED};
    enum Stability{UNSTABLE, DETERMINATE,INDETERMINATE};

    AbstractStaticsModule();
    AbstractStaticsModule(QString path);

    //virtual QVector3D getInternalForce(QString element_id) =0;
    virtual void addExternalForce(Force* force);
    virtual void removeExternalForce(QString id);

    Status getStatus(){return status;}
    Stability getStability(){return stability;}
    virtual AbstractElement* getElementbyName(QString name) =0;

public slots:
    virtual bool readStructure(QString path) =0;
    virtual void update() =0;


protected:
    Status status;
    Stability stability;
    QString generateExternalForceID();
    QList<Force*> force_list;

};

#endif // ABSTRACTSTATICSMODULE_H
