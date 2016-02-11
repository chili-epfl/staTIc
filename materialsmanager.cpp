#include "materialsmanager.h"
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>
MaterialsManager::MaterialsManager(QObject *parent):
    QAbstractListModel(parent)
{
    /*Creating default material*/
    Material default_material;
    default_material.set("default",QUrl("qrc:/images/Images/woodbackground.png"),
                 20,0.50e-9,20000,1250);
    default_material.uniqueID="default";

    m_materials[default_material.uniqueID]=default_material;

    QString materialsDir=":/maparentterials/statics/Materials/";
    QDirIterator it(materialsDir);
    while(it.hasNext()){
        it.next();
        if(it.fileInfo().suffix()=="material"){
            QFile inputFile(it.filePath());
            if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)){
                qDebug()<<"Failed to open material file"<<it.filePath();
                continue;
            }
            QTextStream inputStream(&inputFile);
            QString line;
            QStringList sub_parts,property;
            Material material;
            material.set(QString(),QUrl(),0,0,0,0);
            do{
                line=inputStream.readLine();
                sub_parts=line.split(";",QString::SplitBehavior::SkipEmptyParts);
                Q_FOREACH(QString part,sub_parts){
                    property=part.split(":",QString::SplitBehavior::SkipEmptyParts);
                    if(property.size()!=2){
                        qDebug()<<"Wrong property format: "<<part;
                        continue;
                    }
                    if(property[0]=="Name"){
                        material.name=property[1];
                    }
                    else if(property[0]=="UniqueID"){
                        material.uniqueID=property[1];
                    }
                    else if(property[0]=="Price") {
                        bool ok;
                        material.price=property[1].toFloat(&ok);
                        if(!ok) qDebug()<<"Convertion error:"<<part;
                    }
                    else if(property[0]=="Density") {
                        bool ok;
                        material.density=property[1].toFloat(&ok);
                        if(!ok) qDebug()<<"Convertion error:"<<part;
                    }
                    else if(property[0]=="Young") {
                        bool ok;
                        material.young=property[1].toFloat(&ok);
                        if(!ok) qDebug()<<"Convertion error:"<<part;
                    }
                    else if(property[0]=="G") {
                        bool ok;
                        material.g=property[1].toFloat(&ok);
                        if(!ok) qDebug()<<"Convertion error:"<<part;
                    }
                    else if(property[0]=="TextureImage") {
                        material.texture_img="qrc"+materialsDir+property[1];
                    }
                    else{qDebug()<<"Unknown property:"<<part;}
                }
            }while(!line.isNull());
            if(material.uniqueID.isEmpty() || material.name.isEmpty() || material.density==0 ||
                    material.g==0 || material.young==0 || m_materials.contains(material.uniqueID)){
                qDebug()<<"Invalid material or duplicated Id:"<< it.filePath();
            }
            else{
                m_materials[material.uniqueID]=material;
            }
        }
    }
    m_materialsIndex=m_materials.keys();
}

int MaterialsManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_materials.size();

}

QVariant MaterialsManager::data(const QModelIndex &index, int role) const
{
    if((index.row()+index.column())>= m_materials.size()) return QVariant();
    switch (role) {
    case Qt::DecorationRole:
        return m_materials[m_materialsIndex[(index.row()+index.column())]].texture_img;
        break;
    case Qt::DisplayRole:
        return m_materials[m_materialsIndex[(index.row()+index.column())]].name;
        break;
    default:
        return QVariant();
        break;
    }

}

QVariant MaterialsManager::get(int index, QString info) const
{
    if(index<0 || index>=m_materials.size()) return QVariant();
    if(info.compare("G",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].g;
    }
    else if(info.compare("Young",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].young;
    }
    else if(info.compare("Price",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].price;
    }
    else if(info.compare("Density",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].density;
    }
    else if(info.compare("UniqueID",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].uniqueID;
    }
    return QVariant();
}

QVariant MaterialsManager::get(QString uniqueID, QString info) const
{
    if(!m_materials.contains(uniqueID)) return QVariant();
    if(info.compare("G",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].g;
    }
    else if(info.compare("Young",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].young;
    }
    else if(info.compare("Price",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].price;
    }
    else if(info.compare("Density",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].density;
    }
    else if(info.compare("Index",Qt::CaseInsensitive)==0){
        return m_materialsIndex.indexOf(uniqueID);
    }
    return QVariant();

}




