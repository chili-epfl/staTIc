#ifndef RESOURCESFETCHER_H
#define RESOURCESFETCHER_H

#include <QObject>

class ResourcesFetcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QUrl hostUrl READ hostUrl WRITE setHostUrl NOTIFY hostUrlChanged)

public:
    explicit ResourcesFetcher(QObject *parent = 0);

    QString username(){return m_username;}
    void setUsername(const QString &value);

    QString getPassword(){return m_password;}
    void setPassword(const QString &value);

    QUrl hostUrl(){return m_hostUrl;}
    void setHostUrl(QUrl value);

    Q_INVOKABLE void fetchResources();
signals:
    void usernameChanged();
    void passwordChanged();
    void hostUrlChanged();
public slots:
private:
    QString m_username,m_password;
    QUrl m_hostUrl;
};

#endif // RESOURCESFETCHER_H
