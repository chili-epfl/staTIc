#include "staticshelper.h"
#include <QtGlobal>
#include <QDebug>
StaticsHelper::StaticsHelper(QObject *parent) : QObject(parent)
{

}

QString StaticsHelper::NameResolution(QString s, Roles input_role, Roles output_role){
    QString plainName;

    if(input_role==output_role && input_role==Roles::FBD_ELEMENT){
        int index=s.lastIndexOf(':');
        if(index<=0){

            qFatal("NameResolution: no token found");
            return QString();
        }
        return s.right(s.size()-index);


    }


    if(input_role==output_role)
        return s;

    if(input_role==Roles::MODEL || input_role==Roles::ENTITY3D){
        plainName=s;
    }
    else{
        int index=s.lastIndexOf('@');
        if(index<=0){

            qFatal("NameResolution: no token found");
            return QString();
        }

        if(s.count('@')>1){
            qWarning()<<"NameResolution: more than one separator found;";
        }

        QString suffix=s.right(s.size()-index-1);
        switch (input_role) {
        case Roles::VIEWMODEL:
            if(suffix.compare("VM")!=0)
                qWarning()<<"NameResolution: suffix doesn't corrispond to input Role";
            plainName= s.left(index);
            break;
        case Roles::FBD:
            if(suffix.compare("FBD")!=0)
                qWarning()<<"NameResolution: suffix doesn't corrispond to input Role";
            plainName= s.left(index);
            break;
        case Roles::F_INTERNAL:
            if(suffix.compare("F_INTERNAL")!=0)
                qWarning()<<"NameResolution: suffix doesn't corrispond to input Role";
            plainName= s.left(index);
            break;
        case Roles::F_REACTION:
            if(suffix.compare("F_REACTION")!=0)
                qWarning()<<"NameResolution: suffix doesn't corrispond to input Role";
            plainName= s.left(index);
           break;
        case Roles::FBD_ELEMENT:
            /*example eeee@FBD:ID*/
            index=suffix.lastIndexOf(':');
            if(index<0){
                qFatal("NameResolution: no token found");
                return QString();
            }
            plainName=suffix.right(suffix.size()-index-1);
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
    case Roles::FBD_ELEMENT:
        return QString(":"+plainName);
        break;
    default:
        qFatal("NameResolution: Invalid conversion");
        return QString();
        break;
    }

}
