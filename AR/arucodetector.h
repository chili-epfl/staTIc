#ifndef ARUCODETECTOR_H
#define ARUCODETECTOR_H

#include <QtMultimedia>
#include <QQuickItem>
#include <QUrl>
#include "arucoobject.h"
#include "arucodetectorthread.h"

class ArucoDetector : public QAbstractVideoFilter
{
    Q_OBJECT
    //Q_PROPERTY(QVariantMap tags READ getTags NOTIFY tagsChanged)? if we don't know the size, what is the pose???
    Q_PROPERTY(QMatrix4x4 projectionMatrix READ getProjectionMatrix NOTIFY projectionMatrixChanged)
    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)
public:
    ArucoDetector(QQuickItem *parent = 0);
    QVideoFilterRunnable *createFilterRunnable();

    QMatrix4x4 getProjectionMatrix() const;

    Q_INVOKABLE void addConfigurationFile(QUrl url);

    Q_INVOKABLE void registerObserver(ArucoObject* o);

    bool pause(){return m_pause;}
    void setPause(bool val);

    BoardMap getBoards(){
        return m_boards;
    }
    QList<SingleTag> singleTagList(){
        return m_singleTagList;
    }
    ~ArucoDetector();
signals:

    void projectionMatrixChanged();

    void pauseChanged(bool val);
    void destroying();

public slots:
    Q_INVOKABLE void unregisterObserver(ArucoObject* o);
    void unregisterDestroyedObserver(QObject* o);
    void updateObserver(QString prevObjId);
    void notifyObservers(const PoseMap& poses);
private:

    void loadConfigurationFiles();
    QMultiHash<QString, ArucoObject*> m_observers;

    BoardMap m_boards;
    QList<SingleTag> m_singleTagList;
    bool m_pause;
    qreal m_focalLength;
    QSizeF m_cameraResolution;
    /*This is actually a 3x3 matrix...*/
    QMatrix4x4 m_projectionMatrix;


};

#endif // ARUCODETECTOR_H
