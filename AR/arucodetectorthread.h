#ifndef ARUCODETECTORTHREAD_H
#define ARUCODETECTORTHREAD_H

#include <QVideoFilterRunnable>
#include<QDebug>
#include<QThread>
#include<QObject>
#include<QElapsedTimer>
#include<QMutex>
#include<QWaitCondition>
#include<QVideoFilterRunnable>
#include<opencv2/core.hpp>
#include<opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>


class ArucoDetector;

typedef QHash<QString,QMatrix4x4> PoseMap;
typedef QHash<QString,cv::aruco::Board> BoardMap;

class DetectionTask : public QObject{
Q_OBJECT
public:
    DetectionTask(QMatrix4x4 projectionMatrix);
    virtual ~DetectionTask();
    void presentFrame(cv::Mat frame);
    void stop();

    void setProjectionMatrix(QMatrix4x4 m);
    void setBoards(BoardMap boards);
public slots:
    void doWork();
signals:
    void objectsReady(const PoseMap& poses);

private:

#ifdef QT_DEBUG
    const float FPS_RATE = 0.9f;            ///< Rate of using the older FPS estimates
    const int FPS_PRINT_PERIOD = 500;       ///< Period of printing the FPS estimate, in milliseconds
#endif


    enum State{
        NONE,               ///< Invalid state, new frames will not be presented
        BUSY,               ///< Currently busy
        WAITING_FOR_FRAME   ///< Waiting for a new frame
    };

    QMutex frameLock;                   ///< Mutex that locks the frame transaction
    QWaitCondition nextFrameCond;       ///< Condition to wait on until the next frame arrives
    State state = NONE;                 ///< Thread's current state

    cv::Mat nextFrame;                  ///< The frame that will be processed next

    bool nextFrameAvailable = false;    ///< Whether the next frame is ready and in place
    bool running = false;               ///< Whether the should keep running, we don't need a mutex for this


    std::vector<cv::Ptr<cv::aruco::Board> > m_boards;
    QStringList m_board_names;
    cv::Ptr< cv::aruco::Dictionary> m_dictionary;
    std::vector< int > m_markerIds;
    std::vector< std::vector<cv::Point2f> > m_markerCorners;
    cv::Mat m_distCoeff;
    cv::Mat m_cv_projectionMatrix;
    int test=0;
    cv::Ptr<cv::aruco::DetectorParameters> m_detector_params;
};


class ArucoDetectorThread: public QObject ,public QVideoFilterRunnable
{
    Q_OBJECT
public:
    ArucoDetectorThread(ArucoDetector* detector,QObject* parent=0);
    virtual ~ArucoDetectorThread();
    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);

public slots:
    void start();
    void stop();
    void setPause(bool val);
signals:
    void objectsReady(const PoseMap& poses);
private:
    bool m_pause;
    QThread workerThread;               ///< The thread that Aruco will work in
    DetectionTask* task = NULL;         ///< The loop method and parameter container
    //GLuint fbo=0;                         ///< FBO used in android to move a texture to the memory
    ArucoDetector* m_detector;
};


#endif // ARUCODETECTORTHREAD_H
