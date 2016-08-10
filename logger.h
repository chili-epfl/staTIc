#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QTimer>
#include <QQuaternion>
class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject* parent=0);
    ~Logger();
    Q_INVOKABLE void log(QString action, QVariantMap fields=QVariantMap());
    Q_INVOKABLE void log_static_configuration(QVariantMap fields);
    Q_INVOKABLE void log_position(QString name, QVector3D translation, qreal angle_x, qreal angle_y, qreal angle_z);
    Q_INVOKABLE void restart_logger();
    Q_INVOKABLE void close_logger();
signals:

public slots:

private:


};

#endif // LOGGER_H
