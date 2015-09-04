#ifndef JOINTVMITEMMODEL_H
#define JOINTVMITEMMODEL_H

#include <QAbstractItemModel>
#include <QVector3D>
#include <QString>
class JointVMItemModel: public QAbstractListModel
{
   Q_OBJECT
public:
    enum Role{Icon= Qt::UserRole + 1,
              Fx,Fy,Fz,ExtraInfo};
    enum ForceType{Internal,
                   External};

    JointVMItemModel(QObject*parent=0);
    JointVMItemModel(const JointVMItemModel &other);
    ~JointVMItemModel(){};
    int rowCount(const QModelIndex &parent)const;
    QVariant data(const QModelIndex &index, int role)const;
    QHash<int, QByteArray> roleNames() const;

    void addForce(ForceType type,QVector3D f,QString additionalDescription);
    void changeForce(int index,ForceType type, QVector3D f, QString additionalDescription);
private:

    QVector<ForceType> m_types;
    QVector<QVector3D> m_forces;
    QVector<QString> m_additionalDescriptions;


};
Q_DECLARE_METATYPE(JointVMItemModel)

#endif // JOINTVMITEMMODEL_H
