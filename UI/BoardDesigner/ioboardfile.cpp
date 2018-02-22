#include "ioboardfile.h"
#include <QNetworkInterface>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMatrix4x4>
#include <QDateTime>
#include <QFileInfo>
#include "static_global.h"
IOBoardFile::IOBoardFile(QObject *parent) : QObject(parent)
{

}

void IOBoardFile::loadFile(QString path)
{
    m_loaded=false;

    path=boardsPath+path;
    QFile file(path);
    m_tags_configuration.clear();
    if(file.open(QFile::ReadOnly)){
        QTextStream inputStream(&file);
        QString line;
        int status=-1; //-1 looking for number of tags; 0 looking for tag number; 1 looking for tag size;
        //2,3,4 reading matrix lines
        int tag_number=0;
        qreal x,y;
        QMatrix4x4 m,m_z;
        m.setRow(3,QVector4D(0,0,0,1));
        QMatrix4x4 rotX(1,0,0,0,
                        0,cos(0.143117),-sin(0.143117),0,
                        0,sin(0.143117),cos(0.143117),0,
                        0,0,0,1);
        rotX=rotX.inverted();
        QVariantMap marker_info;
        do{
            line=inputStream.readLine();
            if(line.isEmpty() || line.at(0)=='#' || line.count(' ')==line.length()) continue;
            switch (status){
            case -1: status++; break;
            case 0: tag_number=line.toInt(); status++;break;
            case 1: m_tag_size=line.toDouble();status++;break;
            case 2: m.setRow(0,QVector4D(line.split(" ")[0].toDouble(),line.split(" ")[1].toDouble(),line.split(" ")[2].toDouble(),0));x=line.split(" ")[3].toDouble(); status++;break;
            case 3: m.setRow(1,QVector4D(line.split(" ")[0].toDouble(),line.split(" ")[1].toDouble(),line.split(" ")[2].toDouble(),0));y=line.split(" ")[3].toDouble(); status++;break;
            case 4:
                m.setRow(2,QVector4D(line.split(" ")[0].toDouble(),line.split(" ")[1].toDouble(),line.split(" ")[2].toDouble(),0));
                m_z=m*rotX;
                marker_info["id"]=tag_number;
                marker_info["x"]=x;
                marker_info["y"]=y;
                qreal rotation=round(atan2(m_z(1,0),m_z(1,1))*180/M_PI);
                if(qFuzzyCompare(rotation,-180.0)) rotation=180;
                marker_info["rotation"]=rotation;
                m_tags_configuration.append(marker_info);
                status=0;
                break;
            }
        }while(!line.isNull());
        file.close();
        m_loaded=true;
    }
    else
        qDebug()<<"Can't open file";
    emit loadedChanged();
    emit tag_sizeChanged();
}

bool IOBoardFile::writeFile()
{
    auto netInterfaces=QNetworkInterface::allInterfaces();

    if(netInterfaces.size()==0){
        return false;
    }

    QString mac;
    for(ushort interfaceID=0;interfaceID<netInterfaces.size();interfaceID++){
        mac=netInterfaces[interfaceID].hardwareAddress();
        if(mac!="00:00:00:00:00:00")
            break;
    }
    if(mac=="00:00:00:00:00:00")
        return false;
    mac.remove(':');
    QString filename=boardsPath+mac+"board_conf_"+ QDateTime::currentDateTime().toString("dMyy_Hms")+".data";


    QFile file(filename);
    QTextStream outputstream;
    QMatrix4x4 rotX(1,0,0,0,
                    0,cos(0.143117),-sin(0.143117),0,
                    0,sin(0.143117),cos(0.143117),0,
                    0,0,0,1);
    if(file.open(QFile::WriteOnly)){
        outputstream.setDevice(&file);
        outputstream<<m_tags_configuration.size()<<"\n\n";
        Q_FOREACH(auto _tag, m_tags_configuration){
            auto tag=_tag.value<QVariantMap>();
            outputstream<<tag["id"].toString()<<"\n";
            outputstream<<m_tag_size<<"\n";
            double angleZ=tag["rotation"].toDouble()*M_PI/180.0;
            QMatrix4x4 rotZ(cos(angleZ),-sin(angleZ),0,0,
                            sin(angleZ),cos(angleZ),0,0,
                            0,0,1,0,
                            0,0,0,1);
            QMatrix4x4 compRot=rotZ*rotX;
            outputstream<<compRot(0,0)<<" "<<compRot(0,1)<<" "<<compRot(0,2)<<" "<<tag["x"].toString()<<"\n";
            outputstream<<compRot(1,0)<<" "<<compRot(1,1)<<" "<<compRot(1,2)<<" "<<tag["y"].toString()<<"\n";
            outputstream<<compRot(2,0)<<" "<<compRot(2,1)<<" "<<compRot(2,2)<<" "<<0<<"\n\n";
        }
        outputstream.flush();
        file.close();
        return true;
    }
    else return false;

}

void IOBoardFile::removeFile(QString path){
    path=boardsPath+path;
    if(QFileInfo(path).canonicalFilePath().startsWith(QFileInfo(boardsPath).canonicalFilePath()))
         QFile::remove(path);
}

