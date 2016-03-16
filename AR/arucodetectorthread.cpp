#include "arucodetectorthread.h"
#include "arucodetector.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <QMatrix4x4>

ArucoDetectorThread::ArucoDetectorThread(ArucoDetector* detector,QObject *parent):
    QObject(parent)
{
    m_detector=detector;
    m_pause=false;
    task = new DetectionTask(m_detector->getProjectionMatrix());
    task->setBoards(m_detector->getBoards());
    task->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(started()), task, SLOT(doWork()));
    connect(task, SIGNAL(objectsReady(PoseMap)),
            this, SIGNAL(objectsReady(PoseMap)));
    //TODO: update projection matrix and boards;
}

ArucoDetectorThread::~ArucoDetectorThread()
{
    stop();
    delete task;
    task=NULL;
    qDebug()<<"Destroy thread";
}

QVideoFrame ArucoDetectorThread::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, QVideoFilterRunnable::RunFlags flags)
{
    if(m_pause) return *input;
    auto pixelFormat=surfaceFormat.pixelFormat();
    auto handle=input->handleType();

    /*if(input->map(QAbstractVideoBuffer::ReadOnly)){
        cv::Mat mat;
        if( QVideoFrame::imageFormatFromPixelFormat(pixelFormat)!=QImage::Format_Invalid ){
            QImage img( input->bits(),
                        input->width(),
                        input->height(),
                        input->bytesPerLine(),
                        QVideoFrame::imageFormatFromPixelFormat(pixelFormat));
            img=img.convertToFormat(QImage::Format_Grayscale8);
            mat=cv::Mat(img.height(), img.width(), CV_8UC1,
                        const_cast<uchar*>(img.constBits()), img.bytesPerLine()).clone();
            task->presentFrame(mat);
        }
        else if (pixelFormat == QVideoFrame::PixelFormat::Format_YUV420P ||
                 pixelFormat == QVideoFrame::PixelFormat::Format_NV21 ||
                 pixelFormat == QVideoFrame::PixelFormat::Format_NV12
                 ) {
            mat=cv::Mat(input->height(), input->width(),CV_8UC1 ,
                        const_cast<uchar*>(input->bits())).clone();
            task->presentFrame(mat);
        }
        else if (pixelFormat==QVideoFrame::PixelFormat::Format_BGR32){
            QImage img(input->bits(),input->width(),input->height(),input->bytesPerLine(),QImage::Format_RGBA8888);
            cv::Mat mat=cv::Mat(img.height(), img.width(), CV_8UC4,
                                const_cast<uchar*>(img.constBits()), img.bytesPerLine()).clone();
            task->presentFrame(mat);


            /*cv::Mat mat=cv::Mat(input->height(), input->width(), CV_8UC4,
                                input->bits(), input->bytesPerLine()).clone();
            task->presentFrame(mat);
        }

        else{
            qWarning()<<" Handle: NoHandle, unsupported pixel format:"<<pixelFormat;
        }
        input->unmap();
    }
    else{
        qWarning()<<"Cannot map video buffer";
    }*/

    //qDebug()<<input->sizinte();
    switch (handle) {
    case QAbstractVideoBuffer::NoHandle: ///< Mainly desktop configuration
        if(input->map(QAbstractVideoBuffer::ReadOnly)){
            cv::Mat mat;
            if( QVideoFrame::imageFormatFromPixelFormat(pixelFormat)!=QImage::Format_Invalid ){
                QImage img( input->bits(),
                                   input->width(),
                            input->height(),
                            input->bytesPerLine(),
                            QVideoFrame::imageFormatFromPixelFormat(pixelFormat));
                img=img.convertToFormat(QImage::Format_Grayscale8);
                mat=cv::Mat(img.height(), img.width(), CV_8UC1,
                            const_cast<uchar*>(img.constBits()), img.bytesPerLine()).clone();
                task->presentFrame(mat);
            }
            else if (pixelFormat == QVideoFrame::PixelFormat::Format_YUV420P ||
                     pixelFormat == QVideoFrame::PixelFormat::Format_NV21 ||
                     pixelFormat == QVideoFrame::PixelFormat::Format_NV12
                     ) {
                mat=cv::Mat(input->height(), input->width(),CV_8UC1 ,
                            const_cast<uchar*>(input->bits())).clone();
                task->presentFrame(mat);
            }
            else{
                qWarning()<<" Handle: NoHandle, unsupported pixel format:"<<pixelFormat;
            }
            input->unmap();
        }
        else{
            qWarning()<<"Cannot map video buffer";
        }
        break;
    case QAbstractVideoBuffer::GLTextureHandle:
        if ( pixelFormat == QVideoFrame::Format_BGR32){
            /*QImage img(input->width(),input->height(),QImage::Format_RGBA8888);
            GLuint textureId = input->handle().value<GLuint>();
            QOpenGLContext *ctx = QOpenGLContext::currentContext();
            QOpenGLFunctions *f = ctx->functions();
            //GLuint fbo;
            if(fbo==0)
                f->glGenFramebuffers(1, &fbo);
            GLuint prevFbo;
            f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *) &prevFbo);
            f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
            f->glPixelStorei(GL_PACK_ALIGNMENT,4);
            f->glReadPixels(0, 0, input->width(), input->height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
            f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
            cv::Mat mat=cv::Mat(img.height(), img.width(), CV_8UC4,
                                const_cast<uchar*>(img.constBits()), img.bytesPerLine()).clone();
            task->presentFrame(mat);*/
        }
        else{
            qWarning()<<"Handle: GLTextureHandle, unsupported pixel format:"<<pixelFormat;
        }
        break;
    default:
        qWarning()<<"Unsupported Video Frame Handle:"<<handle;
        break;
    }

    return *input;

}

void ArucoDetectorThread::start()
{
    workerThread.start();
}

void ArucoDetectorThread::stop()
{
    if(task != NULL){
        task->stop();
    }
    workerThread.quit();
    workerThread.wait();
}

void ArucoDetectorThread::setPause(bool val)
{
    if(m_pause!=val){
        m_pause=val;
    }
}


DetectionTask::DetectionTask(QMatrix4x4 projectionMatrix)
{
    m_distCoeff=cv::Mat();
    setProjectionMatrix(projectionMatrix);
    m_dictionary= cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_1000);

}

DetectionTask::~DetectionTask()
{
    qDebug()<<"Destroy task";
}

void DetectionTask::presentFrame(cv::Mat frame)
{
    frameLock.lock();
    switch(state){

        //Invalid state, do not present
        case NONE:
            break;

            //Currently processing, copy buffer over the next frame
        case BUSY:
            nextFrame = frame.clone(); //TODO: We're screwed if this thing does not memcpy the buffer behind the curtains
            nextFrameAvailable = true;
            break;

            //Currently waiting, do not copy buffer but present directly
        case WAITING_FOR_FRAME:
            nextFrame = frame;
            nextFrameAvailable = true;
            nextFrameCond.wakeAll();
            break;
    }
    frameLock.unlock();
}

void DetectionTask::stop()
{
    running = false;
    nextFrameCond.wakeAll();
}

void DetectionTask::setProjectionMatrix(QMatrix4x4 m)
{
    m_cv_projectionMatrix=(cv::Mat_<float>(3,3) <<
                           m.operator ()(0,0),m.operator ()(0,1),m.operator ()(0,2),
                           m.operator ()(1,0),m.operator ()(1,1),m.operator ()(1,2),
                           m.operator ()(2,0),m.operator ()(2,1),m.operator ()(2,2));
}

void DetectionTask::setBoards(BoardMap boards)
{
    m_boards.clear();
    m_board_names.clear();
    Q_FOREACH(QString boardName, boards.keys() ){
        cv::aruco::Board* ptr=new cv::aruco::Board();
        ptr->dictionary=boards[boardName].dictionary;
        ptr->ids=boards[boardName].ids;
        ptr->objPoints=boards[boardName].objPoints;
        m_boards.push_back(cv::Ptr<cv::aruco::Board>(ptr));
        m_board_names.push_back(boardName);
    }
}

void DetectionTask::doWork()
{
    running = true;

#ifdef QT_DEBUG
    QElapsedTimer timer;
    float fps = 0.0f;
    int millisElapsed = 0;
    int millis;
    timer.start();
#endif

    frameLock.lock();
    cv::Mat rvec,tvec;
    cv::Matx33d rmat;
    PoseMap poseMap;
    while(running){

        //Process frame if possible
        if(nextFrameAvailable){
            //Signal that we're consuming the next frame now
            nextFrameAvailable = false;
            state = BUSY;

            //Unlock the lock so that we can present a new frame while it's estimating
            frameLock.unlock();
            poseMap.clear();
            cv::aruco::detectMarkers(nextFrame,m_dictionary,m_markerCorners,m_markerIds);
            for(int i=0;i<m_boards.size();i++){
                if(cv::aruco::estimatePoseBoard(m_markerCorners,m_markerIds,m_boards[i],m_cv_projectionMatrix,
                                             m_distCoeff,rvec,tvec)){
                    cv::Rodrigues(rvec, rmat);
                    poseMap[m_board_names[i]]=
                            QMatrix4x4(
                                (qreal)rmat(0,0),    (qreal)rmat(0,1),    (qreal)rmat(0,2),    (qreal)tvec.at<double>(0),
                                -(qreal)rmat(1,0),    -(qreal)rmat(1,1),   -(qreal)rmat(1,2),    -(qreal)tvec.at<double>(1),
                                -(qreal)rmat(2,0),    -(qreal)rmat(2,1),    -(qreal)rmat(2,2),    -(qreal)tvec.at<double>(2),
                                0,                          0,                          0,                          1
                                );
//                    cv::Mat debug=nextFrame.clone();
//                    cv::cvtColor(debug,debug,CV_GRAY2RGB);
//                    cv::aruco::drawAxis(debug,m_cv_projectionMatrix,m_distCoeff,rvec,tvec,10);
//                    cv::imwrite("test.png",debug);
                }

            }

            frameLock.lock();
            emit objectsReady(poseMap);
        }
        //Wait for next frame
        state = WAITING_FOR_FRAME;
        //frameLock.unlock() is performed by wait below
        nextFrameCond.wait(&frameLock);
        //frameLock.lock() is performed by wait above

#ifdef QT_DEBUG
        millis = (int)timer.restart();
        millisElapsed += millis;
        fps = FPS_RATE*fps + (1.0f - FPS_RATE)*(1000.0f/millis);
        if(millisElapsed >= FPS_PRINT_PERIOD){
            //qDebug("Chilitags is running at %f FPS",fps);
            millisElapsed = 0;
        }
#endif
    }
    state = NONE;
    frameLock.unlock();
}
