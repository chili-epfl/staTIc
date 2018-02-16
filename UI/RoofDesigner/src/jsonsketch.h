#ifndef JSONSKETCH_H
#define JSONSKETCH_H

#include <QList>
#include <QVector2D>
#include <QJsonObject>
#include <QQuickItem>

class JSONSketch : public QObject
{
    Q_OBJECT
public:
    explicit JSONSketch(QObject* parent=0);

    Q_INVOKABLE QString loadSketch(const QString url, QObject* sketch);
    Q_INVOKABLE QString exportJSONSketch(const QString url, QObject* sketch);

private:

    QString read(const QJsonObject json, QObject* sketch);
    void generateSketch(QObject* sketch);

    bool writeJSON(QJsonObject &json, QObject* sketch);

};

#endif // JSONSKETCH_H
