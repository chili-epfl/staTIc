#include "scenariolistmodel.h"



ScenarioListModel::ScenarioListModel(QObject * parent ):
    QAbstractListModel(parent)
{

}

int ScenarioListModel::rowCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return m_scenariosNames.size();
}

QVariant ScenarioListModel::data(const QModelIndex &index, int role)const{
    switch (role) {
    case Qt::DecorationRole:
        return m_scenariosThumbs[m_scenariosNames[index.row()+index.column()]];
        break;
    case Qt::DisplayRole:
        return m_scenariosNames[index.row()+index.column()];
        break;
    default:
        return QVariant();
        break;
    }
}

QVariant ScenarioListModel::get(int index, QString info)const{
    if (index<0 || index>=m_scenariosNames.size()) return QVariant();
    if(info.compare("ScenarioQML",Qt::CaseInsensitive)==0){
        return m_scenariosQml[m_scenariosNames[index]];

    }
    return QVariant();

}


void ScenarioListModel::setSource(QString source){
    m_source= source;
    QDirIterator it(m_source);
    while(it.hasNext()){
        it.next();
        if(it.fileInfo().isDir() ){
            QString name=it.fileInfo().baseName();
            m_scenariosNames.append(name);
            QDirIterator it2(it.fileInfo().canonicalFilePath());
            while(it2.hasNext()){
                it2.next();
                if(it2.fileInfo().baseName()==name && it2.fileInfo().suffix()=="qml"){
                    m_scenariosQml[name]=QUrl("qrc"+m_source+"/"+name+"/"+it2.fileInfo().fileName());
                }
                else if(it2.fileInfo().suffix()=="png")
                    m_scenariosThumbs[name]=QUrl("qrc"+m_source+"/"+name+"/"+it2.fileInfo().fileName());
            }
        }
    }
}
