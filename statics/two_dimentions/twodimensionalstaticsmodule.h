#ifndef STATICSMODULE_H
#define STATICSMODULE_H

#include "statics/elements/joint.h"
#include "statics/elements/beam.h"
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

    virtual Beam* createBeam(Joint* extreme1,Joint* extreme2,QString name=QString(),
                             qreal Ax=0, qreal Asy=0, qreal Asz=0, qreal Jx=0,
                             qreal Iy=0, qreal Iz=0, qreal E=0, qreal G=0,
                             qreal p=0, qreal d=0);
    virtual Joint* createJoint(QVector3D position,QString name=QString(),
                               bool  support_X=false,bool support_Y=false,bool support_Z=false,
                               bool support_XX=false,bool support_YY=false,bool support_ZZ=false );
    virtual NodeLoad* createNodeLoad(QVector3D force, Joint* joint,QString name=QString());
    virtual UniformlyDistributedLoad* createUDLoad(QVector3D force, Beam* beam,QString name=QString());
    
    QList<Joint*> m_joints;
    QList<Beam*> m_beams;
    QList<NodeLoad*> m_node_loads;

public slots:

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
