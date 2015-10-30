#ifndef FRAME3DDKERNEL_H
#define FRAME3DDKERNEL_H

#include <QVector>
#include <QVector3D>
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

    //virtual Force* createForce(QVector3D applicationPoint, QVector3D force_vector, AbstractElement* applicationElement=Q_NULLPTR);
    virtual Beam* createBeam(Joint* extreme1,Joint* extreme2,QString name=QString(),
                             qreal Ax=0, qreal Asy=0, qreal Asz=0, qreal Jx=0,
                             qreal Iy=0, qreal Iz=0, qreal E=0, qreal G=0,
                             qreal p=0, qreal d=0);
    virtual Joint* createJoint(QVector3D position,QString name=QString(),
                               bool  support_X=false,bool support_Y=false,bool support_Z=false,
                               bool support_XX=false,bool support_YY=false,bool support_ZZ=false );
    virtual NodeLoad* createNodeLoad(QVector3D force, Joint* joint,QString name=QString());
    virtual UniformlyDistributedLoad* createUDLoad(QVector3D force, Beam* beam,QString name=QString());
    virtual InteriorPointLoad* createIPLoad(QVector3D force, Beam* beam,QString name=QString());
    QVector<Joint*> joints(){return m_joints;}
    QVector<Beam*> beams(){return m_beams;}

protected slots:
    virtual bool readStructure(QString path) ;
    virtual void update();
private slots:
    void onResourceDeleted(QObject* o);
private:
    void setStatus(Status status);
    void solve();
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
                                double ***eqF_mech, double ***eqF_temp);

    void update_statics( int nN, int nE, int nL, int lc, int DoF,
                        int *J1, int *J2, double *F, double *D,
                        double *R, int *r, double **Q, double err,
                         int ok);

    QVector<Joint*> m_joints;
    QVector<Beam*> m_beams;
    QVector<NodeLoad*> m_node_loads;
    QVector<UniformlyDistributedLoad*> m_uniformly_distributed_loads;

    QVector3D m_gravity;

    int m_shear;
    int m_geom;
    int m_exagg_static;
    int m_dx;

    qreal m_relative_equilibrium_error;

};

#endif // FRAME3DDKERNEL_H
