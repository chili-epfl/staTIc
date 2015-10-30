#ifndef BEAM_H
#define BEAM_H

#include<QString>
#include<QSizeF>
#include "statics/elements/abstractelement.h"

class Joint;

class Beam : public AbstractElement
{
    Q_OBJECT
public:
    Beam(Joint* extreme1, Joint* extreme2,QString name=QString(),QObject* parent=0);

    void extremes(Joint*& e1,Joint*& e2);

    void parameters(qreal& Ax, qreal& Asy, qreal& Asz, qreal& Jx, qreal& Iy, qreal& Iz, qreal& E, qreal& G, qreal& p, qreal& d);
    void set_parameters(qreal Ax, qreal Asy, qreal Asz, qreal Jx, qreal Iy, qreal Iz, qreal E, qreal G, qreal p, qreal d);

    void setForcesAndMoments(int axial_type, qreal Nx, qreal Vy, qreal Vz,
                             qreal Tx,qreal My,qreal Mz,int extreme=-1);
    void ForcesAndMoments(int& axial_type, qreal& Nx, qreal& Vy, qreal& Vz,
                          qreal& Tx,qreal& My,qreal& Mz,int extreme=0);

    void setSize(QSizeF size);
    QSizeF size(){return m_size;}

    void setMaterial(QString material);

    qreal length(){return m_length;}

signals:
    void parametersChanged();
    void stressChanged();
private:
    qreal m_axial_force_extreme_1;
    int m_axial_type_extreme_1;//-1 compression, 0 nul,1 tension

    qreal m_axial_force_extreme_2;
    int m_axial_type_extreme_2;//-1 compression, 0 nul,1 tension

    qreal m_shear_y_extreme_1;
    qreal m_shear_y_extreme_2;

    qreal m_shear_z_extreme_1;
    qreal m_shear_z_extreme_2;

    qreal m_axial_moment_extreme_1;
    qreal m_axial_moment_extreme_2;

    qreal m_y_moment_extreme_1;
    qreal m_y_moment_extreme_2;

    qreal m_z_moment_extreme_1;
    qreal m_z_moment_extreme_2;


    QSizeF m_size;

    Joint* m_extreme1,*m_extreme2;
    qreal m_length;

    qreal m_Ax,	/* cross section area of each element	*/
        m_Asy,	/* shear area in local y direction 	*/
        m_Asz,	/* shear area in local z direction	*/
        m_Jx,	/* torsional moment of inertia 		*/
        m_Iy,	/* bending moment of inertia about y-axis */
        m_Iz,	/* bending moment of inertia about z-axis */
        m_E,	/* frame element Young's modulus	*/
        m_G,	/* frame element shear modulus		*/
        m_p,	/* element rotation angle about local x axis */
        m_d;	/* element mass density			*/

};

#endif // BEAM_H

