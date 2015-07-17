#ifndef STATICSMODULE_H
#define STATICSMODULE_H

#include "joint.h"
#include "beam.h"
#include <QList>
#include <QMap>
#include <opencv2/core.hpp>
#include <QFile>
#include <QTextStream>
#include "abstractstaticsmodule.h"


class TwoDimensionalStaticsModule : public AbstractStaticsModule
{
public:
    TwoDimensionalStaticsModule();
    TwoDimensionalStaticsModule(QString path);
    AbstractElement* getElementbyName(QString name);
    //QVector3D getInternalForce(QString element_id);

public slots:
    bool readStructure(QString path);
    void update();

private:
    void check_stability();
    void update_internalF_matrix();
    void solve();
    QList<Joint*> joints;
    QList<Beam*> beams;
    QList<Joint*> supportJoints;

    cv::Mat internalF_matrix;

};

#endif // STATICSMODULE_H
