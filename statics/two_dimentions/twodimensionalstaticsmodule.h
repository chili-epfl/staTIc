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

    AbstractElement* createElement(AbstractElement::Element_Type type, QVariantList args );
    void removeElement(QString element){removeElement(element,true);}
    AbstractElement* getElement(QString elementName);
    bool containsElement(QString elementName);

    QList<Joint*> m_joints;
    QList<Beam*> m_beams;
    QList<Force*> m_forces;

public slots:
    void onForceUpdate(){solve();}

protected:
    bool readStructure(QString path);
    void update();

private:
    void removeElement(QString element,bool update);

    void check_stability();
    void update_internalF_matrix();
    void solve();

    cv::Mat internalF_matrix;

    //QHash<QString,AbstractElement*> m_elements;



};

#endif // STATICSMODULE_H
