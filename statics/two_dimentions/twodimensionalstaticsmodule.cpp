#include "statics/two_dimentions/twodimensionalstaticsmodule.h"

TwoDimensionalStaticsModule::TwoDimensionalStaticsModule(QObject *parent )
    :AbstractStaticsModule(parent)
{
}

void TwoDimensionalStaticsModule::addElement(AbstractElement* element){
    if(!element) return;
    if(m_elements.contains(element->objectName()))
        removeElement(element,false);
    m_elements[element->objectName()]=element;
    if(element->inherits("Joint")){
        m_joints.append(qobject_cast<Joint*>(element));
        update();
    }
    else if(element->inherits("Beam")){
        m_beams.append(qobject_cast<Beam*>(element));
        update();
    }
    else if(element->inherits("Force")){
        m_forces.append(qobject_cast<Force*>(element));
    }


}
void TwoDimensionalStaticsModule::removeElement(AbstractElement* element,bool update){
    if(!element) return;
    if(m_elements.contains(element->objectName())){
        AbstractElement* old_val=m_elements[element->objectName()];
        m_elements.remove(element->objectName());
        if(old_val->inherits("Joint")){
            m_joints.removeAll(qobject_cast<Joint*>(old_val));
            if(update)
                this->update();
        }
        else if(old_val->inherits("Beam")){
            m_beams.removeAll(qobject_cast<Beam*>(old_val));
            if(update)
                this->update();
        }
        else if(old_val->inherits("Force")){
            m_forces.removeAll(qobject_cast<Force*>(old_val));
        }

    }

}
AbstractElement* TwoDimensionalStaticsModule::getElement(QString elementName){
    if(!m_elements.contains(elementName)) return NULL;
    return m_elements[elementName];
}

bool TwoDimensionalStaticsModule::containsElement(QString elementName){
    return m_elements.contains(elementName);
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
            }
            else if(parts.at(5).compare("rolling",Qt::CaseInsensitive)==0){
                joint->setSupport(Joint::ROLLING);
            }
            m_elements[joint->objectName()]=joint;
            m_joints.append(joint);
        }
        else if(parts.at(0).compare("m")==0){
            if(parts.size()<4) {qWarning("Structure file incorrect format"); return false;}
            Beam* beam=new Beam(parts.at(3));
            Joint* extreme1=m_joints.at(parts.at(1).toInt());
            Joint* extreme2=m_joints.at(parts.at(2).toInt());
            beam->extremes.first=extreme1;
            beam->extremes.second=extreme2;
            m_elements[beam->objectName()]=beam;
            m_beams.append(beam);
        }
    }

    update();

    m_status=LOADED;
    emit statusChanged();

    return true;

}

void TwoDimensionalStaticsModule::solve(){
    cv::Mat external_forces_matrix=cv::Mat::zeros(2*m_joints.size(),1,CV_32F);
    int i=0;
    for(Joint* joint : m_joints){
        for(Force* force :m_forces){
            if(force->applicationElement.compare(joint->objectName())==0){
                external_forces_matrix.at<float>(i,0)=external_forces_matrix.at<float>(i,0)+force->vector.x();
                external_forces_matrix.at<float>(i+1,0)=external_forces_matrix.at<float>(i+1,0)+force->vector.y();
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
        if(i%6==0){ qDebug()<<text;text.clear();}
        text.append( QString::number(*_it) );
        text.append(" ");
        i++;
    }
    qDebug()<<text;
    */
    i=0;
    for(Beam* beam: m_beams){
        beam->setAxialForce(solution.at<float>(i,0));
        i++;
    }
    for(Joint* support: m_joints){
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
    check_stability();

    /*Check stability*/
    if(m_stability!=Stability::DETERMINATE) return;

    /*Compute reactions*/
    int n_reactions=0;
    for(Joint* joint:m_joints){
        if(joint->getSupport()==Joint::FIXED)
            n_reactions+=2;
        else if(joint->getSupport()==Joint::ROLLING)
            n_reactions++;

    }

    internalF_matrix=cv::Mat::zeros(2*m_joints.size(),m_beams.size()+n_reactions,CV_32F);
    int i=0;
    for(Joint* joint : m_joints){
        int j=0;
        for(Beam* beam: m_beams){
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
        for(Joint* support :m_joints){
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

    Stability old_val=m_stability;
    int n_beams=m_beams.size();
    int n_joints=m_joints.size();
    int n_reactions=0;

    for(Joint* joint:m_joints){
        if(joint->getSupport()==Joint::FIXED)
            n_reactions+=2;
        else if(joint->getSupport()==Joint::ROLLING)
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

