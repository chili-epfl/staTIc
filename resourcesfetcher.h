#ifndef RESOURCESFETCHER_H
#define RESOURCESFETCHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
class QFile;
class ResourcesFetcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QUrl hostUrl READ hostUrl WRITE setHostUrl NOTIFY hostUrlChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
public:
    explicit ResourcesFetcher(QObject *parent = 0);

    QString username(){return m_username;}
    void setUsername(const QString &value);

    QString password(){return m_password;}
    void setPassword(const QString &value);

    QUrl hostUrl(){return m_hostUrl;}
    void setHostUrl(QUrl value);

    bool loading(){return m_loading;}

    Q_INVOKABLE void synchronize();
signals:
    void usernameChanged();
    void passwordChanged();
    void hostUrlChanged();
    void loadingChanged();
    void pushCompleted();
public slots:
    void slotError();
    void slotPullPhase2();
    void slotPullPhase1();
    void slotPushPhase1();
    void pullResources();
    void pushMaterials();
    void slotPushPhase2();
    void slotCloseFile();
    void slotThumbnailUploaded();
private:
    QString m_username,m_password;
    QUrl m_hostUrl;
    QNetworkAccessManager manager;
    QMap<QNetworkReply*, short> m_reply_resource_map;

    bool m_loading;
    void finished();

    struct PushData{
        QString filename;
        QString thumbnail;
        short resource_type;
        short replies_S3;
    };
    QMap<QNetworkReply*, PushData > m_pushData_map;
    QMap<QNetworkReply*, QFile* > m_openFile_map;

};

#endif // RESOURCESFETCHER_H
