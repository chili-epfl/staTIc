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
   Q_PROPERTY(QString source WRITE setSource)
public:
    ScenarioListModel(QObject * parent = 0);
    int rowCount(const QModelIndex &parent)const;
    QVariant data(const QModelIndex &index, int role)const;
    void setSource(QString source);
    Q_INVOKABLE QVariant get(int index, QString info) const;

private:
    QString m_source;

    QVector<QString> m_scenariosNames;
    QMap<QString, QUrl> m_scenariosQml;
    QMap<QString, QUrl> m_scenariosThumbs;

};

#endif // ScenarioLISTMODEL_H
