#ifndef MATERIALSETTER_H
#define MATERIALSETTER_H

/*An helper class to change the default material of the entities loaded with a sceneloader.*/

#include <QObject>
#include <Qt3DCore>
#include <Qt3DRenderer>

class MaterialSetter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QEntity* sceneroot WRITE setSceneroot NOTIFY sceneRootChanged)
    /*if entityName is not provided the material is applied to all the nodes from the sceneroot*/
    Q_PROPERTY(QString entityName  WRITE setEntityName)
    Q_PROPERTY(Qt3D::QMaterial* material WRITE setMaterial NOTIFY materialChanged)

public:
    explicit MaterialSetter(QObject *parent = 0);
    void setSceneroot(Qt3D::QEntity* sceneroot){
        if(m_sceneroot!=sceneroot){
            m_sceneroot=sceneroot;
        }
    }
    void setMaterial(Qt3D::QMaterial* material){
        if(m_material!=material){
            m_material=material;
            emit materialChanged();
        }
    }
    void setEntityName(QString entityName){m_entityName=entityName;}
signals:
    void materialChanged();
public slots:
    void onAnyChange();

private:
    Qt3D::QEntity* m_sceneroot;
    Qt3D::QMaterial* m_material;
    QString m_entityName;
};

#endif // MATERIALSETTER_H
