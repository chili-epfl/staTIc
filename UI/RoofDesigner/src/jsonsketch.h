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

    Q_INVOKABLE QString loadSketch(const QString path, const QString name, QObject* sketch);
    Q_INVOKABLE QString exportJSONSketch(const QString path, const QString name, QObject* sketch);
    Q_INVOKABLE QString deleteSketchFile(const QString path, const QString name);

private:

    QString read(const QJsonObject json, QObject* sketch, QString jsonFilePath);
    void generateSketch(QObject* sketch);

    bool writeJSON(QJsonObject &json, QObject* sketch);

};

#endif // JSONSKETCH_H
