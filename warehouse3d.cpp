#include "warehouse3d.h"
#include <QDirIterator>
#include <QDebug>

QSet<QString> Warehouse3D::m_3Dasset_extensions=QSet<QString>()<< "obj"<<"dae"<<"3ds";

QSet<QString> Warehouse3D::m_image_extensions=QSet<QString>()<< "jpg"<<"jpeg"<<"png";

Warehouse3D::Warehouse3D(QObject *parent): QAbstractListModel(parent)
{
    /*3D resources are stored in as foleders*/
    QString warehouseDir="Warehouse3D/";
    QDirIterator it(warehouseDir);
    while(it.hasNext()){
        it.next();
        Object obj;
        if(it.fileInfo().isDir()){
            obj.name=it.fileName();
            QDirIterator it2(it.fileInfo().canonicalFilePath());
            while(it2.hasNext()){
                it2.next();
                if(m_image_extensions.contains(it2.fileInfo().suffix())){
                    obj.decoration_img=QUrl::fromLocalFile(it2.fileInfo().canonicalFilePath());
                }
                else if(m_3Dasset_extensions.contains(it2.fileInfo().suffix())){
                    obj.main_asset_file_path=it2.fileInfo().canonicalFilePath();
                }
            }
            if(!obj.name.isEmpty() && !obj.decoration_img.isEmpty() && !obj.main_asset_file_path.isEmpty())
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
    if(info.compare("Main-Asset",Qt::CaseInsensitive)==0){
        return m_objectsIndexes[index].main_asset_file_path;
    }
    return QVariant();
}


