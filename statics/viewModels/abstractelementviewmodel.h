#ifndef ABSTRACTELEMENTVIEWMODEL_H
#define ABSTRACTELEMENTVIEWMODEL_H

#include <QObject>
#include <Qt3DCore>
#include <QQmlEngine>
#include <QQmlComponent>

class AbstractElement;
class AbstractElementViewModel : public QObject
{
    Q_OBJECT
public:
    explicit AbstractElementViewModel(QObject* uiRoot,Qt3D::QEntity* sceneRoot,QObject *parent = 0);

    void append_3D_resources(Qt3D::QEntity* root);

    virtual AbstractElement* model()=0;

    /*A set to store the 3d entities created by this VM.*/
    QSet<Qt3D::QEntity*> m_3Dresources;

signals:

public slots:
    virtual void onElementDestroyed()=0;

protected:    

    QQmlEngine engine;
    Qt3D::QEntity* m_sceneRoot;
    QObject* m_uiRoot;

};

#endif // ABSTRACTELEMENTVIEWMODEL_H
