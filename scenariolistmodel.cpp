#include "scenariolistmodel.h"
#include "static_global.h"


ScenarioListModel::ScenarioListModel(QObject * parent ):
    QAbstractListModel(parent)
{
    setSource();
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
    if(info.compare("ScenarioStructure",Qt::CaseInsensitive)==0){
        return m_scenariosStructures[m_scenariosNames[index]];
    }
    if(info.compare("Scenario3DAsset",Qt::CaseInsensitive)==0){
        return m_scenarios3DAsset[m_scenariosNames[index]];
    }
    if(info.compare("ScenarioTagConfiguration",Qt::CaseInsensitive)==0){
        return m_scenariosTagConfiguration[m_scenariosNames[index]];
    }
    return QVariant();

}

void ScenarioListModel::reloadResources()
{
    setSource();
}


void ScenarioListModel::setSource(){
    m_scenariosNames.clear();
    m_scenariosStructures.clear();
    m_scenariosThumbs.clear();
    m_scenarios3DAsset.clear();
    m_scenariosTagConfiguration.clear();
    m_source=scenariosPath;
    QDirIterator it(m_source);
    QString name;
    QUrl structure,thumb,asset,tagConfiguration;
    while(it.hasNext()){
        it.next();
        name.clear();
        structure.clear();
        thumb.clear();
        asset.clear();
        tagConfiguration.clear();
        if(it.fileInfo().isDir()&& it.fileName()!="." && it.fileName()!=".."){
            name=it.fileInfo().baseName();
            QDirIterator it2(it.fileInfo().canonicalFilePath());
            bool foundObj=false;
            while(it2.hasNext()){
                it2.next();
                if(it2.fileInfo().suffix()=="structure"){
                    structure=QUrl::fromLocalFile(m_source+"/"+it.fileInfo().fileName()+"/"+it2.fileInfo().fileName());
                }
                else if(it2.fileInfo().suffix()=="png")
                    thumb=QUrl::fromLocalFile(m_source+"/"+it.fileInfo().fileName()+"/"+it2.fileInfo().fileName());
                else if(it2.fileInfo().suffix()=="dae" && !foundObj)
                    asset=QUrl::fromLocalFile(m_source+"/"+it.fileInfo().fileName()+"/"+it2.fileInfo().fileName());
                else if( it2.fileInfo().suffix()=="obj"){
                    asset=QUrl::fromLocalFile(m_source+"/"+it.fileInfo().fileName()+"/"+it2.fileInfo().fileName());
                    foundObj=true;
                }
                else if(it2.fileInfo().suffix()=="yml")
                    tagConfiguration=QUrl::fromLocalFile(m_source+"/"+it.fileInfo().fileName()+"/"+it2.fileInfo().fileName());
            }
        }
        if(!name.isEmpty() && !structure.isEmpty() && !thumb.isEmpty() && !asset.isEmpty()){
            m_scenariosNames.append(name);
            m_scenariosStructures[name]=structure;
            m_scenariosThumbs[name]=thumb;
            m_scenarios3DAsset[name]=asset;
            m_scenariosTagConfiguration[name]=tagConfiguration;
        }

    }

    emit dataChanged(this->createIndex(0,0),this->createIndex(m_scenariosNames.count(),0));

}
