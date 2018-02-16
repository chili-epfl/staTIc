#include "frame3ddkernel.h"
#include <QFile>
#include <QTextStream>
#include "../elements/joint.h"
#include "../elements/beam.h"
#include "../elements/nodeload.h"
#include "../elements/interiorpointload.h"
#include "../elements/uniformlydistributedload.h"
#include "../elements/trapezoidalforce.h"

#include <QDebug>
#include <QtConcurrent>
#include <QJsonArray>
#include <QJsonObject>
#include <QByteArray>
#include <QJsonDocument>

#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"
#include "eig.h"
#include "HPGmatrix.h"
#include "NRutil.h"
#include "coordtrans.h"
#ifdef __cplusplus
}
#endif
const char separator=',';

#ifndef JOINT_MODELLING
//#define JOINT_MODELLING
#endif

/*Utility function to skip comments and new lines in the config file*/
QString readLine(QTextStream* inputStream){
    QString line;
    bool isValid=false;
    do{
        line=inputStream->readLine();
        if(line.isEmpty())
            continue;
        if(line.contains("#"))
            line=line.split("#").at(0);
        bool allEmpty=true;
        Q_FOREACH(QString part, line.split(separator)){
            if(!part.isEmpty()){
                allEmpty=false;
                break;
            }
        }
        if(allEmpty)
            continue;
        isValid=true;
    }while(!isValid);
    return line;
}


Frame3DDKernel::Frame3DDKernel(QObject* parent):
    AbstractStaticsModule(parent),
    m_gravity(0,-9800,0),
    m_shear(0),
    m_geom(1)
{
    connect(&m_lazyupdateTimer, SIGNAL(timeout()), this, SLOT(solve()));
    m_lazyupdateTimer.setSingleShot(true);
    m_lazyupdateTimer.setInterval(700);
    m_maxForce=0;
    m_minForce=0;
}

Frame3DDKernel::~Frame3DDKernel()
{
    m_lazyupdateTimer.stop();
    m_interior_point_loads.clear();
    m_node_loads.clear();
    m_uniformly_distributed_loads.clear();
    m_trapezoidal_loads.clear();
    m_beams.clear();
    m_joints.clear();
}

qreal Frame3DDKernel::maxForce()
{
    return m_maxForce;
}

qreal Frame3DDKernel::minForce()
{
    return 0;
}


bool Frame3DDKernel::readStructure(QString path){

    QHash<QString,int> pointKeys2Idx;

    QFile inputFile(path);
    if (!inputFile.open(QIODevice::ReadOnly)){
        qDebug()<<"Failed to open structure file";
        setStatus(Status::ERROR);
        return false;
    }

    QByteArray data = inputFile.readAll();
    QJsonParseError error;
    QJsonDocument doc(QJsonDocument::fromJson(data, &error));
    if(error.error!=QJsonParseError::NoError){
        qDebug()<<"Failed to open structure file";
        setStatus(Status::ERROR);
        return false;
    }
    auto root=doc.object();

    bool is2d=true;

    /*Model to real scale factor*/
    if(!root["scaleFactorPhysical"].isDouble()){
        qWarning("Fail to read model scale factor");
        setStatus(Status::ERROR);
        return false;
    }
    m_modelScale=root["scaleFactorPhysical"].toDouble();

    /*Get number of nodes*/
    if(!root["points"].isObject()){
        qWarning("Fail to read number of nodes");
        setStatus(Status::ERROR);
        return false;
    }
    auto pointsJson=root["points"].toObject();
    m_joints.reserve(pointsJson.size());

    /*Read node data*/
    for(const QString &key : pointsJson.keys()){
        auto point=pointsJson[key].toObject();
        if(!point["x"].isDouble()){
            qWarning("Fail to convert coordinate:");
            setStatus(Status::ERROR);
            return false;
        }
        qreal x=point["x"].toDouble();

        if(!point["y"].isDouble()){
            qWarning("Fail to convert coordinate:");
            setStatus(Status::ERROR);
            return false;
        }
        qreal y=point["y"].toDouble();

        if(!point["z"].isDouble()){
            qWarning("Fail to convert coordinate:");
            setStatus(Status::ERROR);
            return false;
        }
        qreal z=point["z"].toDouble();

        if(!qFuzzyCompare(z,0)){
            is2d=false;
        }
        if(!point["name"].isString()){
            qWarning("Fail to convert coordinate:");
            setStatus(Status::ERROR);
            return false;
        }
        if(!point["reac_x"].isBool()){
            qWarning("Fail to read reaction");
            setStatus(Status::ERROR);
            return false;
        }
        bool reac_x=point["reac_x"].toBool();
        if(!point["reac_y"].isBool()){
            qWarning("Fail to read reaction");
            setStatus(Status::ERROR);
            return false;
        }
        bool reac_y=point["reac_y"].toBool();
        if(!point["reac_z"].isBool()){
            qWarning("Fail to read reaction");
            setStatus(Status::ERROR);
            return false;
        }
        bool reac_z=point["reac_z"].toBool();
        if(!point["reac_xx"].isBool()){
            qWarning("Fail to read reaction");
            setStatus(Status::ERROR);
            return false;
        }
        bool reac_xx=point["reac_xx"].toBool();
        if(!point["reac_yy"].isBool()){
            qWarning("Fail to read reaction");
            setStatus(Status::ERROR);
            return false;
        }
        bool reac_yy=point["reac_yy"].toBool();
        if(!point["reac_zz"].isBool()){
            qWarning("Fail to read reaction");
            setStatus(Status::ERROR);
            return false;
        }
        bool reac_zz=point["reac_zz"].toBool();
        pointKeys2Idx[key]=m_joints.size();
        createJoint(QVector3D(x,y,z),point["name"].toString(),reac_x,reac_y,reac_z,reac_xx,reac_yy,reac_zz);

    }
    auto linesJson=root["lines"].toObject();
    m_beams.reserve(linesJson.size());

    /*Read beam data*/
    for(const QString &key : linesJson.keys()){
        auto line=linesJson[key].toObject();

        if(!line["p1"].isString() || !pointKeys2Idx.contains(line["p1"].toString())){
            qWarning("Fail to convert extreme index");
            setStatus(Status::ERROR);
            return false;
        }
        int joint_index_1=pointKeys2Idx[line["p1"].toString()];

        if(!line["p2"].isString() || !pointKeys2Idx.contains(line["p2"].toString())){
            qWarning("Fail to convert extreme index");
            setStatus(Status::ERROR);
            return false;
        }
        int joint_index_2=pointKeys2Idx[line["p2"].toString()];
        JointPtr first=m_joints.at(joint_index_1);
        JointPtr second=m_joints.at(joint_index_2);

        if(!line["width"].isDouble()){
            qWarning("Fail to convert area x element");
            setStatus(Status::ERROR);
            return false;
        }
        qreal area_x=line["width"].toDouble();

        if(!line["height"].isDouble()){
            qWarning("Fail to convert area y element");
            setStatus(Status::ERROR);
            return false;
        }
        qreal area_y=line["height"].toDouble();

        if(!line["materialID"].isString()){
            qWarning("Fail to convert material");
            setStatus(Status::ERROR);
            return false;
        }
        QString materialID=line["materialID"].toString();

        if(!line["name"].isString()){
            qWarning("Fail to beam name");
            setStatus(Status::ERROR);
            return false;
        }

        createBeam(first,second,QSizeF(area_x,area_y),materialID,line["name"].toString());

    }

    solve();
    setStatus(Status::LOADED);
    emit is2DChanged();
    emit modelScaleChanged();
    emit initialPoseChanged();
    return true;

}

//bool Frame3DDKernel::readStructure(QString path){

//    bool is2d=true;
//    QList<QVector3D> tmp_nodes;
//    QStringList tmp_nodes_names;

//    QFile inputFile(path);
//    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)){
//        qDebug()<<"Failed to open structure file";
//        setStatus(Status::ERROR);
//        return false;
//    }

//    QTextStream inputStream(&inputFile);

//    QString line=readLine(&inputStream);
//    /*Model to real scale factor*/
//    bool ok;
//    m_modelScale=line.split(separator).at(0).toDouble(&ok);
//    if(!ok){
//        qWarning("Fail to read model scale factor");
//        setStatus(Status::ERROR);
//        return false;
//    }
//    /*Get number of nodes*/
//    line=readLine(&inputStream);
//    int number_nodes=line.split(separator).at(0).toInt(&ok);
//    if(!ok){
//        qWarning("Fail to read number of nodes");
//        setStatus(Status::ERROR);
//        return false;
//    }
//    m_joints.reserve(number_nodes);

//    /*Read node data*/
//    for(int i=0;i<number_nodes;i++){
//        line=readLine(&inputStream);
//        QStringList line_parts=line.split(separator);
//        if(line_parts.size()!=5){
//            qWarning("Issue reading node line:");
//            qWarning(line.toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        qreal x=line_parts[1].toFloat(&ok);
//        if(!ok){
//            qWarning("Fail to convert coordinate:");
//            qWarning(line_parts[1].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        qreal y=line_parts[2].toFloat(&ok);
//        if(!ok){
//            qWarning("Fail to convert coordinate:");
//            qWarning(line_parts[2].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        qreal z=line_parts[3].toFloat(&ok);
//        if(!ok){
//            qWarning("Fail to convert coordinate:");
//            qWarning(line_parts[3].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        if(z!=0){
//            is2d=false;
//        }
//        tmp_nodes.append(QVector3D(x,y,z));
//        tmp_nodes_names.append(line_parts[0]);
//    }
//    m_is2D=is2d;

//    for(int i=0;i<tmp_nodes.size();i++){
//        if(is2d){
//            createJoint(tmp_nodes[i],tmp_nodes_names[i]);
//        }
//        else{
//            //createJoint(QVector3D(tmp_nodes[i].x(),tmp_nodes[i].z(),-tmp_nodes[i].y()),tmp_nodes_names[i]);
//            createJoint(QVector3D(tmp_nodes[i].x(),tmp_nodes[i].y(),tmp_nodes[i].z()),tmp_nodes_names[i]);

//        }
//    }



//    int DoF=6*number_nodes;
//    /*Read number of reactions */
//    line=readLine(&inputStream);
//    int number_reactions=line.split(separator).at(0).toInt(&ok);
//    if(!ok){
//        qWarning("Fail to read number of reactions");
//        qWarning(line.toStdString().c_str());
//        setStatus(Status::ERROR);
//        return false;
//    }
//    if(number_reactions<0 || number_reactions >DoF/6){
//        qWarning("Invalid number of reactions");
//        setStatus(Status::ERROR);
//        return false;
//    }
//    /*Get reactions*/
//    for(int i=0;i<number_reactions;i++){
//        line=readLine(&inputStream);
//        QStringList line_parts=line.split(separator);
//        if(line_parts.size()!=7){
//            qWarning("Issue reading reaction line:");
//            qWarning(line.toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        bool support_x=(line_parts[1].toInt(&ok)==0) ? false : true;
//        if(!ok){
//            qWarning("Fail to convert support");
//            qWarning(line_parts[1].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        bool support_y=(line_parts[2].toInt(&ok)==0) ? false : true;
//        if(!ok){
//            qWarning("Fail to convert support");
//            qWarning(line_parts[2].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        bool support_z=(line_parts[3].toInt(&ok)==0) ? false : true;
//        if(!ok){
//            qWarning("Fail to convert support");
//            qWarning(line_parts[3].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        bool support_xx=(line_parts[4].toInt(&ok)==0) ? false : true;
//        if(!ok){
//            qWarning("Fail to convert support");
//            qWarning(line_parts[4].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        bool support_yy=(line_parts[5].toInt(&ok)==0) ? false : true;
//        if(!ok){
//            qWarning("Fail to convert support");
//            qWarning(line_parts[5].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        bool support_zz=(line_parts[6].toInt(&ok)==0) ? false : true;
//        if(!ok){
//            qWarning("Fail to convert support");
//            qWarning(line_parts[6].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        int jointIndex=line_parts[0].toInt(&ok);
//        if(!ok){
//            qWarning("Fail to read joint support");
//            qWarning(line_parts[0].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        if(jointIndex>=m_joints.size() ||jointIndex<0){
//            qWarning("Invalid support index:");
//            qWarning()<<jointIndex;
//            setStatus(Status::ERROR);
//            return false;
//        }
//        if(is2d)
//            m_joints[jointIndex]->setSupport(support_x,support_y,support_z,
//                                             support_xx,support_yy,support_zz);
//        else
//            //            m_joints[jointIndex]->setSupport(support_x,support_z,support_y,
//            //                                                          support_xx,support_zz,support_yy);
//            m_joints[jointIndex]->setSupport(support_x,support_y,support_z,
//                                             support_xx,support_yy,support_zz);
//    }

//    /*Read number of beams*/
//    line=readLine(&inputStream);
//    int number_beams=line.split(separator).at(0).toInt(&ok);
//    if(!ok){
//        qWarning("Fail to read number of elements");
//        qWarning(line.toStdString().c_str());
//        setStatus(Status::ERROR);
//        return false;
//    }
//    if(number_nodes>number_beams+1){
//        qWarning("Not enough elements");
//        setStatus(Status::ERROR);
//        return false;
//    }
//    m_beams.reserve(number_beams);
//    /*Read beams*/
//    for(int i=0;i<number_beams;i++){
//        line=readLine(&inputStream);
//        QStringList line_parts=line.split(separator);
//        if(line_parts.size()!=8){
//            qWarning("Issue reading element line:");
//            qWarning(line.toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        int joint_index_1=line_parts[1].toInt(&ok);
//        if(!ok){
//            qWarning("Fail to convert extreme index");
//            qWarning(line_parts[1].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        int joint_index_2=line_parts[2].toInt(&ok);
//        if(!ok){
//            qWarning("Fail to convert extreme index");
//            qWarning(line_parts[2].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        if(joint_index_1==joint_index_2 ||
//                joint_index_1>=m_joints.size() ||
//                joint_index_2>=m_joints.size() ||
//                joint_index_1<0 || joint_index_2<0){
//            qWarning("Invalid etreme index:");
//            qWarning()<<joint_index_1;
//            qWarning()<<joint_index_2;
//            setStatus(Status::ERROR);
//            return false;
//        }
//        JointPtr first=m_joints.at(joint_index_1);
//        JointPtr second=m_joints.at(joint_index_2);
//        qreal area_x=line_parts[3].toDouble(&ok);
//        if(!ok){
//            qWarning("Fail to convert area x element");
//            qWarning(line_parts[3].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        qreal area_y=line_parts[4].toDouble(&ok);
//        if(!ok){
//            qWarning("Fail to convert area y element");
//            qWarning(line_parts[4].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        QString materialID=line_parts[5];
//        if(materialID.isEmpty()){
//            qWarning("Fail to convert material");
//            qWarning(line_parts[5].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        qreal tangibleWidth=line_parts[6].toDouble(&ok);
//        if(!ok){
//            qWarning("Fail to convert Tangible Width");
//            qWarning(line_parts[6].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        qreal tangibleHeight=line_parts[7].toDouble(&ok);
//        if(!ok){
//            qWarning("Fail to convert Tangible Height");
//            qWarning(line_parts[7].toStdString().c_str());
//            setStatus(Status::ERROR);
//            return false;
//        }
//        BeamPtr beam=createBeam(first,second,QSizeF(area_x,area_y),materialID,line_parts[0]);
//        beam->setTangibleSection(QSizeF(tangibleWidth,tangibleHeight));
//    }
//    solve();
//    setStatus(Status::LOADED);
//    emit is2DChanged();
//    emit modelScaleChanged();
//    emit initialPoseChanged();
//    return true;
//}

void Frame3DDKernel::setStatus(Status status){
    if(m_status!=status){
        m_status=status;
        emit statusChanged();
    }
}

void Frame3DDKernel::update(){
    emit startingUpdate();
    m_maxForce=0;
    emit maxForceChanged();
    m_lazyupdateTimer.start();
}
/**/
void Frame3DDKernel::solve(){
    if(m_status!=Status::LOADED) return;

    vec3	*xyz;		// X,Y,Z node coordinates (global)

    float	*rj = NULL,	// node size radius, for finite sizes
            *Ax,*Asy, *Asz,	// cross section areas, incl. shear
            *Jx,*Iy,*Iz,	// section inertias
            *E=NULL, *G=NULL,// elastic modulus and shear moduli
            *p=NULL,	// roll of each member, radians
            ***U=NULL,	// uniform distributed member loads
            ***W=NULL,	// trapizoidal distributed member loads
            ***P=NULL,	// member concentrated loads
            ***T=NULL,	// member temperature  loads
            **Dp=NULL,	// prescribed node displacements
            *d, *EMs=NULL,	// member densities and extra inertia
            *NMs=NULL, 	// mass of a node
            *NMx,*NMy,*NMz,	// inertia of a node in global coord
            gX[_NL_],	// gravitational acceleration in global X
            gY[_NL_],	// gravitational acceleration in global Y
            gZ[_NL_];	// gravitational acceleration in global Z
    /*
        pan=1.0,	// >0: pan during animation; 0: don't
        scale=1.0,	// zoom scale for 3D plotting in Gnuplot
        dx=1.0;		// x-increment for internal force data
*/
    double	**K=NULL,	// equilibrium stiffness matrix
            // **Ks=NULL,	// Broyden secant stiffness matrix
            traceK = 0.0,	// trace of the global stiffness matrix
            **M = NULL,	// global mass matrix
            traceM = 0.0,	// trace of the global mass matrix
            ***eqF_mech=NULL,// equivalent end forces from mech loads global
            ***eqF_temp=NULL,// equivalent end forces from temp loads global
            **F_mech=NULL,	// mechanical load vectors, all load cases
            **F_temp=NULL,	// thermal load vectors, all load cases
            *F  = NULL, 	// total load vectors for a load case
            *R  = NULL,	// total reaction force vector
            *dR = NULL,	// incremental reaction force vector
            *D  = NULL,	// displacement vector
            *dD = NULL,	// incremental displacement vector
            //dDdD = 0.0,	// dD' * dD
            *dF = NULL,	// equilibrium error in nonlinear anlys
            *L  = NULL,	// node-to-node length of each element
            *Le = NULL,	// effcve lngth, accounts for node size
            **Q = NULL,	// local member node end-forces
            tol = 1.0e-9,	// tolerance for modal convergence
            shift = 0.0,	// shift-factor for rigid-body-modes
            struct_mass,	// mass of structural system
            total_mass,	// total structural mass and extra mass
            *f  = NULL,	// resonant frequencies
            **V = NULL,	// resonant mode-shapesIf singleShot is true, the timer will be activated only once.
            rms_resid=1.0,	// root mean square of residual displ. error
            error = 1.0,	// rms equilibrium error and reactions
            Cfreq = 0.0,	// frequency used for Guyan condensation
            **Kc, **Mc,	// condensed stiffness and mass matrices
            exagg_static=10,// exaggerate static displ. in mesh data
            exagg_modal=10;	// exaggerate modal displ. in mesh data

    // peak internal forces, moments, and displacments
    // in each frame element and each load case
    double	**pkNx, **pkVy, **pkVz, **pkTx, **pkMy, **pkMz,
            **pkDx, **pkDy, **pkDz, **pkRx, **pkSy, **pkSz;

    int	nN=0,		// number of Nodes
            nE=0,		// number of frame Elements
            nL=0, lc=0,	// number of Load cases
            DoF=0, i, j,	// number of Degrees of Freedom
            nR=0,		// number of restrained nodes
            nD[_NL_],	// number of prescribed nodal displ'nts
            nF[_NL_],	// number of loaded nodes
            nU[_NL_],	// number of members w/ unifm dist loads
            nW[_NL_],	// number of members w/ trapz dist loads
            nP[_NL_],	// number of members w/ conc point loads
            nT[_NL_],	// number of members w/ temp. changes
            nI=0,		// number of nodes w/ extra inertia
            nX=0,		// number of elemts w/ extra mass
            nC=0,		// number of condensed nodes
            *N1, *N2,	// begin and end node numbers
            shear=0,	// indicates shear deformation
            geom=0,		// indicates  geometric nonlinearity
            anlyz=1,	// 1: stiffness analysis, 0: data check
            *q=NULL,*r=NULL,sumR,	// reaction data, total no. of reactions
            nM=0,		// number of desired modes
            Mmethod,	// 1: Subspace Jacobi, 2: Stodola
            nM_calc,	// number of modes to calculate
            lump=1,		// 1: lumped, 0: consistent mass matrix
            iter=0,		// number of iterations
            ok=1,		// number of (-ve) diag. terms of L D L'
            anim[128],	// the modes to be animated
            Cdof=0,		// number of condensed degrees o freedom
            Cmethod=0,	// matrix condensation method
            *c=NULL,	// vector of DoF's to condense
            *m=NULL;	// vector of modes to condense

    double	exagg_flag=-1.0, // over-ride input file value
            tol_flag  =-1.0, // over-ride input file value
            shift_flag=-1.0; // over-ride input file value

    float	pan_flag = -1.0; // over-ride input file value

    char	extn[16];	// Input Data file name extension


    /*    parse_options ( argc, argv, IN_file, OUT_file,
            &shear_flag, &geom_flag, &anlyz_flag, &exagg_flag,
            &D3_flag,
            &lump_flag, &modal_flag, &tol_flag, &shift_flag,
            &pan_flag, &write_matrix, &axial_sign, &condense_flag,
            &verbose, &debug);
*/
    /*    if ( verbose ) { /*  display program name, version and license type */
    /*        textColor('w','b','b','x');
        fprintf(stdout,"\n FRAME3DD version: %s\n", VERSION);
        fprintf(stdout," Analysis of 2D and 3D structural frames with elastic and geometric stiffness.\n");
        fprintf(stdout," http://frame3dd.sf.net\n");
        fprintf(stdout," GPL Copyright (C) 1992-2014, Henri P. Gavin\n");
        fprintf(stdout," This is free software with absolutely no warranty.\n");
        fprintf(stdout," For details, see the GPL license file, LICENSE.txt\n");
        color(0); fprintf(stdout,"\n");
    }

    /* open the input data file */

    /*    if ((fp = fopen (IN_file, "r")) == NULL) { /* open input data file */
    /*       sprintf (errMsg,"\n ERROR: cannot open input data file '%s'\n", IN_file);
        errorMsg(errMsg);
        display_help();
        if ( argc == 1 ) {
            fprintf(stderr," Press the 'Enter' key to close.\n");
            (void) getchar();	// clear the buffer ??
            while( !getchar() ) ;	// wait for the Enter key
        }
        exit(11);
    }

    filetype = get_file_ext( IN_file, extn ); /* .CSV or .FMM or other? */

    //	temp_file_location("frame3dd.3dd",strippedInputFile,FRAME3DD_PATHMAX);
    //    output_path("frame3dd.3dd",strippedInputFile,FRAME3DD_PATHMAX,NULL);
    /*
    parse_input(fp, strippedInputFile);	/* strip comments from input data */
    //    fclose(fp);

    //    if ((fp = fopen (strippedInputFile, "r")) == NULL) { /* open stripped input file */
    //        sprintf(errMsg,"\n ERROR: cannot open stripped input data file '%s'\n", strippedInputFile);
    /*        errorMsg(errMsg);
        exit(13);
    }

    frame3dd_getline(fp, title, MAXL);
    if ( verbose ) {	/*  display analysis title */
    /*        textColor('w','g','b','x');
        fprintf(stdout,"\n");
        fprintf(stdout," ** %s ** \n", title );
        color(0);
        fprintf(stdout,"\n");
    }
*/
    QVector<JointPtr> active_joints;
    Q_FOREACH(JointPtr joint,m_joints){
        if(joint->connectedBeams().size()>0){
            active_joints.append(joint);
        }
    }
    QVector<BeamPtr> active_beams;
    Q_FOREACH(BeamPtr beam,m_beams){
        if(beam->enabled()){
            active_beams.append(beam);
        }
    }

    nN=active_joints.size()
        #ifdef JOINT_MODELLING
            +2*active_beams.length()
        #endif
            ;

    //    sfrv=fscanf(fp, "%d", &nN );		/* number of nodes	*/
    //    if (sfrv != 1)	sferr("nN value for number of nodes");
    //    if ( verbose ) {	/* display nN */
    //        fprintf(stdout," number of nodes ");
    //        dots(stdout,36);	fprintf(stdout," nN =%4d ",nN);
    //    }

    /* allocate memory for node data ... */
    rj  =  vector(1,nN);		/* rigid radius around each node */
    xyz = (vec3 *)malloc(sizeof(vec3)*(1+nN));	/* node coordinates */

    /*Set node data*/
    for(i=0; i<active_joints.size();i++){
        JointPtr joint=active_joints.at(i);
        xyz[i+1].x=static_cast<double>(joint->position().x());
        if(m_is2D){
            xyz[i+1].y=static_cast<double>(joint->position().y());
            xyz[i+1].z=static_cast<double>(joint->position().z());
        }
        else{
            //            xyz[i+1].z=joint->position().y();
            //            xyz[i+1].y=-joint->position().z();
            xyz[i+1].y=static_cast<double>(joint->position().y());
            xyz[i+1].z=-static_cast<double>(joint->position().z());
        }
        rj[i+1]=0.0;
    }

#ifdef JOINT_MODELLING
    for(i=0;i<active_beams.length();i++){
        BeamPtr beam=active_beams[i];
        WeakJointPtr e1,e2;
        beam->extremes(e1,e2);
        JointPtr _e1=e1.toStrongRef();
        JointPtr _e2=e2.toStrongRef();
        QVector3D pos;
        //Virtual extreme 1
        pos=_e1->position()+100*(_e2->position()-_e1->position()).normalized();
        xyz[i*2+1+m_joints.length()].x=static_cast<double>(pos.x());
        xyz[i*2+1+m_joints.length()].y=static_cast<double>(pos.y());
        xyz[i*2+1+m_joints.length()].z=-static_cast<double>(pos.z());
        //Virtual extreme 2
        pos=_e2->position()+100*(_e1->position()-_e2->position()).normalized();
        xyz[i*2+2+m_joints.length()].x=static_cast<double>(pos.x());
        xyz[i*2+2+m_joints.length()].y=static_cast<double>(pos.y());
        xyz[i*2+2+m_joints.length()].z=-static_cast<double>(pos.z());
    }
#endif


    DoF = 6*nN;		/* total number of degrees of freedom	*/

    q   = ivector(1,DoF);	/* allocate memory for reaction data ... */
    r   = ivector(1,DoF);	/* allocate memory for reaction data ... */

    /*Set reaction data*/
    for (i=1; i<=DoF; i++)	r[i] = 0;
    nR=0;
    for(i=0;i<active_joints.size();i++){
        JointPtr joint=active_joints.at(i);
        bool X,Y,Z,XX,YY,ZZ;
        joint->support(X,Y,Z,XX,YY,ZZ);
        if(X || Y || Z || XX || YY || ZZ){
            nR++;
            r[i*6+1]=static_cast<int>(X);
            r[i*6+4]=static_cast<int>(XX);
            //            if(m_is2D){
            r[i*6+2]=static_cast<int>(Y);
            r[i*6+3]=static_cast<int>(Z);
            r[i*6+5]=static_cast<int>(YY);
            r[i*6+6]=static_cast<int>(ZZ);
            //            }else{
            ////                r[i*6+2]=Z;
            ////                r[i*6+3]=Y;
            ////                r[i*6+5]=ZZ;
            ////                r[i*6+6]=YY;
            //                r[i*6+2]=(int)Y;
            //                r[i*6+3]=(int)Z;
            //                r[i*6+5]=(int)YY;
            //                r[i*6+6]=(int)ZZ;
            //            }
        }
    }
#ifdef JOINT_MODELLING
    for(i=m_joints.size();i<nN;i++){
        r[i*6+1]=0;
        r[i*6+2]=0;
        r[i*6+3]=1;
        r[i*6+4]=1;
        r[i*6+5]=1;
        r[i*6+6]=0;
    }
#endif

    sumR=0;	for (i=1;i<=DoF;i++) sumR += r[i];
    for (i=1; i<=DoF; i++)
        if (r[i])
            q[i] = 0;
        else
            q[i] = 1;

    nE=active_beams.size()
        #ifdef JOINT_MODELLING
            *3
        #endif
            ;
    /* allocate memory for frame elements ... */
    L   = dvector(1,nE);	/* length of each element		*/
    Le  = dvector(1,nE);	/* effective length of each element	*/

    N1  = ivector(1,nE);	/* node #1 of each element		*/
    N2  = ivector(1,nE);	/* node #2 of each element		*/

    Ax  =  vector(1,nE);	/* cross section area of each element	*/
    Asy =  vector(1,nE);	/* shear area in local y direction 	*/
    Asz =  vector(1,nE);	/* shear area in local z direction	*/
    Jx  =  vector(1,nE);	/* torsional moment of inertia 		*/
    Iy  =  vector(1,nE);	/* bending moment of inertia about y-axis */
    Iz  =  vector(1,nE);	/* bending moment of inertia about z-axis */

    E   =  vector(1,nE);	/* frame element Young's modulus	*/
    G   =  vector(1,nE);	/* frame element shear modulus		*/
    p   =  vector(1,nE);	/* element rotation angle about local x axis */
    d   =  vector(1,nE);	/* element mass density			*/

    /*Set beam data*/
    for(i=0;i<active_beams.length();i++){
        BeamPtr beam=active_beams.at(i);
        L[i+1]=static_cast<double>(beam->length())
        #ifdef JOINT_MODELLING
                -200
        #endif
                ;
        Le[i+1]=static_cast<double>(beam->length())
        #ifdef JOINT_MODELLING
                -200
        #endif
                ;
        WeakJointPtr e1,e2;
        beam->extremes(e1,e2);
        bool need_swap=false;

        QVector3D pos_e1=e1.toStrongRef()->position();
        QVector3D pos_e2=e2.toStrongRef()->position();

        if(!m_is2D && qFuzzyCompare(pos_e1.x(),pos_e2.x()) && qFuzzyCompare(pos_e1.y(),pos_e2.y()))
            need_swap=true;

#ifdef JOINT_MODELLING
        if(need_swap){
            N1[i+1]=m_joints.length()+2*i+2;
            N2[i+1]=m_joints.length()+2*i+1;
        }else{
            N1[i+1]=m_joints.length()+2*i+1;
            N2[i+1]=m_joints.length()+2*i+2;
        }
#else
        if(need_swap){
            N1[i+1]=active_joints.indexOf(e2.toStrongRef())+1;
            N2[i+1]=active_joints.indexOf(e1.toStrongRef())+1;
        }else{
            N1[i+1]=active_joints.indexOf(e1.toStrongRef())+1;
            N2[i+1]=active_joints.indexOf(e2.toStrongRef())+1;
        }

#endif
        qreal beam_Ax,beam_Asy,beam_Asz,beam_Jx,beam_Iy,beam_Iz,beam_E,beam_G,beam_p,beam_d;
        beam->parameters(beam_Ax,beam_Asy,beam_Asz,beam_Jx,beam_Iy,beam_Iz,beam_E,beam_G,beam_p,beam_d);
        Ax[i+1]=static_cast<float>(beam_Ax);
        Jx[i+1]=static_cast<float>(beam_Jx);
        Asy[i+1]=static_cast<float>(beam_Asy);
        Asz[i+1]=static_cast<float>(beam_Asz);
        Iy[i+1]=static_cast<float>(beam_Iy);
        Iz[i+1]=static_cast<float>(beam_Iz);
        E[i+1]=static_cast<float>(beam_E);
        G[i+1]=static_cast<float>(beam_G);
        p[i+1]=static_cast<float>(beam_p);
        d[i+1]=static_cast<float>(beam_d);
        //        if(beam->objectName()=="CD"){
        //           Jx[i+1]=Jx[i+1]*0.0001;
        //           Iy[i+1]=Iy[i+1]*0.0001;
        //           Iz[i+1]=Iz[i+1]*0.0001;
        //        }
#ifdef JOINT_MODELLING
        //Virtual beam 1
        L[2*i+1+active_beams.length()]=100.0;
        Le[2*i+1+active_beams.length()]=100.0;
        N1[2*i+1+active_beams.length()]=m_joints.indexOf(e1.toStrongRef())+1;
        N2[2*i+1+active_beams.length()]=m_joints.length()+i*2+1;

        Ax[2*i+1+active_beams.length()]=beam_Ax/3;
        Jx[2*i+1+active_beams.length()]=beam_Jx/3;
        Asy[2*i+1+active_beams.length()]=beam_Asy/3;
        Asz[2*i+1+active_beams.length()]=beam_Asz/3;
        Iy[2*i+1+active_beams.length()]=beam_Iy/3;
        Iz[2*i+1+active_beams.length()]=beam_Iz/3;
        E[2*i+1+active_beams.length()]=beam_E;
        G[2*i+1+active_beams.length()]=beam_G;
        p[2*i+1+active_beams.length()]=0;
        d[2*i+1+active_beams.length()]=beam_d;

        Ax[2*i+1+active_beams.length()]=650;
        Jx[2*i+1+active_beams.length()]=41623;
        Asy[2*i+1+active_beams.length()]=65;
        Asz[2*i+1+active_beams.length()]=65;
        Iy[2*i+1+active_beams.length()]=416;
        Iz[2*i+1+active_beams.length()]=416;


        E[2*i+1+active_beams.length()]=beam_E;
        G[2*i+1+active_beams.length()]=beam_G;
        p[2*i+1+active_beams.length()]=0;
        d[2*i+1+active_beams.length()]=beam_d;


        //Virtual beam 2
        L[2*i+2+active_beams.length()]=100.0;
        Le[2*i+2+active_beams.length()]=100.0;

        N1[2*i+2+active_beams.length()]=m_joints.length()+i*2+2;
        N2[2*i+2+active_beams.length()]=m_joints.indexOf(e2.toStrongRef())+1;

        Ax[2*i+2+active_beams.length()]=Ax[2*i+1+active_beams.length()];
        Jx[2*i+2+active_beams.length()]=Jx[2*i+1+active_beams.length()];
        Asy[2*i+2+active_beams.length()]=Asy[2*i+1+active_beams.length()];
        Asz[2*i+2+active_beams.length()]=Asz[2*i+1+active_beams.length()];
        Iy[2*i+2+active_beams.length()]=Iy[2*i+1+active_beams.length()];
        Iz[2*i+2+active_beams.length()]=Iz[2*i+1+active_beams.length()];
        E[2*i+2+active_beams.length()]=E[2*i+1+active_beams.length()];
        G[2*i+2+active_beams.length()]=G[2*i+1+active_beams.length()];
        p[2*i+2+active_beams.length()]=0;
        d[2*i+2+active_beams.length()]=d[2*i+1+active_beams.length()];
#endif

    }

    /*Set Run data*/
    shear=m_shear;
    geom=m_geom;
    //exagg_static=m_exagg_static;
    //dx=m_dx;

    nL=1;//NuMBER OF CASES...IN OUR CASE JUST 1

    /* allocate memory for loads ... */
    U   =  D3matrix(1,nL,1,nE,1,4);    /* uniform load on each member */
    W   =  D3matrix(1,nL,1,10*nE,1,13);/* trapezoidal load on each member */
    P   =  D3matrix(1,nL,1,10*nE,1,5); /* internal point load each member */
    T   =  D3matrix(1,nL,1,nE,1,8);    /* internal temp change each member*/
    Dp  =  matrix(1,nL,1,DoF); /* prescribed displacement of each node */

    F_mech  = dmatrix(1,nL,1,DoF);	/* mechanical load vector	*/
    F_temp  = dmatrix(1,nL,1,DoF);	/* temperature load vector	*/
    F       = dvector(1,DoF);	/* external load vector	*/
    dF	= dvector(1,DoF);	/* equilibrium error {F} - [K]{D} */

    eqF_mech =  D3dmatrix(1,nL,1,nE,1,12); /* eqF due to mech loads */
    eqF_temp =  D3dmatrix(1,nL,1,nE,1,12); /* eqF due to temp loads */

    K   = dmatrix(1,DoF,1,DoF);	/* global stiffness matrix	*/
    Q   = dmatrix(1,nE,1,12);	/* end forces for each member	*/

    D   = dvector(1,DoF);	/* displacments of each node		*/
    dD  = dvector(1,DoF);	/* incremental displ. of each node	*/
    R   = dvector(1,DoF);	/* reaction forces			*/
    dR  = dvector(1,DoF);	/* incremental reaction forces		*/

    EMs =  vector(1,nE);	/* lumped mass for each frame element	*/
    NMs =  vector(1,nN);	/* node mass for each node		*/
    NMx =  vector(1,nN);	/* node inertia about global X axis	*/
    NMy =  vector(1,nN);	/* node inertia about global Y axis	*/
    NMz =  vector(1,nN);	/* node inertia about global Z axis	*/

    c = ivector(1,DoF); 	/* vector of condensed degrees of freedom */
    m = ivector(1,DoF); 	/* vector of condensed mode numbers	*/

    // peak axial forces, shears, torques, and moments along each element
    pkNx = dmatrix(1,nL,1,nE);
    pkVy = dmatrix(1,nL,1,nE);
    pkVz = dmatrix(1,nL,1,nE);
    pkTx = dmatrix(1,nL,1,nE);
    pkMy = dmatrix(1,nL,1,nE);
    pkMz = dmatrix(1,nL,1,nE);

    // peak displacements and slopes along each element
    pkDx = dmatrix(1,nL,1,nE);
    pkDy = dmatrix(1,nL,1,nE);
    pkDz = dmatrix(1,nL,1,nE);
    pkRx = dmatrix(1,nL,1,nE);
    pkSy = dmatrix(1,nL,1,nE);
    pkSz = dmatrix(1,nL,1,nE);

    assemble_loads( nN, nE, nL, DoF, xyz, L, Le, N1, N2,
                    Ax,Asy,Asz, Iy,Iz, E, G, p,
                    d, gX, gY, gZ, r, shear,
                    nF, nU, nW, nP, nT, nD,
                    Q, F_temp, F_mech, F, U, W, P, T,
                    Dp, eqF_mech, eqF_temp,active_joints,active_beams );

    total_mass = struct_mass = 0.0;
    nM=0;
    Cmethod = nC = Cdof = 0;

    srand(time(NULL));
    for (lc=1; lc<=nL; lc++) {	/* begin load case analysis loop */

        /*  initialize displacements and displ. increment to {0}  */
        /*  initialize reactions     and react. increment to {0}  */
        for (i=1; i<=DoF; i++)	D[i] = dD[i] = R[i] = dR[i] = 0.0;

        /*  initialize internal element end forces Q = {0}	*/
        for (i=1; i<=nE; i++)	for (j=1;j<=12;j++)	Q[i][j] = 0.0;

        /*  elastic stiffness matrix  [K({D}^(i))], {D}^(0)={0} (i=0) */
        assemble_K ( K, DoF, nE, xyz, rj, L, Le, N1, N2,
                     Ax, Asy, Asz, Jx,Iy,Iz, E, G, p,
                     shear, geom, Q, 0 );

        /* first apply temperature loads only, if there are any ... */
        if (nT[lc] > 0) {
            /*  solve {F_t} = [K({D=0})] * {D_t} */
            solve_system(K,dD,F_temp[lc],dR,DoF,q,r,&ok,0,&rms_resid);

            /* increment {D_t} = {0} + {D_t} temp.-induced displ */
            for (i=1; i<=DoF; i++)	if (q[i]) D[i] += dD[i];
            /* increment {R_t} = {0} + {R_t} temp.-induced react */
            for (i=1; i<=DoF; i++)	if (r[i]) R[i] += dR[i];

            if (geom) {	/* assemble K = Ke + Kg */
                /* compute   {Q}={Q_t} ... temp.-induced forces     */
                element_end_forces ( Q, nE, xyz, L, Le, N1,N2,
                                     Ax, Asy,Asz, Jx,Iy,Iz, E,G, p,
                                     eqF_temp[lc], eqF_mech[lc], D, shear, geom );

                /* assemble temp.-stressed stiffness [K({D_t})]     */
                assemble_K ( K, DoF, nE, xyz, rj, L, Le, N1, N2,
                             Ax,Asy,Asz, Jx,Iy,Iz, E, G, p,
                             shear,geom, Q, 0 );
            }
        }

        /* ... then apply mechanical loads only, if there are any ... */
        if ( nF[lc]>0 || nU[lc]>0 || nW[lc]>0 || nP[lc]>0 || nD[lc]>0 ||
             gX[lc] != 0 || gY[lc] != 0 || gZ[lc] != 0 ) {

            /* incremental displ at react'ns = prescribed displ */
            for (i=1; i<=DoF; i++)	if (r[i]) dD[i] = Dp[lc][i];

            /*  solve {F_m} = [K({D_t})] * {D_m}	*/
            solve_system(K,dD,F_mech[lc],dR,DoF,q,r,&ok,0,&rms_resid);

            /* combine {D} = {D_t} + {D_m}	*/
            for (i=1; i<=DoF; i++) {
                if (q[i])	D[i] += dD[i];
                else {		D[i]  = Dp[lc][i]; dD[i] = 0.0; }
            }
            /* combine {R} = {R_t} + {R_m} --- for linear systems */
            for (i=1; i<=DoF; i++)	if (r[i]) R[i] += dR[i];
        }


        /*  combine {F} = {F_t} + {F_m} */
        for (i=1; i<=DoF; i++)	F[i] = F_temp[lc][i] + F_mech[lc][i];

        /*  element forces {Q} for displacements {D}	*/
        element_end_forces ( Q, nE, xyz, L, Le, N1,N2,
                             Ax, Asy,Asz, Jx,Iy,Iz, E,G, p,
                             eqF_temp[lc], eqF_mech[lc], D, shear, geom );

        /*  check the equilibrium error	*/
        error = equilibrium_error ( dF, F, K, D, DoF, q,r );
        /* quasi Newton-Raphson iteration for geometric nonlinearity  */
        if (geom) { error = 1.0; ok = 0; iter = 0; } /* re-initialize */
        while ( geom && error > tol && iter < 500 && ok >= 0) {

            ++iter;

            /*  assemble stiffness matrix [K({D}^(i))]	      */
            assemble_K ( K, DoF, nE, xyz, rj, L, Le, N1, N2,
                         Ax,Asy,Asz, Jx,Iy,Iz, E, G, p,
                         shear,geom, Q, 0 );

            /*  compute equilibrium error, {dF}, at iteration i   */
            /*  {dF}^(i) = {F} - [K({D}^(i))]*{D}^(i)	      */
            /*  convergence criteria = || {dF}^(i) ||  /  || F || */
            error = equilibrium_error ( dF, F, K, D, DoF, q,r );

            /*  Powell-Symmetric-Broyden secant stiffness update  */
            // PSB_update ( Ks, dF, dD, DoF );  /* not helpful?   */

            /*  solve {dF}^(i) = [K({D}^(i))] * {dD}^(i)	      */
            solve_system(K,dD,dF,dR,DoF,q,r,&ok,0,&rms_resid);

            if ( ok < 0 ) {	/*  K is not positive definite	      */
                qDebug("   The stiffness matrix is not pos-def. \n");
                qDebug("   Reduce loads and re-run the analysis.\n");
                break;
            }

            /*  increment {D}^(i+1) = {D}^(i) + {dD}^(i)	      */
            for (i=1; i<=DoF; i++)	if ( q[i] )	D[i] += dD[i];

            /*  element forces {Q} for displacements {D}^(i)      */
            element_end_forces ( Q, nE, xyz, L, Le, N1,N2,
                                 Ax, Asy,Asz, Jx,Iy,Iz, E,G, p,
                                 eqF_temp[lc], eqF_mech[lc], D, shear, geom );

        }			/* end quasi Newton-Raphson iteration */

        if ( geom )	compute_reaction_forces( R,F,K, D, DoF, r );

        update_statics( nN,nE,nL, lc, DoF, N1,N2,
                        F,D,R, r,Q, rms_resid, ok,active_joints,active_beams);

        /*  dealocate Broyden secant stiffness matrix, Ks */
        // if ( geom )	free_dmatrix(Ks, 1, DoF, 1, DoF );

        write_internal_forces ( lc, nL, 10, xyz,
                                Q, nN, nE, L, N1, N2,
                                Ax, Asy, Asz, Jx, Iy, Iz, E, G, p,
                                d, gX[lc], gY[lc], gZ[lc],
                                nU[lc],U[lc],nW[lc],W[lc],nP[lc],P[lc],
                                D, shear, error ,active_beams);

        //        static_mesh ( IN_file, infcpath, meshpath, plotpath, title,
        //                    nN, nE, nL, lc, DoF,
        //                    xyz, L, N1,N2, p, D,
        //                    exagg_static, D3_flag, anlyz,
        //                    dx, scale );

    } /* end load case loop */


    if ( nM > 0 ) { /* carry out modal analysis */

    }

    if ( nC > 0 ) {  /* matrix condensation of stiffness and mass */

    }

    /* deallocate memory used for each frame analysis variable */
    deallocate ( nN, nE, nL, nF, nU, nW, nP, nT, DoF, nM,
                 xyz, rj, L, Le, N1, N2, q,r,
                 Ax, Asy, Asz, Jx, Iy, Iz, E, G, p,
                 U,W,P,T, Dp, F_mech, F_temp,
                 eqF_mech, eqF_temp, F, dF,
                 K, Q, D, dD, R, dR,
                 d,EMs,NMs,NMx,NMy,NMz, M,f,V, c, m,
                 pkNx, pkVy, pkVz, pkTx, pkMy, pkMz,
                 pkDx, pkDy, pkDz, pkRx, pkSy, pkSz
                 );

}


void Frame3DDKernel::truss_solve(){

    int dof=0;
    int n_eq;

    if(m_is2D)
        dof=2*m_joints.size();
    else
        dof=3*m_joints.size();

    n_eq=dof;

    QVector<BeamPtr> active_beams;
    Q_FOREACH(BeamPtr beam,m_beams){
        if(beam->enabled()){
            active_beams.append(beam);
        }
    }

    dof-=active_beams.size();

    QVector<JointPtr> supporting_joints;
    QVector<short> support_dim;//1 -> x; 2-> y; 3-> z;
    Q_FOREACH(JointPtr j, m_joints){
        bool x,y,z,dummy;
        j->support(x,y,z,dummy,dummy,dummy);
        dof-=x;
        dof-=y;
        if(!m_is2D)
            dof-=z;
        if(x){
            supporting_joints.append(j);
            support_dim.append(1);
        }
        if(y){
            supporting_joints.append(j);
            support_dim.append(2);
        }

        if(!m_is2D)
            if(z){
                supporting_joints.append(j);
                support_dim.append(3);
            }
    }

    if(dof>0){
        qDebug()<<"Instable";
        return;
    }
    if(dof<0){
        qDebug()<<"Indetermita";
        return;
    }

    /*Allocation*/
    float** A=(float**)malloc(sizeof(float*)*n_eq+1);
    for(int i=1;i<=n_eq;i++){
        A[i]=(float*)malloc(sizeof(float)*n_eq+1);
    }
    for(int i=1;i<=n_eq;i++)
        for (int j = 1; j <= n_eq; j++)
            A[i][j]=0.0f;


    float** B=(float**)malloc(sizeof(float*)*n_eq+1);
    for(int i=1;i<=n_eq;i++){
        B[i]=(float*)malloc(sizeof(float)+1);
    }
    for(int i=1;i<=n_eq;i++)
        B[i][1]=0.0f;

    /*i moves on joints; j on active beams and reactions*/
    JointPtr e1,e2,current_joint;
    WeakJointPtr w_e1,w_e2;
    QVector3D dir;
    for(int i=0;i<m_joints.size();i++){
        current_joint=m_joints[i];
        for(int j=1;j<=n_eq;j++){
            if(j-1<active_beams.size()){
                active_beams[j-1]->extremes(w_e1,w_e2);
                e1=w_e1.toStrongRef();
                e2=w_e2.toStrongRef();
                if( e1==current_joint || e2==current_joint ){
                    if(e1==current_joint)
                        dir=-(current_joint->position()-e2->position()).normalized();
                    else
                        dir=-(current_joint->position()-e1->position()).normalized();
                    if(m_is2D){
                        A[i*2+1][j]=dir.x();
                        A[i*2+2][j]=dir.y();
                    }else{
                        A[i*3+1][j]=dir.x();
                        A[i*3+2][j]=dir.y();
                        A[i*3+3][j]=dir.z();
                    }
                }


            }
            else{
                /*Reactions*/
                int react_index=j-1-active_beams.size();
                if(current_joint!=supporting_joints[react_index]) continue;
                switch (support_dim[react_index]) {
                case 1:
                    if(m_is2D){
                        A[i*2+1][j]=1;
                    }else{
                        A[i*3+1][j]=1;
                    }
                    break;
                case 2:
                    if(m_is2D){
                        A[i*2+2][j]=1;
                    }else{
                        A[i*3+2][j]=1;
                    }
                    break;
                case 3:
                    if(!m_is2D)
                        A[i*3+3][j]=1;
                    break;
                }
            }
        }
    }

    Q_FOREACH(TrapezoidalForcePtr load,m_trapezoidal_loads){
        BeamPtr b=load->beam().toStrongRef();
        if(b->enabled()){
            WeakJointPtr w_e1,w_e2;
            JointPtr e1,e2;
            b->extremes(w_e1,w_e2);
            e1=w_e1.toStrongRef();
            e2=w_e2.toStrongRef();
            QVector3D force=load->force();
            force*=b->length();
            QVector3D rel_pos;
            QVector2D dummy;
            load->relativePosition(rel_pos,dummy);
            QVector3D force_on_e1=force*(1-rel_pos.x());
            QVector3D force_on_e2=force*rel_pos.x();

            int index_e1=m_joints.indexOf(e1);
            int index_e2=m_joints.indexOf(e2);

            if(m_is2D){
                B[index_e1*2+1][1]-=force_on_e1.x();
                B[index_e1*2+2][1]-=force_on_e1.y();

                B[index_e2*2+1][1]-=force_on_e2.x();
                B[index_e2*2+2][1]-=force_on_e2.y();
            }else{

                B[index_e1*3+1][1]-=force_on_e1.x();
                B[index_e1*3+2][1]-=force_on_e1.y();
                B[index_e1*3+3][1]-=force_on_e1.z();

                B[index_e2*3+1][1]-=force_on_e2.x();
                B[index_e2*3+2][1]-=force_on_e2.y();
                B[index_e2*3+3][1]-=force_on_e2.z();
            }
        }
    }

    gaussj(A,n_eq,B,1);

    //    QString line="";
    //    for(int i=1;i<=n_eq;i++){
    //        line="";
    //        for (int j = 1; j <= n_eq; j++)
    //            line+=", "+QString::number(A[i][j]);
    //        qDebug()<<line;
    //    }

    //    for(int i=1;i<=n_eq;i++){
    //        qDebug()<<B[i][1];
    //    }

    QList<QVector4D> segments;
    segments.push_back(QVector4D());
    segments.push_back(QVector4D());

    Q_FOREACH(JointPtr j,m_joints) {
        j->setDisplacement(QVector3D());
        j->setDisplacementRot(QVector3D());
        j->setReaction(QVector3D());
        j->setReactionMomentum(QVector3D());
    }

    JointPtr prev_j;
    QVector3D current_reaction;
    for(int i=1;i<=n_eq;i++){
        if(i-1<active_beams.size()){
            BeamPtr b=active_beams[i-1];
            int axial_type=0;
            if(!qFuzzyCompare(B[i][1],0.0))
                if(B[i][1]>0.0)
                    axial_type=1;
                else if(B[i][1]<0.0)
                    axial_type=-1;

            b->setForcesAndMoments(axial_type,-B[i][1],0,0,0,0,0,1);
            b->setForcesAndMoments(axial_type,B[i][1],0,0,0,0,0,2);

            b->setForcesAndMoments(axial_type,fabs(B[i][1]),0,0,0,0,0,3);
            b->setPeakDisplacements(QVector4D(),QVector4D());
            b->setStressSegment(segments);
        }else{
            int support_index=i-1-active_beams.size();
            if(prev_j!=supporting_joints[support_index]){
                if(!prev_j.isNull()){
                    prev_j->setReaction(current_reaction);
                    current_reaction=QVector3D(0,0,0);
                }
                prev_j=supporting_joints[support_index];
                switch (support_dim[support_index]) {
                case 1:
                    current_reaction.setX(B[i][1]);
                    break;
                case 2:
                    current_reaction.setY(B[i][1]);
                    break;
                case 3:
                    current_reaction.setZ(B[i][1]);
                    break;
                }
            }
        }
    }
    prev_j->setReaction(current_reaction);


    /*Deallocation*/
    for(int i=1;i<=n_eq;i++){
        free(A[i]);
    }
    free(A);

    for(int i=1;i<=n_eq;i++){
        free(B[i]);
    }
    free(B);

}



void Frame3DDKernel::write_internal_forces (
        int lc, int nL, float dx,
        vec3 *xyz,
        double **Q, int nN, int nE, double *L, int *J1, int *J2,
        float *Ax,float *Asy,float *Asz,float *Jx,float *Iy,float *Iz,
        float *E, float *G, float *p,
        float *d, float gX, float gY, float gZ,
        int nU, float **U, int nW, float **W, int nP, float **P,
        double *D, int shear, double error,QVector<BeamPtr> active_beams
        ){
    double	t1, t2, t3, t4, t5, t6, t7, t8, t9, /* coord transformation */
            u1, u2, u3, u4, u5, u6, u7, u8, u9, u10, u11, u12; /* displ. */

    double	xx1,xx2, wx1,wx2,	/* trapz load data, local x dir */
            xy1,xy2, wy1,wy2,	/* trapz load data, local y dir */
            xz1,xz2, wz1,wz2;	/* trapz load data, local z dir */

    double	wx=0, wy=0, wz=0, // distributed loads in local coords at x[i]
            wx_=0,wy_=0,wz_=0,// distributed loads in local coords at x[i-1]
            wxg=0,wyg=0,wzg=0,// gravity loads in local x, y, z coord's
            tx=0.0, tx_=0.0;  // distributed torque about local x coord

    double	xp;		/* location of internal point loads	*/

    double	*x, dx_, dxnx,	/* distance along frame element		*/
            *Nx,		/* axial force within frame el.		*/
            *Vy, *Vz,	/* shear forces within frame el.	*/
            *Tx,		/* torsional moment within frame el.	*/
            *My, *Mz, 	/* bending moments within frame el.	*/
            *Sy, *Sz,	/* transverse slopes of frame el.	*/
            *Dx, *Dy, *Dz,	/* frame el. displ. in local x,y,z, dir's */
            *Rx;		/* twist rotation about the local x-axis */

    double	maxNx, maxVy, maxVz, 	/*  maximum internal forces	*/
            maxTx, maxMy, maxMz,	/*  maximum internal moments	*/
            maxDx, maxDy, maxDz,	/*  maximum element displacements */
            maxRx, maxSy, maxSz;	/*  maximum element rotations	*/

    double	minNx, minVy, minVz, 	/*  minimum internal forces	*/
            minTx, minMy, minMz,	/*  minimum internal moments	*/
            minDx, minDy, minDz,	/*  minimum element displacements */
            minRx, minSy, minSz;	/*  minimum element rotations	*/

    int	n, m,		/* frame element number			*/
            cU=0, cW=0, cP=0, /* counters for U, W, and P loads	*/
            i, nx,		/* number of sections alont x axis	*/
            n1,n2,i1,i2;	/* starting and stopping node no's	*/

    if (dx == -1.0)	return;	// skip calculation of internal forces and displ

    for ( m=1; m <= nE; m++ ) {	// loop over all frame elements
        if(m>active_beams.length()) break;
        n1 = J1[m];	n2 = J2[m]; // node 1 and node 2 of elmnt m

        nx = floor(L[m]/dx);	// number of x-axis increments
        if (nx < 1) nx = 1;	// at least one x-axis increment

        // allocate memory for interior force data for frame element "m"
        x  = dvector(0,nx);
        Nx = dvector(0,nx);
        Vy = dvector(0,nx);
        Vz = dvector(0,nx);
        Tx = dvector(0,nx);
        My = dvector(0,nx);
        Mz = dvector(0,nx);
        Sy = dvector(0,nx);
        Sz = dvector(0,nx);
        Rx = dvector(0,nx);
        Dx = dvector(0,nx);
        Dy = dvector(0,nx);
        Dz = dvector(0,nx);


        // the local x-axis for frame element "m" starts at 0 and ends at L[m]
        for (i=0; i<nx; i++)	x[i] = i*dx;
        x[nx] = L[m];
        dxnx = x[nx]-x[nx-1];	// length of the last x-axis increment


        // find interior axial force, shear forces, torsion and bending moments

        coord_trans ( xyz, L[m], n1, n2,
                      &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, p[m] );

        // distributed gravity load in local x, y, z coordinates
        wxg = d[m]*Ax[m]*(t1*gX + t2*gY + t3*gZ);
        wyg = d[m]*Ax[m]*(t4*gX + t5*gY + t6*gZ);
        wzg = d[m]*Ax[m]*(t7*gX + t8*gY + t9*gZ);

        // add uniformly-distributed loads to gravity load
        for (n=1; n<=nE && cU<nU; n++) {
            if ( (int) U[n][1] == m ) { // load n on element m
                wxg += U[n][2];
                wyg += U[n][3];
                wzg += U[n][4];
                ++cU;
            }
        }

        // interior forces for frame element "m" at (x=0)
        Nx[0] = -Q[m][1];	// positive Nx is tensile
        Vy[0] = -Q[m][2];	// positive Vy in local y direction
        Vz[0] = -Q[m][3];	// positive Vz in local z direction
        Tx[0] = -Q[m][4];	// positive Tx r.h.r. about local x axis
        My[0] =  Q[m][5];	// positive My -> positive x-z curvature
        Mz[0] = -Q[m][6];	// positive Mz -> positive x-y curvature

        dx_ = dx;
        for (i=1; i<=nx; i++) {	/*  accumulate interior span loads */

            // start with gravitational plus uniform loads
            wx = wxg;
            wy = wyg;
            wz = wzg;

            if (i==1) {
                wx_ = wxg;
                wy_ = wyg;
                wz_ = wzg;
                tx_ = tx;
            }

            // add trapezoidally-distributed loads
            for (n=1; n<=10*nE && cW<nW; n++) {
                if ( (int) W[n][1] == m ) { // load n on element m
                    if (i==nx) ++cW;
                    xx1 = W[n][2];  xx2 = W[n][3];
                    wx1 = W[n][4];  wx2 = W[n][5];
                    xy1 = W[n][6];  xy2 = W[n][7];
                    wy1 = W[n][8];  wy2 = W[n][9];
                    xz1 = W[n][10]; xz2 = W[n][11];
                    wz1 = W[n][12]; wz2 = W[n][13];

                    if ( x[i]>xx1 && x[i]<=xx2 )
                        wx += wx1+(wx2-wx1)*(x[i]-xx1)/(xx2-xx1);
                    if ( x[i]>xy1 && x[i]<=xy2 )
                        wy += wy1+(wy2-wy1)*(x[i]-xy1)/(xy2-xy1);
                    if ( x[i]>xz1 && x[i]<=xz2 )
                        wz += wz1+(wz2-wz1)*(x[i]-xz1)/(xz2-xz1);
                }
            }

            // trapezoidal integration of distributed loads
            // for axial forces, shear forces and torques
            if (i==nx)	dx_ = dxnx;
            Nx[i] = Nx[i-1] - 0.5*(wx+wx_)*dx_;
            Vy[i] = Vy[i-1] - 0.5*(wy+wy_)*dx_;
            Vz[i] = Vz[i-1] - 0.5*(wz+wz_)*dx_;
            Tx[i] = Tx[i-1] - 0.5*(tx+tx_)*dx_;

            // update distributed loads at x[i-1]
            wx_ = wx;
            wy_ = wy;
            wz_ = wz;
            tx_ = tx;

            // add interior point loads
            for (n=1; n<=10*nE && cP<nP; n++) {
                if ( (int) P[n][1] == m ) { // load n on element m
                    if (i==nx) ++cP;
                    xp = P[n][5];
                    if ( x[i] <= xp && xp < x[i]+dx ) {
                        Nx[i] -= P[n][2] * 0.5 * (1.0 - (xp-x[i])/dx);
                        Vy[i] -= P[n][3] * 0.5 * (1.0 - (xp-x[i])/dx);
                        Vz[i] -= P[n][4] * 0.5 * (1.0 - (xp-x[i])/dx);

                    }
                    if ( x[i]-dx <= xp && xp < x[i] ) {
                        Nx[i] -= P[n][2] * 0.5 * (1.0 - (x[i]-dx-xp)/dx);
                        Vy[i] -= P[n][3] * 0.5 * (1.0 - (x[i]-dx-xp)/dx);
                        Vz[i] -= P[n][4] * 0.5 * (1.0 - (x[i]-dx-xp)/dx);
                    }
                }
            }
        }
        // linear correction of forces for bias in trapezoidal integration
        for (i=1; i<=nx; i++) {
            Nx[i] -= (Nx[nx]-Q[m][7])  * i/nx;
            Vy[i] -= (Vy[nx]-Q[m][8])  * i/nx;
            Vz[i] -= (Vz[nx]-Q[m][9])  * i/nx;
            Tx[i] -= (Tx[nx]-Q[m][10]) * i/nx;
        }
        // trapezoidal integration of shear force for bending momemnt
        dx_ = dx;
        for (i=1; i<=nx; i++) {
            if (i==nx)	dx_ = dxnx;
            My[i] = My[i-1] - 0.5*(Vz[i]+Vz[i-1])*dx_;
            Mz[i] = Mz[i-1] - 0.5*(Vy[i]+Vy[i-1])*dx_;

        }
        // linear correction of moments for bias in trapezoidal integration
        for (i=1; i<=nx; i++) {
            My[i] -= (My[nx]+Q[m][11]) * i/nx;
            Mz[i] -= (Mz[nx]-Q[m][12]) * i/nx;
        }

        // find interior transverse displacements

        i1 = 6*(n1-1);	i2 = 6*(n2-1);

        /* compute end deflections in local coordinates */

        u1  = t1*D[i1+1] + t2*D[i1+2] + t3*D[i1+3];
        u2  = t4*D[i1+1] + t5*D[i1+2] + t6*D[i1+3];
        u3  = t7*D[i1+1] + t8*D[i1+2] + t9*D[i1+3];

        u4  = t1*D[i1+4] + t2*D[i1+5] + t3*D[i1+6];
        u5  = t4*D[i1+4] + t5*D[i1+5] + t6*D[i1+6];
        u6  = t7*D[i1+4] + t8*D[i1+5] + t9*D[i1+6];

        u7  = t1*D[i2+1] + t2*D[i2+2] + t3*D[i2+3];
        u8  = t4*D[i2+1] + t5*D[i2+2] + t6*D[i2+3];
        u9  = t7*D[i2+1] + t8*D[i2+2] + t9*D[i2+3];

        u10 = t1*D[i2+4] + t2*D[i2+5] + t3*D[i2+6];
        u11 = t4*D[i2+4] + t5*D[i2+5] + t6*D[i2+6];
        u12 = t7*D[i2+4] + t8*D[i2+5] + t9*D[i2+6];


        // rotations and displacements for frame element "m" at (x=0)
        Dx[0] =  u1;	// displacement in  local x dir  at node N1
        Dy[0] =  u2;	// displacement in  local y dir  at node N1
        Dz[0] =  u3;	// displacement in  local z dir  at node N1
        Rx[0] =  u4;	// rotationin about local x axis at node N1
        Sy[0] =  u6;	// slope in  local y  direction  at node N1
        Sz[0] = -u5;	// slope in  local z  direction  at node N1

        // axial displacement along frame element "m"
        dx_ = dx;
        for (i=1; i<=nx; i++) {
            if (i==nx)	dx_ = dxnx;
            Dx[i] = Dx[i-1] + 0.5*(Nx[i-1]+Nx[i])/(E[m]*Ax[m])*dx_;
        }
        // linear correction of axial displacement for bias in trapezoidal integration
        for (i=1; i<=nx; i++) {
            Dx[i] -= (Dx[nx]-u7) * i/nx;
        }

        // torsional rotation along frame element "m"
        dx_ = dx;
        for (i=1; i<=nx; i++) {
            if (i==nx)	dx_ = dxnx;
            Rx[i] = Rx[i-1] + 0.5*(Tx[i-1]+Tx[i])/(G[m]*Jx[m])*dx_;
        }
        // linear correction of torsional rot'n for bias in trapezoidal integration
        for (i=1; i<=nx; i++) {
            Rx[i] -= (Rx[nx]-u10) * i/nx;
        }

        // transverse slope along frame element "m"
        dx_ = dx;
        for (i=1; i<=nx; i++) {
            if (i==nx)	dx_ = dxnx;
            Sy[i] = Sy[i-1] + 0.5*(Mz[i-1]+Mz[i])/(E[m]*Iz[m])*dx_;
            Sz[i] = Sz[i-1] + 0.5*(My[i-1]+My[i])/(E[m]*Iy[m])*dx_;
        }
        // linear correction for bias in trapezoidal integration
        for (i=1; i<=nx; i++) {
            Sy[i] -= (Sy[nx]-u12) * i/nx;
            Sz[i] -= (Sz[nx]+u11) * i/nx;
        }
        if ( shear ) {		// add-in slope due to shear deformation
            for (i=0; i<=nx; i++) {
                Sy[i] += Vy[i]/(G[m]*Asy[m]);
                Sz[i] += Vz[i]/(G[m]*Asz[m]);
            }
        }
        // displacement along frame element "m"
        dx_ = dx;
        for (i=1; i<=nx; i++) {
            if (i==nx)	dx_ = dxnx;
            Dy[i] = Dy[i-1] + 0.5*(Sy[i-1]+Sy[i])*dx_;
            Dz[i] = Dz[i-1] + 0.5*(Sz[i-1]+Sz[i])*dx_;
        }
        // linear correction for bias in trapezoidal integration
        for (i=1; i<=nx; i++) {
            Dy[i] -= (Dy[nx]-u8) * i/nx;
            Dz[i] -= (Dz[nx]-u9) * i/nx;
        }

        // initialize the maximum and minimum element forces and displacements
        maxNx = minNx = Nx[0]; maxVy = minVy = Vy[0]; maxVz = minVz = Vz[0];  	//  maximum internal forces
        maxTx = minTx = Tx[0]; maxMy = minMy = My[0]; maxMz = minMz = Mz[0]; 	//  maximum internal moments
        maxDx = minDx = Dx[0]; maxDy = minDy = Dy[0]; maxDz = minDz = Dz[0]; 	//  maximum element displacements
        maxRx =	minRx = Rx[0]; maxSy = minSy = Sy[0]; maxSz = minSz = Sz[0];	//  maximum element rotations

        // find maximum and minimum internal element forces
        for (i=1; i<=nx; i++) {
            maxNx = (Nx[i] > maxNx) ? Nx[i] : maxNx;
            minNx = (Nx[i] < minNx) ? Nx[i] : minNx;
            maxVy = (Vy[i] > maxVy) ? Vy[i] : maxVy;
            minVy = (Vy[i] < minVy) ? Vy[i] : minVy;
            maxVz = (Vz[i] > maxVz) ? Vz[i] : maxVz;
            minVz = (Vz[i] < minVz) ? Vz[i] : minVz;

            maxTx = (Tx[i] > maxTx) ? Tx[i] : maxTx;
            minTx = (Tx[i] < minTx) ? Tx[i] : minTx;
            maxMy = (My[i] > maxMy) ? My[i] : maxMy;
            minMy = (My[i] < minMy) ? My[i] : minMy;
            maxMz = (Mz[i] > maxMz) ? Mz[i] : maxMz;
            minMz = (Mz[i] < minMz) ? Mz[i] : minMz;
        }

        // find maximum and minimum internal element displacements
        for (i=1; i<=nx; i++) {
            maxDx = (Dx[i] > maxDx) ? Dx[i] : maxDx;
            minDx = (Dx[i] < minDx) ? Dx[i] : minDx;
            maxDy = (Dy[i] > maxDy) ? Dy[i] : maxDy;
            minDy = (Dy[i] < minDy) ? Dy[i] : minDy;
            maxDz = (Dz[i] > maxDz) ? Dz[i] : maxDz;
            minDz = (Dz[i] < minDz) ? Dz[i] : minDz;
            maxRx = (Rx[i] > maxRx) ? Rx[i] : maxRx;
            minRx = (Rx[i] < minRx) ? Rx[i] : minRx;
            maxSy = (Sy[i] > maxSy) ? Sy[i] : maxSy;
            minSy = (Sy[i] < minSy) ? Sy[i] : minSy;
            maxSz = (Sz[i] > maxSz) ? Sz[i] : maxSz;
            minSz = (Sz[i] < minSz) ? Sz[i] : minSz;
        }


        // write results to the internal frame element force output data file
        QList<QVector4D> segments;
        //        fprintf(fpif,"#.x                \tNx        \tVy        \tVz        \tTx       \tMy        \tMz        \tDx        \tDy        \tDz        \tRx\t~\n");
        for (i=0; i<=nx; i++) {
            if(m_is2D)
                segments.push_back(QVector4D(Dx[i], Dy[i], Dz[i], Rx[i]));
            else
                //segments.push_back(QVector4D(Dx[i], Dz[i],-Dy[i], Rx[i]));
                segments.push_back(QVector4D(Dx[i], Dy[i],-Dz[i], Rx[i]));

            //            fprintf(fpif,"%14.6e\t", x[i] );
            //            fprintf(fpif,"%14.6e\t%14.6e\t%14.6e\t",
            //                        Nx[i], Vy[i], Vz[i] );
            //            fprintf(fpif,"%14.6e\t%14.6e\t%14.6e\t",
            //                        Tx[i], My[i], Mz[i] );
            //            fprintf(fpif,"%14.6e\t%14.6e\t%14.6e\t%14.6e\n",
            //                        Dx[i], Dy[i], Dz[i], Rx[i] );
        }
        active_beams[m-1]->setStressSegment(segments);
        int typeMax=0;
        int typeMin=0;
        if(!qFuzzyCompare(maxNx,0.0)){
            if(maxNx>0)
                typeMax=1;
            else typeMax=-1;
        }
        if(!qFuzzyCompare(minNx,0.0)){
            if(minNx>0)
                typeMin=1;
            else typeMin=-1;
        }
        int type=0;
        if(typeMax!=0 && typeMin!=0 && typeMax==typeMin)
            type=typeMax;
        else if(typeMax!=0 && typeMin!=0 && typeMax!=typeMin){
            qWarning("Internal axial forces are not coherent");
            if(fabs(maxNx)>fabs(minNx))
                type=typeMax;
            else
                type=typeMin;
        }
        else if(typeMax!=0 && typeMin==0)
            type=typeMax;
        else if(typeMin!=0 && typeMax==0)
            type=typeMin;

        active_beams[m-1]->setForcesAndMoments(type,qMax(fabs(maxNx),fabs(minNx)),
                                               qMax(fabs(maxVy),fabs(minVy)),
                                               qMax(fabs(maxVz),fabs(minVz)),
                                               qMax(fabs(maxTx),fabs(minTx)),
                                               qMax(fabs(maxMy),fabs(minMy)),
                                               qMax(fabs(maxMz),fabs(minMz)),
                                               3);

        if(m_is2D)
            active_beams[m-1]->setPeakDisplacements(QVector4D(minDx,minDy,minDz,minRx),QVector4D(maxDx,maxDy,maxDz,maxRx));
        else
            active_beams[m-1]->setPeakDisplacements(QVector4D(minDx,minDy,-minDz,minRx),QVector4D(maxDx,maxDy,-maxDz,maxRx));

        // free memory
        free_dvector(x,0,nx);
        free_dvector(Nx,0,nx);
        free_dvector(Vy,0,nx);
        free_dvector(Vz,0,nx);
        free_dvector(Tx,0,nx);
        free_dvector(My,0,nx);
        free_dvector(Mz,0,nx);
        free_dvector(Rx,0,nx);
        free_dvector(Sy,0,nx);
        free_dvector(Sz,0,nx);
        free_dvector(Dx,0,nx);
        free_dvector(Dy,0,nx);
        free_dvector(Dz,0,nx);

    }				// end of loop over all frame elements
}


void Frame3DDKernel::assemble_loads (
        int nN, int nE, int nL, int DoF,
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
        double ***eqF_mech, // equivalent mech loads, global coord
        double ***eqF_temp, // equivalent temp loads, global coord
        QVector<JointPtr> active_joints,QVector<BeamPtr> active_beams
        ){
    float	hy, hz;			/* section dimensions in local coords */

    float	x1,x2, w1,w2;
    double	Ln, R1o, R2o, f01, f02;

    /* equivalent element end forces from distributed and thermal loads */
    double	Nx1, Vy1, Vz1, Mx1=0.0, My1=0.0, Mz1=0.0,
            Nx2, Vy2, Vz2, Mx2=0.0, My2=0.0, Mz2=0.0;
    double	Ksy, Ksz, 		/* shear deformatn coefficients	*/
            a, b,			/* point load locations */
            t1, t2, t3, t4, t5, t6, t7, t8, t9;	/* 3D coord Xfrm coeffs */
    int	i,j,l, n, n1, n2;

    /* initialize load data vectors and matrices to zero */
    for (j=1; j<=DoF; j++)	Fo[j] = 0.0;
    for (j=1; j<=DoF; j++)
        F_temp[1][j] = F_mech[1][j] = 0.0;
    for (i=1; i<=12; i++)
        for (n=1; n<=nE; n++)
            eqF_mech[1][n][i] = eqF_temp[1][n][i] = 0.0;

    for (i=1; i<=DoF; i++) Dp[1][i] = 0.0;

    for (i=1;i<=nE;i++)	for(j=1;j<=12;j++)	Q[i][j] = 0.0;


    /* gravity loads applied uniformly to all frame elements ------- */
    gX[1]=(float)m_gravity.x();
    if(m_is2D){
        gY[1]=(float)m_gravity.y();
        gZ[1]=(float)m_gravity.z();
    }else{
        //        gY[1]=-m_gravity.z();
        //        gZ[1]=m_gravity.y();
        gY[1]=(float)m_gravity.y();
        gZ[1]=-(float)m_gravity.z();
    }
    for (n=1; n<=nE; n++) {

        n1 = J1[n];	n2 = J2[n];

        coord_trans ( xyz, L[n], n1, n2,
                      &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, p[n] );

        eqF_mech[1][n][1]  = d[n]*Ax[n]*L[n]*gX[1] / 2.0;
        eqF_mech[1][n][2]  = d[n]*Ax[n]*L[n]*gY[1] / 2.0;
        eqF_mech[1][n][3]  = d[n]*Ax[n]*L[n]*gZ[1] / 2.0;

        eqF_mech[1][n][4]  = d[n]*Ax[n]*L[n]*L[n] / 12.0 *
                ( (-t4*t8+t5*t7)*gY[1] + (-t4*t9+t6*t7)*gZ[1] );
        eqF_mech[1][n][5]  = d[n]*Ax[n]*L[n]*L[n] / 12.0 *
                ( (-t5*t7+t4*t8)*gX[1] + (-t5*t9+t6*t8)*gZ[1] );
        eqF_mech[1][n][6]  = d[n]*Ax[n]*L[n]*L[n] / 12.0 *
                ( (-t6*t7+t4*t9)*gX[1] + (-t6*t8+t5*t9)*gY[1] );

        eqF_mech[1][n][7]  = d[n]*Ax[n]*L[n]*gX[1] / 2.0;
        eqF_mech[1][n][8]  = d[n]*Ax[n]*L[n]*gY[1] / 2.0;
        eqF_mech[1][n][9]  = d[n]*Ax[n]*L[n]*gZ[1] / 2.0;

        eqF_mech[1][n][10] = d[n]*Ax[n]*L[n]*L[n] / 12.0 *
                ( ( t4*t8-t5*t7)*gY[1] + ( t4*t9-t6*t7)*gZ[1] );
        eqF_mech[1][n][11] = d[n]*Ax[n]*L[n]*L[n] / 12.0 *
                ( ( t5*t7-t4*t8)*gX[1] + ( t5*t9-t6*t8)*gZ[1] );
        eqF_mech[1][n][12] = d[n]*Ax[n]*L[n]*L[n] / 12.0 *
                ( ( t6*t7-t4*t9)*gX[1] + ( t6*t8-t5*t9)*gY[1] );

    }					/* end gravity loads */

    /* node point loads -------------------------------------------- */
    nF[1]=m_node_loads.size();

    Q_FOREACH(NodeLoadPtr nodeload, m_node_loads){
        int application_joint_index=active_joints.indexOf(nodeload->joint().toStrongRef());
        if(application_joint_index<0) {
            nF[1]=nF[1]-1;
            continue;
        }
        F_mech[1][6*application_joint_index+1]+=nodeload->force().x();//I added a plus...
        F_mech[1][6*application_joint_index+4]+=nodeload->momentum().x();
        if(m_is2D){
            F_mech[1][6*application_joint_index+2]+=nodeload->force().y();
            F_mech[1][6*application_joint_index+3]+=nodeload->force().z();
            F_mech[1][6*application_joint_index+5]+=nodeload->momentum().y();
            F_mech[1][6*application_joint_index+6]+=nodeload->momentum().z();
        }
        else{
            //            F_mech[1][6*application_joint_index+2]-=nodeload->force().z();
            //            F_mech[1][6*application_joint_index+3]+=nodeload->force().y();
            //            F_mech[1][6*application_joint_index+5]-=nodeload->momentum().z();
            //            F_mech[1][6*application_joint_index+6]+=nodeload->momentum().y();
            F_mech[1][6*application_joint_index+2]+=nodeload->force().y();
            F_mech[1][6*application_joint_index+3]-=nodeload->force().z();
            F_mech[1][6*application_joint_index+5]+=nodeload->momentum().y();
            F_mech[1][6*application_joint_index+6]-=nodeload->momentum().z();

        }
    }


    /* uniformly distributed loads --------------------------------- */

    nU[1]=m_uniformly_distributed_loads.size();

    Q_FOREACH(UniformlyDistributedLoadPtr UDLoad,m_uniformly_distributed_loads){
        int i=1;
        int application_beam_index=active_beams.indexOf(UDLoad->beam().toStrongRef())+1;
        if(application_beam_index<=0){
            qDebug("UDLoad on disabled beam");
            nU[1]=nU[1]-1;
            continue ;
        }
        U[1][i][1] = (double) application_beam_index;
        U[1][i][2] = UDLoad->forceLocal().x();
        if(m_is2D){
            U[1][i][3] = UDLoad->forceLocal().y();
            U[1][i][4] = UDLoad->forceLocal().z();
        }else{
            //            U[1][i][3] = -UDLoad->forceLocal().z();
            //            U[1][i][4] = UDLoad->forceLocal().y();
            U[1][i][3] = UDLoad->forceLocal().y();
            U[1][i][4] = -UDLoad->forceLocal().z();
        }

        Nx1 = Nx2 = U[1][i][2]*Le[application_beam_index] / 2.0;
        Vy1 = Vy2 = U[1][i][3]*Le[application_beam_index] / 2.0;
        Vz1 = Vz2 = U[1][i][4]*Le[application_beam_index] / 2.0;
        Mx1 = Mx2 = 0.0;
        My1 = -U[1][i][4]*Le[application_beam_index]*Le[application_beam_index] / 12.0;	My2 = -My1;
        Mz1 =  U[1][i][3]*Le[application_beam_index]*Le[application_beam_index] / 12.0;	Mz2 = -Mz1;

        n1 = J1[application_beam_index];	n2 = J2[application_beam_index];

        coord_trans ( xyz, L[application_beam_index], n1, n2,
                      &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, p[application_beam_index] );

        /* {F} = [T]'{Q} */
        eqF_mech[1][application_beam_index][1]  += ( Nx1*t1 + Vy1*t4 + Vz1*t7 );
        eqF_mech[1][application_beam_index][2]  += ( Nx1*t2 + Vy1*t5 + Vz1*t8 );
        eqF_mech[1][application_beam_index][3]  += ( Nx1*t3 + Vy1*t6 + Vz1*t9 );
        eqF_mech[1][application_beam_index][4]  += ( Mx1*t1 + My1*t4 + Mz1*t7 );
        eqF_mech[1][application_beam_index][5]  += ( Mx1*t2 + My1*t5 + Mz1*t8 );
        eqF_mech[1][application_beam_index][6]  += ( Mx1*t3 + My1*t6 + Mz1*t9 );

        eqF_mech[1][application_beam_index][7]  += ( Nx2*t1 + Vy2*t4 + Vz2*t7 );
        eqF_mech[1][application_beam_index][8]  += ( Nx2*t2 + Vy2*t5 + Vz2*t8 );
        eqF_mech[1][application_beam_index][9]  += ( Nx2*t3 + Vy2*t6 + Vz2*t9 );
        eqF_mech[1][application_beam_index][10] += ( Mx2*t1 + My2*t4 + Mz2*t7 );
        eqF_mech[1][application_beam_index][11] += ( Mx2*t2 + My2*t5 + Mz2*t8 );
        eqF_mech[1][application_beam_index][12] += ( Mx2*t3 + My2*t6 + Mz2*t9 );

        i++;
    }
    /* end uniformly distributed loads */


    /* trapezoidally distributed loads ----------------------------- */
    nW[1]=m_trapezoidal_loads.size();
    //JUST REMEBER THAT lc=1 AND THE INDEXES ARE FROM 1
    if ( nW[1] > 10*nE ) {
        qWarning("Too many trapezoidal loads");
        nW[1]=10*nE;
    }
    for (i=1; i <= nW[1]; i++){	/* ! local element coordinates ! */
        TrapezoidalForcePtr trz_load=m_trapezoidal_loads[i-1];
        //n is the idnex of the beam the force is applied on
        n=active_beams.indexOf(trz_load->beam().toStrongRef())+1;
        if(n<=0){
            qDebug("TFPLoad on disabled beam");
            nW[1]=nW[1]-1;
            continue ;
        }
        QVector3D begin,end;
        trz_load->positionOnBeam(begin,end);
        QVector3D localForce=trz_load->forceLocal();
        W[1][i][1] = (double) n;
        W[1][i][2]=floor(begin.x());//xx1
        W[1][i][3]=floor(end.x());//xx2
        W[1][i][4]=localForce.x();//wx1
        W[1][i][5]=localForce.x();//wx1
        if(m_is2D){
            W[1][i][6]=floor(begin.y());
            W[1][i][7]=floor(end.y());
            W[1][i][8]=localForce.y();
            W[1][i][9]=localForce.y();
            W[1][i][10]=floor(begin.z());
            W[1][i][11]=floor(end.z());
            W[1][i][12]=localForce.z();
            W[1][i][13]=localForce.z();
        }else{

            //            W[1][i][6]=begin.z();
            //            W[1][i][7]=end.z();
            //            W[1][i][8]=-trz_load->forceLocal().z();
            //            W[1][i][9]=-trz_load->forceLocal().z();
            //            W[1][i][10]=begin.y();
            //            W[1][i][11]=end.y();
            //            W[1][i][12]=trz_load->forceLocal().y();
            //            W[1][i][13]=trz_load->forceLocal().y();
            W[1][i][6]=floor(begin.y());
            W[1][i][7]=floor(end.y());
            W[1][i][8]=localForce.y();
            W[1][i][9]=localForce.y();
            W[1][i][10]=floor(begin.z());
            W[1][i][11]=floor(end.z());
            W[1][i][12]=-localForce.z();
            W[1][i][13]=-localForce.z();

        }


        Ln = L[n];

        /* error checking */

        if ( W[1][i][ 4]==0 && W[1][i][ 5]==0 &&
             W[1][i][ 8]==0 && W[1][i][ 9]==0 &&
             W[1][i][12]==0 && W[1][i][13]==0 ) {
            qWarning("Warning: All trapezoidal loads applied to frame element  are zero\n");
        }

        if ( W[1][i][ 2] < 0 ) {
            qWarning("  error in trapezoidal loads");

        }
        if ( W[1][i][ 2] > W[1][i][3] ) {
            qWarning("error in trapezoidal loads");
        }
        if ( W[1][i][ 3] > Ln ) {
            qWarning("  error in trapezoidal loads");

        }
        if ( W[1][i][ 6] < 0 ) {
            qWarning("  error in trapezoidal loads");

        }
        if ( W[1][i][ 6] > W[1][i][7] ) {
            qWarning("  error in trapezoidal loads");
        }
        if ( W[1][i][ 7] > Ln ) {
            qWarning("  error in trapezoidal loads");
        }
        if ( W[1][i][10] < 0 ) {
            qWarning("  error in trapezoidal loads");
        }
        if ( W[1][i][10] > W[1][i][11] ) {
            qWarning("  error in trapezoidal loads");
        }
        if ( W[1][i][11] > Ln ) {
            qWarning("  error in trapezoidal loads");

        }

        if ( shear ) {
            Ksy = (12.0*E[n]*Iz[n]) / (G[n]*Asy[n]*Le[n]*Le[n]);
            Ksz = (12.0*E[n]*Iy[n]) / (G[n]*Asz[n]*Le[n]*Le[n]);
        } else	Ksy = Ksz = 0.0;

        /* x-axis trapezoidal loads (along the frame element length) */
        x1 =  W[1][i][2]; x2 =  W[1][i][3];
        w1 =  W[1][i][4]; w2 =  W[1][i][5];

        Nx1 = ( 3.0*(w1+w2)*Ln*(x2-x1) - (2.0*w2+w1)*x2*x2 + (w2-w1)*x2*x1 + (2.0*w1+w2)*x1*x1 ) / (6.0*Ln);
        Nx2 = ( -(2.0*w1+w2)*x1*x1 + (2.0*w2+w1)*x2*x2  - (w2-w1)*x1*x2 ) / ( 6.0*Ln );

        /* y-axis trapezoidal loads (across the frame element length) */
        x1 =  W[1][i][6];  x2 = W[1][i][7];
        w1 =  W[1][i][8]; w2 =  W[1][i][9];

        R1o = ( (2.0*w1+w2)*x1*x1 - (w1+2.0*w2)*x2*x2 +
                3.0*(w1+w2)*Ln*(x2-x1) - (w1-w2)*x1*x2 ) / (6.0*Ln);
        R2o = ( (w1+2.0*w2)*x2*x2 + (w1-w2)*x1*x2 -
                (2.0*w1+w2)*x1*x1 ) / (6.0*Ln);

        f01 = (  3.0*(w2+4.0*w1)*x1*x1*x1*x1 -  3.0*(w1+4.0*w2)*x2*x2*x2*x2
                 - 15.0*(w2+3.0*w1)*Ln*x1*x1*x1 + 15.0*(w1+3.0*w2)*Ln*x2*x2*x2
                 -  3.0*(w1-w2)*x1*x2*(x1*x1 + x2*x2)
                 + 20.0*(w2+2.0*w1)*Ln*Ln*x1*x1 - 20.0*(w1+2.0*w2)*Ln*Ln*x2*x2
                 + 15.0*(w1-w2)*Ln*x1*x2*(x1+x2)
                 -  3.0*(w1-w2)*x1*x1*x2*x2 - 20.0*(w1-w2)*Ln*Ln*x1*x2 ) / 360.0;

        f02 = (  3.0*(w2+4.0*w1)*x1*x1*x1*x1 - 3.0*(w1+4.0*w2)*x2*x2*x2*x2
                 -  3.0*(w1-w2)*x1*x2*(x1*x1+x2*x2)
                 - 10.0*(w2+2.0*w1)*Ln*Ln*x1*x1 + 10.0*(w1+2.0*w2)*Ln*Ln*x2*x2
                 -  3.0*(w1-w2)*x1*x1*x2*x2 + 10.0*(w1-w2)*Ln*Ln*x1*x2 ) / 360.0;

        Mz1 = -( 4.0*f01 + 2.0*f02 + Ksy*(f01 - f02) ) / ( Ln*Ln*(1.0+Ksy) );
        Mz2 = -( 2.0*f01 + 4.0*f02 - Ksy*(f01 - f02) ) / ( Ln*Ln*(1.0+Ksy) );

        Vy1 =  R1o + Mz1/Ln + Mz2/Ln;
        Vy2 =  R2o - Mz1/Ln - Mz2/Ln;

        /* z-axis trapezoidal loads (across the frame element length) */
        x1 =  W[1][i][10]; x2 =  W[1][i][11];
        w1 =  W[1][i][12]; w2 =  W[1][i][13];

        R1o = ( (2.0*w1+w2)*x1*x1 - (w1+2.0*w2)*x2*x2 +
                3.0*(w1+w2)*Ln*(x2-x1) - (w1-w2)*x1*x2 ) / (6.0*Ln);
        R2o = ( (w1+2.0*w2)*x2*x2 + (w1-w2)*x1*x2 -
                (2.0*w1+w2)*x1*x1 ) / (6.0*Ln);

        f01 = (  3.0*(w2+4.0*w1)*x1*x1*x1*x1 -  3.0*(w1+4.0*w2)*x2*x2*x2*x2
                 - 15.0*(w2+3.0*w1)*Ln*x1*x1*x1 + 15.0*(w1+3.0*w2)*Ln*x2*x2*x2
                 -  3.0*(w1-w2)*x1*x2*(x1*x1 + x2*x2)
                 + 20.0*(w2+2.0*w1)*Ln*Ln*x1*x1 - 20.0*(w1+2.0*w2)*Ln*Ln*x2*x2
                 + 15.0*(w1-w2)*Ln*x1*x2*(x1+x2)
                 -  3.0*(w1-w2)*x1*x1*x2*x2 - 20.0*(w1-w2)*Ln*Ln*x1*x2 ) / 360.0;

        f02 = (  3.0*(w2+4.0*w1)*x1*x1*x1*x1 - 3.0*(w1+4.0*w2)*x2*x2*x2*x2
                 -  3.0*(w1-w2)*x1*x2*(x1*x1+x2*x2)
                 - 10.0*(w2+2.0*w1)*Ln*Ln*x1*x1 + 10.0*(w1+2.0*w2)*Ln*Ln*x2*x2
                 -  3.0*(w1-w2)*x1*x1*x2*x2 + 10.0*(w1-w2)*Ln*Ln*x1*x2 ) / 360.0;

        My1 = ( 4.0*f01 + 2.0*f02 + Ksz*(f01 - f02) ) / ( Ln*Ln*(1.0+Ksz) );
        My2 = ( 2.0*f01 + 4.0*f02 - Ksz*(f01 - f02) ) / ( Ln*Ln*(1.0+Ksz) );

        Vz1 =  R1o - My1/Ln - My2/Ln;
        Vz2 =  R2o + My1/Ln + My2/Ln;

        /* debugging ... check internal force values
        printf("n=%d\n Nx1=%9.3f\n Nx2=%9.3f\n Vy1=%9.3f\n Vy2=%9.3f\n Vz1=%9.3f\n Vz2=%9.3f\n My1=%9.3f\n My2=%9.3f\n Mz1=%9.3f\n Mz2=%9.3f\n",
                n, Nx1,Nx2,Vy1,Vy2,Vz1,Vz2, My1,My2,Mz1,Mz2 );
        */

        n1 = J1[n];	n2 = J2[n];

        coord_trans ( xyz, Ln, n1, n2,
                      &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, p[n] );

        /* debugging ... check coordinate transformation coefficients
        printf("t1=%5.2f t2=%5.2f t3=%5.2f \n", t1, t2, t3 );
        printf("t4=%5.2f t5=%5.2f t6=%5.2f \n", t4, t5, t6 );
        printf("t7=%5.2f t8=%5.2f t9=%5.2f \n", t7, t8, t9 );
        */

        /* {F} = [T]'{Q} */
        eqF_mech[1][n][1]  += ( Nx1*t1 + Vy1*t4 + Vz1*t7 );
        eqF_mech[1][n][2]  += ( Nx1*t2 + Vy1*t5 + Vz1*t8 );
        eqF_mech[1][n][3]  += ( Nx1*t3 + Vy1*t6 + Vz1*t9 );
        eqF_mech[1][n][4]  += ( Mx1*t1 + My1*t4 + Mz1*t7 );
        eqF_mech[1][n][5]  += ( Mx1*t2 + My1*t5 + Mz1*t8 );
        eqF_mech[1][n][6]  += ( Mx1*t3 + My1*t6 + Mz1*t9 );

        eqF_mech[1][n][7]  += ( Nx2*t1 + Vy2*t4 + Vz2*t7 );
        eqF_mech[1][n][8]  += ( Nx2*t2 + Vy2*t5 + Vz2*t8 );
        eqF_mech[1][n][9]  += ( Nx2*t3 + Vy2*t6 + Vz2*t9 );
        eqF_mech[1][n][10] += ( Mx2*t1 + My2*t4 + Mz2*t7 );
        eqF_mech[1][n][11] += ( Mx2*t2 + My2*t5 + Mz2*t8 );
        eqF_mech[1][n][12] += ( Mx2*t3 + My2*t6 + Mz2*t9 );

        /* debugging ... check eqF data
        for (l=1;l<=13;l++) printf(" %9.2e ", W[1][i][l] );
        printf("\n");
        printf("n=%d ", n);
        for (l=1;l<=12;l++) {
            if (eqF_mech[1][n][l] != 0)
               printf(" eqF %d = %9.3f ", l, eqF_mech[1][n][l] );
        }
        printf("\n");
        */
    }			/* end trapezoidally distributed loads */

    /* internal element point loads -------------------------------- */
    nP[1]=m_interior_point_loads.size();
    if ( nP[1] > 10*nE ) {
        qDebug("Invalid number of concentrated frame element point loads");
        return;
    }
    for (i=1; i <= nP[1]; i++) {	/* ! local element coordinates ! */
        InteriorPointLoadPtr ipl=m_interior_point_loads.at(i-1);
        n=active_beams.indexOf(ipl->beam().toStrongRef())+1;
        if ( n <= 0 ) {
            qDebug("Error in internal point loads: wrong beam reference");
            return;
        }
        P[1][i][1] = (double) n;
        P[1][i][2] = ipl->forceLocal().x();
        if(m_is2D){
            P[1][i][3] = ipl->forceLocal().y();
            P[1][i][4] = ipl->forceLocal().z();
        }
        else{
            //            P[1][i][3] = -ipl->forceLocal().z();
            //            P[1][i][4] = ipl->forceLocal().y();
            P[1][i][3] = ipl->forceLocal().y();
            P[1][i][4] = -ipl->forceLocal().z();
        }
        P[1][i][5] = ipl->distance();

        a = P[1][i][5];	b = L[n] - a;

        if ( a < 0 || L[n] < a || b < 0 || L[n] < b ) {
            qDebug("Invalid distance for internal point load");
            return;
        }

        if ( shear ) {
            Ksy = (12.0*E[n]*Iz[n]) / (G[n]*Asy[n]*Le[n]*Le[n]);
            Ksz = (12.0*E[n]*Iy[n]) / (G[n]*Asz[n]*Le[n]*Le[n]);
        } else	Ksy = Ksz = 0.0;

        Ln = L[n];

        Nx1 = P[1][i][2]*a/Ln;
        Nx2 = P[1][i][2]*b/Ln;

        Vy1 = (1./(1.+Ksz))    * P[1][i][3]*b*b*(3.*a + b) / ( Ln*Ln*Ln ) +
                (Ksz/(1.+Ksz)) * P[1][i][3]*b/Ln;
        Vy2 = (1./(1.+Ksz))    * P[1][i][3]*a*a*(3.*b + a) / ( Ln*Ln*Ln ) +
                (Ksz/(1.+Ksz)) * P[1][i][3]*a/Ln;

        Vz1 = (1./(1.+Ksy))    * P[1][i][4]*b*b*(3.*a + b) / ( Ln*Ln*Ln ) +
                (Ksy/(1.+Ksy)) * P[1][i][4]*b/Ln;
        Vz2 = (1./(1.+Ksy))    * P[1][i][4]*a*a*(3.*b + a) / ( Ln*Ln*Ln ) +
                (Ksy/(1.+Ksy)) * P[1][i][4]*a/Ln;

        Mx1 = Mx2 = 0.0;

        My1 = -(1./(1.+Ksy))  * P[1][i][4]*a*b*b / ( Ln*Ln ) -
                (Ksy/(1.+Ksy))* P[1][i][4]*a*b   / (2.*Ln);
        My2 =  (1./(1.+Ksy))  * P[1][i][4]*a*a*b / ( Ln*Ln ) +
                (Ksy/(1.+Ksy))* P[1][i][4]*a*b   / (2.*Ln);

        Mz1 =  (1./(1.+Ksz))  * P[1][i][3]*a*b*b / ( Ln*Ln ) +
                (Ksz/(1.+Ksz))* P[1][i][3]*a*b   / (2.*Ln);
        Mz2 = -(1./(1.+Ksz))  * P[1][i][3]*a*a*b / ( Ln*Ln ) -
                (Ksz/(1.+Ksz))* P[1][i][3]*a*b   / (2.*Ln);

        n1 = J1[n];	n2 = J2[n];

        coord_trans ( xyz, Ln, n1, n2,
                      &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, p[n] );

        /* {F} = [T]'{Q} */
        eqF_mech[1][n][1]  += ( Nx1*t1 + Vy1*t4 + Vz1*t7 );
        eqF_mech[1][n][2]  += ( Nx1*t2 + Vy1*t5 + Vz1*t8 );
        eqF_mech[1][n][3]  += ( Nx1*t3 + Vy1*t6 + Vz1*t9 );
        eqF_mech[1][n][4]  += ( Mx1*t1 + My1*t4 + Mz1*t7 );
        eqF_mech[1][n][5]  += ( Mx1*t2 + My1*t5 + Mz1*t8 );
        eqF_mech[1][n][6]  += ( Mx1*t3 + My1*t6 + Mz1*t9 );

        eqF_mech[1][n][7]  += ( Nx2*t1 + Vy2*t4 + Vz2*t7 );
        eqF_mech[1][n][8]  += ( Nx2*t2 + Vy2*t5 + Vz2*t8 );
        eqF_mech[1][n][9]  += ( Nx2*t3 + Vy2*t6 + Vz2*t9 );
        eqF_mech[1][n][10] += ( Mx2*t1 + My2*t4 + Mz2*t7 );
        eqF_mech[1][n][11] += ( Mx2*t2 + My2*t5 + Mz2*t8 );
        eqF_mech[1][n][12] += ( Mx2*t3 + My2*t6 + Mz2*t9 );
    }					/* end element point loads */

    /* thermal loads ----------------------------------------------- */
    nT[1]=0;

    //      for (i=1; i <= nT[1]; i++) {	/* ! local element coordinates ! */
    //        sfrv=fscanf(fp,"%d", &n );
    //        if (sfrv != 1) sferr("frame element number in temperature load data");
    //        if ( n < 1 || n > nE ) {
    //            sprintf(errMsg,"\n  error in temperature loads: frame element number %d is out of range\n",n);
    //            errorMsg(errMsg);
    //            exit(161);
    //        }
    //        T[lc][i][1] = (double) n;
    //        for (l=2; l<=8; l++) {
    //            sfrv=fscanf(fp,"%f", &T[lc][i][l] );
    //            if (sfrv != 1) sferr("value in temperature load data");
    //        }
    //        a  = T[lc][i][2];
    //        hy = T[lc][i][3];
    //        hz = T[lc][i][4];

    //        if ( hy < 0 || hz < 0 ) {
    //            sprintf(errMsg,"\n  error in thermal load data: section dimension < 0\n   Frame element number: %d  hy: %f  hz: %f\n", n,hy,hz);
    //            errorMsg(errMsg);
    //            exit(162);
    //        }

    //        Nx2 = a*(1.0/4.0)*( T[lc][i][5]+T[lc][i][6]+T[lc][i][7]+T[lc][i][8])*E[n]*Ax[n];
    //        Nx1 = -Nx2;
    //        Vy1 = Vy2 = Vz1 = Vz2 = 0.0;
    //        Mx1 = Mx2 = 0.0;
    //        My1 =  (a/hz)*(T[lc][i][8]-T[lc][i][7])*E[n]*Iy[n];
    //        My2 = -My1;
    //        Mz1 =  (a/hy)*(T[lc][i][5]-T[lc][i][6])*E[n]*Iz[n];
    //        Mz2 = -Mz1;

    //        n1 = J1[n];	n2 = J2[n];

    //        coord_trans ( xyz, L[n], n1, n2,
    //            &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, p[n] );

    //        /* {F} = [T]'{Q} */
    //        eqF_temp[lc][n][1]  += ( Nx1*t1 + Vy1*t4 + Vz1*t7 );
    //        eqF_temp[lc][n][2]  += ( Nx1*t2 + Vy1*t5 + Vz1*t8 );
    //        eqF_temp[lc][n][3]  += ( Nx1*t3 + Vy1*t6 + Vz1*t9 );
    //        eqF_temp[lc][n][4]  += ( Mx1*t1 + My1*t4 + Mz1*t7 );
    //        eqF_temp[lc][n][5]  += ( Mx1*t2 + My1*t5 + Mz1*t8 );
    //        eqF_temp[lc][n][6]  += ( Mx1*t3 + My1*t6 + Mz1*t9 );

    //        eqF_temp[lc][n][7]  += ( Nx2*t1 + Vy2*t4 + Vz2*t7 );
    //        eqF_temp[lc][n][8]  += ( Nx2*t2 qreal distance=-1+ Vy2*t5 + Vz2*t8 );
    //        eqF_temp[lc][n][9]  += ( Nx2*t3 + Vy2*t6 + Vz2*t9 );
    //        eqF_temp[lc][n][10] += ( Mx2*t1 + My2*t4 + Mz2*t7 );
    //        eqF_temp[lc][n][11] += ( Mx2*t2 + My2*t5 + Mz2*t8 );
    //        eqF_temp[lc][n][12] += ( Mx2*t3 + My2*t6 + Mz2*t9 );
    //      }				/* end thermal loads	*/

    // assemble all element equivalent loads into
    // separate load vectors for mechanical and thermal loading
    for (n=1; n<=nE; n++) {
        n1 = J1[n];	n2 = J2[n];
        for (i=1; i<= 6; i++) F_mech[1][6*n1- 6+i] += eqF_mech[1][n][i];
        for (i=7; i<=12; i++) F_mech[1][6*n2-12+i] += eqF_mech[1][n][i];
        for (i=1; i<= 6; i++) F_temp[1][6*n1- 6+i] += eqF_temp[1][n][i];
        for (i=7; i<=12; i++) F_temp[1][6*n2-12+i] += eqF_temp[1][n][i];
    }

    /* prescribed displacements ------------------------------------ */
    nD[1]=0;
    //      sfrv=fscanf(fp,"%d", &nD[lc] );
    //      if (sfrv != 1) sferr("nD value in load data");
    //      if ( verbose ) {
    //        fprintf(stdout,"  number of prescribed displacements ");
    //        dots(stdout,16);	fprintf(stdout," nD = %3d\n", nD[lc] );
    //      }
    //      for (i=1; i <= nD[lc]; i++) {
    //        sfrv=fscanf(fp,"%d", &j);
    //        if (sfrv != 1) sferr("node number value in prescribed displacement data");
    //        for (l=5; l >=0; l--) {
    //            sfrv=fscanf(fp,"%f", &Dp[lc][6*j-l] );
    //            if (sfrv != 1) sferr("prescribed displacement value");
    //            if ( r[6*j-l] == 0 && Dp[lc][6*j-l] != 0.0 ) {
    //                sprintf(errMsg," Initial displacements can be prescribed only at restrained coordinates\n  node: %d  dof: %d  r: %d\n",
    //                j, 6-l, r[6*j-l] );
    //                errorMsg(errMsg);
    //                exit(171);
    //            }
    //        }
    //      }
}


void Frame3DDKernel::update_statics(
        int nN, int nE, int nL, int lc, int DoF,
        int *J1, int *J2,
        double *F, double *D, double *R, int *r, double **Q,
        double err, int ok,QVector<JointPtr> active_joints,QVector<BeamPtr> active_beams
        ){
    double	disp;
    int	i,j,n;

    if ( ok < 0 ) {
        setStability(UNSTABLE);
        qDebug("  * The Stiffness Matrix is not positive-definite *\n");
        qDebug("    Check that all six rigid-body translations are restrained\n");
        qDebug("    If geometric stiffness is included, reduce the loads.\n");
        emit updated();
        QtConcurrent::run(this,&Frame3DDKernel::log);
        return;
    }
    setStability(DETERMINATE);
    /*Node Displacement.
    * j node index, starting from 1.
    * X-dsp       Y-dsp       Z-dsp X-rot       Y-rot       Z-rot*/
    for (j=1; j<= nN; j++) {
        if(j>active_joints.length()) break;
        disp = 0.0;
        for ( i=5; i>=0; i-- ) disp += fabs( D[6*j-i] );
        JointPtr joint=active_joints.at(j-1);
        if ( disp > 0.0 ) {
            if(m_is2D){
                joint->setDisplacement(QVector3D(D[6*j-5],D[6*j-4],D[6*j-3]));
                joint->setDisplacementRot(QVector3D(D[6*j-2],D[6*j-1],D[6*j]));
            }else{
                //                joint->setDisplacement(QVector3D(D[6*j-5],D[6*j-3],-D[6*j-4]));
                //                joint->setDisplacementRot(QVector3D(D[6*j-2],D[6*j],D[6*j-1]));
                joint->setDisplacement(QVector3D(D[6*j-5],D[6*j-4],-D[6*j-3]));
                joint->setDisplacementRot(QVector3D(D[6*j-2],D[6*j-1],-D[6*j]));
            }
        }else{
            joint->setDisplacement(QVector3D(0,0,0));
            joint->setDisplacementRot(QVector3D(0,0,0));
        }
    }
    /*      Nx          Vy         Vz   Txx        Myy        Mzz*/;
    float maxF=0,minF=100000;
    for (n=1; n<= nE; n++) {
        if(n>active_beams.length()) break;
        BeamPtr beam=active_beams.at(n-1);
        //qDebug()<<beam->objectName()<<Q[n][1]<<" "<<Q[n][7];
        int axial_type;
        if (fabs(Q[n][1]) < 0.0001)
            axial_type=0;
        if ( Q[n][1] >=  0.0001 ) axial_type=-1;//compression
        if ( Q[n][1] <= -0.0001 ) axial_type=1;//tension

        if(m_is2D)
            beam->setForcesAndMoments(axial_type,Q[n][1],Q[n][2],Q[n][3],Q[n][4],Q[n][5],Q[n][6],1);
        else
            //            beam->setForcesAndMoments(axial_type,Q[n][1],Q[n][3],-Q[n][2],Q[n][4],Q[n][6],-Q[n][5],1);
            beam->setForcesAndMoments(axial_type,Q[n][1],Q[n][2],-Q[n][3],Q[n][4],Q[n][5],-Q[n][6],1);
        if (fabs(Q[n][7]) < 0.0001)
            axial_type=0;
        if ( Q[n][7] >=  0.0001 ) axial_type=1;//tension
        if ( Q[n][7] <= -0.0001 ) axial_type=-1;//compression
        if(m_is2D)
            beam->setForcesAndMoments(axial_type,Q[n][7],Q[n][8],Q[n][9],Q[n][10],Q[n][11],Q[n][12],2);
        else
            //            beam->setForcesAndMoments(axial_type,Q[n][7],Q[n][9],-Q[n][8],Q[n][10],Q[n][12],-Q[n][11],2);
            beam->setForcesAndMoments(axial_type,Q[n][7],Q[n][8],-Q[n][9],Q[n][10],Q[n][11],-Q[n][12],2);


        if( maxF < (fabs(Q[n][7])+fabs(Q[n][1]))*0.5 ){
            maxF=(fabs(Q[n][7])+fabs(Q[n][1]))*0.5;
        }
        if( minF > (fabs(Q[n][7])+fabs(Q[n][1]))*0.5 ){
            minF=(fabs(Q[n][7])+fabs(Q[n][1]))*0.5;
        }
    }

    /*Reactions Fx          Fy          Fz  Mxx         Myy         Mzz*/
    for (j=1; j<=nN; j++) {
        if(j>active_joints.length()) break;
        JointPtr joint=active_joints.at(j-1);
        i = 6*(j-1);
        if ( r[i+1] || r[i+2] || r[i+3] ||
             r[i+4] || r[i+5] || r[i+6] ) {
            if(m_is2D){
                joint->setReaction(QVector3D(R[6*j-5],R[6*j-4],R[6*j-3]));
                joint->setReactionMomentum(QVector3D(R[6*j-2],R[6*j-1],R[6*j]));
            }else{
                //                joint->setReaction(QVector3D(R[6*j-5],R[6*j-3],-R[6*j-4]));
                //                joint->setReactionMomentum(QVector3D(R[6*j-2],R[6*j],-R[6*j-1]));
                joint->setReaction(QVector3D(R[6*j-5],R[6*j-4],-R[6*j-3]));
                joint->setReactionMomentum(QVector3D(R[6*j-2],R[6*j-1],-R[6*j]));
            }
        }
        if( maxF < joint->reaction().length() ){
            maxF= joint->reaction().length();
        }
        if( minF > joint->reaction().length() ){
            minF=joint->reaction().length();
        }
    }
    m_maxForce=maxF;
    emit maxForceChanged();

    m_relative_equilibrium_error=err;
    emit updated();
    QtConcurrent::run(this,&Frame3DDKernel::log);
    return;
}

BeamPtr Frame3DDKernel::createBeam(JointPtr extreme1,JointPtr extreme2,QSizeF size,qreal E,
                                   qreal G, qreal d,QString name){
    if(!extreme1.isNull() && !extreme2.isNull() && extreme1!=extreme2){
        BeamPtr beam(new Beam(extreme1,extreme2,m_materialsManager,name,this));
        extreme1->addConnectedBeam(beam);
        extreme2->addConnectedBeam(beam);
        beam->setSize(size);
        beam->setYoungModulus(E);
        beam->setShearModulus(G);
        beam->setDensity(d);
        m_beams.append(beam);
        connect(beam.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        connect(beam.data(),SIGNAL(parametersChanged()),this,SLOT(update()));
        if(m_sceneRoot){
            beam->setSceneRoot(m_sceneRoot);
            //beam->createQmlEntity();
        }
        update();
        return beam;
    }
    return BeamPtr();
}

BeamPtr Frame3DDKernel::createBeam(JointPtr extreme1, JointPtr extreme2, QSizeF size, QString materialID, QString name)
{
    if(!extreme1.isNull() && !extreme2.isNull() && extreme1!=extreme2){
        /*Check for duplicates*/
        WeakJointPtr _e1,_e2;
        Q_FOREACH(WeakBeamPtr _b,extreme1->connectedBeams()){
            _b.toStrongRef()->extremes(_e1,_e2);
            if(_e1==extreme2 || _e2==extreme2)
                return BeamPtr();
        }

        JointPtr tmp;
        //Reordering
        if(this->is2D()){
            if(extreme1->position().x()>extreme2->position().x()){
                tmp=extreme1;
                extreme1=extreme2;
                extreme2=tmp;
            }
            else if(extreme1->position().x()==extreme2->position().x()){
                if(extreme1->position().y()>extreme2->position().y()){
                    tmp=extreme1;
                    extreme1=extreme2;
                    extreme2=tmp;
                }
                else if(extreme1->position().y()==extreme2->position().y()){
                    if(extreme1->position().z()>extreme2->position().z()){
                        tmp=extreme1;
                        extreme1=extreme2;
                        extreme2=tmp;
                    }
                }
            }
        }else{
            if(extreme1->position().x()>extreme2->position().x()){
                tmp=extreme1;
                extreme1=extreme2;
                extreme2=tmp;
            }
            else if(extreme1->position().x()==extreme2->position().x()){
                if(extreme1->position().y()>extreme2->position().y()){//Check with the flipped coordinates
                    tmp=extreme1;
                    extreme1=extreme2;
                    extreme2=tmp;
                }
                else if(extreme1->position().y()==extreme2->position().y()){
                    if(extreme1->position().z()>extreme2->position().z()){
                        tmp=extreme1;
                        extreme1=extreme2;
                        extreme2=tmp;
                    }
                }
            }
        }

        BeamPtr beam(new Beam(extreme1,extreme2,m_materialsManager,name,this));
        extreme1->addConnectedBeam(beam);
        extreme2->addConnectedBeam(beam);
        beam->setSize(size);
        beam->setMaterial(materialID);
        m_beams.append(beam);
        connect(beam.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        connect(beam.data(),SIGNAL(parametersChanged()),this,SLOT(update()));
        if(m_sceneRoot){
            beam->setSceneRoot(m_sceneRoot);
            beam->createQmlEntity();
        }
        update();
        return beam;
    }
    return BeamPtr();
}
JointPtr Frame3DDKernel::createJoint(QVector3D position, QString name,
                                     bool  support_X,bool support_Y,bool support_Z,
                                     bool support_XX,bool support_YY,bool support_ZZ){

    JointPtr joint(new Joint(position,name,this));
    joint->setSupport(support_X,support_Y,support_Z, support_XX,support_YY,support_ZZ);
    m_joints.append(joint);
    connect(joint.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
    connect(joint.data(),SIGNAL(supportChanged()),this,SLOT(update()));
    connect(joint.data(),SIGNAL(connectedBeamsChanged()),this,SLOT(update()));//suspicious....
    if(m_sceneRoot){
        joint->setSceneRoot(m_sceneRoot);
        joint->createQmlEntity();
    }
    update();

    return joint;
}

NodeLoadPtr Frame3DDKernel::createNodeLoad(QVector3D force, JointPtr joint,QString name){
    if(!force.isNull() && !joint.isNull()){
        NodeLoadPtr nodeLoad(new NodeLoad(joint,name,this));
        nodeLoad->setForce(force);
        m_node_loads.append(nodeLoad);
        connect(nodeLoad.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        connect(nodeLoad.data(),SIGNAL(forceChanged()),this,SLOT(update()));
        connect(nodeLoad.data(),SIGNAL(momentumChanged()),this,SLOT(update()));
        if(m_sceneRoot){
            nodeLoad->setSceneRoot(m_sceneRoot);
            nodeLoad->createQmlEntity();
        }
        update();
        return nodeLoad;

    }
    return NodeLoadPtr();
}

UniformlyDistributedLoadPtr Frame3DDKernel::createUDLoad(QVector3D force, BeamPtr beam,QString name){
    if(!force.isNull() && !beam.isNull()){
        UniformlyDistributedLoadPtr udLoad(new UniformlyDistributedLoad(beam,name, this));
        udLoad->setForce(force);
        m_uniformly_distributed_loads.append(udLoad);
        connect(udLoad.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        connect(udLoad.data(),SIGNAL(forceChanged()),this,SLOT(update()));
        if(m_sceneRoot){
            udLoad->setSceneRoot(m_sceneRoot);
            udLoad->createQmlEntity();
        }
        update();
        return udLoad;
    }
    return UniformlyDistributedLoadPtr();

}

InteriorPointLoadPtr Frame3DDKernel::createIPLoad(QVector3D force, BeamPtr beam, qreal distance,QString name){
    if(!force.isNull() && !beam.isNull()){
        if(distance<0)
            distance=beam->length()/2;
        InteriorPointLoadPtr ipLoad(new InteriorPointLoad(beam,distance,name,this));
        ipLoad->setForce(force);
        m_interior_point_loads.append(ipLoad);
        connect(ipLoad.data(),SIGNAL(forceChanged()),this,SLOT(update()));
        connect(ipLoad.data(),SIGNAL(distanceChanged()),this,SLOT(update()));
        connect(ipLoad.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        if(m_sceneRoot){
            ipLoad->setSceneRoot(m_sceneRoot);
            ipLoad->createQmlEntity();
        }
        update();
        return ipLoad;
    }
    return InteriorPointLoadPtr();
}

TrapezoidalForcePtr Frame3DDKernel::createTPZLoad(QVector3D force,
                                                  BeamPtr beam,
                                                  QVector3D localPosition,
                                                  QVector2D extent, QString name)
{
    if(!force.isNull() && !beam.isNull()){
        TrapezoidalForcePtr tpzLoad(new TrapezoidalForce(beam,name,this));
        tpzLoad->setForce(force);
        tpzLoad->setRelativePosition(localPosition,extent);
        m_trapezoidal_loads.append(tpzLoad);
        connect(tpzLoad.data(),SIGNAL(forceChanged()),this,SLOT(update()));
        connect(tpzLoad.data(),SIGNAL(relativePositionChanged()),this,SLOT(update()));
        connect(tpzLoad.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        if(m_sceneRoot){
            tpzLoad->setSceneRoot(m_sceneRoot);
            tpzLoad->createQmlEntity();
        }
        update();
        return tpzLoad;
    }
    return TrapezoidalForcePtr();
}

void Frame3DDKernel::createTPZLoad(Beam* beam,QVariantMap aesthetic)
{
    BeamPtr beamptr;
    Q_FOREACH(BeamPtr b,m_beams)
        if(b==beam){
            beamptr=b;
            break;
        }
    if(!beamptr.isNull()){
        TrapezoidalForcePtr tpzLoad(new TrapezoidalForce(beamptr,"",this));
        m_trapezoidal_loads.append(tpzLoad);
        connect(tpzLoad.data(),SIGNAL(forceChanged()),this,SLOT(update()));
        connect(tpzLoad.data(),SIGNAL(relativePositionChanged()),this,SLOT(update()));
        connect(tpzLoad.data(),SIGNAL(extentChanged()),this,SLOT(update()));
        connect(tpzLoad.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        if(m_sceneRoot){
            tpzLoad->setSceneRoot(m_sceneRoot);
            tpzLoad->createQmlEntity(aesthetic);
        }
        update();
    }
}

void Frame3DDKernel::createNodeLoad(Joint *joint, QVariantMap aesthetic)
{
    JointPtr jointptr;
    Q_FOREACH(JointPtr j,m_joints)
        if(j==joint){
            jointptr=j;
            break;
        }
    if(!jointptr.isNull()){
        NodeLoadPtr nodeLoad(new NodeLoad(jointptr,"",this));
        m_node_loads.append(nodeLoad);
        connect(nodeLoad.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        connect(nodeLoad.data(),SIGNAL(forceChanged()),this,SLOT(update()));
        connect(nodeLoad.data(),SIGNAL(momentumChanged()),this,SLOT(update()));
        if(m_sceneRoot){
            nodeLoad->setSceneRoot(m_sceneRoot);
            nodeLoad->createQmlEntity(aesthetic);
        }
        update();
    }
}

void Frame3DDKernel::createUDLoad(Beam *beam, QVariantMap aesthetic)
{
    BeamPtr beamptr;
    Q_FOREACH(BeamPtr b,m_beams)
        if(b==beam){
            beamptr=b;
            break;
        }
    if(!beamptr.isNull()){
        UniformlyDistributedLoadPtr udLoad(new UniformlyDistributedLoad(beamptr,"",this));
        m_uniformly_distributed_loads.append(udLoad);
        connect(udLoad.data(),SIGNAL(forceChanged()),this,SLOT(update()));
        connect(udLoad.data(),SIGNAL(killMe()),this,SLOT(onKillRequest()));
        if(m_sceneRoot){
            udLoad->setSceneRoot(m_sceneRoot);
            udLoad->createQmlEntity(aesthetic);
        }
        update();
    }
}

QVector3D Frame3DDKernel::initialPose()
{
    qreal xmin=5000,xmax=0,ymin=5000,ymax=0;
    Q_FOREACH(JointPtr joint, m_joints){
        QVector3D j_pos=joint->position()*m_modelScale;
        if(j_pos.x()>xmax)
            xmax=j_pos.x();
        if(j_pos.x()<xmin)
            xmin=j_pos.x();
        if(j_pos.y()>ymax)
            ymax=j_pos.y();
        if(j_pos.y()<ymin)
            ymin=j_pos.y();
    }
    //return QVector3D(0,0,-1000);
    return QVector3D(-0.5*xmin-0.5*xmax,-0.5*ymin-0.5*ymax,-1000);

}
bool Frame3DDKernel::splitBeam(BeamPtr beam, qreal offset,JointPtr &new_joint){
    if(!beam.isNull() && offset>0 && offset<=beam->length()){
        qDebug()<<"Splitting";
        WeakJointPtr extreme1_w, extreme2_w;
        beam->extremes(extreme1_w,extreme2_w);
        JointPtr extreme1=extreme1_w.toStrongRef();
        JointPtr extreme2=extreme2_w.toStrongRef();
        //TODO:2D?? constarints
        QVector3D direction=extreme2->position()-extreme1->position();
        direction.normalize();
        QVector3D new_joint_position=extreme1->position()+(direction*offset);
        new_joint=createJoint(new_joint_position);

        BeamPtr segment_1,segment_2;

        qreal dummy,E,G,d;

        beam->parameters(dummy,dummy,dummy,dummy,dummy,dummy,E,G,dummy,d);
        segment_1=createBeam(extreme1,new_joint,beam->size(),E,G,d,beam->objectName()+QString("Part_1"));
        segment_1->setParentBeam(beam);

        segment_2=createBeam(new_joint,extreme2,beam->size(),E,G,d,beam->objectName()+QString("Part_2"));
        segment_2->setParentBeam(beam);

        beam->addPart(segment_1);
        beam->addPart(segment_2);
        beam->split();

        return true;
    }
    return false;
}

bool Frame3DDKernel::unifyBeam(BeamPtr beam){
    if(!beam.isNull() && m_beams.contains(beam)){
        WeakJointPtr e1,e2,e_mid;
        Q_FOREACH(WeakBeamPtr b,beam->subParts()){
            WeakJointPtr e1_sub,e2_sub;
            b.toStrongRef()->extremes(e1_sub,e2_sub);
            if(e1==e1_sub || e2==e1_sub){
                e_mid=e2_sub;
            }
            else{
                e_mid=e1_sub;
            }
            unifyBeam_recursive_step(b.toStrongRef());
        }
        m_joints.removeAll(e_mid);
        beam->unify();
        return true;
    }
    return false;
}


void Frame3DDKernel::setGravity(QVector3D v)
{
    if(v!=m_gravity){
        m_gravity=v;
        emit gravityChanged();
    }
}

void Frame3DDKernel::unifyBeam_recursive_step(BeamPtr beam){
    if(!beam.isNull()){
        WeakJointPtr e1,e2,e_mid;
        beam->extremes(e1,e2);
        Q_FOREACH(WeakBeamPtr b,beam->subParts()){
            WeakJointPtr e1_sub,e2_sub;
            b.toStrongRef()->extremes(e1_sub,e2_sub);
            if(e1==e1_sub || e2==e1_sub){
                e_mid=e2_sub;
            }
            else{
                e_mid=e1_sub;
            }
            unifyBeam_recursive_step(b);
        }
        m_joints.removeAll(e_mid);
        m_beams.removeAll(beam);
    }
}

void Frame3DDKernel::onKillRequest(){
    QObject* sender=QObject::sender();
    Joint* j=qobject_cast<Joint*>(sender);
    if(j!=Q_NULLPTR){
        JointPtr _ptr;
        Q_FOREACH(JointPtr item, m_joints){
            if(item.data()==j){
                _ptr=item;
                break;
            }
        }
        if(!_ptr.isNull()){
            m_joints.removeAll(_ptr);
            update();
            return;
        }
    }
    Beam* b=qobject_cast<Beam*>(sender);
    if(b!=Q_NULLPTR){
        BeamPtr _ptr;
        Q_FOREACH(BeamPtr item, m_beams){
            if(item.data()==b){
                _ptr=item;
                break;
            }
        }
        if(!_ptr.isNull()){
            m_beams.removeAll(_ptr);
            update();
            return;
        }
    }
    NodeLoad* nl=qobject_cast<NodeLoad*>(sender);
    if(nl!=Q_NULLPTR ){
        NodeLoadPtr _ptr;
        Q_FOREACH(NodeLoadPtr item, m_node_loads){
            if(item.data()==nl){
                _ptr=item;
                break;
            }
        }
        if(!_ptr.isNull()){
            m_node_loads.removeAll(_ptr);
            update();
            return;
        }
    }
    UniformlyDistributedLoad* udl=qobject_cast<UniformlyDistributedLoad*>(sender);
    if(udl!=Q_NULLPTR){
        UniformlyDistributedLoadPtr _ptr;
        Q_FOREACH(UniformlyDistributedLoadPtr item, m_uniformly_distributed_loads){
            if(item.data()==udl){
                _ptr=item;
                break;
            }
        }
        if(!_ptr.isNull()){
            m_uniformly_distributed_loads.removeAll(_ptr);
            update();
            return;
        }
    }
    InteriorPointLoad* ipl=qobject_cast<InteriorPointLoad*>(sender);
    if(ipl!=Q_NULLPTR) {
        InteriorPointLoadPtr _ptr;
        Q_FOREACH(InteriorPointLoadPtr item, m_interior_point_loads){
            if(item.data()==ipl){
                _ptr=item;
                break;
            }
        }
        if(!_ptr.isNull()){
            m_interior_point_loads.removeAll(_ptr);
            update();
            return;
        }
    }
    TrapezoidalForce* tpz=qobject_cast<TrapezoidalForce*>(sender);
    if(tpz!=Q_NULLPTR) {
        TrapezoidalForcePtr _ptr;
        Q_FOREACH(TrapezoidalForcePtr item, m_trapezoidal_loads){
            if(item.data()==tpz){
                _ptr=item;
                break;
            }
        }
        if(!_ptr.isNull()){
            m_trapezoidal_loads.removeAll(_ptr);
            update();
            return;
        }
    }
}

void Frame3DDKernel::removeBeam(BeamPtr item){
    m_beams.removeAll(item);
    update();
}

void Frame3DDKernel::removeJoint(JointPtr item){
    m_joints.removeAll(item);
    update();
}

void Frame3DDKernel::removeIPLoad(InteriorPointLoadPtr item){
    m_interior_point_loads.removeAll(item);
    update();
}

void Frame3DDKernel::removeTPZLoad(TrapezoidalForcePtr item)
{
    m_trapezoidal_loads.removeAll(item);
    update();
}

void Frame3DDKernel::removeNodeLoad(NodeLoadPtr item){
    m_node_loads.removeAll(item);
    update();
}

void Frame3DDKernel::removeUDLoad(UniformlyDistributedLoadPtr item){
    m_uniformly_distributed_loads.removeAll(item);
    update();
}

void Frame3DDKernel::log(){
    QVariantMap logger_map;
    logger_map["status"]=m_stability;
    logger_map["gravity"]="("+QString::number(m_gravity.x())+","+QString::number(m_gravity.y())+
            ","+QString::number(m_gravity.z())+")";
    logger_map["n_joints"]=m_joints.size();
    logger_map["n_beams"]=m_beams.size();
    logger_map["n_loads"]=m_trapezoidal_loads.size();

    QVariantList element_list;
    QVariantMap element;
    Q_FOREACH (JointPtr j,m_joints){
        element.clear();
        element["Name"]=j->objectName();
        element["Position"]="("+QString::number(j->position().x())+","+QString::number(j->position().y())+
                ","+QString::number(j->position().z())+")";
        bool X,Y,Z,XX,YY,ZZ;
        j->support(X,Y,Z,XX,YY,ZZ);
        element["Support"]="("+QString::number(X)+","+QString::number(Y)+","+QString::number(Z)
                +","+QString::number(XX)+","+QString::number(YY)+","+QString::number(ZZ)+")";
        element["Displacement"]="("+QString::number(j->displacement().x())+","+QString::number(j->displacement().y())+
                ","+QString::number(j->displacement().z())+")";
        element_list.append(element);
    }
    logger_map["joints"]=element_list;

    element_list.clear();
    Q_FOREACH(BeamPtr b, m_beams){
        element.clear();
        element["Name"]=b->objectName();
        element["Enabled"]=b->enabled();
        WeakJointPtr e1,e2;
        b->extremes(e1,e2);
        element["Extreme_1"]=e1.toStrongRef()->objectName();
        element["Extreme_2"]=e2.toStrongRef()->objectName();
        element["MaterialID"]=b->materialID();
        element["Size"]="("+QString::number(b->size().width())+","+QString::number(b->size().height())+")";
        int force_type;
        qreal dummy;
        b->ForcesAndMoments(force_type,dummy,dummy,dummy,
                            dummy,dummy,dummy,1);
        element["Axial_force_type_extreme_1"]=QString::number(force_type);
        b->ForcesAndMoments(force_type,dummy,dummy,dummy,
                            dummy,dummy,dummy,2);
        element["Axial_force_type_extreme_2"]=QString::number(force_type);

        qreal stress_axial,stress_shear,stress_bending;
        b->stressRatio(stress_axial,stress_bending,stress_shear,3);
        element["Stress_ratio_axial"]=QString::number(stress_axial);
        element["Stress_ratio_shear"]=QString::number(stress_shear);
        element["Stress_ratio_beanding"]=QString::number(stress_bending);
        element_list.append(element);

    }
    logger_map["beams"]=element_list;

    element_list.clear();
    Q_FOREACH(TrapezoidalForcePtr l, m_trapezoidal_loads){
        element.clear();
        element["Beam"]=l->beam().toStrongRef()->objectName();
        QVector3D pos;
        QVector2D ext;
        l->relativePosition(pos,ext);
        element["Relative_position"]="("+QString::number(pos.x())+","+QString::number(pos.y())+
                ","+QString::number(pos.z())+")";
        element["Extent"]="("+QString::number(ext.x())+","+QString::number(ext.y())+")";
        element["Force"]="("+QString::number(l->force().x())+","+QString::number(l->force().y())+
                ","+QString::number(l->force().z())+")";
        element_list.append(element);
    }
    logger_map["loads"]=element_list;

    m_logger.log_static_configuration(logger_map);

}


