#include "statics/two_dimentions/twodimensionalstaticsmodule.h"
#include "staticshelper.h"

TwoDimensionalStaticsModule::TwoDimensionalStaticsModule(QObject *parent )
    :AbstractStaticsModule(parent)
{
}




AbstractElement* TwoDimensionalStaticsModule::createElement(AbstractElement::Element_Type type, QVariantList args ){
    /*Kind of a factory method. The function takes care of creating the viewmodel as well.
    *   args[0]-> name of the object
    *   Case BEAM
    *       args[1-2]-> void* extremes
    *   Case JOINT:
    *       args[1]-> QVector3D Position
    *       args[2]-> QString Support
    *   Case FORCE
    *       args[1]-> QVector3D ApplicationPoint
    *       args[2]-> QVector3D Vector
    *       args[3]-> QString ApplicationElement
    *       args[4]-> **optional** bool hasTiponApplicationPoint
    *   TODO:Check if the object exists
    **/
    switch (type) {
    case (AbstractElement::BEAM): {

        Beam* beam=new Beam(args[0].toString(),this);
        QPair<Joint*, Joint*> extremes;
        extremes.first=(Joint*)args[1].value<void*>();
        extremes.second=(Joint*)args[2].value<void*>();

        beam->setExtremes(extremes);
        m_beams.append(beam);

        extremes.first->connected_beams.append(beam);
        extremes.second->connected_beams.append(beam);

        BeamVM* beamVM=new BeamVM(this);
        beamVM->setEntityName(beam->objectName());
        beamVM->setObjectName(StaticsHelper::NameResolution(beam->objectName(),
                                                            StaticsHelper::Roles::MODEL,StaticsHelper::Roles::VIEWMODEL));
        beamVM->setSceneRoot(m_sceneRoot);

        beam->setVm(beamVM);

        /*Missing bindings*/
        connect(this,SIGNAL(sceneRootChanged(Qt3D::QEntity*)),beamVM,SLOT(setSceneRoot(Qt3D::QEntity*)));

        return beam;
        break;

    }
    case (AbstractElement::JOINT):{

        Joint* joint=new Joint(args[0].toString(),this);
        joint->setPosition(args[1].value<QVector3D>());
        if(args.size()<3)
            joint->setSupportType(Joint::NOSUPPORT);
        else if(args[2].toString().compare("fixed",Qt::CaseInsensitive)==0){
            joint->setSupportType(Joint::FIXED);
        }
        else if(args[2].toString().compare("rolling",Qt::CaseInsensitive)==0){
            joint->setSupportType(Joint::ROLLING);
        }
        m_joints.append(joint);

        JointVM* jointVM=new JointVM(this);
        jointVM->setEntityName(joint->objectName());
        jointVM->setObjectName(StaticsHelper::NameResolution(joint->objectName(),
                                                             StaticsHelper::Roles::MODEL,StaticsHelper::Roles::VIEWMODEL));
        jointVM->setSceneRoot(m_sceneRoot);

        joint->setVm(jointVM);

        /*Missing bindings*/
        connect(this,SIGNAL(sceneRootChanged(Qt3D::QEntity*)),jointVM,SLOT(setSceneRoot(Qt3D::QEntity*)));
        return joint;
        break;
    }
    case (AbstractElement::FORCE):{

        Force* force=new Force(this);
        force->setApplicationPoint(args[1].value<QVector3D>());
        force->setVector(args[2].value<QVector3D>());
        force->setApplicationElement(args[3].toString());        
        m_forces.append(force);

        connect(force,SIGNAL(vectorChanged(QVector3D)),this,SLOT(onForceUpdate()));
        connect(force,SIGNAL(applicationPointChanged(QVector3D)),this,SLOT(onForceUpdate()));
        connect(force,SIGNAL(applicationElementChanged(QString)),this,SLOT(onForceUpdate()));

        ForceVM* forceVM=new ForceVM(this);
        forceVM->setEntityName(force->objectName());
        forceVM->setObjectName(StaticsHelper::NameResolution(force->objectName(),
                                                             StaticsHelper::Roles::MODEL,StaticsHelper::Roles::VIEWMODEL));
        forceVM->setSceneRoot(m_sceneRoot);
        if(args.size()>4)
            forceVM->setHasTipOnApplicationPoint(args[4].toBool());

        force->setVm(forceVM);

        /*Missing bindings*/
        connect(this,SIGNAL(sceneRootChanged(Qt3D::QEntity*)),forceVM,SLOT(setSceneRoot(Qt3D::QEntity*)));

        solve();

        return force;
        break;
}
    default:
        return 0;
        break;
    }
}

void TwoDimensionalStaticsModule::removeElement( QString element,bool update){
    AbstractElement* old_val=findChild<AbstractElement*>(element);
    if(old_val){
        if(old_val->inherits("Joint")){
            /*m_joints.removeAll(qobject_cast<Joint*>(old_val));
            if(update){
                this->update();
                solve();

            }*/
            old_val->deleteLater();

        }
        else if(old_val->inherits("Beam")){
            /*m_beams.removeAll(qobject_cast<Beam*>(old_val));
            if(update){
                this->update();
                solve();
            }
            Remebr to remove the beam from the list of the extreme joints
            */
            old_val->deleteLater();

        }
        else if(old_val->inherits("Force")){
            m_forces.removeAll(qobject_cast<Force*>(old_val));
            solve();
            old_val->deleteLater();
        }
    }

}

AbstractElement* TwoDimensionalStaticsModule::getElement(QString elementName){
    return this->findChild<AbstractElement*>(elementName);
}

bool TwoDimensionalStaticsModule::containsElement(QString elementName){
    return (this->findChild<AbstractElement*>(elementName)!=0);
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
            QVariantList args;
            args.append(parts.at(4));
            args.append(QVector3D( parts.at(1).toFloat(), parts.at(2).toFloat(), parts.at(3).toFloat()));
            if(parts.size()>=6)
                args.append(parts.at(5));
            createElement(AbstractElement::JOINT,args);
        }
        else if(parts.at(0).compare("m")==0){
            if(parts.size()<4) {qWarning("Structure file incorrect format"); return false;}
            QVariantList args;
            args.append(parts.at(3));
            args.append(qVariantFromValue((void*)m_joints.at(parts.at(1).toInt())));
            args.append(qVariantFromValue((void*)m_joints.at(parts.at(2).toInt())));
            createElement(AbstractElement::BEAM,args);
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
            if(force->applicationElement().compare(joint->objectName())==0){
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
    for(Beam* beam: m_beams){
        //qDebug()<<beam->objectName()<<solution.at<float>(i,0);
        beam->setAxialForce(solution.at<float>(i,0));
        i++;
    }
    for(Joint* support: m_joints){
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
    for(Joint* joint:m_joints){
        if(joint->supportType()==Joint::FIXED)
            n_reactions+=2;
        else if(joint->supportType()==Joint::ROLLING)
            n_reactions++;

    }

    internalF_matrix=cv::Mat::zeros(2*m_joints.size(),m_beams.size()+n_reactions,CV_32F);
    int i=0;
    for(Joint* joint : m_joints){
        int j=0;
        for(Beam* beam: m_beams){
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
        for(Joint* support :m_joints){
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
    int n_beams=m_beams.size();
    int n_joints=m_joints.size();
    int n_reactions=0;

    for(Joint* joint:m_joints){
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



