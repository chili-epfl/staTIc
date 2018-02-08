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
    Q_INVOKABLE QString exportJSONSketch(const QString url, QObject* sketch, int mode);

    Q_INVOKABLE int addPoint(int x, int y);
    Q_INVOKABLE int addLine(int p1, int p2);
    Q_INVOKABLE int addConstraint(int type, int valA, int ptA, int ptB, int entityA, int entityB);
private:
    int originId;
    int nextPointId;
    int nextLineId;
    int nextConstraintId;
    QMap<int, QVector2D> points;
    QMap<int, QVector2D> lines;
    QMap<int, QList<int>> constraints;
    QString read(const QJsonObject json, QObject* sketch);
    void generateSketch(QObject* sketch);
    bool writeAll(QJsonObject &json, QObject* sketch);
    bool writeMm(QJsonObject &json, QObject* sketch);
    int incrementPointsId();
    int incrementLinesId();
    int incrementConstraintId();
};

#endif // JSONSKETCH_H
