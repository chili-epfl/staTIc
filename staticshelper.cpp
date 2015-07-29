#include "staticshelper.h"
#include <QtGlobal>
#include <QDebug>
StaticsHelper::StaticsHelper(QObject *parent) : QObject(parent)
{

}

QString StaticsHelper::NameResolution(QString s, Roles input_role, Roles output_role){
    QString plainName;

    if(input_role==output_role)
        return s;

    if(input_role==Roles::MODEL || input_role==Roles::ENTITY3D){
        plainName=s;
    }
    else{
        int index=s.lastIndexOf('@');
        if(index<0){

            qFatal("NameResolution: no token found");
            return QString();
        }

        if(s.count('@')>1){
            qWarning()<<"NameResolution: more than one separator found;";
        }
        plainName= s.left(index);

        QString suffix=s.right(s.size()-index-1);
        switch (input_role) {
        case Roles::VIEWMODEL:
            if(suffix.compare("VM")!=0)
                qWarning()<<"NameResolution: suffix doesn't corrispond to input Role";
            break;
        case Roles::FBD:
            if(suffix.compare("FBD")!=0)
                qWarning()<<"NameResolution: suffix doesn't corrispond to input Role";
            break;
        case Roles::F_INTERNAL:
            if(suffix.compare("F_INTERNAL")!=0)
                qWarning()<<"NameResolution: suffix doesn't corrispond to input Role";
            break;
        case Roles::F_REACTION:
            if(suffix.compare("F_REACTION")!=0)
                qWarning()<<"NameResolution: suffix doesn't corrispond to input Role";
           break;
        default:
            break;
        }

    }

    switch (output_role) {
    case Roles::ENTITY3D:
        return plainName;
        break;
    case Roles::MODEL:
        return plainName;
    case Roles::VIEWMODEL:
        return QString(plainName+"@VM");
        break;
    case Roles::FBD:
        return QString(plainName+"@FBD");
        break;
    case Roles::F_INTERNAL:
        return QString(plainName+"@F_INTERNAL");
        break;
    case Roles::F_REACTION:
        return QString(plainName+"@F_REACTION");
       break;
    default:
        qFatal("NameResolution: Invalid conversion");
        return QString();
        break;
    }

}
