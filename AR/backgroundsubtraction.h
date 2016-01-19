#ifndef BACKGROUNDSUBTRACTION_H
#define BACKGROUNDSUBTRACTION_H

#include <QtMultimedia>

class BackgroundSubtraction : public QAbstractVideoFilter {
    Q_OBJECT
    Q_PROPERTY(qreal entropy READ entropy NOTIFY entropyChanged)
public:
    BackgroundSubtraction(QObject* parent=0);
    QVideoFilterRunnable *createFilterRunnable();
    qreal entropy(){return m_entropy;}
private slots:
    void updateEntropy(qreal);
signals:
    void finished(QObject *result);
    void entropyChanged();
private:
    qreal m_entropy;
};


#endif // BACKGROUNDSUBTRACTION_H
