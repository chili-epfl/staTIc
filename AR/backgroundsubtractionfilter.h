#ifndef BACKGROUNDSUBTRACTIONFILTER_H
#define BACKGROUNDSUBTRACTIONFILTER_H

#include <QtMultimedia>
//opencv
#include "opencv2/imgproc.hpp"
#include <opencv2/video.hpp>

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker():QObject(){
        m_pMOG2=cv::createBackgroundSubtractorMOG2();
    }
public slots:
    void doWork(const cv::Mat &mat) {
        m_pMOG2->apply(mat, m_fgMaskMOG2);
        cv::medianBlur(m_fgMaskMOG2,m_fgMaskMOG2,3);
        emit resultReady((qreal)cv::countNonZero(m_fgMaskMOG2)/(m_fgMaskMOG2.cols*m_fgMaskMOG2.rows));
    }
signals:
    void resultReady(qreal result);
private:
    cv::Ptr<cv::BackgroundSubtractor> m_pMOG2;
    cv::Mat m_fgMaskMOG2; //fg mask fg mask generated by MOG2 method
};



class BackgroundSubtractionFilter : public QObject, public QVideoFilterRunnable {
    Q_OBJECT
public:
    BackgroundSubtractionFilter();
    ~BackgroundSubtractionFilter();
    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);
signals:
    void updated(qreal nonZeroPixels);
    void operate(const cv::Mat &mat);
private:
     QThread m_workerThread;

};

#endif // BACKGROUNDSUBTRACTIONFILTER_H
