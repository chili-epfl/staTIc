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
    explicit AbstractElementViewModel(Qt3D::QEntity* sceneRoot,QObject *parent = 0);

    void append_3D_resources(Qt3D::QEntity* root);

    virtual AbstractElement* model()=0;

    QSet<Qt3D::QEntity*> m_3Dresources;

signals:

public slots:
    virtual void onElementDestroyed()=0;

protected:    

    QQmlEngine engine;
    Qt3D::QEntity* m_sceneRoot;


};

#endif // ABSTRACTELEMENTVIEWMODEL_H
