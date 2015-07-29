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
    explicit AbstractElementViewModel(QObject *parent = 0);
    void setEntityName(QString name);
    void setElement(AbstractElement* element);

signals:
    void statusComplete();

public slots:
    void setSceneRoot(Qt3D::QEntity* sceneRoot);

    virtual void onElementVmChanged()=0;
    virtual void onElementNameChanged(QString)=0;
    virtual void onElementDestroyed()=0;
    virtual void onStatusComplete()=0;


protected:
    Qt3D::QEntity* m_sceneRoot;
    QString m_entity_name;
    AbstractElement* m_element;
    QQmlEngine engine;

};

#endif // ABSTRACTELEMENTVIEWMODEL_H
