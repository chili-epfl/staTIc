#include "materialsmanager.h"
#include "static_global.h"
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>
#include <QNetworkInterface>
#include <QDateTime>
#include <QImage>
MaterialsManager::MaterialsManager(QObject *parent):
    QAbstractListModel(parent)
{
    init();
}

void MaterialsManager::init()
{
    beginResetModel();
    m_materialsFilePath.clear();
    m_materials.clear();
    m_materialsIndex.clear();

    /*Creating default material*/
    Material default_material;
    default_material.set("Conifers(C14)",QUrl("qrc:/images/Images/woodbackground.png"),
                         20,0.350,7000,440,8,16,14,0.4,2,3);
    default_material.uniqueID="default";
    m_materials[default_material.uniqueID]=default_material;
    m_materialsFilePath.append("");
    m_materialsIndex.append(default_material.uniqueID);
    /*....*/
    QString materialsDir=materialsPath;
    QDirIterator dirIt(materialsDir);
    while(dirIt.hasNext()){
        dirIt.next();
        if(dirIt.fileInfo().isDir()&& dirIt.fileName()!="." && dirIt.fileName()!=".."){
            QDirIterator it(dirIt.fileInfo().canonicalFilePath());
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
                    material.set(QString(),QUrl(),0,0,0,0,0,0,0,0,0,0);
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
                                material.texture_img=QUrl::fromLocalFile(it.fileInfo().canonicalPath()+
                                                                         "/"+property[1]);
                            }
                            else if(property[0]=="ft0") {
                                bool ok;
                                material.ft0=property[1].toFloat(&ok);
                                if(!ok) qDebug()<<"Convertion error:"<<part;
                            }
                            else if(property[0]=="fc0") {
                                bool ok;
                                material.fc0=property[1].toFloat(&ok);
                                if(!ok) qDebug()<<"Convertion error:"<<part;
                            }
                            else if(property[0]=="fmk") {
                                bool ok;
                                material.fmk=property[1].toFloat(&ok);
                                if(!ok) qDebug()<<"Convertion error:"<<part;
                            }
                            else if(property[0]=="ft90") {
                                bool ok;
                                material.ft90=property[1].toFloat(&ok);
                                if(!ok) qDebug()<<"Convertion error:"<<part;
                            }
                            else if(property[0]=="fc90") {
                                bool ok;
                                material.fc90=property[1].toFloat(&ok);
                                if(!ok) qDebug()<<"Convertion error:"<<part;
                            }
                            else if(property[0]=="fvk") {
                                bool ok;
                                material.fvk=property[1].toFloat(&ok);
                                if(!ok) qDebug()<<"Convertion error:"<<part;
                            }
                            else{qDebug()<<"Unknown property:"<<part;}
                        }
                    }while(!line.isNull());
                    if(material.uniqueID.isEmpty() || material.name.isEmpty() || material.density<=0 ||
                            material.g<=0 || material.young<=0 || m_materials.contains(material.uniqueID)
                            || material.fc0<=0 || material.fc90<=0 || material.fmk<=0 || material.ft0<=0
                            ||material.ft90<=0||material.fvk<=0){
                        qDebug()<<"Invalid material or duplicated Id:"<< it.filePath();
                    }
                    else{
                        m_materials[material.uniqueID]=material;
                        m_materialsFilePath.append(it.fileInfo().canonicalFilePath());
                        m_materialsIndex.append(material.uniqueID);
                    }
                }
            }
        }
    }
    endResetModel();
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
    else if(info.compare("ft0",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].ft0;
    }
    else if(info.compare("fc0",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].fc0;
    }
    else if(info.compare("fmk",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].fmk;
    }
    else if(info.compare("ft90",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].ft90;
    }
    else if(info.compare("fc90",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].fc90;
    }
    else if(info.compare("fvk",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].fvk;
    }
    else if(info.compare("image",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].texture_img;
    }
    else if(info.compare("name",Qt::CaseInsensitive)==0){
        return m_materials[m_materialsIndex[index]].name;
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
    else if(info.compare("ft0",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].ft0;
    }
    else if(info.compare("fc0",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].fc0;
    }
    else if(info.compare("fmk",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].fmk;
    }
    else if(info.compare("ft90",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].ft90;
    }
    else if(info.compare("fc90",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].fc90;
    }
    else if(info.compare("fvk",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].fvk;
    }
    else if(info.compare("name",Qt::CaseInsensitive)==0){
        return m_materials[uniqueID].name;
    }

    return QVariant();

}

bool MaterialsManager::createFile(QUrl imageUrl, QString name, QString density, QString price, QString Young, QString G,
                          QString fc0,QString fc90,
                          QString fmk,QString ft0,
                          QString ft90, QString fvk)
{
    if(name.isEmpty() ||  density.isEmpty() || price.isEmpty() || Young.isEmpty() || G.isEmpty()){
        //emit error("All fields should be filled");
        return false;
    }

    auto netInterfaces=QNetworkInterface::allInterfaces();

    if(netInterfaces.size()==0){
        return false;
    }

    QString mac;
    for(ushort interfaceID=0;interfaceID<netInterfaces.size();interfaceID++){
        mac=netInterfaces[interfaceID].hardwareAddress();
        if(!mac.isEmpty() && mac!="00:00:00:00:00:00" && mac!="02:00:00:00:00:00")
            break;
        else{
            QFile macFile("/sys/class/net/"+netInterfaces[interfaceID].name()+"/address");
            if (macFile.open(QFile::ReadOnly)){
                QTextStream textStream(&macFile);
                mac= QString(textStream.readLine());
                macFile.close();
                if(!mac.isEmpty()&&mac!="00:00:00:00:00:00" && mac!="02:00:00:00:00:00")
                    break;
            }
        }
    }
    if(mac.isEmpty() || mac=="00:00:00:00:00:00" || mac!="02:00:00:00:00:00"){
        qsrand(QDateTime::currentMSecsSinceEpoch());
        mac="AAAAAA"+mac.setNum(qrand()%5000000,16);
    }

    mac.remove(':');

    QString uniqueStr=mac+"-"+QDateTime::currentDateTime().toString(QString("dMMMyyhms"));
    QString path=materialsPath+"/"+uniqueStr+"/";
    QDir dir;
    if(dir.exists(path)){
        //emit error("Directoy exists");
        return false;
    }
    if(!dir.mkdir(path)){
        //emit error("Can't create dir");
        return false;
    }

    QFile file(path+uniqueStr+".material");
    file.open(QFile::WriteOnly);
    QImage image(imageUrl.toLocalFile());

    if(file.isOpen() && !image.isNull()){
        QTextStream out(&file);
        out<< "UniqueID:"<<uniqueStr<<";\n"
        <<"Name:"<<name<<";\n"
        <<"Density:"<<density<<"e-9"<<";\n"
        <<"Price:"<<price<<";\n"
        <<"Young:"<<Young<<";\n"
        <<"G:"<<G<<";\n"
        <<"fc0:"<<fc0<<";\n"
        <<"fc90:"<<fc90<<";\n"
        <<"ft0:"<<ft0<<";\n"
        <<"ft90:"<<ft90<<";\n"
        <<"fmk:"<<fmk<<";\n"
        <<"fvk:"<<fvk<<";\n"
        <<"TextureImage:"<<uniqueStr<<".png;";
        file.close();
        image=image.scaledToWidth(200);
        image.save(path+uniqueStr+".png");
        init();
        return true;
    }
    else{
        file.remove();
        //emit error("Can't create file or read image");
        return false;
    }

}


void MaterialsManager::deleteMaterial(int index){
    if(index<0 || index>m_materialsFilePath.size()) return;
    if(m_materialsFilePath[index].isEmpty()) return;

    if(m_materialsFilePath[index].startsWith(QFileInfo(materialsPath).canonicalFilePath())){
        QFile::remove(m_materialsFilePath[index]);
        QUrl img_url=m_materials[m_materialsIndex[index]].texture_img;
        if(img_url.isLocalFile()){
            QString img_path=img_url.toLocalFile();
            QFileInfo img_fileInfo(img_path);
            if(img_fileInfo.canonicalFilePath().startsWith(QFileInfo(materialsPath).canonicalFilePath())){
                 QFile::remove(img_path);
            }
        }
        init();
    }

}


