#ifndef STATICSMODULE_H
#define STATICSMODULE_H

#include "statics/elements/joint.h"
#include "statics/elements/beam.h"
#include <QList>
#include <QMap>
#include <opencv2/core.hpp>
#include <QFile>
#include <QTextStream>
#include "statics/abstractstaticsmodule.h"
#include "statics/two_dimentions/twodimentionaleventhandler.h"


class TwoDimensionalStaticsModule : public AbstractStaticsModule
{
    Q_OBJECT
public:
    TwoDimensionalStaticsModule(QObject* parent=0);
    void addElement(AbstractElement* element);
    void removeElement(AbstractElement* element){removeElement(element,false);}
    AbstractElement* getElement(QString elementName);
    bool containsElement(QString elementName);

protected:
    bool readStructure(QString path);
    void update();

private:
    void removeElement(AbstractElement* element,bool update);
    void check_stability();
    void update_internalF_matrix();
    void solve();

    cv::Mat internalF_matrix;

    QHash<QString,AbstractElement*> m_elements;

    QList<Joint*> m_joints;
    QList<Beam*> m_beams;
    QList<Force*> m_forces;

};

#endif // STATICSMODULE_H
