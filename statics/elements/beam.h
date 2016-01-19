#ifndef BEAM_H
#define BEAM_H

#include<QString>
#include<QSizeF>
#include<QVector4D>
#include "statics/elements/abstractelement.h"

class Joint;
typedef QSharedPointer<Joint> JointPtr;
typedef QWeakPointer<Joint> WeakJointPtr;

class Beam : public AbstractElement
{
    Q_OBJECT
public:
    Beam(JointPtr extreme1, JointPtr extreme2,QString name=QString(),QObject* parent=0);

    void extremes(WeakJointPtr& e1,WeakJointPtr& e2);

    void parameters(qreal& Ax, qreal& Asy, qreal& Asz, qreal& Jx, qreal& Iy, qreal& Iz, qreal& E, qreal& G, qreal& p, qreal& d);

    void setForcesAndMoments(int axial_type, qreal Nx, qreal Vy, qreal Vz,
                             qreal Tx,qreal My,qreal Mz,int extreme);

    void ForcesAndMoments(int& axial_type, qreal& Nx, qreal& Vy, qreal& Vz,
                          qreal& Tx,qreal& My,qreal& Mz,int extreme=0);


    void setSize(QSizeF size);
    QSizeF size(){return m_size;}

    //Getter in parameters(.....)
    void setYoungModulus(qreal E);
    void setShearModulus(qreal G);
    void setDensity(qreal d);

    qreal length(){return m_length;}
    qreal scaleLength();

    void cloneProperties(QSharedPointer<Beam> beam);

    void split();
    void unify();

    bool enable() {return m_enable;}
    void setEnable(bool enable);

    QList<QVector4D> stress_segments(){return m_stress_segments;}
    void setStressSegment(QList<QVector4D> segments){m_stress_segments=segments;}


    /*Splitting stuff*/
    QList<QWeakPointer<Beam> > subParts(){return m_sub_parts;}
    void addPart(QSharedPointer<Beam> beam);
    void removePart(QSharedPointer<Beam> beam);

    void setParentBeam(QSharedPointer<Beam> parent);
    QWeakPointer<Beam> parentBeam(){return m_parent_beam;}

signals:
    void parametersChanged();
    void stressChanged();

    void enableChanged(bool);

    void hasBeenSplit();
    void hasBeenUnified();

private:

    /*THES ARE NOT SETTERS*/
    qreal computeTorsionShearConstant(QSizeF size=QSizeF()); // C
    qreal computeCrossSectionalArea(QSizeF size=QSizeF());// Ax
    qreal computeShearAreaY(qreal Ax=-1);//Ay
    qreal computeShearAreaZ(qreal Ax=-1);// Az
    qreal computeTorsionalMomentInertia(QSizeF size=QSizeF());//Jx
    qreal computeBendingMomentInertiaY(QSizeF size=QSizeF());//Iy
    qreal computeBendingMomentInertiaZ(QSizeF size=QSizeF());//Iz
    qreal computeSectionModulusY(QSizeF size=QSizeF());//Sy
    qreal computeSectionModulusZ(QSizeF size=QSizeF());//Sz


    bool m_enable;

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

    qreal m_axial_stress_extreme1;
    qreal m_shear_stress_y_extreme1;
    qreal m_shear_stress_z_extreme1;
    qreal m_torsional_stess_extreme1;

    qreal m_axial_stress_extreme2;
    qreal m_shear_stress_y_extreme2;
    qreal m_shear_stress_z_extreme2;
    qreal m_torsional_stess_extreme2;

    // dx,dy,dz,rotX
    QList<QVector4D> m_stress_segments;

    QSizeF m_size;

    WeakJointPtr m_extreme1,m_extreme2;

    QList<QWeakPointer<Beam> > m_sub_parts;
    QWeakPointer<Beam> m_parent_beam;

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
    m_d,	/* element mass density			*/
    m_Sy,   /*Section modulus  Y*/
    m_Sz,   /*Section modulus  Z*/
    m_C;    /*Torsion Shear Constant*/


};

typedef QSharedPointer<Beam> BeamPtr;
typedef QWeakPointer<Beam> WeakBeamPtr;

#endif // BEAM_H

