#include "structureitemmodel.h"



StructureItemModel::StructureItemModel(QObject * parent ):
    QAbstractListModel(parent)
{

}

/*QModelIndex StructureItemModel::index(int row, int column, const QModelIndex &parent) const {
    if((row+column)<m_fileNames.size())
        return createIndex(row,column);
    else
        return QModelIndex();
}
*/
QVariant StructureItemModel::data(const QModelIndex &index, int role)const{
    //qDebug()<<index.row()<<"Col"<<index.column();
    switch (role) {
    case Qt::DecorationRole:
        return m_thumbs[m_fileNames[index.row()+index.column()]];
        break;
    case Qt::DisplayRole:
        return m_fileNames[index.row()+index.column()];
        break;
    default:
        return QVariant();
        break;
    }
}

void StructureItemModel::setSource(QString source){
    m_source= source;
    QDirIterator it(m_source);
        while(it.hasNext()){
        it.next();
        if(it.fileInfo().isFile() ){
            if(it.fileInfo().suffix()=="obj")
                m_fileNames.append(it.fileInfo().baseName());
            else if(it.fileInfo().suffix()=="png"){
                QString tmp=it.fileInfo().canonicalFilePath();
                m_thumbs[it.fileInfo().baseName()]=QUrl::fromLocalFile(it.fileInfo().canonicalFilePath());
            }
        }
    }
}
