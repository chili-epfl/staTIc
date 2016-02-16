#include "resourcesfetcher.h"

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



