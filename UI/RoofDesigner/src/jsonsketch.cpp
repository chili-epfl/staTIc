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
#include <QMatrix4x4>

#include "jsonsketch.h"
#include "constraints.h"

bool pointSortingByX(const QObject* o1, const  QObject* o2)
{
    return o1->property("x").toDouble() < o2->property("x").toDouble();
}

JSONSketch::JSONSketch(QObject *parent): QObject(parent) {}

QString JSONSketch::loadSketch(const QString path,const QString name, QObject* sketch)
{
    QFile file(path+name);

    if (!file.open(QIODevice::ReadOnly)) {
        return "cannot open the JSON file";
    }

    QByteArray data = file.readAll();

    QJsonParseError error;
    QJsonDocument doc(QJsonDocument::fromJson(data, &error));
    if(!doc.isObject()){
        return "cannot open the JSON object";
    }
    return read(doc.object(), sketch,path);
}

QString JSONSketch::read(const QJsonObject json, QObject* sketch,QString jsonFilePath)
{
    bool is2D=true;
    if(!json["background_picture"].isString())
        return "Cannot read picture";
    if(json["background_picture"].toString().contains('/'))
        return "Picture seems a path";
    if(!json["background_picture"].toString().isEmpty())
        sketch->setProperty("background_picture_url", QUrl::fromLocalFile(jsonFilePath+ json["background_picture"].toString()));

    if(!json["points"].isObject())
        return "Cannot read points";
    QJsonObject qPoints = json["points"].toObject();
    if(!json["lines"].isObject())
        return "Cannot read lines";
    QJsonObject qLines = json["lines"].toObject();
    if(!json["constraints"].isArray())
        return "Cannot read constraints";
    QJsonArray qConstraints = json["constraints"].toArray();

    qreal max_x=-HUGE_VAL,min_x=HUGE_VAL,max_y=-HUGE_VAL,min_y=HUGE_VAL,centre_x=0,centre_y=0;
    for (const QString &key : qPoints.keys()){
        if(!qPoints[key].isObject())
            return "Cannot read point";
        auto currPoint=qPoints[key].toObject();
        if(!currPoint["x"].isDouble() || !currPoint["y"].isDouble() || !currPoint["z"].isDouble())
            return "Cannot read coordinates";
        if(!qFuzzyCompare(currPoint["z"].toDouble(),0)){
            is2D=false;
        }
        max_x=qMax(max_x,currPoint["x"].toDouble() );
        max_y=qMax(max_y,-currPoint["y"].toDouble());
        min_x=qMin(min_x,currPoint["x"].toDouble());
        min_y=qMin(min_y,-currPoint["y"].toDouble());
        centre_x+=currPoint["x"].toDouble();
        centre_y-=currPoint["y"].toDouble();
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

    //Actual Generation
    sketch->setProperty("scaleFactor", scale); // px/mm

    //Points
    QQmlComponent point_component(qmlEngine(sketch),sketch);
    point_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Point.qml"));

    QHash<QString, QQuickItem*> pointsQML;

    for (const QString &key : qPoints.keys()){
        auto currPoint=qPoints[key].toObject();
        QQmlContext* point_context = new QQmlContext(qmlContext(sketch),sketch);
        QQuickItem* point = qobject_cast<QQuickItem*>(point_component.create(point_context));

        point_context->setContextObject(point);
        point->setProperty("x", (currPoint["x"].toDouble()-centre_x)*scale+sketch->property("width").toInt()/2);
        point->setProperty("y", (-currPoint["y"].toDouble()-centre_y)*scale+sketch->property("height").toInt()/2);

        point->setParent(sketch);
        point->setParentItem(qobject_cast<QQuickItem*>(sketch));
        point_component.completeCreate();

        auto structureData=point->findChild<QObject*>(QString("structureData"), Qt::FindDirectChildrenOnly);
        if(!currPoint["name"].isString() || !currPoint["reac_x"].isBool() ||!currPoint["reac_y"].isBool() ||
                !currPoint["reac_z"].isBool() ||!currPoint["reac_xx"].isBool() ||!currPoint["reac_yy"].isBool() ||
                !currPoint["reac_zz"].isBool() )
            return "Cannot read porint properties";
        structureData->setProperty("visual_name",currPoint["name"].toString());
        structureData->setProperty("reac_x",currPoint["reac_x"].toBool());
        structureData->setProperty("reac_y",currPoint["reac_y"].toBool());
        structureData->setProperty("reac_z",currPoint["reac_z"].toBool());
        structureData->setProperty("reac_xx",currPoint["reac_xx"].toBool());
        structureData->setProperty("reac_yy",currPoint["reac_yy"].toBool());
        structureData->setProperty("reac_zz",currPoint["reac_zz"].toBool());
        pointsQML[key]=point;
    }
    //Line
    QQmlComponent line_component(qmlEngine(sketch),sketch);
    line_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Line.qml"));
    QHash<QString,QQuickItem*> linesQML;
    for (const QString &key : qLines.keys()){
        if(!qLines[key].isObject())
            return "Cannot read line";
        auto currLine=qLines[key].toObject();

        QQmlContext* line_context = new QQmlContext(qmlContext(sketch),sketch);
        QQuickItem* line = qobject_cast<QQuickItem*>(line_component.beginCreate(line_context));

        line_context->setContextObject(line);
        if(!currLine["p1"].isString() || !currLine["p2"].isString())
            return "Cannot read extremes";
        line->setProperty("p1", qVariantFromValue(pointsQML[currLine["p1"].toString()]));
        line->setProperty("p2", qVariantFromValue(pointsQML[currLine["p2"].toString()]));

        line->setParent(sketch);
        line->setParentItem(qobject_cast<QQuickItem*>(sketch));
        line_component.completeCreate();

        if(!currLine["name"].isString() || !currLine["materialID"].isString() ||
                !currLine["width"].isDouble()||!currLine["height"].isDouble())
            return "Cannot read line properties";
        auto structureData=line->findChild<QObject*>(QString("structureData"), Qt::FindDirectChildrenOnly);
        structureData->setProperty("visual_name",currLine["name"].toString());
        structureData->setProperty("width",currLine["width"].toDouble());
        structureData->setProperty("height",currLine["height"].toDouble());
        structureData->setProperty("materialID",currLine["materialID"].toString());
        linesQML[key]=line;

    }

    //Constraints
    QQmlComponent const_component(qmlEngine(sketch),sketch);
    const_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Constraint.qml"));
    for(int c(0); c < qConstraints.size(); c++){
        QQmlContext* const_context = new QQmlContext(qmlContext(sketch),sketch);
        QQuickItem* constraint = qobject_cast<QQuickItem*>(const_component.beginCreate(const_context));

        const_context->setContextObject(constraint);

        constraint->setParent(sketch);
        constraint->setParentItem(qobject_cast<QQuickItem*>(sketch));
        const_component.completeCreate();

        if(!qConstraints[c].isObject())
            return "Cannot read constraint";

        auto jsonConstrain=qConstraints[c].toObject();
        if(!jsonConstrain["type"].isDouble() ||!jsonConstrain["valA"].isDouble() ||
                !jsonConstrain["ptA"].isString() ||!jsonConstrain["ptB"].isString() ||
                !jsonConstrain["entityA"].isString() ||!jsonConstrain["entityB"].isString() )
            return "Cannot  constraint properties";

        constraint->setProperty("type", jsonConstrain["type"].toInt());
        constraint->setProperty("valA", jsonConstrain["valA"].toDouble());
        constraint->setProperty("ptA", jsonConstrain["ptA"].toString() =="-1" ? qVariantFromValue(nullptr) :
                                                                                qVariantFromValue(pointsQML[jsonConstrain["ptA"].toString()]));
        constraint->setProperty("ptB", jsonConstrain["ptB"].toString() =="-1" ? qVariantFromValue(nullptr) :
                                                                                qVariantFromValue(pointsQML[jsonConstrain["ptB"].toString()]));
        constraint->setProperty("entityA", jsonConstrain["entityA"].toString() =="-1" ? qVariantFromValue(nullptr) :
                                                                                        qVariantFromValue(linesQML[jsonConstrain["entityA"].toString()]));
        constraint->setProperty("entityB", jsonConstrain["entityB"].toString() =="-1" ? qVariantFromValue(nullptr) :
                                                                                        qVariantFromValue(linesQML[jsonConstrain["entityB"].toString()]));

    }
    if(!json["origin_id"].isString())
        return "Cannot read origin";

    if (json["origin_id"].toString() != "-1"){
        sketch->setProperty("origin", qVariantFromValue(pointsQML[json["origin_id"].toString()]));
    }
    auto structureData=sketch->findChild<QObject*>(QString("structureData"), Qt::FindDirectChildrenOnly);
    if(structureData){
        if(!json["scaleFactorPhysical"].isDouble())
            return "Cannot read scaleFactorPhysical";
        structureData->setProperty("scaleFactorPhysical",json["scaleFactorPhysical"].toDouble());
        if(!json["poseOffsetTranslation"].isArray())
            return "Cannot read poseOffsetTranslation";
        auto poseOffsetTranslationJSON =json["poseOffsetTranslation"].toArray();
        if(poseOffsetTranslationJSON.size()!=3)
            return "Cannot read poseOffsetTranslation";
        for(int i=0;i<3;i++)
            if(!poseOffsetTranslationJSON[i].isDouble())
                return "Cannot read poseOffsetTranslation";

        structureData->setProperty("poseOffsetTranslation",QVector3D(poseOffsetTranslationJSON[0].toDouble(),
                                   poseOffsetTranslationJSON[1].toDouble(),
                                    poseOffsetTranslationJSON[2].toDouble()));

        if(!json["poseOffsetEulerAngles"].isArray())
            return "Cannot read poseOffsetEulerAngles";
        auto poseOffsetEulerAnglesJSON =json["poseOffsetEulerAngles"].toArray();
        if(poseOffsetEulerAnglesJSON.size()!=3)
            return "Cannot read poseOffsetEulerAngles";
        for(int i=0;i<3;i++)
            if(!poseOffsetEulerAnglesJSON[i].isDouble())
                return "Cannot read poseOffsetEulerAngles";

        structureData->setProperty("poseOffsetEulerAngles",QVector3D(poseOffsetEulerAnglesJSON[0].toDouble(),
                                   poseOffsetEulerAnglesJSON[1].toDouble(),
                                    poseOffsetEulerAnglesJSON[2].toDouble()));
    }
    if(!is2D)
        return "3D Sketch";
    return "Sketch read";
}



//void JSONSketch::generateSketch(QObject* sketch) {
//    QQmlComponent point_component(qmlEngine(sketch),sketch);
//    point_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Point.qml"));

//    QMap<int, QObject*> qPoints;

//    foreach (int pid, points.keys()) {
//        QQmlContext* point_context = new QQmlContext(qmlContext(sketch),sketch);
//        QQuickItem* point = qobject_cast<QQuickItem*>(point_component.create(point_context));

//        point_context->setContextObject(point);
//        point->setProperty("x", points[pid].x());
//        point->setProperty("y", points[pid].y());

//        point->setParent(sketch);
//        point->setParentItem(qobject_cast<QQuickItem*>(sketch));
//        point_component.completeCreate();

//        qPoints.insert(pid, point);
//    }

//    if (originId >= 0){
//        qPoints[originId]->setProperty("color", "green");
//        sketch->setProperty("origin", qVariantFromValue(qPoints[originId]));
//    }

//    QQmlComponent line_component(qmlEngine(sketch),sketch);
//    line_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Line.qml"));

//    QMap<int, QObject*> qLines;

//    foreach (int lid, lines.keys()) {
//        QQmlContext* line_context = new QQmlContext(qmlContext(sketch),sketch);
//        QQuickItem* line = qobject_cast<QQuickItem*>(line_component.beginCreate(line_context));

//        line_context->setContextObject(line);

//        line->setProperty("p1", qVariantFromValue(qPoints[lines[lid].x()]));
//        line->setProperty("p2", qVariantFromValue(qPoints[lines[lid].y()]));

//        line->setParent(sketch);
//        line->setParentItem(qobject_cast<QQuickItem*>(sketch));
//        line_component.completeCreate();

//        qLines.insert(lid, line);
//    }

//    QQmlComponent const_component(qmlEngine(sketch),sketch);
//    const_component.loadUrl(QUrl("qrc:/ui/UI/RoofDesigner/Constraint.qml"));

//    foreach (int cid, constraints.keys()) {
//        QQmlContext* const_context = new QQmlContext(qmlContext(sketch),sketch);
//        QQuickItem* constraint = qobject_cast<QQuickItem*>(const_component.beginCreate(const_context));

//        const_context->setContextObject(constraint);

//        constraint->setParent(sketch);
//        constraint->setParentItem(qobject_cast<QQuickItem*>(sketch));
//        const_component.completeCreate();

//        constraint->setProperty("type", constraints[cid][0]);
//        constraint->setProperty("valA", constraints[cid][1]);
//        constraint->setProperty("ptA", qVariantFromValue(constraints[cid][2] == -1 ? nullptr : qPoints[constraints[cid][2]]));
//        constraint->setProperty("ptB", qVariantFromValue(constraints[cid][3] == -1 ? nullptr : qPoints[constraints[cid][3]]));
//        constraint->setProperty("entityA", qVariantFromValue(constraints[cid][4] == -1 ? nullptr : qLines[constraints[cid][4]]));
//        constraint->setProperty("entityB", qVariantFromValue(constraints[cid][5] == -1 ? nullptr : qLines[constraints[cid][5]]));
//    }
//}

QString JSONSketch::exportJSONSketch(const QString path, const QString name,  QObject* sketch) {

    QJsonObject object;
    if(!writeJSON(object, sketch)){
        return "Empty sketch";
    }

    QFile file(path+name+".json");

    if (!file.open(QIODevice::WriteOnly)) {
        return "cannot open the JSON file";
    }

    QString background_picture=object["background_picture"].toString();
    if(!background_picture.isEmpty()){
        QString image_path=QUrl(background_picture).path();
        QStringList name_parts=image_path.split('.');
        QString new_image_path=path+name+"."+name_parts[name_parts.length()-1];
        if(QUrl::fromLocalFile(image_path)!=QUrl::fromLocalFile(new_image_path)){
            QFile::remove(new_image_path);
            QFile::copy(image_path,new_image_path);
        }
        object["background_picture"]=name+"."+name_parts[name_parts.length()-1];
    }
    QJsonDocument doc(object);
    file.write(doc.toJson());
    file.close();

    return "Sketch exported";
}

QString JSONSketch::deleteSketchFile(const QString path, const QString name)
{
    QFile file(path+name);

    if (!file.open(QIODevice::ReadOnly)) {
        return "cannot open the JSON file";
    }

    QByteArray data = file.readAll();

    QJsonParseError error;
    QJsonDocument doc(QJsonDocument::fromJson(data, &error));
    if(doc.isObject()){
        auto jsonroot=doc.object();
        if(jsonroot["background_picture"].isString()){
            auto img_name=jsonroot["background_picture"].toString();
            if(!img_name.isEmpty() && !img_name.contains('/')){
                QFile::remove(path+img_name);
            }
        }
    }
    file.close();
    QFile::remove(path+name);
    return QString("Done");
}

bool JSONSketch::writeJSON(QJsonObject &json, QObject* sketch)
{
    int nextPointId = 0;
    int nextLineId = 0;

    QJsonObject qPoints;
    QJsonObject qLines;
    QJsonArray qConstraints;

    QHash<QObject*, int> mapIds;
    QObject* origin = qvariant_cast<QObject*>(sketch->property("origin"));
    if (origin == nullptr){
        qreal origin_candidate_x=HUGE_VAL;
        qreal origin_candidate_y=-HUGE_VAL;
        foreach (QObject* child, sketch->children()) {
            if (!QString::compare(child->property("class_type").toString(), "Point")
                    && child->property("existing").toBool()) {
                auto y=child->property("y").toReal();
                auto x=child->property("x").toReal();
                //Lowest point has max y
                if(origin_candidate_y<y){
                    origin_candidate_y=y;
                    origin_candidate_x=x;
                    origin=child;
                }
                else if(origin_candidate_y==y){
                    if(origin_candidate_x>x){
                        origin_candidate_x=x;
                        origin=child;
                    }
                }
            }
        }
        if(origin!=nullptr){
            sketch->setProperty("origin", qVariantFromValue(origin));
            QMetaObject::invokeMethod(sketch,"store_state", Qt::DirectConnection,Q_ARG(QVariant,0));
        }
    }
    //At the end of this block origin could be null if there were no points

    qreal sketch_height=sketch->property("height").toReal();
    qreal origin_coordinate_x=0.0,origin_coordinate_y=0.0;

    if(origin!=nullptr){
        origin_coordinate_x=origin->property("x").toReal();
        origin_coordinate_y=sketch_height-origin->property("y").toReal();
    }

    //Generate names (very inefficient)
    QVector<QObject*> sorted_points;
    QHash<QObject*,QString> pointsNames;
    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Point")
                && child->property("existing").toBool()) {
            sorted_points.append(child);
        }
    }
    qSort(sorted_points.begin(),sorted_points.end(),pointSortingByX);
    for(int i=0;i<sorted_points.length();i++) {
        auto structureData=sorted_points[i]->findChild<QObject*>(QString("structureData"), Qt::FindDirectChildrenOnly);
        auto name=QString(QChar(65+(i%26))).repeated(1+(int)(i/26));
        structureData->setProperty("visual_name",name);
        pointsNames[sorted_points[i]]=name;
    }

    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Point")
                && child->property("existing").toBool()) {
            int id = nextPointId++;
            mapIds[child]=id;
            QJsonObject currPoint;
            currPoint.insert("x",(child->property("x").toReal()-origin_coordinate_x)/
                             sketch->property("scaleFactor").toReal());
            currPoint.insert("y",(sketch_height-child->property("y").toReal()-origin_coordinate_y)/
                             sketch->property("scaleFactor").toReal());
            currPoint.insert("z",0);
            auto structureData=child->findChild<QObject*>(QString("structureData"), Qt::FindDirectChildrenOnly);
            currPoint.insert("name", structureData->property("visual_name").toString());
            currPoint.insert("reac_x", structureData->property("reac_x").toBool());
            currPoint.insert("reac_y", structureData->property("reac_y").toBool());
            currPoint.insert("reac_z", structureData->property("reac_z").toBool());
            currPoint.insert("reac_xx", structureData->property("reac_xx").toBool());
            currPoint.insert("reac_yy", structureData->property("reac_yy").toBool());
            currPoint.insert("reac_zz", structureData->property("reac_zz").toBool());

            qPoints.insert(QString::number(id), currPoint);
        }
    }
    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Line")
                && child->property("existing").toBool()) {
            QObject* p1 = qvariant_cast<QObject*>(child->property("p1"));
            QObject* p2 = qvariant_cast<QObject*>(child->property("p2"));
            if(p1 != nullptr && p2 !=nullptr){
                int id = nextLineId++;
                mapIds[child]=id;
                QJsonObject currLine;
                currLine.insert("p1",QString::number(mapIds[p1]));
                currLine.insert("p2",QString::number(mapIds[p2]));
                auto structureData=child->findChild<QObject*>(QString("structureData"), Qt::FindDirectChildrenOnly);
                //currLine.insert("name", structureData->property("visual_name").toString());
                currLine.insert("name",pointsNames[p1]+"-"+pointsNames[p2]);
                currLine.insert("width", structureData->property("width").toReal());
                currLine.insert("height", structureData->property("height").toReal());
                currLine.insert("materialID", structureData->property("materialID").toString());
                qLines.insert(QString::number(id), currLine);
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

            QJsonObject currConst;
            currConst.insert("type",type);
            currConst.insert("valA",valA);
            ptA == nullptr ? currConst.insert("ptA","-1") : currConst.insert("ptA",QString::number(mapIds[ptA]));
            ptB == nullptr ? currConst.insert("ptB","-1") : currConst.insert("ptB",QString::number(mapIds[ptB]));
            entityA == nullptr ? currConst.insert("entityA","-1") : currConst.insert("entityA",QString::number(mapIds[entityA]));
            entityB == nullptr ? currConst.insert("entityB","-1") : currConst.insert("entityB",QString::number(mapIds[entityB]));
            qConstraints.append(currConst);
        }
    }

    if (origin != nullptr){
        json["origin_id"] = QString::number(mapIds[origin]);
    } else {
        json["origin_id"] = "-1";
    }

    auto structureData=sketch->findChild<QObject*>(QString("structureData"), Qt::FindDirectChildrenOnly);

    json["points"] = qPoints;
    json["lines"] = qLines;
    json["constraints"] = qConstraints;
    json["background_picture"] = sketch->property("background_picture_url").toString();

    json["scaleFactorPhysical"]=structureData->property("scaleFactorPhysical").toReal();

    auto poseOffsetTranslation=qvariant_cast<QVector3D>(structureData->property("poseOffsetTranslation"));

    QJsonArray poseOffsetTranslationJson;
    poseOffsetTranslationJson.append(poseOffsetTranslation.x());
    poseOffsetTranslationJson.append(poseOffsetTranslation.y());
    poseOffsetTranslationJson.append(poseOffsetTranslation.z());
    json["poseOffsetTranslation"]=poseOffsetTranslationJson;

    auto poseOffsetEulerAngles=qvariant_cast<QVector3D>(structureData->property("poseOffsetEulerAngles"));

    QJsonArray poseOffsetEulerAnglesJson;
    poseOffsetEulerAnglesJson.append(poseOffsetEulerAngles.x());
    poseOffsetEulerAnglesJson.append(poseOffsetEulerAngles.y());
    poseOffsetEulerAnglesJson.append(poseOffsetEulerAngles.z());
    json["poseOffsetEulerAngles"]=poseOffsetEulerAnglesJson;

    return qPoints.size() > 0;
}






//bool JSONSketch::writeJSON(QJsonObject &json, QObject* sketch)
//{
//    nextPointId = 0;
//    nextLineId = 0;
//    nextConstraintId = 0;

//    QJsonObject qPoints;
//    QJsonArray qPid;

//    QJsonObject qLines;
//    QJsonArray qLid;

//    QJsonObject qConstraints;
//    QJsonArray qCid;

//    foreach (QObject* child, sketch->children()) {
//        if (!QString::compare(child->property("class_type").toString(), "Point")
//                && child->property("existing").toBool()) {
//            int id = addPoint(child->property("x").toInt(), child->property("y").toInt());
//            child->setProperty("id", id);
//            qPid.append(id);
//            QJsonArray currPoint;
//            currPoint.append(child->property("x").toReal()/
//                             sketch->property("scaleFactor").toReal());
//            currPoint.append(child->property("y").toReal()/
//                             sketch->property("scaleFactor").toReal());
//            qPoints.insert(QString::number(id), currPoint);
//        }
//    }
//    foreach (QObject* child, sketch->children()) {
//        if (!QString::compare(child->property("class_type").toString(), "Line")
//                && child->property("existing").toBool()) {
//            QObject* p1 = qvariant_cast<QObject*>(child->property("p1"));
//            QObject* p2 = qvariant_cast<QObject*>(child->property("p2"));
//            if(p1 != nullptr && p2 !=nullptr){
//                int id = addLine(p1->property("id").toInt(), p2->property("id").toInt());
//                child->setProperty("id", id);
//                qLid.append(id);
//                QJsonArray currLine;
//                currLine.append(p1->property("id").toInt());
//                currLine.append(p2->property("id").toInt());
//                qLines.insert(QString::number(id), currLine);
//            } else {
//                child->setProperty("existing", false);
//            }
//        }
//    }

//    foreach (QObject* child, sketch->children()) {
//        if (!QString::compare(child->property("class_type").toString(), "Constraint")
//                && child->property("existing").toBool()) {
//            int type = qvariant_cast<int>(child->property("type"));
//            if (type == 9) { // if type == WHERE_DRAGGED
//                continue;
//            }
//            double valA = qvariant_cast<double>(child->property("valA"));
//            QObject* ptA = qvariant_cast<QObject*>(child->property("ptA"));
//            QObject* ptB = qvariant_cast<QObject*>(child->property("ptB"));
//            QObject* entityA = qvariant_cast<QObject*>(child->property("entityA"));
//            QObject* entityB = qvariant_cast<QObject*>(child->property("entityB"));

//            int id = addConstraint(type, valA,
//                                   ptA == nullptr ? -1 : ptA->property("id").toInt(),
//                                   ptB == nullptr ? -1 : ptB->property("id").toInt(),
//                                   entityA == nullptr ? -1 : entityA->property("id").toInt(),
//                                   entityB == nullptr ? -1 : entityB->property("id").toInt());
//            child->setProperty("id", id);
//            qCid.append(id);
//            QJsonArray currConst;
//            currConst.append(type);
//            currConst.append(valA);
//            ptA == nullptr ? currConst.append(-1) : currConst.append(ptA->property("id").toInt());
//            ptB == nullptr ? currConst.append(-1) : currConst.append(ptB->property("id").toInt());
//            entityA == nullptr ? currConst.append(-1) : currConst.append(entityA->property("id").toInt());
//            entityB == nullptr ? currConst.append(-1) : currConst.append(entityB->property("id").toInt());
//            qConstraints.insert(QString::number(id), currConst);
//        }
//    }

//    QObject* origin = qvariant_cast<QObject*>(sketch->property("origin"));
//    if (origin != nullptr){
//        json["origin_id"] = origin->property("id").toInt();
//    } else {
//        json["origin_id"] = -1;
//    }


//    json["pid"] = qPid;
//    json["points"] = qPoints;

//    json["lid"] = qLid;
//    json["lines"] = qLines;

//    json["cid"] = qCid;
//    json["constraints"] = qConstraints;

//    json["background_picture_url"] = sketch->property("background_picture_url").toString();

//    return qPoints.size() > 0;
//}

