#ifndef ABSTRACTELEMENT_H
#define ABSTRACTELEMENT_H

#include <QObject>

class AbstractElement : public QObject
{
    Q_OBJECT
public:
    explicit AbstractElement(QString name, QObject *parent = 0);

signals:

public slots:
};

#endif // ABSTRACTELEMENT_H
