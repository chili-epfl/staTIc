#include "jointvmitemmodel.h"
#include <QDebug>

#define PRECISION 2
JointVMItemModel::JointVMItemModel(QObject* parent):
    QAbstractListModel(parent)
{

}

JointVMItemModel::JointVMItemModel(const JointVMItemModel &other):
    QAbstractListModel(other.parent())
{
      for(int i=0;i<other.m_types.size();i++){
          m_types.append(other.m_types[i]);
          m_additionalDescriptions.append(other.m_additionalDescriptions[i]);
          m_forces.append(other.m_forces[i]);
      }
}


int JointVMItemModel::rowCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return m_types.size();
}

QVariant JointVMItemModel::data(const QModelIndex &index, int role) const{

    switch (role) {
    case Icon:
        return "qrc:/icons/icons/Force_Icon.svg";
        break;
    case Fx:
        return QString::number(m_forces[index.row()+index.column()].x(),'g',PRECISION);
        break;
    case Fy:
        return QString::number(m_forces[index.row()+index.column()].y(),'g',PRECISION);
        break;
    case Fz:
        return QString::number(m_forces[index.row()+index.column()].z(),'g',PRECISION);
        break;
    case ExtraInfo:
        return m_additionalDescriptions[index.row()+index.column()];
        break;
    default:
        return QVariant();
        break;
    }

}

QHash<int, QByteArray> JointVMItemModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[Icon]="icon";
    roles[Fx]="fx";
    roles[Fy]="fy";
    roles[Fz]="fz";
    roles[ExtraInfo]="extra";
    return roles;
}

void JointVMItemModel::addForce(ForceType type, QVector3D f, QString additionalDescription){
    m_types.append(type);
    m_forces.append(f);
    m_additionalDescriptions.append(additionalDescription);
}

void JointVMItemModel::changeForce(int index,ForceType type, QVector3D f, QString additionalDescription){
    if(index<0 || index>m_types.size()-1) return;
    m_types[index]=type;
    m_forces[index]=f;
    m_additionalDescriptions[index]=additionalDescription;
    emit dataChanged(createIndex(index,0),createIndex(index,0));
}
