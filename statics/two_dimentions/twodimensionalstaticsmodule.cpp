#include "statics/two_dimentions/twodimensionalstaticsmodule.h"
#include "statics/elements/nodeload.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"

TwoDimensionalStaticsModule::TwoDimensionalStaticsModule(QObject *parent )
    :AbstractStaticsModule(parent)
{

}

Beam* TwoDimensionalStaticsModule::createBeam(Joint* extreme1,Joint* extreme2,QString name,
                                 qreal Ax, qreal Asy, qreal Asz, qreal Jx,
                                 qreal Iy, qreal Iz, qreal E, qreal G,
                                 qreal p, qreal d){
    Beam* beam=new Beam(name,this);
    QPair<Joint*, Joint*> extremes;

    extremes.first=extreme1;
    extremes.second=extreme2;

    extremes.first->connected_beams.append(beam);
    extremes.second->connected_beams.append(beam);
    /*IMPORTANT: the order is important because is actually beam that is triggering the signal
    connectedBeamsChange in the joint class*/
    beam->setExtremes(extremes);

    m_beams.append(beam);

    connect(beam,SIGNAL(extremesChanged()),this,SLOT(update()));
    return beam;

}
Joint* TwoDimensionalStaticsModule::createJoint(QVector3D position, QString name,
                                    bool  support_X,bool support_Y,bool support_Z,
                                    bool support_XX,bool support_YY,bool support_ZZ){

    Joint* joint=new Joint(name,this);
    joint->setPosition(position);
    joint->setSupport( support_X,support_Y,support_Z, support_XX,support_YY,support_ZZ);
    m_joints.append(joint);

    connect(joint,SIGNAL(positionChanged(QVector3D)),this,SLOT(update()));
    connect(joint,SIGNAL(reactionChanged(QVector3D)),this,SLOT(update()));
    connect(joint,SIGNAL(supportChanged()),this,SLOT(update()));

    return joint;
}

NodeLoad* TwoDimensionalStaticsModule::createNodeLoad(QVector3D force, Joint* joint,QString name){
    NodeLoad* nodeLoad=new NodeLoad(name,this);
    nodeLoad->setForce(force);
    m_node_loads.append(nodeLoad);
    connect(nodeLoad,SIGNAL(forceChanged()),this,SLOT(update()));
    return nodeLoad;
}

UniformlyDistributedLoad* TwoDimensionalStaticsModule::createUDLoad(QVector3D force, Beam* beam,QString name){
    qWarning("TwoDimensionalStaticsModule doesn't support UniformlyDistributedLoad");
    return Q_NULLPTR;
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
            QString name=parts.at(4);
            QVector3D position(parts.at(1).toFloat(), parts.at(2).toFloat(), parts.at(3).toFloat());
            QString support;
            if(parts.size()>=6)
                support=parts.at(5);
            createJoint(position,support,name);
        }
        else if(parts.at(0).compare("m")==0){
            if(parts.size()<4) {qWarning("Structure file incorrect format"); return false;}
            QString name=parts.at(3);
            Joint* extreme1=joints.at(parts.at(1).toInt());
            Joint* extreme2=joints.at(parts.at(2).toInt());
            createBeam(extreme1,extreme2,name);
        }
    }

    update();

    /*QVariantList args;
    args.append("");
    args.append(QVector3D());
    args.append(QVector3D(10,10,0));
    args.append("B");
    createElement(AbstractElement::FORCE,args);*/


    m_status=LOADED;
    emit statusChanged();

    return true;

}

void TwoDimensionalStaticsModule::solve(){
    cv::Mat external_forces_matrix=cv::Mat::zeros(2*joints.size(),1,CV_32F);
    int i=0;
    for(Joint* joint : joints){
        for(Force* force :forces){
            if(force->applicationElement()==joint){
                external_forces_matrix.at<float>(i,0)=external_forces_matrix.at<float>(i,0)-force->vector().x();
                external_forces_matrix.at<float>(i+1,0)=external_forces_matrix.at<float>(i+1,0)-force->vector().y();
                //external_forces_matrix.at<float>(i+2,0)=external_forces_matrix.at<float>(i+2,0)+force.second.z();
            }
        }
        i+=2;
    }

    cv::Mat solution;
    cv::solve(internalF_matrix,external_forces_matrix,solution);

    /*cv::MatIterator_<float> _it = internalF_matrix.begin<float>();
    i=0;
    QString text;
    for(;_it!=internalF_matrix.end<float>(); _it++){
        if(i%8==0){ qDebug()<<text;text.clear();}
        text.append( QString::number(*_it) );
        text.append(" ");
        i++;
    }
    qDebug()<<text;

    text.clear();
    _it = external_forces_matrix.begin<float>();
    i=0;
    for(;_it!=external_forces_matrix.end<float>(); _it++){
        if(i%8==0){ qDebug()<<text;text.clear();}
        text.append( QString::number(*_it) );
        text.append(" ");
        i++;
    }
    qDebug()<<text;*/



    i=0;
    for(Beam* beam: beams){
        //qDebug()<<beam->objectName()<<solution.at<float>(i,0);
        beam->setAxialForce(solution.at<float>(i,0));
        i++;
    }
    for(Joint* support: joints){
        if(support->supportType()==Joint::FIXED){
            support->setReaction(QVector3D(solution.at<float>(i,0),solution.at<float>(i+1,0),0));
            i+=2;
        }
        else if(support->supportType()==Joint::ROLLING){
            support->setReaction(QVector3D(0,solution.at<float>(i,0),0));
            i+=1;
        }
    }

}

void TwoDimensionalStaticsModule::update(){
    check_stability();

    /*Check stability*/
    if(m_stability!=Stability::DETERMINATE) return;

    /*Compute reactions*/
    int n_reactions=0;
    for(Joint* joint:joints){
        if(joint->supportType()==Joint::FIXED)
            n_reactions+=2;
        else if(joint->supportType()==Joint::ROLLING)
            n_reactions++;

    }

    internalF_matrix=cv::Mat::zeros(2*joints.size(),beams.size()+n_reactions,CV_32F);
    int i=0;
    for(Joint* joint : joints){
        int j=0;
        for(Beam* beam: beams){
            if(beam->extremes().first==joint || beam->extremes().second==joint){
                QVector3D firstPosition=beam->extremes().first->position();
                QVector3D secondPosition=beam->extremes().second->position();
                QVector3D dir;
                if(beam->extremes().first==joint)
                    dir=firstPosition-secondPosition;
                else
                    dir=secondPosition-firstPosition;
                dir.normalize();
                //qDebug()<<joint->objectName()<<" "<<beam->objectName()<<" "<<dir;
                internalF_matrix.at<float>(i,j)=dir.x();
                internalF_matrix.at<float>(i+1,j)=dir.y();
            }
            j++;
        }
        for(Joint* support :joints){
            //TODO: supports can have orientation
            if(support->supportType()==Joint::FIXED){
                if(support==joint){
                    internalF_matrix.at<float>(i,j)=1.0f;
                    internalF_matrix.at<float>(i+1,j+1)=1.0f;
                }
                j+=2;
            }
            else if(support->supportType()==Joint::ROLLING){
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

    Stability old_val=m_stability;
    int n_beams=beams.size();
    int n_joints=joints.size();
    int n_reactions=0;

    for(Joint* joint:joints){
        if(joint->supportType()==Joint::FIXED)
            n_reactions+=2;
        else if(joint->supportType()==Joint::ROLLING)
            n_reactions++;

    }

    if (2*n_joints==n_beams+n_reactions){
        m_stability=Stability::DETERMINATE;
    }
    else if(2*n_joints<n_beams+n_reactions){
        m_stability=Stability::UNSTABLE;

    }
    else
        m_stability=Stability::INDETERMINATE;

    if(old_val!=m_stability)
        emit stabilityChanged();
}



