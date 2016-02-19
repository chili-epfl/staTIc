#include "resourcesfetcher.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDirIterator>
#include "static_global.h"

const short material_code=1;
const short asset3d_code=2;
const short scenario_code=3;

ResourcesFetcher::ResourcesFetcher(QObject *parent) : QObject(parent)
{
    connect(this,SIGNAL(pushCompleted()),this, SLOT(pullResources()));

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
    pushMaterials();
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


void ResourcesFetcher::pushMaterials(){
    QDir materialsPrivateDir(materialsPrivatePath);
    QStringList files=materialsPrivateDir.entryList();
    Q_FOREACH(QString file, files){
        if(!file.endsWith(".static_material")) continue;
        QString basename=file.split(".static_material")[0];
        if(!files.contains(basename+".png")) continue;
        QNetworkRequest push;
        QUrl url=m_hostUrl;
        QString query="type=request-put&resourceType=material&email="+m_username
                +"&filename="+file+"&thumbnail="+basename+".png";
        url.setQuery(query);
        push.setUrl(url);
        PushData pushData;
        pushData.filename=file;
        pushData.thumbnail=basename+".png";
        pushData.resource_type=material_code;
        pushData.replies_S3=0;
        QNetworkReply* reply=manager.get(push);
        connect(reply, SIGNAL(finished()), this, SLOT(slotPushPhase1()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(slotError()));
        m_pushData_map[reply]=pushData;
    }
    if(m_pushData_map.count()==0)
        emit pushCompleted();
}


void ResourcesFetcher::slotPushPhase1(){
    QNetworkReply *reply=static_cast<QNetworkReply *>(QObject::sender());
    if(reply->error()==QNetworkReply::NoError){
        QString urlFile, urlThumbnail;
        urlFile=reply->readLine();
        urlFile=urlFile.split("\r\n")[0];

        urlThumbnail=reply->readLine();
        urlThumbnail=urlThumbnail.split("\r\n")[0];
        qDebug()<<urlFile;
        qDebug()<<urlThumbnail;

        QNetworkRequest putFile;
        QUrl URLFile=urlFile;
        putFile.setUrl(URLFile);
        QString filePath;
        switch (m_pushData_map[reply].resource_type) {
        case material_code:
            filePath=materialsPrivatePath;
            break;
        case asset3d_code:
            filePath=assets3DPrivatePath;
            break;
        case scenario_code:
            filePath=scenariosPrivatePath;
            break;
        }
        QFile* file=new QFile(filePath+m_pushData_map[reply].filename);
        file->open(QIODevice::ReadOnly);
        QNetworkReply *replyFile = manager.put(putFile,file);
        connect(replyFile, SIGNAL(finished()), this, SLOT(slotCloseFile()));
        connect(replyFile, SIGNAL(error(QNetworkReply::NetworkError)),
                  this, SLOT(slotError()));
        m_openFile_map[replyFile]=file;

        QNetworkRequest putThumbnail;
        QUrl URLThumbnail=urlThumbnail;
        putThumbnail.setUrl(URLThumbnail);
        QString thumbnailPath;
        switch (m_pushData_map[reply].resource_type) {
        case material_code:
            thumbnailPath=materialsPrivatePath;
            break;
        case asset3d_code:
            thumbnailPath=assets3DPrivatePath;
            break;
        case scenario_code:
            thumbnailPath=scenariosPrivatePath;
            break;
        }
        QFile* thumbnailFile=new QFile(thumbnailPath+m_pushData_map[reply].thumbnail);
        thumbnailFile->open(QIODevice::ReadOnly);
        QNetworkReply *replyThumbnail = manager.put(putThumbnail,thumbnailFile);
        connect(replyThumbnail, SIGNAL(finished()), this, SLOT(slotCloseFile()));
        connect(replyThumbnail, SIGNAL(finished()), this, SLOT(slotThumbnailUploaded()));
        connect(replyThumbnail, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(slotError()));
        m_openFile_map[replyThumbnail]=thumbnailFile;
        m_pushData_map[replyThumbnail]=m_pushData_map[reply];
    }
    m_pushData_map.remove(reply);
    reply->deleteLater();
    if(m_pushData_map.count()==0){
        emit pushCompleted();
    }
}
void ResourcesFetcher::slotPushPhase2(){
    QNetworkReply *reply=static_cast<QNetworkReply *>(QObject::sender());
    m_pushData_map.remove(reply);
    reply->deleteLater();
    if(m_pushData_map.count()==0){
        emit pushCompleted();
    }
}
void ResourcesFetcher::slotThumbnailUploaded(){
    QNetworkReply *reply=static_cast<QNetworkReply *>(QObject::sender());
    if(reply->error()==QNetworkReply::NoError){
        /*Create the Post*/
        QNetworkRequest createPost;
        QUrl createPostURL=m_hostUrl;

        QString resourceType="";
        switch (m_pushData_map[reply].resource_type) {
        case material_code:
            resourceType="resourceType=material";
            break;
        case asset3d_code:
            resourceType="resourceType=asset3D";
            break;
        case scenario_code:
            resourceType="resourceType=scenario";
            break;
        }
        QString email="email="+m_username;

        QString filename="filename="+m_pushData_map[reply].filename;
        QString thumbnail="thumbnail="+m_pushData_map[reply].thumbnail;

        QString query=QString("type=create-post&")+email+"&"+resourceType+"&"+filename
                +"&"+thumbnail;
        createPostURL.setQuery(query);
        createPost.setUrl(createPostURL);
        QNetworkReply *postReply = manager.get(createPost);
        connect(postReply, SIGNAL(finished()), this, SLOT(slotPushPhase2()));
        connect(postReply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(slotError()));
        m_pushData_map[postReply]=m_pushData_map[reply];
    }
    m_pushData_map.remove(reply);
    reply->deleteLater();
    if(m_pushData_map.count()==0){
        emit pushCompleted();
    }
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
        }
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

void ResourcesFetcher::slotCloseFile(){
    QNetworkReply *reply=static_cast<QNetworkReply *>(QObject::sender());
    if(m_openFile_map.contains(reply)){
        QFile* f=m_openFile_map[reply];
        m_openFile_map.remove(reply);
        f->close();
        f->deleteLater();
    }
}

void ResourcesFetcher::finished(){
    m_loading=false;
    emit loadingChanged();
}


void ResourcesFetcher::slotError(){
    QNetworkReply *reply=static_cast<QNetworkReply *>(QObject::sender());
    qDebug()<<reply->error();
    if(m_openFile_map.contains(reply)){
        QFile* f=m_openFile_map[reply];
        m_openFile_map.remove(reply);
        f->close();
        f->deleteLater();
    }
    m_reply_resource_map.remove(reply);
    reply->deleteLater();
    if(m_reply_resource_map.count()==0){
        finished();
    }

}
