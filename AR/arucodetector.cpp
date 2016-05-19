#include "arucodetector.h"
#include <QXmlStreamReader>
#include "arucodetectorthread.h"
ArucoDetector::ArucoDetector(QQuickItem *parent):
    QAbstractVideoFilter(parent)
{

    qRegisterMetaType<PoseMap>("PoseMap");

    m_pause=false;
    m_focalLength=700;
    m_cameraResolution=QSizeF(640,480);
//    m_projectionMatrix=QMatrix4x4(
//        m_focalLength,    0,              m_cameraResolution.width()/2, 0 ,
//        0,              m_focalLength,    m_cameraResolution.height()/2, 0,
//        0,              0,              1,  0,
//        0,              0,              0,  1
//                );
#if VIDEOFORMAT == 4_3
    m_projectionMatrix=QMatrix4x4(
                6.1029330646666438e+02, 0., 319.5 ,0,
                0., 6.1038146342831521e+02, 239.5,0,
                0,              0,              1,  0,
                0,              0,              0,  1
                );
#else
    m_projectionMatrix=QMatrix4x4(
                5.7446355001778966e+02, 0., 3.9950000000000000e+02 ,0,
                0., 5.7415636327290406e+02, 2.2350000000000000e+02,0,
                0,              0,              1,  0,
                0,              0,              0,  1
                );
#endif
    loadConfigurationFiles();
    m_time.start();
}

ArucoDetector::~ArucoDetector(){
   qDebug()<<"Destroy";
   //emit destroying();
}

QVideoFilterRunnable *ArucoDetector::createFilterRunnable()
{
    ArucoDetectorThread* thread=new ArucoDetectorThread(this);
    thread->start();
    thread->setPause(m_pause);
    connect(this,SIGNAL(destroying()),thread,SLOT(deleteLater()));
    connect(this,SIGNAL(pauseChanged(bool)),thread,SLOT(setPause(bool)));
    connect(thread,SIGNAL(objectsReady(PoseMap)),this,SLOT(notifyObservers(PoseMap)));
    return thread;
}

QMatrix4x4 ArucoDetector::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

void ArucoDetector::addConfigurationFile(QUrl url)
{

}




void ArucoDetector::registerObserver(ArucoObject *o)
{
    if(o){
        m_observers.insert(o->objectId(),o);
        connect(o,SIGNAL(destroyed(QObject*)),this,SLOT(unregisterDestroyedObserver(QObject*)));
        connect(o,SIGNAL(objectIdChanged(QString)),this,SLOT(updateObserver(QString)));
    }

}

void ArucoDetector::setPause(bool val)
{
    if(val!=m_pause){
        m_pause=val;
        emit pauseChanged(m_pause);
    }
}

void ArucoDetector::unregisterObserver(ArucoObject *o)
{
    m_observers.remove(o->objectId(),o);
}

void ArucoDetector::unregisterDestroyedObserver(QObject *o)
{
    QMultiHash<QString, ArucoObject*>::iterator i =m_observers.begin();
    while (i != m_observers.end()) {
        if(i.value()!=o){
            m_observers.remove(i.key(),i.value());
            break;
        }
        ++i;
    }
}

void ArucoDetector::updateObserver(QString prevObjId)
{
    ArucoObject* o=static_cast<ArucoObject*>(QObject::sender());
    m_observers.remove(prevObjId,o);
    m_observers.insert(o->objectId(),o);
    //TODO: update matrix and visibility
}

void ArucoDetector::notifyObservers(const PoseMap &poses)
{
//    if(m_time.elapsed()>1000){
//        qDebug()<<m_ticks;
//        m_ticks=1;
//        m_time.restart();
//    }else{
//        m_ticks++;
//    }
    Q_FOREACH(QString key, m_observers.keys()){
        if(poses.contains(key)){
            Q_FOREACH(ArucoObject* observer, m_observers.values(key)){
                Pose p=poses[key];
                observer->setPose(p.first,p.second);
                observer->setVisible(true);
            }
        }
        else{
            Q_FOREACH(ArucoObject* observer, m_observers.values(key))
                observer->setVisible(false);
        }
    }

}

void ArucoDetector::loadConfigurationFiles()
{
    QFile configFile(":/AR/board.yml");
    configFile.open(QFile::ReadOnly);
    if(configFile.isOpen()){
        cv::FileStorage fs(configFile.readAll().toStdString(), cv::FileStorage::READ | cv::FileStorage::MEMORY);
        cv::aruco::Board board;
        board.dictionary=cv::aruco::getPredefinedDictionary(
                    cv::aruco::PREDEFINED_DICTIONARY_NAME((int)fs["dictionary"]));
        fs["ids"]>>board.ids;
        std::vector<cv::Point3f> points;
        fs["points"]>>points;
        for(int i=0;i<points.size();i++){
            if(i%4==0) board.objPoints.push_back(std::vector<cv::Point3f>());
            board.objPoints[i/4].push_back(points[i]);
        }
        fs.release();
        m_boards["Default"]=board;
//        for(int i=0;i<test.ids.size();i++)
//            std::cout<<test.ids.at(i)<<"\n";
//        for(int i=0;i<test.objPoints.size();i++)
//            for(int j=0;j<test.objPoints.at(i).size();j++)
//                std::cout<<test.objPoints.at(i).at(j)<<"\n";

//        QXmlStreamReader xmlReader;//(configFile);
//        while (!xmlReader.atEnd()) {
//              xmlReader.readNext();
//        }
//        if (xmlReader.hasError()) {
//            qDebug()<<"Reading error:"<<xmlReader.errorString()<<
//                      "/n At line:"<<xmlReader.lineNumber();
//        }
    }else{
        qDebug()<<"Can't open file :/AR/board.yml";
    }
    configFile.close();

    configFile.setFileName(":/load.yml");
    configFile.open(QFile::ReadOnly);
    if(configFile.isOpen()){
        cv::FileStorage fs(configFile.readAll().toStdString(), cv::FileStorage::READ | cv::FileStorage::MEMORY);
        cv::aruco::Board board;
        board.dictionary=cv::aruco::getPredefinedDictionary(
                    cv::aruco::PREDEFINED_DICTIONARY_NAME((int)fs["dictionary"]));
        fs["ids"]>>board.ids;
        std::vector<cv::Point3f> points;
        fs["points"]>>points;
        for(int i=0;i<points.size();i++){
            if(i%4==0) board.objPoints.push_back(std::vector<cv::Point3f>());
            board.objPoints[i/4].push_back(points[i]);
        }
        fs.release();
        m_boards["load"]=board;
//        for(int i=0;i<test.ids.size();i++)
//            std::cout<<test.ids.at(i)<<"\n";
//        for(int i=0;i<test.objPoints.size();i++)
//            for(int j=0;j<test.objPoints.at(i).size();j++)
//                std::cout<<test.objPoints.at(i).at(j)<<"\n";

//        QXmlStreamReader xmlReader;//(configFile);
//        while (!xmlReader.atEnd()) {
//              xmlReader.readNext();
//        }
//        if (xmlReader.hasError()) {
//            qDebug()<<"Reading error:"<<xmlReader.errorString()<<
//                      "/n At line:"<<xmlReader.lineNumber();
//        }
    }else{
        qDebug()<<"Can't open file :/load.yml";
    }
    configFile.close();
    /**/
    configFile.setFileName(":/AR/singleTags.yml");
    configFile.open(QFile::ReadOnly);
    if(configFile.isOpen()){
        cv::FileStorage fs(configFile.readAll().toStdString(), cv::FileStorage::READ | cv::FileStorage::MEMORY);
        cv::FileNode n=fs["tags"];
        if (n.type() != cv::FileNode::SEQ)
        {
            qDebug() << "Cannot read tag list";
            return ;
        }
        cv::FileNodeIterator it = n.begin(), it_end = n.end(); // Go through the node
        for (; it != it_end; ++it){
            SingleTag tag;
            tag.id=(int)(*it)["id"];
            tag.size=(double)(*it)["size"];
            m_singleTagList.append(tag);
        }
    }else{
        qDebug()<<"Can't open file :/AR/singleMarkers.yml";
    }
}

