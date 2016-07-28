#include "warehouse3d.h"
#include <QDirIterator>
#include <QDebug>
#include <static_global.h>
#include <QImage>
#include <QVector2D>
QSet<QString> Warehouse3D::m_3Dasset_extensions=QSet<QString>()<< "obj"<<"dae"<<"3ds";
QSet<QString> Warehouse3D::m_image_extensions=QSet<QString>()<< "jpg"<<"jpeg"<<"png";

Warehouse3D::Warehouse3D(QObject *parent): QAbstractListModel(parent)
{
    /*Default models*/
    Object solar_pannel;
    solar_pannel.decoration_img="qrc:/resources/Resources/Assets3D/solar_pannel/preview.png";
    solar_pannel.properties["thumbnail"]=solar_pannel.decoration_img;
    solar_pannel.properties["main_asset_url"]="qrc:/resources/Resources/Assets3D/solar_pannel/Solar.obj";
    solar_pannel.properties["main_asset_diffuse_map_url"]="qrc:/resources/Resources/Assets3D/solar_pannel/Solar.png";
    solar_pannel.properties["weight"]=25;
    solar_pannel.properties["extent"]=QVector2D(-500,500);
    m_objectsIndexes.append(solar_pannel);
    /*.........*/
    QDirIterator it(assets3DPath);
    while(it.hasNext()){
        it.next();
        Object obj;
        if(it.fileInfo().isDir() && it.fileName()!="." && it.fileName()!=".." ){
            obj.name=it.fileName();
            QDirIterator it2(it.fileInfo().canonicalFilePath());
            while(it2.hasNext()){
                it2.next();
                if(it2.fileInfo().suffix()=="asset"){
                    QFile configFile(it2.filePath());
                    configFile.open(QFile::ReadOnly);
                    if(configFile.isOpen()){
                        QTextStream inputStream(&configFile);
                        QString line;
                        do{
                            line=inputStream.readLine();
                            if(!line.isEmpty()){
                                QStringList parts=line.split(":");
                                if(parts.size()>1){
                                    obj.properties[parts[0]]=parts[1];
                                }
                            }
                        }while(!line.isEmpty());
                        configFile.close();
                    }

                    if(obj.properties.contains("thumbnail")){
                        QString thumbFile=it.fileInfo().canonicalFilePath()+"/"+obj.properties["thumbnail"].toString();
                        QImage img(thumbFile);
                        if(!img.isNull()){
                            obj.decoration_img=QUrl::fromLocalFile(thumbFile);
                            obj.properties["thumbnail"]=QUrl::fromLocalFile(thumbFile);
                        }
                    }
                    if(obj.properties.contains("main_asset_url")){
                        obj.properties["main_asset_url"]=QUrl::fromLocalFile(it.fileInfo().canonicalFilePath()+
                                                                             "/"+obj.properties["main_asset_url"].toString());
                    }

                    if(obj.properties.contains("main_asset_diffuse_map_url")){
                        obj.properties["main_asset_diffuse_map_url"]=QUrl::fromLocalFile(it.fileInfo().canonicalFilePath()+
                                                                             "/"+obj.properties["main_asset_diffuse_map_url"].toString());
                    }
                    if(obj.properties.contains("weight")){
                        obj.properties["weight"]=obj.properties["weight"].toString().toFloat();
                    }
                    if(obj.properties.contains("extent")){
                        QString stringExtent=obj.properties["extent"].toString();
                        QStringList minmax=stringExtent.split(",");
                        obj.properties["extent"]=QVector2D(minmax[0].toFloat(),minmax[1].toFloat());
                    }
                    break;
                }
            }
            if(!obj.name.isEmpty() && !obj.decoration_img.isEmpty())
                m_objectsIndexes.append(obj);
        }
    }
}

int Warehouse3D::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_objectsIndexes.size();
}

QVariant Warehouse3D::data(const QModelIndex &index, int role) const
{
    if((index.row()+index.column())>= m_objectsIndexes.size()) return QVariant();
    switch (role) {
    case Qt::DecorationRole:
        return m_objectsIndexes[index.row()+index.column()].decoration_img;
        break;
    case Qt::DisplayRole:
        return m_objectsIndexes[index.row()+index.column()].name;
        break;
    default:
        return QVariant();
        break;
    }
}

QVariant Warehouse3D::get(int index, QString info) const
{
    if(index<0 || index>=m_objectsIndexes.size()) return QVariant();
    if(info.compare("Properties",Qt::CaseInsensitive)==0){
        return m_objectsIndexes[index].properties;
    }
    if(info.compare("main_asset_url",Qt::CaseInsensitive)==0){
        if(m_objectsIndexes[index].properties.contains("main_asset_url"))
            return m_objectsIndexes[index].properties["main_asset_url"];
        else
            return QVariant();
    }
    if(info.compare("main_asset_diffuse_map_url",Qt::CaseInsensitive)==0){
        if(m_objectsIndexes[index].properties.contains("main_asset_diffuse_map_url"))
            return m_objectsIndexes[index].properties["main_asset_diffuse_map_url"];
        else
            return QVariant();
    }
    if(info.compare("weight",Qt::CaseInsensitive)==0){
        if(m_objectsIndexes[index].properties.contains("weight"))
            return m_objectsIndexes[index].properties["weight"];
        else
            return QVariant();
    }
    if(info.compare("extent",Qt::CaseInsensitive)==0){
        if(m_objectsIndexes[index].properties.contains("extent"))
            return m_objectsIndexes[index].properties["extent"];
        else
            return QVariant();
    }
    return QVariant();
}


