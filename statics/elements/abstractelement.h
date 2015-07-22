#ifndef ABSTRACTELEMENT_H
#define ABSTRACTELEMENT_H

#include <QObject>
#include <Qt3DCore>


class AbstractElement : public QObject
{
    Q_OBJECT
public:
    AbstractElement(QObject *parent = 0);
    AbstractElement(QString name, QObject *parent = 0);

signals:

public slots:

protected:

};

#endif // ABSTRACTELEMENT_H
