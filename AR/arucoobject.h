#ifndef ARUCOOBJECT_H
#define ARUCOOBJECT_H

#include <QMatrix4x4>
#include <QQuickItem>
#include <QString>
class ArucoObject : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QMatrix4x4 poseMatrix READ poseMatrix NOTIFY poseMatrixChanged)
    Q_PROPERTY(bool isVisible READ isVisible NOTIFY isVisibleChanged)
    Q_PROPERTY(QString objectId READ objectId WRITE setObjectId NOTIFY objectIdChanged)
public:
    ArucoObject(QQuickItem * parent = 0);

    QMatrix4x4 poseMatrix(){return m_posematrix;}

    void setPoseMatrix(QMatrix4x4 m){
        m_posematrix=m;
        emit poseMatrixChanged();
    }

    bool isVisible(){
        return m_visible;
    }
    void setVisible(bool val){
        if(val!=m_visible){
            m_visible=val;
            emit isVisibleChanged();
        }
    }

    QString objectId(){return m_objectId;}
    void setObjectId(QString val){
        if(val!=m_objectId){
            QString tmp=m_objectId;
            m_objectId=val;
            emit objectIdChanged(tmp);
        }
    }

signals:
    void poseMatrixChanged();
    void isVisibleChanged();
    void objectIdChanged(QString previousObjID);

private:
    QMatrix4x4 m_posematrix;
    bool m_visible;
    QString m_objectId;

};

#endif // ARUCOOBJECT_H
