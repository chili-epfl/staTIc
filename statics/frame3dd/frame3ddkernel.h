#ifndef FRAME3DDKERNEL_H
#define FRAME3DDKERNEL_H

#include <QVector>
#include <QVector3D>
#include <QTimer>
#include "../abstractstaticsmodule.h"
#include "../elements/nodeload.h"
#include "../elements/uniformlydistributedload.h"
#include "../elements/interiorpointload.h"
#include "frame3dd.h"

class Frame3DDKernel : public AbstractStaticsModule
{
    Q_OBJECT
public:
    Frame3DDKernel(QObject* parent=0);
    ~Frame3DDKernel();
    virtual qreal maxForce(){return m_maxForce;}
    virtual qreal minForce(){return m_minForce;}

    //virtual Force* createForce(QVector3D applicationPoint, QVector3D force_vector, AbstractElement* applicationElement=Q_NULLPTR);
    /*TODO: REMOVE*/
    virtual BeamPtr createBeam(JointPtr extreme1,JointPtr extreme2,QSizeF size,qreal E,
                               qreal G, qreal d,QString name=QString());

    virtual BeamPtr createBeam(JointPtr extreme1,JointPtr extreme2,QSizeF size,QString materialID,QString name=QString());

    virtual JointPtr createJoint(QVector3D position,QString name=QString(),
                               bool  support_X=false,bool support_Y=false,bool support_Z=false,
                               bool support_XX=false,bool support_YY=false,bool support_ZZ=false );
    virtual NodeLoadPtr createNodeLoad(QVector3D force, JointPtr joint,QString name=QString());
    virtual UniformlyDistributedLoadPtr createUDLoad(QVector3D force, BeamPtr beam,QString name=QString());
    virtual InteriorPointLoadPtr createIPLoad(QVector3D force, BeamPtr beam,qreal distance=-1,QString name=QString());
    virtual TrapezoidalForcePtr createTPZLoad(QVector3D force, BeamPtr beam, QVector3D localPosition, QVector2D extent, QString name=QString());
    virtual void removeBeam(BeamPtr);
    virtual void removeJoint(JointPtr);
    virtual void removeNodeLoad(NodeLoadPtr);
    virtual void removeUDLoad(UniformlyDistributedLoadPtr);
    virtual void removeIPLoad(InteriorPointLoadPtr);
    virtual void removeTPZLoad(TrapezoidalForcePtr);

    virtual bool splitBeam(BeamPtr beam, qreal offset,JointPtr &new_joint);
    virtual bool unifyBeam(BeamPtr beam);

    QVector<JointPtr> joints(){return m_joints;}
    QVector<BeamPtr> beams(){return m_beams;}
    QVector<TrapezoidalForcePtr> TPLoads(){return m_trapezoidal_loads;}
protected slots:
    virtual bool readStructure(QString path) ;
    virtual void update();
private slots:
    void onKillRequest();
    void solve();
private:
    void unifyBeam_recursive_step(BeamPtr beam);
    void setStatus(Status status);
    void assemble_loads(int nN, int nE, int nL, int DoF,
                                vec3 *xyz,
                                double *L, double *Le,
                                int *J1, int *J2,
                                float *Ax, float *Asy, float *Asz,
                                float *Iy, float *Iz, float *E, float *G,
                                float *p,
                                float *d, float *gX, float *gY, float *gZ,
                                int *r,
                                int shear,
                                int *nF, int *nU, int *nW, int *nP, int *nT, int *nD,
                                double **Q,
                                double **F_temp, double **F_mech, double *Fo,
                                float ***U, float ***W, float ***P, float ***T, float **Dp,
                                double ***eqF_mech, double ***eqF_temp,QVector<BeamPtr> active_beams);

    void update_statics( int nN, int nE, int nL, int lc, int DoF,
                        int *J1, int *J2, double *F, double *D,
                        double *R, int *r, double **Q, double err,
                         int ok, QVector<BeamPtr> active_beams);
    void write_internal_forces(int lc, int nL, float dx, vec3 *xyz,
                               double **Q, int nN, int nE, double *L, int *J1,
                               int *J2, float *Ax, float *Asy, float *Asz, float *Jx,
                               float *Iy, float *Iz, float *E, float *G, float *p,
                               float *d, float gX, float gY, float gZ, int nU,
                               float **U, int nW, float **W, int nP, float **P,
                               double *D, int shear, double error, QVector<BeamPtr> active_beams);

    QVector<JointPtr> m_joints;
    QVector<BeamPtr> m_beams;
    QVector<NodeLoadPtr> m_node_loads;
    QVector<UniformlyDistributedLoadPtr> m_uniformly_distributed_loads;
    QVector<InteriorPointLoadPtr> m_interior_point_loads;
    QVector<TrapezoidalForcePtr> m_trapezoidal_loads;

    QVector3D m_gravity;

    int m_shear;
    int m_geom;
    int m_exagg_static;
    int m_dx;

    float m_maxForce;
    float m_minForce;

    qreal m_relative_equilibrium_error;
    QTimer m_lazyupdateTimer;

};

#endif // FRAME3DDKERNEL_H
