#ifndef STATICSMODULE_H
#define STATICSMODULE_H

#include "statics/elements/joint.h"
#include "statics/elements/beam.h"
#include "statics/elements/force.h"
#include "statics/abstractstaticsmodule.h"

#include <QList>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <opencv2/core.hpp>

class TwoDimensionalStaticsModule : public AbstractStaticsModule
{
    Q_OBJECT

public:
    TwoDimensionalStaticsModule(QObject* parent=0);

    /*AbstractElement* createElement(AbstractElement::Element_Type type, QVariantList args );
    void removeElement(QString element){removeElement(element,true);}
    AbstractElement* getElement(QString elementName);
    bool containsElement(QString elementName);*/

    Force* createForce(QVector3D applicationPoint, QVector3D force_vector, AbstractElement* applicationElement=Q_NULLPTR);
    Beam* createBeam(Joint* extreme1,Joint* extreme2,QString name=QString());
    Joint* createJoint(QVector3D position, QString supportType=QString(),  QString name=QString() );

    QList<Joint*> joints;
    QList<Beam*> beams;
    QList<Force*> forces;

public slots:
    void onForceUpdate();
    void onForceDeleted(QObject* obj);

protected:
    bool readStructure(QString path);
    void update();

private:

    void check_stability();
    void update_internalF_matrix();
    void solve();

    cv::Mat internalF_matrix;
};

#endif // STATICSMODULE_H
