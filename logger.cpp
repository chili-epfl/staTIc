#include "logger.h"
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include "static_global.h"

class LoggerPrivate{
public:
    LoggerPrivate(LoggerPrivate const&)=delete;
    void operator=(LoggerPrivate const&) = delete;
    void log(QString action, QVariantMap fields)
    {
        m_mutex.lock();
        m_file.open(QFile::WriteOnly | QFile::Append);
        m_textstream.setDevice(&m_file);
        m_textstream<<"{\n";
        m_textstream<<"\t\"Date\":"<<"\""<<QDateTime::currentDateTime().toString("yyyy.MM.dd.HH.mm.ss.zzz")<<"\",\n";
        m_textstream<<"\t\"Action\":"<<"\""<<action<<"\",\n";
        m_textstream<<"\t\"Fields\":[\n";
        int i=1;
        Q_FOREACH(QString field, fields.keys()){
            m_textstream<<"\t\t{\""<<field<<"\":\""<<fields[field].toString()<<"\"}";
            if(i<fields.keys().size())
                m_textstream<<",\n";
            else
                m_textstream<<"\n";
            i++;
        }
        m_textstream<<"\t]\n";
        m_textstream<<"}\n";
        m_file.close();
        m_mutex.unlock();
    }
    static LoggerPrivate& get_instance(){
        static LoggerPrivate instance;
        return instance;
    }
private:
    LoggerPrivate(){
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
            m_file.setFileName(logFilesPath+log_file_name+".json");
        }

    }
    QFile m_file;
    QMutex m_mutex;
    QTextStream m_textstream;

};



Logger::Logger(QObject *parent): QObject(parent)
{

}

void Logger::log(QString action, QVariantMap fields)
{
    LoggerPrivate::get_instance().log(action,fields);
}

