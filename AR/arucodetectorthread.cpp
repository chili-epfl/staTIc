#include "arucodetectorthread.h"
#include "arucodetector.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <QMatrix4x4>
#include <iostream>
#include <algorithm>
ArucoDetectorThread::ArucoDetectorThread(ArucoDetector* detector,QObject *parent):
    QObject(parent)
{
    m_detector=detector;
    m_pause=false;
    task = new DetectionTask(m_detector->getProjectionMatrix());
    task->setBoards(m_detector->getBoards());
    task->setSingleTagList(m_detector->singleTagList());
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
    m_distCoeff=(cv::Mat_<float>(1,5) <<
                 4.1105624753282284e-02, 7.9058604507765057e-02,
                       -2.7890683219554159e-03, 2.8142764725885486e-04,
                       -4.0278534170454272e-01);
    setProjectionMatrix(projectionMatrix);
    m_dictionary= cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_1000);
    m_detector_params=cv::aruco::DetectorParameters::create();
    m_detector_params->polygonalApproxAccuracyRate=0.04;
    m_detector_params->doCornerRefinement=false;
    //m_detector_params->adaptiveThreshWinSizeMax=3;
}

DetectionTask::~DetectionTask()
{
    qDebug()<<"Destroy task";
    Q_FOREACH(LinearKalmanFilter* f, m_LKFilters.values())
        delete f;
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

void DetectionTask::setSingleTagList(QList<SingleTag> list)
{
    Q_FOREACH(SingleTag t,list){
        m_singleTagSizes[t.id]=t.size;
    }
}

void DetectionTask::doWork()
{
    running = true;

#ifdef QT_DEBUG
    QElapsedTimer timer;
    float fps = 0.0f;
    long millisElapsed = 0;
    long millis;
    timer.start();
#endif

    frameLock.lock();
    cv::Mat rvec(3, 1, CV_64F);
    cv::Mat tvec(3, 1, CV_64F);
    cv::Mat rmat,lap,lap_ROI;
    cv::Mat prev_Image;

    qreal norm;
    cv::Scalar mu, sigma;
    float roll,pitch,yaw;
    QQuaternion measur_quad;
    PoseMap poseMap;
    int min_x,min_y,max_x,max_y;
    std::vector<uchar> status;
    std::vector<float> err;
    QHash<int,QList<double> > sharpness_tags;
    QList<double> sort_list;
    double focusMeasure;
    while(running){

        //Process frame if possible
        if(nextFrameAvailable){
            //Signal that we're consuming the next frame now
            nextFrameAvailable = false;
            state = BUSY;

            //Unlock the lock so that we can present a new frame while it's estimating
            frameLock.unlock();
            poseMap.clear();
//            if(!prev_Image.empty() && m_markerIds_prev.size()>0){
//                cv::calcOpticalFlowPyrLK(prev_Image,nextFrame,m_markerCorners_prev,m_tracked_corners,status,err);
//            }
            cv::aruco::detectMarkers(nextFrame,m_dictionary,m_markerCorners,m_markerIds,m_detector_params);
//            bool skip=false;
//            for(int i=0;i<m_markerIds_prev.size();i++){
//                if(status[i*4]==0 || status[i*4+1]==0
//                   || status[i*4+2]==0 || status[i*4+2]==0  ) continue;
//                if(m_tracked_corners[i*4].x<=0 || m_tracked_corners[i*4].x>=nextFrame.cols
//                        ||m_tracked_corners[i*4].y<=0 || m_tracked_corners[i*4].y>=nextFrame.rows
//                        || m_tracked_corners[i*4+1].x<=0 || m_tracked_corners[i*4+1].x>=nextFrame.cols
//                        ||m_tracked_corners[i*4+1].y<=0 || m_tracked_corners[i*4+1].y>=nextFrame.rows
//                        || m_tracked_corners[i*4+2].x<=0 || m_tracked_corners[i*4+2].x>=nextFrame.cols
//                        ||m_tracked_corners[i*4+2].y<=0 || m_tracked_corners[i*4+2].y>=nextFrame.rows
//                        || m_tracked_corners[i*4+3].x<=0 || m_tracked_corners[i*4+3].x>=nextFrame.cols
//                        ||m_tracked_corners[i*4+3].y<=0 || m_tracked_corners[i*4+3].y>=nextFrame.rows) continue;
//                skip=false;
//                for(int j=0;j<m_markerIds.size();j++){
//                    if(m_markerIds[j]==m_markerIds_prev[i]){
//                        skip=true;
//                        break;
//                    }
//                }
//                if(skip) continue;
//                m_markerIds.push_back(m_markerIds_prev[i]);
//                std::vector<cv::Point2f> tmp;
//                tmp.push_back(m_tracked_corners[i*4]);
//                tmp.push_back(m_tracked_corners[i*4+1]);
//                tmp.push_back(m_tracked_corners[i*4+2]);
//                tmp.push_back(m_tracked_corners[i*4+3]);
//                m_markerCorners.push_back(tmp);
//            }
            cv::Laplacian(nextFrame,lap,CV_32F);
            for(int i=0;i<m_markerIds.size();i++){
                min_x=10000000;
                min_y=10000000;
                max_x=-1;
                max_y=-1;
                for(int j=0;j<m_markerCorners[i].size();j++){
                    if(m_markerCorners[i][j].x>max_x)
                        max_x=m_markerCorners[i][j].x;
                    if(m_markerCorners[i][j].x<min_x)
                        min_x=m_markerCorners[i][j].x;
                    if(m_markerCorners[i][j].y>max_y)
                        max_y=m_markerCorners[i][j].y;
                    if(m_markerCorners[i][j].y<min_y)
                        min_y=m_markerCorners[i][j].y;
                }
                lap_ROI=lap(cv::Rect(min_x,min_y,max_x-min_x,max_y-min_y));
                cv::meanStdDev(lap_ROI, mu, sigma);
                focusMeasure = sigma.val[0]*sigma.val[0];
                sharpness_tags[m_markerIds[i]].append(focusMeasure);

                if(sharpness_tags[m_markerIds[i]].size()>16)
                    sharpness_tags[m_markerIds[i]].removeFirst();
                sort_list.clear();
                sort_list.append(sharpness_tags[m_markerIds[i]]);
                std::sort(sort_list.begin(),sort_list.end());
                if(sort_list.size()==16 && focusMeasure>(sort_list.at(12)+1.5*(sort_list.at(12)-sort_list.at(3))) ){
                    m_markerIds[i]=502100;
                }
            }

            for(int i=0;i<m_markerIds.size();i++){
                if(m_singleTagSizes.contains(m_markerIds[i])){
                    std::vector< std::vector<cv::Point2f> > corners;
                    std::vector< cv::Vec3d > rvecs,tvecs;
                    corners.push_back(m_markerCorners[i]);
                    cv::aruco::estimatePoseSingleMarkers(corners,m_singleTagSizes[m_markerIds[i]],m_cv_projectionMatrix,m_distCoeff,
                            rvecs,tvecs);
                    norm=cv::norm(rvecs[0]);
                    measur_quad=QQuaternion::fromAxisAndAngle(rvecs[0].operator [](0)/norm,rvecs[0].operator [](1)/norm,rvecs[0].operator [](2)/norm,180.0*norm/CV_PI);

                    if(!m_use_filter){
                        poseMap[QString::number(m_markerIds[i])]=Pose(QVector3D((qreal)tvecs[0].operator [](0),-(qreal)tvecs[0].operator [](1),-(qreal)tvecs[0].operator [](2)),
                                                       m_rotationOpencvToOpenGL* measur_quad);

                    }else{
                        QString string_id=QString::number(m_markerIds[i]);
                        LinearKalmanFilter* filter;

                        if(!m_LKFilters.contains(string_id)){
                            filter=new LinearKalmanFilter();
                            m_LKFilters[string_id]=filter;
                        }
                        else
                            filter=m_LKFilters[string_id];

                        tvec.at <double>(0) = tvecs[0].operator [](0);
                        tvec.at <double>(1) = tvecs[0].operator [](1);
                        tvec.at <double>(2) = tvecs[0].operator [](2);


                        measur_quad.getEulerAngles(&pitch,&yaw,&roll);

                        rvec.at <double>(0) = pitch;
                        rvec.at <double>(1) = yaw;
                        rvec.at <double>(2) = roll;

                        filter->fillMeasurements(tvec,rvec);
                        filter->updateKalmanFilter(tvec,rvec);
                        poseMap[QString::number(m_markerIds[i])]=Pose(QVector3D((qreal)tvecs[0].operator [](0),-(qreal)tvecs[0].operator [](1),-(qreal)tvecs[0].operator [](2)),
                                                       m_rotationOpencvToOpenGL* measur_quad.fromEulerAngles(rvec.at <double>(0),rvec.at <double>(1),rvec.at <double>(2)));
                    }
                }
                else{
                    QString string_id=QString::number(m_markerIds[i]);
                    if(m_LKFilters.contains(string_id)){
                        m_LKFilters[string_id]->updateKalmanFilter(tvec,rvec);
                    }
                }
            }

            for(int i=0;i<m_boards.size();i++){
                float err;
                if(cv::aruco::estimatePoseBoard(m_markerCorners,m_markerIds,m_boards[i],m_cv_projectionMatrix,
                                             m_distCoeff,rvec,tvec)){
                    norm=cv::norm(rvec);
                    measur_quad=QQuaternion::fromAxisAndAngle(rvec.at<double>(0)/norm,rvec.at<double>(1)/norm,rvec.at<double>(2)/norm,180.0*norm/CV_PI);
                    if(!m_use_filter){
                        poseMap[m_board_names[i]]=Pose(QVector3D((qreal)tvec.at<double>(0),-(qreal)tvec.at<double>(1),-(qreal)tvec.at<double>(2)),
                                                       m_rotationOpencvToOpenGL*measur_quad);
                    }
                    else{
                        LinearKalmanFilter* filter;

                        if(!m_LKFilters.contains(m_board_names[i])){
                            filter=new LinearKalmanFilter();
                            m_LKFilters[m_board_names[i]]=filter;
                        }
                        else
                            filter=m_LKFilters[m_board_names[i]];

                        measur_quad.getEulerAngles(&pitch,&yaw,&roll);

                        rvec.at <double>(0) = pitch;
                        rvec.at <double>(1) = yaw;
                        rvec.at <double>(2) = roll;

                        filter->fillMeasurements(tvec,rvec);
                        filter->updateKalmanFilter(tvec,rvec);
                        poseMap[m_board_names[i]]= Pose(QVector3D((qreal)tvec.at<double>(0),-(qreal)tvec.at<double>(1),-(qreal)tvec.at<double>(2)),
                                                        m_rotationOpencvToOpenGL*measur_quad.fromEulerAngles(rvec.at <double>(0),rvec.at <double>(1),rvec.at <double>(2)));

                    }
                    //qDebug()<< poseMap[m_board_names[i]];
//                    cv::Mat debug=nextFrame.clone();
//                    cv::cvtColor(debug,debug,CV_GRAY2RGB);
//                    cv::aruco::drawAxis(debug,m_cv_projectionMatrix,m_distCoeff,rvec,tvec,10);
//                    cv::imwrite("test.png",debug);
                }
                else{
                    if(m_LKFilters.contains(m_board_names[i])){
                        m_LKFilters[m_board_names[i]]->updateKalmanFilter(tvec,rvec);
                    }
                }
            }
//            prev_Image=nextFrame;
//            m_markerIds_prev=m_markerIds;
//            m_markerCorners_prev.clear();
//            for(int i=0;i<m_markerCorners.size();i++){
//                m_markerCorners_prev.push_back(m_markerCorners[i][0]);
//                m_markerCorners_prev.push_back(m_markerCorners[i][1]);
//                m_markerCorners_prev.push_back(m_markerCorners[i][2]);
//                m_markerCorners_prev.push_back(m_markerCorners[i][3]);
//            }
            frameLock.lock();
            emit objectsReady(poseMap);
        }
        //Wait for next frame
        state = WAITING_FOR_FRAME;
        //frameLock.unlock() is performed by wait below
        nextFrameCond.wait(&frameLock);
        //frameLock.lock() is performed by wait above

#ifdef QT_DEBUG
        millis = (long)timer.restart();
        millisElapsed += millis;
        if(millis>0){
            fps = FPS_RATE*fps + (1.0f - FPS_RATE)*(1000.0f/millis);
            if(millisElapsed >= FPS_PRINT_PERIOD){
                //qDebug("Aruco is running at %f FPS",fps);
                millisElapsed = 0;
            }
        }
#endif
    }
    state = NONE;
    frameLock.unlock();
}
