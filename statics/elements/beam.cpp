#include "beam.h"
#include "statics/elements/joint.h"

Beam::Beam(Joint* extreme1, Joint* extreme2,QString name,QObject* parent):
    AbstractElement(name,parent),
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
    m_d(0)
{
    m_extreme1=extreme1;
    m_extreme2=extreme2;
    m_length=extreme1->position().distanceToPoint(extreme2->position());
    m_extreme1->addConnectedBeam(this);
    m_extreme2->addConnectedBeam(this);

    connect(m_extreme1,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    connect(m_extreme2,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
}

void Beam::extremes(Joint*& e1,Joint*& e2){
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



void Beam::set_parameters(qreal Ax, qreal Asy, qreal Asz,
                          qreal Jx, qreal Iy, qreal Iz,
                          qreal E, qreal G, qreal p, qreal d){
    bool updated=false;
    if(m_Ax!=Ax){
        m_Ax=Ax;
        updated=true;
    }
    if(m_Asy!=Asy){
        m_Asy=Asy;
        updated=true;
    }
    if(m_Asz!=Asz){
        m_Asz=Asz;
        updated=true;
    }
    if(m_Jx!=Jx){
        m_Jx=Jx;
        updated=true;
    }
    if(m_Iy!=Iy){
        m_Iy=Iy;
        updated=true;
    }
    if(m_Iz!=Iz){
        m_Iz=Iz;
        updated=true;
    }
    if(m_E!=E){
        m_E=E;
        updated=true;
    }
    if(m_G!=G){
        m_G=G;
        updated=true;
    }
    if(m_p!=p){
        m_p=p;
        updated=true;
    }
    if(m_d!=d){
        m_d=d;
        updated=true;
    }
    if(updated)
        emit parametersChanged();
}

void Beam::setForcesAndMoments(int axial_type,qreal Nx, qreal Vy, qreal Vz,
                               qreal Tx,qreal My,qreal Mz,int extreme){
    bool updated=false;
    if(extreme==0 || extreme<0){
        if(m_axial_force_extreme_1!=Nx){
            m_axial_force_extreme_1=Nx;
            m_axial_type_extreme_1=axial_type;
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
    }
    if(extreme==1 || extreme<0){
        if(m_axial_force_extreme_2!=Nx){
            m_axial_force_extreme_2=Nx;
            m_axial_type_extreme_2=axial_type;
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
    }
    if(updated)
        emit stressChanged();
}

void Beam::ForcesAndMoments(int& axial_type, qreal& Nx, qreal& Vy, qreal& Vz,
                      qreal& Tx,qreal& My,qreal& Mz,int extreme){
    if(extreme==0){
        axial_type=m_axial_type_extreme_1;
        Nx=m_axial_force_extreme_1;
        Vy=m_shear_y_extreme_1;
        Vz=m_shear_z_extreme_1;
        Tx=m_axial_moment_extreme_1;
        My=m_y_moment_extreme_1;
        Mz=m_z_moment_extreme_1;
    }
    else if(extreme==1){
        axial_type=m_axial_type_extreme_2;
        Nx=m_axial_force_extreme_2;
        Vy=m_shear_y_extreme_2;
        Vz=m_shear_z_extreme_2;
        Tx=m_axial_moment_extreme_2;
        My=m_y_moment_extreme_2;
        Mz=m_z_moment_extreme_2;
    }
    else{
        //qWarning("Returning an average");
        axial_type=m_axial_type_extreme_2;
        Nx=0.5*m_axial_force_extreme_1+0.5*m_axial_force_extreme_2;
        Vy=0.5*m_shear_y_extreme_1+0.5*m_shear_y_extreme_2;
        Vz=0.5*m_shear_z_extreme_1+0.5*m_shear_z_extreme_2;
        Tx=0.5*m_axial_moment_extreme_1+0.5*m_axial_moment_extreme_2;
        My=0.5*m_y_moment_extreme_1+0.5*m_y_moment_extreme_2;
        Mz=0.5*m_z_moment_extreme_1+0.5*m_z_moment_extreme_2;
    }


}

void Beam::setSize(QSizeF size){
    if(m_size!=size){
        m_size=size;
        qreal Ax=m_size.height()*m_size.width();
        qreal v=0.4;
        qreal Asy = Ax*(5+5*v)/(6+5*v);
        set_parameters(Ax,Asy,Asy,m_Jx,m_Iy,m_Iz,m_E,m_G,m_p,m_d);

    }
}

void Beam::setMaterial(QString material){
    Q_UNUSED(material);
    //Default one is ideal truss
    set_parameters(6451.6,645.16,645.16,416231,416231,4162,200000,80000,0,0.0000000001);
}
