#include "twodimensionalstaticsmodule.h"
#include <QDebug>

TwoDimensionalStaticsModule::TwoDimensionalStaticsModule()
    :AbstractStaticsModule()
{
}
TwoDimensionalStaticsModule::TwoDimensionalStaticsModule(QString path)
    :AbstractStaticsModule(path)
{
    readStructure(path);

}

bool TwoDimensionalStaticsModule::readStructure(QString path){

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning("Structure file not loaded");
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts=line.split(",");
        if(parts.size()==0) continue;
        if(parts.at(0).compare("j")==0) {
            if(parts.size()<5) {qWarning("Structure file incorrect format"); return false;}
            Joint* joint=new Joint(parts.at(4));
            joint->setPosition(QVector3D( parts.at(1).toFloat(), parts.at(2).toFloat(), parts.at(3).toFloat()));
            if(parts.size()<6)
                joint->setSupport(Joint::NOSUPPORT);
            else if(parts.at(5).compare("fixed",Qt::CaseInsensitive)==0){
                joint->setSupport(Joint::FIXED);
                supportJoints.append(joint);
            }
            else if(parts.at(5).compare("rolling",Qt::CaseInsensitive)==0){
                joint->setSupport(Joint::ROLLING);
                supportJoints.append(joint);
            }
            joints.append(joint);
        }
        else if(parts.at(0).compare("m")==0){
            if(parts.size()<4) {qWarning("Structure file incorrect format"); return false;}
            Beam* beam=new Beam(parts.at(3));
            Joint* extreme1=joints.at(parts.at(1).toInt());
            Joint* extreme2=joints.at(parts.at(2).toInt());
            beam->extremes.first=extreme1;
            beam->extremes.second=extreme2;
            beams.append(beam);
        }
    }

    status=LOADED;

    check_stability();
    update();

    return true;

}

void TwoDimensionalStaticsModule::solve(){
    cv::Mat external_forces_matrix=cv::Mat::zeros(2*joints.size(),1,CV_32F);
    int i=0;
    for(Joint* joint : joints){
        for(Force* force :force_list){
            if(force->applicationElement.compare(joint->objectName())==0){
                external_forces_matrix.at<float>(i,0)=external_forces_matrix.at<float>(i,0)+force->vector.x();
                external_forces_matrix.at<float>(i+1,0)=external_forces_matrix.at<float>(i+1,0)+force->vector.y();
                //external_forces_matrix.at<float>(i+2,0)=external_forces_matrix.at<float>(i+2,0)+force.second.z();
            }
        }
        i+=2;
    }

    cv::Mat solution;
    bool solved=cv::solve(internalF_matrix,external_forces_matrix,solution);
    /*cv::MatIterator_<float> _it = internalF_matrix.begin<float>();
    i=0;
    QString text;
    for(;_it!=internalF_matrix.end<float>(); _it++){
        if(i%6==0){ qDebug()<<text;text.clear();}
        text.append( QString::number(*_it) );
        text.append(" ");
        i++;
    }
    qDebug()<<text;
    */
    i=0;
    for(Beam* beam: beams){
        beam->setAxialForce(solution.at<float>(i,0));
        i++;
    }
    for(Joint* support: supportJoints){
        if(support->getSupport()==Joint::FIXED){
            support->setReaction(QVector3D(solution.at<float>(i,0),solution.at<float>(i+1,0),0));
            i+=2;
        }
        else if(support->getSupport()==Joint::ROLLING){
            support->setReaction(QVector3D(0,solution.at<float>(i,0),0));
            i+=1;
        }
    }

}


void TwoDimensionalStaticsModule::update(){

    /*Check stability*/
    if(stability!=Stability::DETERMINATE) return;

    /*Compute reactions*/
    int n_reactions=0;
    for(Joint* joint: joints){
        if(joint->getSupport()==Joint::FIXED)
            n_reactions+=2;
        else if(joint->getSupport()==Joint::ROLLING)
            n_reactions+=1;
    }


    internalF_matrix=cv::Mat::zeros(2*joints.size(),beams.size()+n_reactions,CV_32F);
    int i=0;
    for(Joint* joint : joints){
        int j=0;
        for(Beam* beam: beams){
            if(beam->extremes.first==joint || beam->extremes.second==joint){
                QVector3D firstPosition=beam->extremes.first->getPosition();
                QVector3D secondPosition=beam->extremes.second->getPosition();
                QVector3D dir;
                if(beam->extremes.first==joint)
                    dir=firstPosition-secondPosition;
                else
                    dir=secondPosition-firstPosition;
                dir.normalize();
                internalF_matrix.at<float>(i,j)=dir.x();
                internalF_matrix.at<float>(i+1,j)=dir.y();
            }
            j++;
        }
        for(Joint* support :supportJoints){
            //TODO: supports can have orientation
            if(support->getSupport()==Joint::FIXED){
                if(support==joint){
                    internalF_matrix.at<float>(i,j)=1.0f;
                    internalF_matrix.at<float>(i+1,j+1)=1.0f;
                }
                j+=2;
            }
            else if(support->getSupport()==Joint::ROLLING){
                if(support==joint){
                    internalF_matrix.at<float>(i+1,j)=1.0f;
                }
                j+=1;
            }
        }

        i+=2;
    }
}

void TwoDimensionalStaticsModule::check_stability(){
    int n_beams=beams.size();
    int n_joints=joints.size();

    int n_reactions=0;

    for(Joint* joint: joints){
        if(joint->getSupport()==Joint::FIXED)
            n_reactions+=2;
        else if(joint->getSupport()==Joint::ROLLING)
            n_reactions+=1;
    }

    if (2*n_joints==n_beams+n_reactions){
        stability=Stability::DETERMINATE;
    }
    else if(2*n_joints<n_beams+n_reactions){
        stability=Stability::UNSTABLE;

    }
    else
        stability=Stability::INDETERMINATE;
}

AbstractElement* TwoDimensionalStaticsModule::getElementbyName(QString name){
    for(Joint* joint : joints)
        if(joint->objectName().compare(name)==0)
            return joint;
    for(Beam* beam : beams)
        if(beam->objectName().compare(name)==0)
            return beam;

    return NULL;
}
