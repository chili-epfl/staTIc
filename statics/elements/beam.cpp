#include "beam.h"
#include "statics/elements/joint.h"
#include "statics/abstractstaticsmodule.h"
#include <QDebug>
Beam::Beam(JointPtr extreme1, JointPtr extreme2,MaterialsManager* mm,QString name,QObject* parent):
    AbstractElement(name,parent),
    m_enable(true),
    m_axial_force_extreme_1(0),
    m_axial_type_extreme_1(0),
    m_axial_force_extreme_2(0),
    m_axial_type_extreme_2(0),
    m_shear_y_extreme_1(0),
    m_shear_y_extreme_2(0),
    m_shear_z_extreme_1(0),
    m_shear_z_extreme_2(0),
    m_axial_moment_extreme_1(0),
    m_axial_moment_extreme_2(0),
    m_y_moment_extreme_1(0),
    m_y_moment_extreme_2(0),
    m_z_moment_extreme_1(0),
    m_z_moment_extreme_2(0),
    m_length(0),
    m_Ax(0),
    m_Asy(0),
    m_Asz(0),
    m_Jx(0),
    m_Iy(0),
    m_Iz(0),
    m_E(0),
    m_G(0),
    m_p(0),
    m_d(0),
    m_Sy(0),
    m_Sz(0),
    m_C(0),
    m_materialId()
{
    m_peak_axial_force=0;
    m_peak_shear_y=0;
    m_peak_shear_z=0;
    m_peak_y_moment=0;
    m_peak_z_moment=0;
    m_peak_axial_moment=0;
    m_peak_axial_stress=0;
    m_peak_shear_y_stress=0;
    m_peak_shear_z_stress=0;
    m_peak_bending_stress=0;
    m_peak_torsional_stress=0;

    m_dirty=DirtyFlag::Clean;
    m_extreme1=extreme1;
    m_extreme2=extreme2;
    m_length=extreme1->position().distanceToPoint(extreme2->position());
    m_materialsManager=mm;
    m_tangibleSection=QSizeF(34,17);
    connect(m_extreme1.data(),SIGNAL(destroyed(QObject*)),this,SIGNAL(killMe()));
    connect(m_extreme2.data(),SIGNAL(destroyed(QObject*)),this,SIGNAL(killMe()));
    m_lazy_signal_emitter.setInterval(500);
    connect(&m_lazy_signal_emitter,SIGNAL(timeout()),this,SLOT(lazy_update()));
}

void Beam::lazy_update(){
    if(m_dirty.testFlag(DirtyFlag::ParametersChanged))
        emit parametersChanged();
    if(m_dirty.testFlag(DirtyFlag::StressChanged))
        emit stressChanged();
    m_dirty=DirtyFlag::Clean;
}

void Beam::extremes(WeakJointPtr& e1,WeakJointPtr& e2){
    e1=m_extreme1;
    e2=m_extreme2;
}

void Beam::parameters(qreal& Ax, qreal& Asy, qreal& Asz,
                      qreal& Jx, qreal& Iy, qreal& Iz,
                      qreal& E, qreal& G, qreal& p, qreal& d){
    Ax=m_Ax;
    Asy=m_Asy;
    Asz=m_Asz;
    Jx=m_Jx;
    Iy=m_Iy;
    Iz=m_Iz;
    E=m_E;
    G=m_G;
    p=m_p;
    d=m_d;
}

void Beam::setEnable(bool enable){
    if(m_enable!=enable){
        m_enable=enable;
        emit enableChanged(m_enable);
    }
}

void Beam::setMaterial(QString uniqueID)
{
    if(m_materialId==uniqueID) return;
    if(m_materialsManager!=Q_NULLPTR){        
        QVariant g,young,density;
        g=m_materialsManager->get(uniqueID,"G");
        young=m_materialsManager->get(uniqueID,"Young");
        density=m_materialsManager->get(uniqueID,"Density");
        if(g.isNull() || young.isNull() || density.isNull()){
            qDebug()<<"Setting default material";
            m_materialId="default";
            g=m_materialsManager->get(m_materialId,"G");
            young=m_materialsManager->get(m_materialId,"Young");
            density=m_materialsManager->get(m_materialId,"Density");
            setDensity(density.toDouble());
            setYoungModulus(young.toDouble());
            setShearModulus(g.toDouble());
            emit materialChanged();
        }
        else {            
           m_materialId=uniqueID;
           setDensity(density.toDouble());
           setYoungModulus(young.toDouble());
           setShearModulus(g.toDouble());
           emit materialChanged();
        }
    }
    else{
        qCritical("Missing Material Manager");
    }
}

void Beam::setForcesAndMoments(int axial_type,qreal Nx, qreal Vy, qreal Vz,
                               qreal Tx,qreal My,qreal Mz,int extreme){
    bool updated=false;
    if(extreme!=1 && extreme!=2 && extreme!=3){
        qWarning("Wrong extreme index");
        return;
    }
    if(extreme==1){
        if(m_axial_force_extreme_1!=Nx){
            if(fabs(Nx)<0.001){
                m_axial_force_extreme_1=0;
                m_axial_type_extreme_1=0;
            }else{
                m_axial_force_extreme_1=Nx;
                m_axial_type_extreme_1=axial_type;
            }
            updated=true;
        }
        if(m_shear_y_extreme_1!=Vy){
            m_shear_y_extreme_1=Vy;
            updated=true;
        }
        if(m_shear_z_extreme_1!=Vz){
            m_shear_z_extreme_1=Vz;
            updated=true;
        }
        if(m_axial_moment_extreme_1!=Tx){
            m_axial_moment_extreme_1=Tx;
            updated=true;
        }
        if(m_y_moment_extreme_1!=My){
            m_y_moment_extreme_1=My;
            updated=true;
        }
        if(m_z_moment_extreme_1!=Mz){
           m_z_moment_extreme_1=Mz;
           updated=true;
        }
        m_axial_stress_extreme1=-m_axial_force_extreme_1/m_Ax;
        m_bending_stress_extreme1=fabs(m_y_moment_extreme_1)/m_Sy+fabs(m_z_moment_extreme_1)/m_Sz;
        m_shear_stress_y_extreme1=fabs(m_shear_y_extreme_1)/m_Asy;
        m_shear_stress_z_extreme1=fabs(m_shear_z_extreme_1)/m_Asz;
        m_torsional_stess_extreme1=fabs(m_axial_moment_extreme_1)/m_C;
//        qDebug()<<this->objectName();
//        qDebug()<<"Etxreme1";
//        qDebug()<<m_y_moment_extreme_1<<"  "<< m_Sy;
//        qDebug()<<m_z_moment_extreme_1<<"  "<< m_Sz;
//        qDebug()<<m_bending_stress_extreme1;
    }
    if(extreme==2){
        if(m_axial_force_extreme_2!=Nx){
            if(fabs(Nx)<0.001){
                m_axial_force_extreme_2=0;
                m_axial_type_extreme_2=0;
            }else{
                m_axial_force_extreme_2=Nx;
                m_axial_type_extreme_2=axial_type;
            }
            updated=true;
        }
        if(m_shear_y_extreme_2!=Vy){
            m_shear_y_extreme_2=Vy;
            updated=true;
        }
        if(m_shear_z_extreme_2!=Vz){
            m_shear_z_extreme_2=Vz;
            updated=true;
        }
        if(m_axial_moment_extreme_2!=Tx){
            m_axial_moment_extreme_2=Tx;
            updated=true;
        }
        if(m_y_moment_extreme_2!=My){
            m_y_moment_extreme_2=My;
            updated=true;
        }
        if(m_z_moment_extreme_2!=Mz){
           m_z_moment_extreme_2=Mz;
           updated=true;
        }
        m_axial_stress_extreme2=m_axial_force_extreme_2/m_Ax;
        m_bending_stress_extreme2=fabs(m_y_moment_extreme_2)/m_Sy+fabs(m_z_moment_extreme_2)/m_Sz;
        m_shear_stress_y_extreme2=fabs(m_shear_y_extreme_2)/m_Asy;
        m_shear_stress_z_extreme2=fabs(m_shear_z_extreme_2)/m_Asz;
        m_torsional_stess_extreme2=fabs(m_axial_moment_extreme_2)/m_C;
//        qDebug()<<this->objectName();
//        qDebug()<<"Etxreme2";
//        qDebug()<<m_y_moment_extreme_2<<"  "<< m_Sy;
//        qDebug()<<m_z_moment_extreme_2<<"  "<< m_Sz;
//        qDebug()<<m_bending_stress_extreme2;


    }

    if(extreme==3){
        if(m_peak_axial_force!=Nx){
            m_peak_axial_force=Nx;
            m_peak_axial_type=axial_type;
            updated=true;
        }
        if(m_peak_shear_y!=Vy){
            m_peak_shear_y=Vy;
            updated=true;
        }
        if(m_peak_shear_z!=Vz){
            m_peak_shear_z=Vz;
            updated=true;
        }
        if(m_peak_axial_moment!=Tx){
            m_peak_axial_moment=Tx;
            updated=true;
        }
        if(m_peak_y_moment!=My){
            m_peak_y_moment=My;
            updated=true;
        }
        if(m_peak_z_moment!=Mz){
           m_peak_z_moment=Mz;
           updated=true;
        }
        m_peak_axial_stress=m_peak_axial_force/m_Ax;
        m_peak_bending_stress=fabs(m_peak_y_moment)/m_Sy+fabs(m_peak_z_moment)/m_Sz;
        m_peak_shear_y_stress=fabs(m_peak_shear_y)/m_Asy;
        m_peak_shear_z_stress=fabs(m_peak_shear_z)/m_Asz;
        m_peak_torsional_stress=fabs(m_peak_axial_moment)/m_C;

    }

    if(updated){
        m_dirty.operator |=( DirtyFlag::StressChanged);
        m_lazy_signal_emitter.start();
    }

//    qDebug()<<"Beam:"<<objectName();
//    qDebug()<<"Axial stress extreme 1:"<<m_axial_stress_extreme1;
//    qDebug()<<"Shear stress extreme 1:"<<m_shear_stress_y_extreme1;
//    qDebug()<<"Shear stress extreme 1:"<<m_shear_stress_z_extreme1;
//    qDebug()<<"Bending Stress extreme 1:"<<m_bending_stress_extreme1;
//    qDebug()<<"Axial force extreme 1:"<<m_axial_force_extreme_1;
//    qDebug()<<"Axial shear y extreme 1:"<<m_shear_y_extreme_1;
//    qDebug()<<"Axial shear z extreme 1:"<<m_shear_z_extreme_1;
//    qDebug()<<"Torque extreme 1:"<<m_axial_moment_extreme_1;
//    qDebug()<<"Momentum y extreme 1:"<<m_y_moment_extreme_1;
//    qDebug()<<"Momentum z extreme 1:"<<m_z_moment_extreme_1;

//    qDebug()<<"Axial stress extreme 2:"<<m_axial_stress_extreme2;
//    qDebug()<<"Shear stress extreme 2:"<<m_shear_stress_y_extreme2;
//    qDebug()<<"Shear stress extreme 2:"<<m_shear_stress_z_extreme2;
//    qDebug()<<"Bending Stress extreme 2:"<<m_bending_stress_extreme2;

//    qDebug()<<"Axial force extreme 2:"<<m_axial_force_extreme_2;
//    qDebug()<<"Axial shear y extreme 2:"<<m_shear_y_extreme_2;
//    qDebug()<<"Axial shear z extreme 2:"<<m_shear_z_extreme_2;
//    qDebug()<<"Torque extreme 2:"<<m_axial_moment_extreme_2;
//    qDebug()<<"Momentum y extreme 2:"<<m_y_moment_extreme_2;
//    qDebug()<<"Momentum z extreme 2:"<<m_z_moment_extreme_2;
//    qDebug()<<m_Sy<<" "<<m_Sz;


}

void Beam::ForcesAndMoments(int& axial_type, qreal& Nx, qreal& Vy, qreal& Vz,
                      qreal& Tx,qreal& My,qreal& Mz,int extreme){

    if(extreme==1){
        axial_type=m_axial_type_extreme_1;
        Nx=m_axial_force_extreme_1;
        Vy=m_shear_y_extreme_1;
        Vz=m_shear_z_extreme_1;
        Tx=m_axial_moment_extreme_1;
        My=m_y_moment_extreme_1;
        Mz=m_z_moment_extreme_1;
    }
    else if(extreme==2 ){
        axial_type=m_axial_type_extreme_2;
        Nx=m_axial_force_extreme_2;
        Vy=m_shear_y_extreme_2;
        Vz=m_shear_z_extreme_2;
        Tx=m_axial_moment_extreme_2;
        My=m_y_moment_extreme_2;
        Mz=m_z_moment_extreme_2;
    }
    else if(extreme==3){
        axial_type=m_peak_axial_type;
        Nx=m_peak_axial_type;
        Vy=m_peak_shear_y;
        Vz=m_peak_shear_z;
        Tx=m_peak_axial_moment;
        My=m_peak_y_moment;
        Mz=m_peak_z_moment;
    }
    else{
        qCritical("Wrong extreme value");
    }


}

void Beam::stress(qreal &axial,qreal &bending, qreal &shearY, qreal &shearZ, qreal &torsion, int extreme)
{
    if(extreme==1){
        axial=m_axial_stress_extreme1;
        bending=m_bending_stress_extreme1;
        shearY=m_shear_stress_y_extreme1;
        shearZ=m_shear_stress_z_extreme1;
        torsion=m_torsional_stess_extreme1;
    }
    else if(extreme==2){
        axial=m_axial_stress_extreme2;
        bending=m_bending_stress_extreme2;
        shearY=m_shear_stress_y_extreme2;
        shearZ=m_shear_stress_z_extreme2;
        torsion=m_torsional_stess_extreme2;
    }
    else if(extreme==3){
        axial=m_peak_axial_stress;
        bending=m_peak_bending_stress;
        shearY=m_peak_shear_y_stress;
        shearZ=m_peak_shear_z_stress;
        torsion=m_peak_torsional_stress;
    }
    else{
        qCritical("Wrong extreme value");
    }
}

void Beam::stressRatio(qreal& axialComponent, qreal& shearComponent, int extreme)
{
    axialComponent=0;
    shearComponent=0;
    if(extreme==1){
        if(m_axial_type_extreme_1>0){//tension
            axialComponent+= m_axial_stress_extreme1/m_materialsManager->get(m_materialId,"ft0").toDouble();
        }
        else{
            axialComponent+= m_axial_stress_extreme1/m_materialsManager->get(m_materialId,"fc0").toDouble();
        }
        axialComponent+=fabs(m_bending_stress_extreme1)/(2*m_materialsManager->get(m_materialId,"fmk").toDouble());
        axialComponent=fabs(axialComponent);
        shearComponent+=fabs(m_shear_stress_y_extreme1)/m_materialsManager->get(m_materialId,"fvk").toDouble();
        shearComponent+=fabs(m_shear_stress_z_extreme1)/m_materialsManager->get(m_materialId,"fvk").toDouble();
        shearComponent+=fabs(m_torsional_stess_extreme1)/m_materialsManager->get(m_materialId,"fvk").toDouble();
    }
    else if(extreme==2){
        if(m_axial_type_extreme_2>0){//Tension
            axialComponent+= m_axial_stress_extreme2/m_materialsManager->get(m_materialId,"ft0").toDouble();
        }
        else{
            axialComponent+= m_axial_stress_extreme2/m_materialsManager->get(m_materialId,"fc0").toDouble();
        }
        axialComponent+=fabs(m_bending_stress_extreme2)/(2*m_materialsManager->get(m_materialId,"fmk").toDouble());
        axialComponent=fabs(axialComponent);
        shearComponent+=fabs(m_shear_stress_y_extreme2)/m_materialsManager->get(m_materialId,"fvk").toDouble();
        shearComponent+=fabs(m_shear_stress_z_extreme2)/m_materialsManager->get(m_materialId,"fvk").toDouble();
        shearComponent+=fabs(m_torsional_stess_extreme2)/m_materialsManager->get(m_materialId,"fvk").toDouble();
    }
    else if(extreme==3){
        if(m_peak_axial_type>0){//Tension
            axialComponent+= m_peak_axial_stress/m_materialsManager->get(m_materialId,"ft0").toDouble();
        }
        else{
            axialComponent+= m_peak_axial_stress/m_materialsManager->get(m_materialId,"fc0").toDouble();
        }
        axialComponent+=fabs(m_peak_bending_stress)/(2*m_materialsManager->get(m_materialId,"fmk").toDouble());
        axialComponent=fabs(axialComponent);
        shearComponent+=fabs(m_peak_shear_y_stress)/m_materialsManager->get(m_materialId,"fvk").toDouble();
        shearComponent+=fabs(m_peak_shear_z_stress)/m_materialsManager->get(m_materialId,"fvk").toDouble();
        shearComponent+=fabs(m_peak_torsional_stress)/m_materialsManager->get(m_materialId,"fvk").toDouble();
    }
    else{
        qCritical("Wrong extreme value");
    }
}

void Beam::setSize(QSizeF size){
    if(m_size!=size){
        m_size=size;
        m_Ax=computeCrossSectionalArea();
        m_Asy=computeShearAreaY();
        m_Asz=computeShearAreaZ();
        m_Jx=computeTorsionalMomentInertia();
        m_Iy=computeBendingMomentInertiaY();
        m_Iz=computeBendingMomentInertiaZ();
        m_Sy=computeSectionModulusY();
        m_Sz=computeSectionModulusZ();
        m_C=computeTorsionShearConstant();
        m_dirty.operator |=( DirtyFlag::ParametersChanged);
        m_lazy_signal_emitter.start();
    }
}

QSizeF Beam::scaledSize()
{
    return m_size*AbstractStaticsModule::modelScale();
}


void Beam::setYoungModulus(qreal E)
{
    if(m_E!=E){
        m_E=E;
        m_dirty.operator |=( DirtyFlag::ParametersChanged);
        m_lazy_signal_emitter.start();
    }
}

void Beam::setShearModulus(qreal G)
{
    if(m_G!=G){
        m_G=G;
        m_dirty.operator |=( DirtyFlag::ParametersChanged);
        m_lazy_signal_emitter.start();    }
}

void Beam::setDensity(qreal d)
{
    if(m_d!=d){
        m_d=d;
        m_dirty.operator |=( DirtyFlag::ParametersChanged);
        m_lazy_signal_emitter.start();    }
}

qreal Beam::scaledLength()
{
    return m_length*AbstractStaticsModule::modelScale();
}

void Beam::setTangibleSection(QSizeF val)
{
    if(val!=m_tangibleSection){
        m_tangibleSection=val;
        emit tangibleSectionChanged();
    }
}

void Beam::cloneProperties(BeamPtr beam){
    m_size=beam->m_size;
    m_Ax=beam->m_Ax;
    m_Asy=beam->m_Asy;
    m_Asz=beam->m_Asz;
    m_Jx=beam->m_Jx;
    m_Iy=beam->m_Iy;
    m_Iz=beam->m_Iz;
    m_E=beam->m_E;
    m_G=beam->m_G;
    m_p=beam->m_p;
    m_d=beam->m_d;
    m_Sy=beam->m_Sy;
    m_Sz=beam->m_Sz;
    m_C=beam->m_C;
    m_dirty.operator |=( DirtyFlag::ParametersChanged);
    m_lazy_signal_emitter.start();}

void Beam::split(){
    setEnable(false);
    emit hasBeenSplit();
}

void Beam::unify(){
    m_sub_parts.clear();
    setEnable(true);
    emit hasBeenUnified();
}

void Beam::addPart(BeamPtr beam){
    m_sub_parts.append(beam.toWeakRef());
}

void Beam::removePart(BeamPtr beam){
    if(m_sub_parts.contains(beam.toWeakRef())){
           m_sub_parts.removeAll(beam.toWeakRef());
    }
}

void Beam::setParentBeam(QSharedPointer<Beam> parent){
    m_parent_beam=parent;
}

qreal Beam::computeTorsionShearConstant(QSizeF size)
{
    if(!size.isValid()){
        if(!m_size.isValid()) return -1;
        size=this->m_size;
    }
    qreal ratio=size.height()/size.width();
    qreal beta;
    if(ratio<=1.25){
        beta=0.141;
    }
    else if(ratio<=1.75){
        beta=0.196;
    }
    else if(ratio<=2.25){
        beta=0.229;
    }
    else if(ratio<=2.75){
        beta=0.249;
    }
    else if(ratio<=3.5){
        beta=0.263;
    }
    else if(ratio<=4.5){
        beta=0.281;
    }
    else{
        beta=0.291;
    }
    return beta*size.height()*pow(size.width(),3);
}


qreal Beam::computeCrossSectionalArea(QSizeF size)
{
    if(!size.isValid()){
        if(!m_size.isValid()) return -1;
        size=this->m_size;
    }
    return size.width()*size.height();
}

qreal Beam::computeShearAreaY(qreal Ax)
{
    if(Ax <= 0){
        if(m_Ax <= 0) return -1;
        Ax=m_Ax;
    }
    return Ax*2.0/3.0;
}

qreal Beam::computeShearAreaZ(qreal Ax)
{
    if(Ax <= 0){
        if(m_Ax <= 0) return -1;
        Ax=m_Ax;
    }
    return Ax*2.0/3.0;
}

qreal Beam::computeTorsionalMomentInertia(QSizeF size)
{
    if(!size.isValid()){
        if(!m_size.isValid()) return -1;
        size=this->m_size;
    }
    qreal ratio=size.height()/size.width();
    qreal Q=0.3333-0.2244/(ratio+0.1607);
    return Q*size.height()*pow(size.width(),3);

}

qreal Beam::computeBendingMomentInertiaY(QSizeF size)
{
    if(!size.isValid()){
        if(!m_size.isValid()) return -1;
        size=this->m_size;
    }
    return size.height()*pow(size.width(),3)/12;
}

qreal Beam::computeBendingMomentInertiaZ(QSizeF size)
{
    if(!size.isValid()){
        if(!m_size.isValid()) return -1;
        size=this->m_size;
    }
    return size.width()*pow(size.height(),3)/12;
}

qreal Beam::computeSectionModulusY(QSizeF size)
{
    if(!size.isValid()){
        if(!m_size.isValid()) return -1;
        size=this->m_size;
    }
    qreal Iy= computeBendingMomentInertiaY(size);
    return 2*Iy/size.width();
}

qreal Beam::computeSectionModulusZ(QSizeF size)
{
    if(!size.isValid()){
        if(!m_size.isValid()) return -1;
        size=this->m_size;
    }
    qreal Iz= computeBendingMomentInertiaZ(size);
    return 2*Iz/size.height();
}
