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
    void setEntityName(QString name){m_entity_name=name;emit propertyChanged();}
    void setElement(AbstractElement* element){m_element=element;emit propertyChanged();}
signals:
    void propertyChanged();
public slots:
    void setSceneRoot(Qt3D::QEntity* sceneRoot){m_sceneRoot=sceneRoot;emit propertyChanged();}
    virtual void onElementChanged()=0;
    virtual void onPropertyChanged()=0;
    virtual void onElementDestroyed()=0;

protected:
    Qt3D::QEntity* m_sceneRoot;
    QString m_entity_name;
    AbstractElement* m_element;
    QQmlEngine engine;

};

#endif // ABSTRACTELEMENTVIEWMODEL_H
