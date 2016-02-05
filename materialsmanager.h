#ifndef MATERIALSMANAGER_H
#define MATERIALSMANAGER_H
#include <QAbstractItemModel>
#include <QUrl>

class MaterialsManager:public QAbstractListModel
{
    Q_OBJECT
public:
    MaterialsManager(QObject* parent=0);
    int rowCount(const QModelIndex &parent)const;
    QVariant data(const QModelIndex &index, int role)const;
    Q_INVOKABLE QVariant get(int index, QString info) const;

private:
    struct Material{
        int id;
        QString name;
        QUrl texture_img;
        qreal price;
        qreal density;
        qreal young;
        qreal g;
        void set(QString _name,
        QUrl _texture_img,
        qreal _price,
        qreal _density, qreal _young, qreal _g){
            name=_name;
            texture_img=_texture_img;
            price=_price;
            density=_density;
            young=_young;
            g=_g;
        }
    };
    QList<Material> m_materials;

};

#endif // MATERIALSMANAGER_H
