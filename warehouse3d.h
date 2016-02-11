#ifndef WAREHOUSE3D_H
#define WAREHOUSE3D_H
#include <QAbstractItemModel>
#include <QSet>
#include <QUrl>

class Warehouse3D:public QAbstractListModel
{
    Q_OBJECT
public:
    Warehouse3D(QObject *parent=0);
    int rowCount(const QModelIndex &parent)const;
    QVariant data(const QModelIndex &index, int role)const;
    Q_INVOKABLE QVariant get(int index, QString info) const;
private:

    struct Object{
        QString name;
        QString main_asset_file_path;
        QUrl decoration_img;
    };

    QList<Object> m_objectsIndexes;
    static QSet<QString> m_image_extensions;
    static QSet<QString> m_3Dasset_extensions;

};

#endif // WAREHOUSE3D_H
