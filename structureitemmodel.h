#ifndef STRUCTURELISTMODEL_H
#define STRUCTURELISTMODEL_H

#include <QDirIterator>
#include <QAbstractItemModel>
#include <QUrl>
#include <QDebug>
/*The class defines the model for displaying the structure inside a gridview or similar views. */

class StructureItemModel: public QAbstractListModel
{

   Q_OBJECT
   Q_PROPERTY(QString source WRITE setSource)
public:
    StructureItemModel(QObject * parent = 0);
    //QModelIndex index(int row, int column, const QModelIndex &parent) const ;
    //QModelIndex parent(const QModelIndex &child)const{return QModelIndex();} ;
    int rowCount(const QModelIndex &parent)const{return m_fileNames.size();} ;
    //int columnCount(const QModelIndex &parent)const{return 0;} ;
    QVariant data(const QModelIndex &index, int role)const;
    void setSource(QString source);
    Q_INVOKABLE QVariant get(int index, QString info) const;

private:
    QString m_source;
    QVector<QString> m_fileNames;

    QMap<QString, QUrl> m_thumbs;
    QMap<QString, QUrl> m_modelfiles;
    QMap<QString, QUrl> m_staticsfiles;
};

#endif // STRUCTURELISTMODEL_H
