#ifndef BEAM_H
#define BEAM_H

#include<QString>
#include<QSizeF>
#include<QVector4D>
#include "statics/elements/abstractelement.h"
#include "materialsmanager.h"
#include <QDebug>

class Joint;
typedef QSharedPointer<Joint> JointPtr;
typedef QWeakPointer<Joint> WeakJointPtr;

class Beam : public AbstractElement
{
    Q_OBJECT
public:
    enum DirtyFlag {
            Clean = 0,
            ParametersChanged = 1 ,
            StressChanged = 2
    };
    Q_DECLARE_FLAGS(DirtyFlags, DirtyFlag)

    Beam(JointPtr extreme1, JointPtr extreme2,MaterialsManager* mm,QString name=QString(),QObject* parent=0);

    void extremes(WeakJointPtr& e1,WeakJointPtr& e2);

    void parameters(qreal& Ax, qreal& Asy, qreal& Asz, qreal& Jx, qreal& Iy, qreal& Iz, qreal& E, qreal& G, qreal& p, qreal& d);

    void setForcesAndMoments(int axial_type, qreal Nx, qreal Vy, qreal Vz,
                             qreal Tx,qreal My,qreal Mz,int extreme);

    void ForcesAndMoments(int& axial_type, qreal& Nx, qreal& Vy, qreal& Vz,
                          qreal& Tx,qreal& My,qreal& Mz,int extreme=0);

    void stress(qreal& axial, qreal &bending, qreal& shearY, qreal& shearZ, qreal& torsion, int extreme);

    /*The proportion between the stress and the material limit values*/
    void stressRatio(qreal& axialComponent, qreal& shearComponent,int extreme);

    void setPeakDisplacements(QVector4D min, QVector4D max);
    void peakDisplacements(QVector4D& min, QVector4D& max);



    void setSize(QSizeF size);
    QSizeF size(){return m_size;}
    QSizeF scaledSize();

    //Getter in parameters(.....)
    void setYoungModulus(qreal E);
    void setShearModulus(qreal G);
    void setDensity(qreal d);

    qreal length(){return m_length;}
    qreal scaledLength();

    QSizeF tangibleSection(){return m_tangibleSection;}
    void setTangibleSection(QSizeF val);

    void cloneProperties(QSharedPointer<Beam> beam);

    void split();
    void unify();

    bool enable() {return m_enable;}
    void setEnable(bool enable);

    QList<QVector4D> stress_segments(){return m_stress_segments;}
    void setStressSegment(QList<QVector4D> segments){m_stress_segments=segments;emit segmentsChanged();}

    QString materialID(){return m_materialId;}
    void setMaterial(QString uniqueID);

    /*Splitting stuff*/
    QList<QWeakPointer<Beam> > subParts(){return m_sub_parts;}
    void addPart(QSharedPointer<Beam> beam);
    void removePart(QSharedPointer<Beam> beam);
    void setParentBeam(QSharedPointer<Beam> parent);
    QWeakPointer<Beam> parentBeam(){return m_parent_beam;}

public slots:
    void lazy_update();

signals:
    void parametersChanged();
    void tangibleSectionChanged();
    void stressChanged();

    void segmentsChanged();
    void materialChanged();
    void enableChanged(bool);
    void hasBeenSplit();
    void hasBeenUnified();
private:



    DirtyFlags m_dirty;



    /*THES ARE NOT relativeAxialStressSETTERS*/
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


    /*Absolute peaks in the frame(Positive!)*/
    qreal m_peak_axial_force;
    qreal m_peak_axial_type;
    qreal m_peak_shear_y;
    qreal m_peak_shear_z;
    qreal m_peak_y_moment;
    qreal m_peak_z_moment;
    qreal m_peak_axial_moment;

    qreal m_peak_axial_stress;
    qreal m_peak_shear_y_stress;
    qreal m_peak_shear_z_stress;
    qreal m_peak_bending_stress;
    qreal m_peak_torsional_stress;


    qreal m_axial_stress_extreme1;
    qreal m_bending_stress_extreme1;
    qreal m_shear_stress_y_extreme1;
    qreal m_shear_stress_z_extreme1;
    qreal m_torsional_stess_extreme1;

    qreal m_axial_stress_extreme2;
    qreal m_bending_stress_extreme2;
    qreal m_shear_stress_y_extreme2;
    qreal m_shear_stress_z_extreme2;
    qreal m_torsional_stess_extreme2;


    // dx,dy,dz,rotX
    QVector4D m_min_disp,m_max_disp;
    qreal m_relative_disp;
    QList<QVector4D> m_stress_segments;


    QSizeF m_size;
    QSizeF m_tangibleSection;

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

    MaterialsManager* m_materialsManager;
    QString m_materialId;




};
Q_DECLARE_OPERATORS_FOR_FLAGS(Beam::DirtyFlags)

typedef QSharedPointer<Beam> BeamPtr;
typedef QWeakPointer<Beam> WeakBeamPtr;

#endif // BEAM_H

