#ifndef ABSTRACTELEMENT_H
#define ABSTRACTELEMENT_H

#include <QObject>
#include <QSharedPointer>

#define EPSILON 0.001

class AbstractElement : public QObject
{
    Q_OBJECT
public:
    AbstractElement(QObject *parent = 0);
    AbstractElement(QString name, QObject *parent = 0);
signals:
    void killMe();
};

#endif // ABSTRACTELEMENT_H
