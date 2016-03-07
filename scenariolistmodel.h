#ifndef ScenarioLISTMODEL_H
#define ScenarioLISTMODEL_H

#include <QDirIterator>
#include <QAbstractItemModel>
#include <QUrl>
#include <QDebug>

/*The class defines the model for displaying a scenario inside
 * a gridview or similar views. */
class ScenarioListModel: public QAbstractListModel
{

   Q_OBJECT
public:
    ScenarioListModel(QObject * parent = 0);
    int rowCount(const QModelIndex &parent)const;
    QVariant data(const QModelIndex &index, int role)const;
    Q_INVOKABLE QVariant get(int index, QString info) const;

private:
    void setSource();
    QString m_source;
    QVector<QString> m_scenariosNames;
    QMap<QString, QUrl> m_scenariosStructures;
    QMap<QString, QUrl> m_scenariosThumbs;
    QMap<QString, QUrl> m_scenarios3DAsset;
    QMap<QString, QUrl> m_scenariosTagConfiguration;

};

#endif // ScenarioLISTMODEL_H
