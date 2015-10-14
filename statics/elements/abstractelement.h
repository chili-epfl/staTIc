#ifndef ABSTRACTELEMENT_H
#define ABSTRACTELEMENT_H

#include <QObject>

#define EPSILON 0.001

class AbstractElement : public QObject
{
    Q_OBJECT
public:

    AbstractElement(QObject *parent = 0);
    AbstractElement(QString name, QObject *parent = 0);

};

#endif // ABSTRACTELEMENT_H
