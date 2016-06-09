#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject* parent=0);
    Q_INVOKABLE void log(QString action, QVariantMap fields=QVariantMap());

signals:

public slots:

private:


};

#endif // LOGGER_H
