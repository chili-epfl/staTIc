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

QVariant StructureItemModel::get(int index, QString info)const{
    //qDebug()<<index.row()<<"Col"<<index.column();
    if (index<0 || index>=m_fileNames.size()) return QVariant();
    if(info.compare("ModelFile",Qt::CaseInsensitive)==0){
        return m_modelfiles[m_fileNames[index]];

    }
    else if(info.compare("StaticsFile",Qt::CaseInsensitive)==0){
        return m_staticsfiles[m_fileNames[index]];

    }
    else if(info.compare("TagFile",Qt::CaseInsensitive)==0){
        return m_tagfiles[m_fileNames[index]];

    }
    else if(info.compare("Name",Qt::CaseInsensitive)==0){
        return m_fileNames[index];
    }


    return QVariant();

}


void StructureItemModel::setSource(QString source){
    m_source= source;
    QDirIterator it(m_source);
    while(it.hasNext()){
        it.next();
        if(it.fileInfo().isDir() ){
            QString name=it.fileInfo().baseName();
            m_fileNames.append(name);
            QDirIterator it2(it.fileInfo().canonicalFilePath());
            while(it2.hasNext()){
                it2.next();
                if(it2.fileInfo().suffix()=="dae")
                    m_modelfiles[name]=QUrl::fromLocalFile(it2.fileInfo().canonicalFilePath());
                else if(it2.fileInfo().suffix()=="png")
                    m_thumbs[name]=QUrl::fromLocalFile(it2.fileInfo().canonicalFilePath());
                else if(it2.fileInfo().suffix()=="lol")
                    m_staticsfiles[name]=QUrl::fromLocalFile(it2.fileInfo().canonicalFilePath());
                else if(it2.fileInfo().suffix()=="yml")
                    m_tagfiles[name]=QUrl::fromLocalFile(it2.fileInfo().canonicalFilePath());

            }
        }
    }
}
