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
    Q_INVOKABLE QVariant get(QString uniqueID, QString info) const;
    Q_INVOKABLE bool createFile(QUrl imageUrl, QString name, QString density, QString price, QString Young, QString G, QString fc0, QString fc90, QString fmk, QString ft0, QString ft90, QString fvk);
    Q_INVOKABLE void deleteMaterial(int index);
private:
    struct Material{
        QString uniqueID;
        QString name;
        QUrl texture_img;
        qreal price;
        qreal density;
        qreal young;
        qreal g;
        qreal ft0,fc0,fmk,ft90,fc90,fvk;
        void set(QString _name,
        QUrl _texture_img,
        qreal _price,
        qreal _density, qreal _young, qreal _g,
        qreal _ft0,qreal _fc0,qreal _fmk,//parallel tension, parallel compression, bending,
        qreal _ft90,qreal _fc90,qreal _fvk)//perpendicular tension, perpendicular compression, shear
        {
            name=_name;
            texture_img=_texture_img;
            price=_price;
            density=_density;
            young=_young;
            g=_g;
            ft0=_ft0;
            fc0=_fc0;
            fmk=_fmk;
            ft90=_ft90;
            fc90=_fc90;
            fvk=_fvk;
        }
    };
    void init();
    //QList<Material> m_materials;
    QMap<QString,Material> m_materials;
    QList<QString> m_materialsIndex;
    QList<QString> m_materialsFilePath;

};

#endif // MATERIALSMANAGER_H
