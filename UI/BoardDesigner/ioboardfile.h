#ifndef IOBOARDFILE_H
#define IOBOARDFILE_H

#include <QObject>
#include <QVariantList>
class IOBoardFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList tags_configuration READ tags_configuration WRITE setTags_configuration NOTIFY tags_configurationChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(qreal tag_size READ tag_size WRITE setTag_size NOTIFY tag_sizeChanged)
public:    
    explicit IOBoardFile(QObject *parent = 0);
    QVariantList tags_configuration(){return m_tags_configuration;}
    void setTags_configuration(QVariantList list){m_tags_configuration=list; emit tags_configurationChanged();}
    bool loaded(){return m_loaded;}
    qreal tag_size(){return m_tag_size;}
    void setTag_size(qreal s){m_tag_size=s;emit tag_sizeChanged();}
signals:
    void tags_configurationChanged();
    void loadedChanged();
    void tag_sizeChanged();
public slots:
    Q_INVOKABLE void loadFile(QString path);
    Q_INVOKABLE bool writeFile();
    Q_INVOKABLE void removeFile(QString path);
private:
    QVariantList m_tags_configuration;
    bool m_loaded=false;
    qreal m_tag_size=35;
};

#endif // IOBOARDFILE_H
