#include "resourcesfetcher.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDirIterator>
#include <quazip/JlCompress.h>
#include "static_global.h"

const short material_code=1;
const short asset3d_code=2;
const short scenario_code=3;

ResourcesFetcher::ResourcesFetcher(QObject *parent) : QObject(parent)
{

}

void ResourcesFetcher::setUsername(const QString &value)
{
    if(value!=m_username){
        m_username=value;
        emit usernameChanged();
    }
}

void ResourcesFetcher::setPassword(const QString &value)
{
    if(m_password!=value){
        m_password = value;
        emit passwordChanged();
    }
}

void ResourcesFetcher::setHostUrl(QUrl value)
{
    if(m_hostUrl!=value){
        m_hostUrl=value;
        emit hostUrlChanged();
    }
}

void ResourcesFetcher::synchronize()
{
    if(m_hostUrl.isEmpty() || m_username.isEmpty() ) return;
    m_loading=true;
    emit loadingChanged();
    pullResources();
}

void ResourcesFetcher::pullResources(){

    m_reply_resource_map.clear();
    /*Fetching Materials*/
    QNetworkRequest requestMaterials;
    QUrl fetchMaterialsURL=m_hostUrl;
    fetchMaterialsURL.setQuery(QString("type=fetch-resources&resourceType=material&email=")+m_username);
    requestMaterials.setUrl(fetchMaterialsURL);
    QNetworkReply *replyMaterials = manager.get(requestMaterials);
    connect(replyMaterials, SIGNAL(finished()), this, SLOT(slotPullPhase1()));
    connect(replyMaterials, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotError()));
    m_reply_resource_map[replyMaterials]=material_code;

    /*Fetching 3D assets*/
    QNetworkRequest requestAssets3D;
    QUrl fetchAssets3DURL=m_hostUrl;
    fetchAssets3DURL.setQuery(QString("type=fetch-resources&resourceType=asset3D&email=")+m_username);
    requestAssets3D.setUrl(fetchAssets3DURL);
    QNetworkReply *replyAssets3D = manager.get(requestAssets3D);
    connect(replyAssets3D, SIGNAL(finished()), this, SLOT(slotPullPhase1()));
    connect(replyAssets3D, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotError()));
    m_reply_resource_map[replyAssets3D]=asset3d_code;

    /*Fetching Scenario*/
    QNetworkRequest requestScenarios;
    QUrl fetchScenariosURL=m_hostUrl;
    fetchScenariosURL.setQuery(QString("type=fetch-resources&resourceType=scenario&email=")+m_username);
    requestScenarios.setUrl(fetchScenariosURL);
    QNetworkReply *replyScenarios = manager.get(requestScenarios);
    connect(replyScenarios, SIGNAL(finished()), this, SLOT(slotPullPhase1()));
    connect(replyScenarios, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotError()));
    m_reply_resource_map[replyScenarios]=scenario_code;


}



void ResourcesFetcher::slotPullPhase1(){
    QNetworkReply *reply=static_cast<QNetworkReply *>(QObject::sender());
    if(reply->error()==QNetworkReply::NoError){
        QString current_url;
        do{
            current_url=reply->readLine();
            current_url=current_url.split("\r\n")[0];
            qDebug()<<current_url;
            if(!current_url.isEmpty()){
                QNetworkRequest request;
                QUrl URL=current_url;
                request.setUrl(URL);
                QNetworkReply *replyPhase2 = manager.get(request);
                connect(replyPhase2, SIGNAL(finished()), this, SLOT(slotPullPhase2()));
                m_reply_resource_map[replyPhase2]=m_reply_resource_map[reply];
            }
        }while(!current_url.isEmpty());
    }else{
        qDebug()<<reply->error();
        qDebug()<<reply->readAll();
    }
    m_reply_resource_map.remove(reply);
    reply->deleteLater();
    if(m_reply_resource_map.count()==0){
        finished();
    }

}

void ResourcesFetcher::slotPullPhase2(){
    QNetworkReply *reply=static_cast<QNetworkReply *>(QObject::sender());
    if(reply->error()==QNetworkReply::NoError){
        QString filename=reply->url().path();
        if(m_reply_resource_map[reply]==scenario_code)
            filename.prepend(scenariosPath);
        else if(m_reply_resource_map[reply]==asset3d_code)
            filename.prepend(assets3DPath);
        else if(m_reply_resource_map[reply]==material_code)
            filename.prepend(materialsPath);
        else
            qCritical("Error: invalid reply type");
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        if(file.isOpen()){
            file.write(reply->readAll());
            file.close();
            QStringList fileList=JlCompress::extractDir(filename,filename.split(".static_").at(0));
            if(fileList.isEmpty())
                qDebug()<<"Can't extract file";
            file.remove();
        }
        else
            qDebug()<<"Can't save file";
    }else{
        qDebug()<<reply->error();
        qDebug()<<reply->readAll();
    }
    m_reply_resource_map.remove(reply);
    reply->deleteLater();
    if(m_reply_resource_map.count()==0){
        finished();
    }
}


void ResourcesFetcher::finished(){
    m_loading=false;
    emit loadingChanged();
}


void ResourcesFetcher::slotError(){
    QNetworkReply *reply=static_cast<QNetworkReply *>(QObject::sender());
    qDebug()<<reply->error();
    m_reply_resource_map.remove(reply);
    reply->deleteLater();
    if(m_reply_resource_map.count()==0){
        finished();
    }

}