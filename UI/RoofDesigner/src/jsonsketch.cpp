#include <QJsonArray>
#include <QList>
#include <QVector2D>
#include <QJsonObject>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QQuickItem>
#include <QJsonDocument>
#include <QString>
#include <QQmlContext>
#include <QQmlComponent>

#include "jsonsketch.h"
#include "constraints.h"

JSONSketch::JSONSketch(QObject *parent): QObject(parent), nextPointId(0) {}

QString JSONSketch::loadSketch(const QString url, QObject* sketch)
{
    QFile file(url);

    if (!file.open(QIODevice::ReadOnly)) {
        return "cannot open the JSON file";
    }

    QByteArray data = file.readAll();

    QJsonParseError error;
    QJsonDocument doc(QJsonDocument::fromJson(data, &error));

    return read(doc.object(), sketch) + ", Sketch loaded";
}

QString JSONSketch::read(const QJsonObject json, QObject* sketch)
{
    //sketch->setProperty("scaleFactor", json["scaleFactor"].toDouble());

    int json_sketch_width=json["sketch_width"].toInt();
    int json_sketch_height=json["sketch_height"].toInt();

    originId = json["origin_id"].toInt();

    qreal scale_w=(qreal)sketch->property("width").toInt()/json_sketch_width;
    qreal scale_h=(qreal)sketch->property("height").toInt()/json_sketch_height;




    QJsonArray qPid = json["pid"].toArray();
    QJsonArray qLid = json["lid"].toArray();
    QJsonArray qCid = json["cid"].toArray();

    QJsonObject qPoints = json["points"].toObject();
    QJsonObject qLines = json["lines"].toObject();
    QJsonObject qConstraints = json["constraints"].toObject();

    QJsonArray currPoint;

    qreal max_x=-HUGE_VAL,min_x=HUGE_VAL,max_y=-HUGE_VAL,min_y=HUGE_VAL,centre_x=0,centre_y=0;
    for(int p(0); p < qPoints.size(); p++){
        int intPid = qPid[p].toInt();
        currPoint = qPoints[QString::number(intPid)].toArray();
        if (currPoint.size() != 2){
            return "corrupted point with id: " + QString::number(intPid);
        }
        max_x=qMax(max_x,currPoint[0].toDouble());
        max_y=qMax(max_y,currPoint[1].toDouble());
        min_x=qMin(min_x,currPoint[0].toDouble());
        min_y=qMin(min_y,currPoint[1].toDouble());
        centre_x+=currPoint[0].toDouble();
        centre_y+=currPoint[1].toDouble();
    }

    centre_x/=qPoints.size();
    centre_y/=qPoints.size();


    qreal sketch_width=2*(qreal)sketch->property("width").toInt()/3;
    qreal sketch_height=2*(qreal)sketch->property("height").toInt()/3;


    qreal scale;
    if(qFuzzyCompare(max_y-min_y,0) && qFuzzyCompare(max_x-min_x,0) )
        scale=1;
    else if(qFuzzyCompare(max_y-min_y,0)) scale=sketch_width/(max_x-min_x);
    else if(qFuzzyCompare(max_x-min_x,0)) scale=sketch_height/(max_y-min_y);
    else scale=qMin(sketch_width/(max_x-min_x), sketch_height/(max_y-min_y));


    for(int p(0); p < qPoints.size(); p++){
        int intPid = qPid[p].toInt();
        currPoint = qPoints[QString::number(intPid)].toArray();
        if (currPoint.size() != 2){
            return "corrupted point with id: " + QString::number(intPid);
        }
        //points.insert(intPid, QVector2D(currPoint[0].toInt()*scale_w, currPoint[1].toInt()*scale_h));
        points.insert(intPid, QVector2D( (currPoint[0].toDouble()-centre_x)*scale+sketch->property("width").toInt()/2,
                                          (currPoint[1].toDouble()-centre_y)*scale+sketch->property("height").toInt()/2));
    }

    sketch->setProperty("scaleFactor", scale); // px/mm
    QJsonArray currLine;
    for(int l(0); l < qLines.size(); l++){
        int intLid = qLid[l].toInt();
        currLine = qLines[QString::number(intLid)].toArray();
        if (currLine.size() != 2){
            return "corrupted line with id: " + QString::number(intLid);
        }
        if(currLine[0] == currLine[1] ||
                !points.contains(currLine[0].toInt()) ||
                !points.contains(currLine[1].toInt())) {
            return "corrupted point id of line with id: ";
        }
        lines.insert(intLid, QVector2D(currLine[0].toInt(), currLine[1].toInt()));
    }

    QJsonArray currConst;
    for(int c(0); c < qConstraints.size(); c++){
        int intCid = qCid[c].toInt();
        currConst = qConstraints[QString::number(intCid)].toArray();
        if (currConst.size() != 6){
            return "corrupted constraints with id: " + QString::number(intCid);
        }
        constraints.insert(intCid, QList<int>(
        {currConst[0].toInt(), currConst[1].toInt(),
         currConst[2].toInt(), currConst[3].toInt(),
         currConst[4].toInt(), currConst[5].toInt()}));
    }

    generateSketch(sketch);

    sketch->setProperty("background_picture_url", qvariant_cast<QUrl>(json["background_picture_url"].toString()));

    return "Sketch read";
}

void JSONSketch::generateSketch(QObject* sketch) {
    QQmlComponent point_component(qmlEngine(sketch),sketch);
    point_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Point.qml"));

    QMap<int, QObject*> qPoints;

    foreach (int pid, points.keys()) {
        QQmlContext* point_context = new QQmlContext(qmlContext(sketch),sketch);
        QQuickItem* point = qobject_cast<QQuickItem*>(point_component.create(point_context));

        point_context->setContextObject(point);
        point->setProperty("x", points[pid].x());
        point->setProperty("y", points[pid].y());

        point->setParent(sketch);
        point->setParentItem(qobject_cast<QQuickItem*>(sketch));
        point_component.completeCreate();

        qPoints.insert(pid, point);
    }

    if (originId >= 0){
        qPoints[originId]->setProperty("color", "green");
        sketch->setProperty("origin", qVariantFromValue(qPoints[originId]));
    }

    QQmlComponent line_component(qmlEngine(sketch),sketch);
    line_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Line.qml"));

    QMap<int, QObject*> qLines;

    foreach (int lid, lines.keys()) {
        QQmlContext* line_context = new QQmlContext(qmlContext(sketch),sketch);
        QQuickItem* line = qobject_cast<QQuickItem*>(line_component.beginCreate(line_context));

        line_context->setContextObject(line);

        line->setProperty("p1", qVariantFromValue(qPoints[lines[lid].x()]));
        line->setProperty("p2", qVariantFromValue(qPoints[lines[lid].y()]));

        line->setParent(sketch);
        line->setParentItem(qobject_cast<QQuickItem*>(sketch));
        line_component.completeCreate();

        qLines.insert(lid, line);
    }

    QQmlComponent const_component(qmlEngine(sketch),sketch);
    const_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Constraint.qml"));

    foreach (int cid, constraints.keys()) {
        QQmlContext* const_context = new QQmlContext(qmlContext(sketch),sketch);
        QQuickItem* constraint = qobject_cast<QQuickItem*>(const_component.beginCreate(const_context));

        const_context->setContextObject(constraint);

        constraint->setParent(sketch);
        constraint->setParentItem(qobject_cast<QQuickItem*>(sketch));
        const_component.completeCreate();

        constraint->setProperty("type", constraints[cid][0]);
        constraint->setProperty("valA", constraints[cid][1]);
        constraint->setProperty("ptA", qVariantFromValue(constraints[cid][2] == -1 ? nullptr : qPoints[constraints[cid][2]]));
        constraint->setProperty("ptB", qVariantFromValue(constraints[cid][3] == -1 ? nullptr : qPoints[constraints[cid][3]]));
        constraint->setProperty("entityA", qVariantFromValue(constraints[cid][4] == -1 ? nullptr : qLines[constraints[cid][4]]));
        constraint->setProperty("entityB", qVariantFromValue(constraints[cid][5] == -1 ? nullptr : qLines[constraints[cid][5]]));
    }
}

QString JSONSketch::exportJSONSketch(const QString url, QObject* sketch, int mode) {
    QList<QString> modeStrings = {"", "mm"};
    if (mode < 0 || mode >= modeStrings.length()){
        return "invalid mode";
    }

    QJsonObject object;
    switch (mode){
    case 0:
        if(!writeAll(object, sketch)){
            return "Empty sketch";
        }
        break;
    case 1:
        if(!writeMm(object, sketch)){
            return "No origin";
        }
        break;
    default:
        return "unknown mode";
        break;
    }

    QFile file(url + modeStrings[mode]);

    if (!file.open(QIODevice::WriteOnly)) {
        return "cannot open the JSON file";
    }

    QJsonDocument doc(object);
    file.write(doc.toJson());
    file.close();

    return "Sketch exported";
}

bool JSONSketch::writeAll(QJsonObject &json, QObject* sketch)
{
    nextPointId = 0;
    nextLineId = 0;
    nextConstraintId = 0;

    QJsonObject qPoints;
    QJsonArray qPid;

    QJsonObject qLines;
    QJsonArray qLid;

    QJsonObject qConstraints;
    QJsonArray qCid;

    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Point")
                && child->property("existing").toBool()) {
            int id = addPoint(child->property("x").toInt(), child->property("y").toInt());
            child->setProperty("id", id);
            qPid.append(id);
            QJsonArray currPoint;
            currPoint.append(child->property("x").toReal()/
                             sketch->property("scaleFactor").toReal());
            currPoint.append(child->property("y").toReal()/
                             sketch->property("scaleFactor").toReal());
            qPoints.insert(QString::number(id), currPoint);
        }
    }
    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Line")
                && child->property("existing").toBool()) {
            QObject* p1 = qvariant_cast<QObject*>(child->property("p1"));
            QObject* p2 = qvariant_cast<QObject*>(child->property("p2"));
            if(p1 != nullptr && p2 !=nullptr){
                int id = addLine(p1->property("id").toInt(), p2->property("id").toInt());
                child->setProperty("id", id);
                qLid.append(id);
                QJsonArray currLine;
                currLine.append(p1->property("id").toInt());
                currLine.append(p2->property("id").toInt());
                qLines.insert(QString::number(id), currLine);
            } else {
                child->setProperty("existing", false);
            }
        }
    }

    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Constraint")
                && child->property("existing").toBool()) {
            int type = qvariant_cast<int>(child->property("type"));
            if (type == 9) { // if type == WHERE_DRAGGED
                continue;
            }
            double valA = qvariant_cast<double>(child->property("valA"));
            QObject* ptA = qvariant_cast<QObject*>(child->property("ptA"));
            QObject* ptB = qvariant_cast<QObject*>(child->property("ptB"));
            QObject* entityA = qvariant_cast<QObject*>(child->property("entityA"));
            QObject* entityB = qvariant_cast<QObject*>(child->property("entityB"));

            int id = addConstraint(type, valA,
                                   ptA == nullptr ? -1 : ptA->property("id").toInt(),
                                   ptB == nullptr ? -1 : ptB->property("id").toInt(),
                                   entityA == nullptr ? -1 : entityA->property("id").toInt(),
                                   entityB == nullptr ? -1 : entityB->property("id").toInt());
            child->setProperty("id", id);
            qCid.append(id);
            QJsonArray currConst;
            currConst.append(type);
            currConst.append(valA);
            ptA == nullptr ? currConst.append(-1) : currConst.append(ptA->property("id").toInt());
            ptB == nullptr ? currConst.append(-1) : currConst.append(ptB->property("id").toInt());
            entityA == nullptr ? currConst.append(-1) : currConst.append(entityA->property("id").toInt());
            entityB == nullptr ? currConst.append(-1) : currConst.append(entityB->property("id").toInt());
            qConstraints.insert(QString::number(id), currConst);
        }
    }

    json["sketch_width"] = sketch->property("width").toInt();
    json["sketch_height"] = sketch->property("height").toInt();

    json["scaleFactor"] = sketch->property("scaleFactor").toReal();
    QObject* origin = qvariant_cast<QObject*>(sketch->property("origin"));
    if (origin != nullptr){
        json["origin_id"] = origin->property("id").toInt();
    } else {
        json["origin_id"] = -1;
    }


    json["pid"] = qPid;
    json["points"] = qPoints;

    json["lid"] = qLid;
    json["lines"] = qLines;

    json["cid"] = qCid;
    json["constraints"] = qConstraints;

    json["background_picture_url"] = sketch->property("background_picture_url").toString();

    return qPoints.size() > 0;
}

bool JSONSketch::writeMm(QJsonObject &json, QObject* sketch)
{
    if (qvariant_cast<QObject*>(sketch->property("origin")) == nullptr){
        return false;
    }

    nextPointId = 0;
    nextLineId = 0;

    QJsonObject qPoints;
    QJsonArray qPid;

    QJsonObject qLines;

    QObject* origin = qvariant_cast<QObject*>(sketch->property("origin"));

    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Point")
                && child->property("existing").toBool()) {
            int id = addPoint(child->property("x").toInt(), child->property("y").toInt());
            child->setProperty("id", id);
            qPid.append(id);
            QJsonArray currPoint;
            currPoint.append((child->property("x").toInt() - origin->property("x").toInt()) /
                             sketch->property("scaleFactor").toReal());
            currPoint.append((child->property("y").toInt() - origin->property("y").toInt()) /
                             sketch->property("scaleFactor").toReal());
            qPoints.insert(QString::number(id), currPoint);
        }
    }

    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Line")
                && child->property("existing").toBool()) {
            QObject* p1 = qvariant_cast<QObject*>(child->property("p1"));
            QObject* p2 = qvariant_cast<QObject*>(child->property("p2"));
            if(p1 != nullptr && p2 !=nullptr){
                int id = addLine(p1->property("id").toInt(), p2->property("id").toInt());
                child->setProperty("id", id);
                QJsonArray currLine;
                currLine.append(p1->property("id").toInt());
                currLine.append(p2->property("id").toInt());
                qLines.insert(QString::number(id), currLine);
            } else {
                child->setProperty("existing", false);
            }
        }
    }

    json["origin_id"] = origin->property("id").toInt();

    json["pid"] = qPid;
    json["points"] = qPoints;

    json["lines"] = qLines;

    return qPoints.length() > 0;
}

int JSONSketch::addPoint(int x, int y) {
    int id = incrementPointsId();
    points.insert(id, QVector2D(x, y));
    return id;
}

int JSONSketch::addLine(int p1, int p2) {
    int id = incrementLinesId();
    lines.insert(id, QVector2D(p1, p2));
    return id;
}

int JSONSketch::addConstraint(int type, int valA, int ptA, int ptB, int entityA, int entityB) {
    int id = incrementConstraintId();
    constraints.insert(id, QList<int>({type, valA, ptA, ptB, entityA, entityB}));
    return id;
}

int JSONSketch::incrementPointsId() {
    return nextPointId++;
}

int JSONSketch::incrementLinesId() {
    return nextLineId++;
}

int JSONSketch::incrementConstraintId() {
    return nextConstraintId++;
}
