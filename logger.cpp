#include "logger.h"
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include "static_global.h"
#include <QtConcurrent>


class LoggerPrivate{
public:
    LoggerPrivate(LoggerPrivate const&)=delete;    
    void operator=(LoggerPrivate const&) = delete;
    ~LoggerPrivate(){
        close_logger();
    }
    void log(QDateTime date,QString action, QVariantMap fields)
    {
        if(m_file.isOpen()){
            m_mutex.lock();
            //        m_file.open(QFile::WriteOnly | QFile::Append);
            //        m_textstream.setDevice(&m_file);
            if(init_file){
                m_textstream<<"[\n";
                init_file=false;
            }else{
                m_textstream<<",\n";
            }
            m_textstream<<"{\n";
            m_textstream<<"\t\"Date\":"<<"\""<<date.toString("yyyy.MM.dd.HH.mm.ss.zzz")<<"\",\n";
            m_textstream<<"\t\"Action\":"<<"\""<<action<<"\",\n";
            m_textstream<<"\t\"Fields\":[\n";
            int i=1;
            Q_FOREACH(QString field, fields.keys()){
                if(fields[field].canConvert(QVariant::List)){
                    m_textstream<<"\t\t{\""<<field<<"\":[";
                    int j=1;
                    Q_FOREACH(QString s, fields[field].toStringList()){
                        m_textstream<<"\""<<s<<"\"";
                        if(j<fields[field].toStringList().length())
                            m_textstream<<",";
                        j++;
                    }
                    m_textstream<<"]}";
                }
                else
                    m_textstream<<"\t\t{\""<<field<<"\":\""<<fields[field].toString()<<"\"}";

                if(i<fields.keys().size())
                    m_textstream<<",\n";
                else
                    m_textstream<<"\n";
                i++;
            }
            m_textstream<<"\t]\n";
            m_textstream<<"}\n";
            m_textstream.flush();
            m_mutex.unlock();
        }
    }
    void log_static_configuration(QDateTime date,QVariantMap fields){
        if(m_file_static_conf.isOpen()){
            m_mutex4static_conf.lock();

            if(init_file4static_conf){
                m_textstream4static_conf<<"[\n";
                init_file4static_conf=false;
            }else{
                m_textstream4static_conf<<",\n";
            }
            m_textstream4static_conf<<"{\n";
            m_textstream4static_conf<<"\t\"Date\":"<<"\""<<date.toString("yyyy.MM.dd.HH.mm.ss.zzz")<<"\",\n";

            m_textstream4static_conf<<"\t\"Status\":\""<<fields["status"].toString()<<"\",\n";
            m_textstream4static_conf<<"\t\"Gravity\":\""<<fields["gravity"].toString()<<"\",\n";
            m_textstream4static_conf<<"\t\"n_joints\":\""<<fields["n_joints"].toString()<<"\",\n";
            m_textstream4static_conf<<"\t\"n_beams\":\""<<fields["n_beams"].toString()<<"\",\n";
            m_textstream4static_conf<<"\t\"n_loads\":\""<<fields["n_loads"].toString()<<"\",\n";

            m_textstream4static_conf<<"\t\"Joints\":[\n";
            QVariantList joints=fields["joints"].value<QVariantList>();
            for(int i=0; i<joints.length();i++){
                QVariantMap joint=joints.at(i).value<QVariantMap>();
                int j=1;
                m_textstream4static_conf<<"\t\t\t{\n";
                Q_FOREACH(QString field, joint.keys()){
                    m_textstream4static_conf<<"\t\t\t\""<<field<<"\":\""<<joint[field].toString()<<"\"";
                    if(j<joint.keys().length())
                        m_textstream4static_conf<<",\n";
                    else
                        m_textstream4static_conf<<"\n";
                    j++;
                }
                if(i<joints.length()-1)
                    m_textstream4static_conf<<"\t\t\t},\n";
                else
                    m_textstream4static_conf<<"\t\t\t}\n";
            }
            m_textstream4static_conf<<"\t],\n";


            m_textstream4static_conf<<"\t\"Beams\":[\n";
            QVariantList beams=fields["beams"].value<QVariantList>();
            for(int i=0; i<beams.length();i++){
                QVariantMap beam=beams.at(i).value<QVariantMap>();
                int j=1;
                m_textstream4static_conf<<"\t\t\t{\n";
                Q_FOREACH(QString field, beam.keys()){
                    m_textstream4static_conf<<"\t\t\t\""<<field<<"\":\""<<beam[field].toString()<<"\"";
                    if(j<beam.keys().length())
                        m_textstream4static_conf<<",\n";
                    else
                        m_textstream4static_conf<<"\n";
                    j++;
                }
                if(i<beams.length()-1)
                    m_textstream4static_conf<<"\t\t\t},\n";
                else
                    m_textstream4static_conf<<"\t\t\t}\n";
            }
            m_textstream4static_conf<<"\t],\n";


            m_textstream4static_conf<<"\t\"Loads\":[\n";
            QVariantList loads=fields["loads"].value<QVariantList>();
            for(int i=0; i<loads.length();i++){
                QVariantMap load=loads.at(i).value<QVariantMap>();
                int j=1;
                m_textstream4static_conf<<"\t\t\t{\n";
                Q_FOREACH(QString field, load.keys()){
                    m_textstream4static_conf<<"\t\t\t\""<<field<<"\":\""<<load[field].toString()<<"\"";
                    if(j<load.keys().length())
                        m_textstream4static_conf<<",\n";
                    else
                        m_textstream4static_conf<<"\n";
                    j++;
                }
                if(i<loads.length()-1)
                    m_textstream4static_conf<<"\t\t\t},\n";
                else
                    m_textstream4static_conf<<"\t\t\t}\n";
            }
            m_textstream4static_conf<<"\t]\n";

            m_textstream4static_conf<<"}\n";

            m_textstream4static_conf.flush();
            m_mutex4static_conf.unlock();
        }

    }
    void log_position(QDateTime date , QString name, QVector3D translation, QVector3D angles){
        if(prev_rotations.contains(name)){
            QVector3D prev_rot=prev_rotations[name];
            if(translation.distanceToPoint(prev_translations[name])<25 &&
                    fabs(prev_rot.x()-angles.x())<5 &&
                    fabs(prev_rot.y()-angles.y())<5 &&
                    fabs(prev_rot.z()-angles.z())<5){
                prev_translations[name]=translation;
                prev_rotations[name]=angles;
                return;
            }
        }

        prev_translations[name]=translation;
        prev_rotations[name]=angles;


        if(m_file_positions.isOpen()){
            m_mutex4positions.lock();

            if(init_file4positions){
                m_textstream4file_positions<<"[\n";
                init_file4positions=false;
            }else{
                m_textstream4file_positions<<",\n";
            }
            m_textstream4file_positions<<"{\n";
            m_textstream4file_positions<<"\t\"Date\":"<<"\""<<date.toString("yyyy.MM.dd.HH.mm.ss.zzz")<<"\",\n";
            m_textstream4file_positions<<"\t\"Object\":\""<<name<<"\",\n";
            m_textstream4file_positions<<"\t\"Translation\":\"("<<translation.x()<<","<<
                                         translation.y()<<","<<translation.z()<<")\",\n";
            m_textstream4file_positions<<"\t\"Rotation\":\"("<<angles.x()<<","<<
                                         angles.y()<<","<<angles.z()<<")\"\n";
            m_textstream4file_positions<<"}\n";
            m_textstream4file_positions.flush();
            m_mutex4positions.unlock();
        }

    }

    void restart_logger(){
        close_logger();       
        QDir localDir;
        bool directoryAvailable=localDir.exists(logFilesPath);
        if(!directoryAvailable){
            if(!localDir.mkpath(logFilesPath)){
                directoryAvailable=false;
                qWarning()<<"Cannot create directory of the logs.";
            }
            else
                directoryAvailable=true;
        }
        if(directoryAvailable) {
            QString log_file_name=QDateTime::currentDateTime().toString("dMyyhms");
            m_file.setFileName(logFilesPath+"user_actions_"+log_file_name+".json");
            m_file_positions.setFileName(logFilesPath+"positions_"+log_file_name+".json");
            m_file_static_conf.setFileName(logFilesPath+"static_config_"+log_file_name+".json");

        }
        init_file=true;
        if(!m_file.open(QFile::WriteOnly))
            qDebug()<<"Cannot open file"<<m_file.fileName();
        else
            m_textstream.setDevice(&m_file);

        if(!m_file_positions.open(QFile::WriteOnly))
            qDebug()<<"Cannot open file"<<m_file_positions.fileName();
        else
            m_textstream4file_positions.setDevice(&m_file_positions);

        if(!m_file_static_conf.open(QFile::WriteOnly))
            qDebug()<<"Cannot open file"<<m_file_static_conf.fileName();
        else
            m_textstream4static_conf.setDevice(&m_file_static_conf);

        init_file4positions=true;
        init_file4static_conf=true;
        prev_rotations.clear();
        prev_translations.clear();
    }
    void close_logger(){
        if(m_file.isOpen()){
            m_mutex.lock();
            m_textstream<<"\n]";
            m_textstream.flush();
            m_file.close();
            m_mutex.unlock();
        }

        if(m_file_positions.isOpen()){
            m_mutex4positions.lock();
            m_textstream4file_positions<<"\n]";
            m_textstream4file_positions.flush();
            m_file_positions.close();
            m_mutex4positions.unlock();
        }

        if(m_file_static_conf.isOpen()){
            m_mutex4static_conf.lock();
            m_textstream4static_conf<<"\n]";
            m_textstream4static_conf.flush();
            m_file_static_conf.close();
            m_mutex4static_conf.unlock();
        }
    }

    static LoggerPrivate& get_instance(){
        static LoggerPrivate instance;
        return instance;
    }

private:
    LoggerPrivate(){
        //restart_logger();
    }



    QFile m_file;
    bool init_file;
    QMutex m_mutex;
    QTextStream m_textstream;

    QFile m_file_positions,m_file_static_conf;
    bool init_file4positions,init_file4static_conf;
    QTextStream m_textstream4file_positions, m_textstream4static_conf;
    QMutex m_mutex4positions,m_mutex4static_conf;

    QHash<QString,QVector3D> prev_translations;
    QHash<QString,QVector3D> prev_rotations;

};


Logger::Logger(QObject *parent): QObject(parent)
{

}

Logger::~Logger()
{

}

void Logger::log(QString action, QVariantMap fields)
{
    QtConcurrent::run(&LoggerPrivate::get_instance(),&LoggerPrivate::log,QDateTime::currentDateTime(),action,fields);
}

void Logger::log_static_configuration(QVariantMap fields)
{
    QtConcurrent::run(&LoggerPrivate::get_instance(),&LoggerPrivate::log_static_configuration,QDateTime::currentDateTime(),fields);
}

void Logger::log_position(QString name, QVector3D translation, qreal angle_x,qreal angle_y,qreal angle_z)
{
    QVector3D angles(angle_x,angle_y,angle_z);
    QtConcurrent::run(&LoggerPrivate::get_instance(),&LoggerPrivate::log_position,QDateTime::currentDateTime(),name,translation,angles);
}

void Logger::restart_logger()
{
    QtConcurrent::run(&LoggerPrivate::get_instance(),&LoggerPrivate::restart_logger);

}

void Logger::close_logger()
{
    QtConcurrent::run(&LoggerPrivate::get_instance(),&LoggerPrivate::close_logger);

}
